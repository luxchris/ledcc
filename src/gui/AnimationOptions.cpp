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

// ThirdParty
#include "aqp.hpp"
#include "alt_key.hpp"
// Qt Includes
#include <QMessageBox>
#include <QTimer>
#ifdef _DEBUG_
#include <QDebug>
#endif

/*!
 \brief Slot gets called when the user has pressed the edit button from \a AnimationPlayListWidget.
 The first animation options are displayed, and the window will be shown, all other animation options
 will be inserted either in \a optionsNextAnimation or \a optionsPrevAnimation slot function.

 \param itemsList
*/

AnimationOptions::AnimationOptions(QList<AnimationItem*> &itemsList, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AnimationOptions),
    m_animationAt(-1)
{
    Q_ASSERT(!itemsList.isEmpty());
    // Setup the user interface
    ui->setupUi(this);
    // Create connections ( Signal & Slot )
    connect(ui->m_nextPB, &QPushButton::pressed, this, &AnimationOptions::optionsNextAnimation);
    connect(ui->m_prevPB, &QPushButton::pressed, this, &AnimationOptions::optionsPrevAnimation);
    connect(ui->m_applyPB, &QPushButton::pressed, this, &AnimationOptions::applyAnimationOptions);
    connect(ui->m_cancelPB, &QPushButton::pressed, this, &AnimationOptions::cancel);
    connect(ui->m_okPB, &QPushButton::pressed, this, &AnimationOptions::ok);
    connect(this, &QDialog::rejected, this, &AnimationOptions::cancel);


    m_itemList = itemsList;
    optionsNextAnimation();
    AQP::accelerateWidget(this);
}

/*!
 \brief Destructor of class

*/
AnimationOptions::~AnimationOptions()
{
    delete ui;
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
    if(isWindowModified()){
        setWindowModified(false);
        AnimationItem::Options options;
        options.m_speed = ui->m_speedSpinB->value();
        for (int i = 1; i < AnimationItem::TOTAL_ARGUMENTS; i++) {
            switch ( (1 << i) &  m_animationToUpdate->getAvailableAnimationOptions() )
            {
            case AnimationItem::Direction:
                options.m_direction = static_cast<Draw::Direction>(ui->m_directionComB->currentIndex());
                break;
            case AnimationItem::Axis:
                options.m_axis = static_cast<Draw::Axis>(ui->m_axisComB->currentIndex());
                break;
            case AnimationItem::Leds:
                options.m_leds = ui->m_ledsSpinB->value();
                break;
            case AnimationItem::Particls:
                options.m_leds = ui->m_particlesSpinB->value();
                break;
            case AnimationItem::Delay:
                options.m_delay = ui->m_delaySpinB->value();
                break;
            case AnimationItem::Iterations:
                options.m_iteration = ui->m_iterationsSpinB->value();
                break;
            case AnimationItem::Invert:
                options.m_invert = ui->m_invertCheckB->isChecked();
                break;
            case AnimationItem::CenterStart:
                options.m_invert = ui->m_invertCheckB->isChecked();
                break;
            case AnimationItem::Text:
                options.m_text = ui->m_textLineE->text();
                break;
            case AnimationItem::LedState:
               options.m_state = static_cast<Draw::BixelState>(ui->m_ledStateCheckB->isChecked());
                break;
            default:
                break;
            }
        }
        m_animationToUpdate->setOptions(options);

        Q_EMIT applyNewAnimationArguments(m_animationToUpdate);
    }
}

/*!
 \brief

*/
void AnimationOptions::cancel()
{
    close();
}

/*!
 \brief

*/
void AnimationOptions::ok()
{
    shouldeApplyNewOptions();
    close();
}

/*!
 \brief

*/
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
    shouldeApplyNewOptions();
    m_animationToUpdate = m_itemList.at(m_animationAt);
    ui->m_propertiesGB->setTitle(tr("Properties of animation ") + m_animationToUpdate->text());
    hideShowWidgetsDisplayOptions();
}

/*!
 \brief

*/
void AnimationOptions::optionsPrevAnimation()
{
    if(m_animationAt)
        m_animationAt--;
    shouldeApplyNewOptions();
    m_animationToUpdate = m_itemList.at(m_animationAt);
    ui->m_propertiesGB->setTitle(tr("Properties of animation ") + m_animationToUpdate->text());
    hideShowWidgetsDisplayOptions();
}

