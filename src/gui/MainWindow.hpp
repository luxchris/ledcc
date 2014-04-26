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
#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include <QMainWindow>
#include <SettingsDialog.hpp>
#include <QThread>
#include <Global.hpp>
#include <QTimer>


/*Forward deceleration*/
class QTimer;
class QShortcut;


    class AnimationHandler;

namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

protected:
    void closeEvent (QCloseEvent *);
public:
    explicit MainWindow(QWidget *parent = Q_NULLPTR);
    virtual ~MainWindow();
Q_SIGNALS:
private Q_SLOTS:
    void resizeEvent(QResizeEvent *e);
    void about(void);
    void setDirty() { setWindowModified ( true ); }
    void updateUi(bool portOpen);
    void updateAnimationActions();
private:
    bool okToContinue(void);
    void readSettings(void);
    void saveSettings(void);
    void connectSignals(void);
private:
//    friend class AnimationHandler;
    Ui::MainWindow *ui;

    QShortcut *m_focusAnimationList;
    QShortcut *m_focusAnimationPlaylist;
    QShortcut *m_scSellectAll;
    bool m_open; /* Port open or not */
    AnimationHandler *m_animationHandler;
    Q_DISABLE_COPY(MainWindow)
};

namespace Settings{
    const QString SMainWindowGeometrySettings("geometry");
    }



#endif // MAINWINDOW_H
