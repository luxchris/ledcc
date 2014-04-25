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
#ifndef ANIMATIONHANDLER_HPP
#define ANIMATIONHANDLER_HPP
#include "Options.hpp"
#include <QObject>
#include <QHash>
//#include "AnimationListWidget.hpp"
//#include "AnimationListWidget.hpp"
class AnimationPlayListWidget;
class Animation;
class AnimationItem;
class Sender;
class SettingsDialog;
class QAction;
    /*!
     \brief Class \a AnimationHandler interface between all animations

    */
class AnimationHandler : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool isPortOpen READ isPortOpen WRITE setIsPortOpen )
public:
    explicit AnimationHandler(QObject *object = Q_NULLPTR, QWidget *parent = Q_NULLPTR);
    virtual ~AnimationHandler();
    Sender * getSender() { return m_sender; }
    SettingsDialog *m_settingsDialog;

    bool isPortOpen() const
    {
        return m_isPortOpen;
    }

    void setAction(QAction *action, int i);
Q_SIGNALS:
    void startAnimation();
    void stopPlay();
    void updateUi();
    void okClosePort();
    void openSerialInterface();

public Q_SLOTS:
    void animationDone(void);
    void playAnimations(void);
    void openCloseSerialPort(void);
    void stopThreads(void);
    void closePort(const QString &message);
    void portOpen(const QString &message);
    void portClosed(const QString &message);
    void setIsPortOpen(bool arg)
    {
        m_isPortOpen = arg;
    }

private Q_SLOTS:
    void displayPortErrorMessage(const QString &message);

private:
    void setupSenderThread(void);
    void playNextAnimation(const AnimationItem *item);
private:
    QAction *m_playAction;
    QAction *m_stopAction;
    QThread *m_createThread;
    QThread *m_senderThread;
    Sender *m_sender;
    bool m_stopPlay;
    bool m_portOpen;
    AnimationItem *m_currentAnimationItem;
    Animation *m_currentAnimation;
    AnimationPlayListWidget *m_animationPlaylist;
    bool m_isPortOpen;
    Q_DISABLE_COPY(AnimationHandler)
};
#endif // ANIMATIONHANDLER_HPP