/*!
 \brief

 \param hasOption
 \param options
*/
void AnimationOptions::hideShowWidgetsDisplayOptions()
{
    const AnimationItem::Options *options = m_animationToUpdate->getOptions();
    /*
     * Disconnect all widgets first, otherwise the signal gets triggert when
     * we insert a new value into the widget, which is different than the current one
     */
    disconnectAll();

    for (int i = 0; i < AnimationItem::TOTAL_ARGUMENTS; i++) {
        switch ( (1 << i) & m_animationToUpdate->getAvailableAnimationOptions() )
        {
        case AnimationItem::Speed:
            ui->m_speedLabel->setEnabled(true);
            ui->m_speedSpinB->setEnabled(true);
            ui->m_speedSpinB->setValue(options->m_speed);
            break;
        case AnimationItem::Direction:
            ui->m_directionLabel->setEnabled(true);
            ui->m_directionComB->setEnabled(true);
            ui->m_directionComB->setCurrentIndex(options->m_direction);
            break;
        case AnimationItem::Axis:
            ui->m_axisLabel->setEnabled(true);
            ui->m_axisComB->setEnabled(true);
            ui->m_axisComB->setCurrentIndex(options->m_axis);
            break;
        case AnimationItem::Leds:
            ui->m_ledsLabel->setEnabled(true);
            ui->m_ledsSpinB->setEnabled(true);
            ui->m_ledsSpinB->setValue(options->m_leds);
            break;
        case AnimationItem::Particls:
            ui->m_particlesLabel->setEnabled(true);
            ui->m_particlesSpinB->setEnabled(true);
            ui->m_particlesSpinB->setValue(options->m_leds);
            break;
        case AnimationItem::Delay:
            ui->m_delayLabel->setEnabled(true);
            ui->m_delaySpinB->setEnabled(true);
            ui->m_delaySpinB->setValue(options->m_delay);
            break;
        case AnimationItem::Iterations:
            ui->m_iterationsLabel->setEnabled(true);
            ui->m_iterationsSpinB->setEnabled(true);
            ui->m_iterationsSpinB->setValue(options->m_iteration);
            break;
        case AnimationItem::Invert:
            ui->m_invertCheckB->setEnabled(true);
            ui->m_invertCheckB->setChecked(options->m_invert);
            break;
        case AnimationItem::CenterStart:
            ui->m_invertCheckB->setEnabled(true);
            ui->m_invertCheckB->setChecked(options->m_invert);
            break;
        case AnimationItem::Text:
            ui->m_textLabel->setEnabled(true);
            ui->m_textLineE->setEnabled(true);
            ui->m_textLineE->setText(options->m_text);
            break;
        case AnimationItem::LedState:
            ui->m_ledStateCheckB->setEnabled(true);
            ui->m_ledStateCheckB->setChecked(options->m_state);
            break;
        default:
            switch ( 1 << i )
            {
            case AnimationItem::Speed:
                ui->m_speedLabel->setEnabled(false);
                ui->m_speedSpinB->setEnabled(false);
                break;
            case AnimationItem::Direction:
                ui->m_directionLabel->setEnabled(false);
                ui->m_directionComB->setEnabled(false);
                break;
            case AnimationItem::Axis:
                ui->m_axisLabel->setEnabled(false);
                ui->m_axisComB->setEnabled(false);
                break;
            case AnimationItem::Leds:
                ui->m_ledsLabel->setEnabled(false);
                ui->m_ledsSpinB->setEnabled(false);
                break;
            case AnimationItem::Particls:
                ui->m_particlesLabel->setEnabled(false);
                ui->m_particlesSpinB->setEnabled(false);
                break;
            case AnimationItem::Delay:
                ui->m_delayLabel->setEnabled(false);
                ui->m_delaySpinB->setEnabled(false);
                break;
            case AnimationItem::Iterations:
                ui->m_iterationsLabel->setEnabled(false);
                ui->m_iterationsSpinB->setEnabled(false);
                break;
            case AnimationItem::Invert:
                ui->m_invertCheckB->setEnabled(false);
                break;
            case AnimationItem::CenterStart:
                ui->m_invertCheckB->setEnabled(false);
                break;
            case AnimationItem::Text:
                ui->m_textLabel->setEnabled(false);
                ui->m_textLineE->setEnabled(false);
                break;
            case AnimationItem::LedState:
                ui->m_ledStateCheckB->setEnabled(false);
                break;
            default: /* Should never come here */
                break;
            }
            break;
        }
    }
    /*
     * We can now connect the widgets
     */
    connectAll();
    Q_EMIT updateUi();
}

/*!
 \brief

*/
void AnimationOptions::shouldeApplyNewOptions()
{
    if (isWindowModified()) {
        if (okToContinue() == QMessageBox::Ok)
            applyAnimationOptions();
        else
            setWindowModified(false);
    }
}

/*!
 \brief

 \return int
*/
int AnimationOptions::okToContinue()
{
    QMessageBox msgb;
    msgb.setText("Options have been modified!");
    msgb.setInformativeText("Do you want apply the new options?");
    msgb.setWindowTitle("Options Modified");
    msgb.setIcon(QMessageBox::Information);
    msgb.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
    msgb.setDefaultButton(QMessageBox::Ok);
    return msgb.exec();
}

