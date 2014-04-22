/**
 * Copyright (C) 2014  Christian Schwarzgruber <christiandev9@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#include "AnimationOptions.hpp"
#include "ui_AnimationOptions.h"
#include "alt_key.hpp"
#include "aqp.hpp"
#include "AnimationItem.hpp"
#include "Animation.hpp"
#include "Draw.hpp"
#include "Lift.hpp"
#include "Firework.hpp"
#include "AxisNailWall.hpp"
#include "WireBoxCenterShrinkGrow.hpp"
#include "WireBoxCornerShrinkGrow.hpp"
#include "Loadbar.hpp"
#include "RandomFiller.hpp"
#include "RandomSpark.hpp"
#include "RandomSparkFlash.hpp"
#include "RandomZLift.hpp"
#include "Wall.hpp"
#include "Rain.hpp"
#include "StringFly.hpp"

#include <QTimer>
// Qt Includes
#ifdef _DEBUG_
#include <QDebug>
#endif


AnimationOptions::AnimationOptions(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AnimationOptions),
    m_animationOptionsModefied(false),
    m_animationAt(-1)
{
    // Setup the user interface
    ui->setupUi(this);
    // Create connections ( Signal & Slot )
    connect(ui->m_nextPB, &QPushButton::pressed, this, &AnimationOptions::optionsNextAnimation);
    connect(ui->m_prevPB, &QPushButton::pressed, this, &AnimationOptions::optionsPrevAnimation);
    connect(ui->m_applyPB, &QPushButton::pressed, this, &AnimationOptions::applyAnimationOptions);
    connect(ui->m_cancelPB, &QPushButton::pressed, this, &AnimationOptions::cancel);
    connect(ui->m_okPB, &QPushButton::pressed, this, &AnimationOptions::ok);
    connect(this, &QDialog::rejected, this, &AnimationOptions::cancel);
    setWindowModified(true);
    AQP::accelerateWidget(this);


}

AnimationOptions::~AnimationOptions()
{
    delete ui;
}

/*!
 \brief Slot gets called when the user has pressed the edit button from \a AnimationPlayListWidget.
 The first animation options are displayed, and the window will be shown, all other animation options
 will be inserted either in \a optionsNextAnimation or \a optionsPrevAnimation slot function.

 \param itemsList
*/
void AnimationOptions::adjustAnimationOptions(QList<AnimationItem*> &itemsList)
{
    if(itemsList.isEmpty())
       return;
    m_itemList = itemsList;
    optionsNextAnimation();
    show();
}


/*!
 \brief Gets called if the ui changes (e.g. the language)

 \param e
*/
void AnimationOptions::changeEvent(QEvent *e)
{
    QDialog::changeEvent(e);
    switch (e->type())
    {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

/*!
 \brief

*/
void AnimationOptions::applyAnimationOptions()
{
    if(compareOldNewAnimationOptions()){
        Options options;
        options.m_speed = ui->m_speedSpinB->value();
        if (ui->m_axisComB->currentIndex() == Draw::X_AXIS)
            options.m_axis = Draw::X_AXIS;
        else if (ui->m_axisComB->currentIndex() == Draw::Y_AXIS)
            options.m_axis = Draw::Y_AXIS;
        else
            options.m_axis = Draw::Z_AXIS;

        options.m_delay = ui->m_delaySpinB->value();
        options.m_direction = ui->m_directionComB->currentIndex() == Draw::Forward ? Draw::Forward : Draw::Backward ;
        options.m_invert = ui->m_invertCheckB->isChecked();
        options.m_iteration = ui->m_iterationsSpinB->value();
        options.m_leds = ui->m_ledsSpinB->value();
        options.m_state = ui->m_ledStateCheckB->isChecked() == true ? Draw::ON : Draw::OFF;
        options.m_text = ui->m_textLineE->text();
        m_animationToUpdate->setOptions(options);
        // FIXME::Options are already applyed we need to check now if the updated animationitem the same is as the current one played
        Q_EMIT updateAnimation(m_animationToUpdate);
        Q_EMIT applyNewAnimationArguments(m_animationToUpdate);
    }
}

/*!
 \brief

*/
void AnimationOptions::cancel()
{
    hide();
    m_animationAt = -1;
}

/*!
 \brief

*/
void AnimationOptions::ok()
{
    if (compareOldNewAnimationOptions()) {
        QMessageBox msgb;
        msgb.setText("Options have been modified!");
        msgb.setInformativeText("Do you want apply the new options?");
        msgb.setWindowTitle("Options Modified");
        msgb.setIcon(QMessageBox::Information);
        msgb.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
        msgb.setDefaultButton(QMessageBox::Ok);
        int ret = msgb.exec();

        if (ret == QMessageBox::Ok)
           Q_EMIT updateAnimation(m_animationToUpdate);

    }
    hide();
    // If current animation has ben modefied ask user if he wants to apply the settings
    // If nothing has changed hide application
}

void AnimationOptions::updateUi()
{
    if (m_animationAt){
        ui->m_prevPB->setEnabled(true);
    } else {
        ui->m_prevPB->setDisabled(true);
    }
    if(m_animationAt >= m_itemList.count() - 1 )
        ui->m_nextPB->setDisabled(true);
    else
        ui->m_nextPB->setEnabled(true);

}

/*!
 \brief

*/
void AnimationOptions::optionsNextAnimation()
{
    if( m_animationAt < m_itemList.count() -1)
        m_animationAt++;
    m_animationToUpdate = m_itemList.at(m_animationAt);
    ui->m_propertiesGB->setTitle("Properties of " + m_animationToUpdate->text());
    hideShowWidgetsDisplayOptions();
}

/*!
 \brief

*/
void AnimationOptions::optionsPrevAnimation()
{
    if(m_animationAt)
        m_animationAt--;
    m_animationToUpdate = m_itemList.at(m_animationAt);
    ui->m_propertiesGB->setTitle("Properties of " + m_animationToUpdate->text());
    hideShowWidgetsDisplayOptions();
}

/*!
 \brief

 \param hasOption
 \param options
*/
void AnimationOptions::hideShowWidgetsDisplayOptions()
{
    const Options *options = m_animationToUpdate->getOptions();
    const int hasOption = m_animationToUpdate->getAvailableAnimationOptions();

    for (int i = 0; i < TOTAL_ARGUMENTS; i++) {
        switch ( (1<<i) & hasOption )
        {
        case Speed:
            ui->m_speedLabel->setEnabled(true);
            ui->m_speedSpinB->setEnabled(true);
            ui->m_speedSpinB->setValue(options->m_speed);
            break;
        case Direction:
            ui->m_directionLabel->setEnabled(true);
            ui->m_directionComB->setEnabled(true);
            ui->m_directionComB->setCurrentIndex(options->m_direction);
            break;
        case Axis:
            ui->m_axisLabel->setEnabled(true);
            ui->m_axisComB->setEnabled(true);
            ui->m_axisComB->setCurrentIndex(options->m_axis);
            break;
        case Leds:
            ui->m_ledsLabel->setEnabled(true);
            ui->m_ledsSpinB->setEnabled(true);
            ui->m_ledsSpinB->setValue(options->m_leds);
            break;
        case Particls:
            ui->m_particlesLabel->setEnabled(true);
            ui->m_particlesSpinB->setEnabled(true);
            ui->m_particlesSpinB->setValue(options->m_leds);
            break;
        case Delay:
            ui->m_delayLabel->setEnabled(true);
            ui->m_delaySpinB->setEnabled(true);
            ui->m_delaySpinB->setValue(options->m_delay);
            break;
        case Iterations:
            ui->m_iterationsLabel->setEnabled(true);
            ui->m_iterationsSpinB->setEnabled(true);
            ui->m_iterationsSpinB->setValue(options->m_iteration);
            break;
        case Invert:
            ui->m_invertCheckB->setEnabled(true);
            ui->m_invertCheckB->setChecked(options->m_invert);
            break;
        case CenterStart:
            ui->m_invertCheckB->setEnabled(true);
            ui->m_invertCheckB->setChecked(options->m_invert);
            break;
        case Text:
            ui->m_textLabel->setEnabled(true);
            ui->m_textLineE->setEnabled(true);
            ui->m_textLineE->setText(options->m_text);
            break;
        case LedState:
            ui->m_ledStateCheckB->setEnabled(true);
            ui->m_ledStateCheckB->setChecked(options->m_state);
            break;
        default:
            switch ((1<<i))
            {
            case Speed:
                ui->m_speedLabel->setEnabled(false);
                ui->m_speedSpinB->setEnabled(false);
                break;
            case Direction:
                ui->m_directionLabel->setEnabled(false);
                ui->m_directionComB->setEnabled(false);
                break;
            case Axis:
                ui->m_axisLabel->setEnabled(false);
                ui->m_axisComB->setEnabled(false);
                break;
            case Leds:
                ui->m_ledsLabel->setEnabled(false);
                ui->m_ledsSpinB->setEnabled(false);
                break;
            case Particls:
                ui->m_particlesLabel->setEnabled(false);
                ui->m_particlesSpinB->setEnabled(false);
                break;
            case Delay:
                ui->m_delayLabel->setEnabled(false);
                ui->m_delaySpinB->setEnabled(false);
                break;
            case Iterations:
                ui->m_iterationsLabel->setEnabled(false);
                ui->m_iterationsSpinB->setEnabled(false);
                break;
            case Invert:
                ui->m_invertCheckB->setEnabled(false);
                break;
            case CenterStart:
                ui->m_invertCheckB->setEnabled(false);
                break;
            case Text:
                ui->m_textLabel->setEnabled(false);
                ui->m_textLineE->setEnabled(false);
                break;
            case LedState:
                ui->m_ledStateCheckB->setEnabled(false);
                break;
            default: /* Should never come here */
                break;
            }
            break;
        }
    }
//    m_updateUi->start();

    Q_EMIT updateUi();
}


// TODO:: Create class for the Options better for intial values and comparies,
// in these case the value of the options are undefined and therfor the behavior will be undefined
u_int8_t AnimationOptions::compareOldNewAnimationOptions()
{
    const Options *options = m_animationToUpdate->getOptions();
    if (options->m_speed != ui->m_speedSpinB->value())
        return 1;
    else if(options->m_axis != ui->m_axisComB->currentIndex())
        return 1;

//    else if(options->delay != ui->)
}