void AnimationOptions::connectAll()
{
    void (AnimationOptions:: *slot1)() = &AnimationOptions::compareOldNewAnimationOptions; /* Resolve the overloaded function compareOldNewAnimationOptions */
    connect(ui->m_speedSpinB, &QSpinBox::editingFinished, this, slot1);
    connect(ui->m_delaySpinB, &QSpinBox::editingFinished, this,slot1);
    connect(ui->m_ledsSpinB, &QSpinBox::editingFinished, this, slot1);
    connect(ui->m_particlesSpinB, &QSpinBox::editingFinished, this, slot1);
    connect(ui->m_iterationsSpinB, &QSpinBox::editingFinished, this, slot1);

    /* Resolve functions before creating the connection */
    void (QComboBox:: *signal2)(int) = &QComboBox::currentIndexChanged;
    void (AnimationOptions:: *slot2)(int) = &AnimationOptions::compareOldNewAnimationOptions;
    connect(ui->m_axisComB, signal2,this, slot2);
    connect(ui->m_directionComB, signal2,this, slot2);

    connect(ui->m_invertCheckB, &QCheckBox::stateChanged, this, slot2);
    connect(ui->m_ledStateCheckB, &QCheckBox::stateChanged, this, slot2);

    connect(ui->m_textLineE, &QLineEdit::editingFinished, this, slot1);
}

void AnimationOptions::disconnectAll()
{
    void (AnimationOptions:: *slot1)() = &AnimationOptions::compareOldNewAnimationOptions; /* Resolve the overloaded function compareOldNewAnimationOptions */
    disconnect(ui->m_speedSpinB, &QSpinBox::editingFinished, this, slot1);
    disconnect(ui->m_delaySpinB, &QSpinBox::editingFinished, this,slot1);
    disconnect(ui->m_ledsSpinB, &QSpinBox::editingFinished, this, slot1);
    disconnect(ui->m_particlesSpinB, &QSpinBox::editingFinished, this, slot1);
    disconnect(ui->m_iterationsSpinB, &QSpinBox::editingFinished, this, slot1);

    /* Resolve functions before creating the connection */
    void (QComboBox:: *signal2)(int) = &QComboBox::currentIndexChanged;
    void (AnimationOptions:: *slot2)(int) = &AnimationOptions::compareOldNewAnimationOptions;
    disconnect(ui->m_axisComB, signal2,this, slot2);
    disconnect(ui->m_directionComB, signal2,this, slot2);

    disconnect(ui->m_invertCheckB, &QCheckBox::stateChanged, this, slot2);
    disconnect(ui->m_ledStateCheckB, &QCheckBox::stateChanged, this, slot2);

    disconnect(ui->m_textLineE, &QLineEdit::editingFinished, this, slot1);
}


/*!
 \brief

 \return u_int8_t
*/
void AnimationOptions::compareOldNewAnimationOptions()
{
    const AnimationItem::Options options = *m_animationToUpdate->getOptions();
    int modefied = 0;
    modefied = options.m_speed != ui->m_speedSpinB->value() ? 1 : 0;
    for (int i = 1; i < AnimationItem::TOTAL_ARGUMENTS && !modefied; i++) {
        switch ( (1 << i) & m_animationToUpdate->getAvailableAnimationOptions() )
        {
        case AnimationItem::Direction:
            modefied += options.m_direction != ui->m_directionComB->currentIndex() ? 1 : 0;
            break;
        case AnimationItem::Axis:
            modefied += options.m_axis != ui->m_axisComB->currentIndex() ? 1 : 0;
            break;
        case AnimationItem::Leds:
            modefied += options.m_leds != ui->m_ledsSpinB->value() ? 1 : 0;
            break;
        case AnimationItem::Particls:
            modefied += options.m_leds != ui->m_particlesSpinB->value() ? 1 : 0;
            break;
        case AnimationItem::Delay:
            modefied += options.m_delay != ui->m_delaySpinB->value() ? 1 : 0;
            break;
        case AnimationItem::Iterations:
            modefied += options.m_iteration != ui->m_iterationsSpinB->value() ? 1 : 0;
            break;
        case AnimationItem::Invert:
            modefied += options.m_invert != ui->m_invertCheckB->isChecked() ? 1 : 0;
            break;
        case AnimationItem::CenterStart:
            modefied += options.m_invert != ui->m_invertCheckB->isChecked() ? 1 : 0;
            break;
        case AnimationItem::Text:
            modefied += options.m_text.compare(ui->m_textLineE->text()) ? 1 : 0;
            break;
        case AnimationItem::LedState:
            modefied += options.m_state != ui->m_ledStateCheckB->isChecked() ? 1 : 0;
            break;
        default:
            break;
        }
    }
//    qDebug() << "Options Led State:" << options.m_state << "Current Led State: " <<  ui->m_ledStateCheckB->isChecked();
    if(modefied)
        setWindowModified(true);
    else
        setWindowModified(false);
}

void AnimationOptions::compareOldNewAnimationOptions(int )
{
    compareOldNewAnimationOptions();
}

