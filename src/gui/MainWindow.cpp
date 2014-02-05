/*************************************************************
 * 	File Name       : mainwindows.cpp
 *************************************************************
 *  Created on	 : Jan 30, 2013
 *  Author 		 : Christian Schwarzgruber
 *  Version		 : 1.0
 *************************************************************
 *  Description	 : MainWindow
 *************************************************************/

#include "MainWindow.hpp"
#include "ui_MainWindow.h"
//#include "adddialog.hpp"
#include "SettingsDialog.hpp"
#include "./aqp/alt_key.hpp"
#include "./aqp/aqp.hpp"
#include "DebugDockWidget.hpp"
#include "AnimationThread.hpp"
#include "Animations.hpp"
#include "SendThread.hpp"
#include "Global.hpp"


//#include <QListWidget>
#include <QMessageBox>
#include <QTreeWidgetItem>
#include <QModelIndex>
#include <QCloseEvent>
#include <QFileDialog>
#include <QSettings>
#include <QMenu>
#include <QMenuBar>
#include <QToolBar>
#include <QByteArray>
#include <QEventLoop>
#include <QSerialPort>
#include <QMultiMap>
#include <iostream>
#include <QShortcut>
//#ifdef _DEBUG_
#include <QDebug>
//#endif
using namespace std;
#define ANIMATIONS 12
// TODO:: Write cleanUp

namespace{
    const QString GeometrySettings("geometry");

    }



/**
 * @brief MainWindow::MainWindow
 * @param parent
 */
MainWindow::MainWindow(QWidget *parent) :  //Init MainWindow
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    sdialog(new SettingsDialog),
//    playList(new QHash<QString,AnimationStruct>),
//    alist(new QHash<QString,AnimationStruct>),
    serial(new QSerialPort),
    shortCutSA(new  QShortcut(QKeySequence(tr("Ctrl+A")),this))
{
    qRegisterMetaType<CubeArray>("CubeArray");
    qRegisterMetaType<AnimationStruct>("AnimationStruct");
    ui->setupUi(this);
#ifdef DEBUGWINDOW
    debugDockWidget = new DebugDockWidget(this);
    addDockWidget(Qt::RightDockWidgetArea,debugDockWidget,Qt::Vertical );
#endif
    readSettings ();
    createActions ();
    createToolbar ();
    connectSignals();
    setupAnimationList();
    playAction->setDisabled(true);
    pauseAction->setDisabled(true);
    AQP::accelerateWidget (this);  //Give each button a accelerater
}

/**
 * @brief MainWindow::~MainWindow
 */
MainWindow::~MainWindow(void) //Deinit MainWindow
{
    delete ui;
}

/**
 * @brief MainWindow::closeEvent
 * @param event
 */
void MainWindow::closeEvent( QCloseEvent *event ) {  //Close application
    if ( okToContinue() ) {
        if(serial.isOpen())
            serial.close();
        saveSettings ();
        close ();
        sdialog->close();
        event->accept();
    }
    else
        event->ignore();
}

/**
 * @brief MainWindow::okToContinue
 * @return
 */
bool MainWindow::okToContinue(void) { //Check if window is not modified
    if(isWindowModified ()){
        int r = QMessageBox::warning( this, tr( "3D-LED Cube" ),
                                      tr( "Do you really want to exit the programm?" ),
                                      QMessageBox::Yes | QMessageBox::Default,
                                      QMessageBox::Cancel | QMessageBox::Escape );

        if ( r == QMessageBox::Yes )
            return true;
        else if ( r == QMessageBox::Cancel )
            return false;
        return true;
    }
    return true;
}
/**
 * @brief MainWindow::resizeEvent
 * @param e
 */
void MainWindow::resizeEvent(QResizeEvent *e){ //Resize Window
    QMainWindow::resizeEvent(e);
}
/**
 * @brief MainWindow::saveSettings
 */
void MainWindow::saveSettings(void){  //Save geometry of application
    QSettings settings("Schwarz Software Inc.","3D-LED Cube");
    settings.setValue (GeometrySettings,saveGeometry ());
}
/**
 * @brief MainWindow::readSettings
 */
void MainWindow::readSettings (void){ //Load geometry of application
    QSettings settings("Schwarz Software Inc.","3D-LED Cube");
    restoreGeometry (settings.value (GeometrySettings).toByteArray ());
}

/**
 * @author  Christian Schwarzgruber
 * @brief   MainWindow::updateUi gets called when the serial port gets opened or closed
 *          and when the animation playlist gets modified.
 *          Disables/Enables the playbutton and changes the Seriel connect button appropriated
 */
void MainWindow::updateUi(void) // Update Button state
{
    if(serial.isOpen()){
        if(openPortAction->text() == "Open port"){
            openPortAction->setText(tr("Close port"));
            openPortAction->setIcon( QIcon( "://images/disconnect.png"));
            openPortAction->setToolTip(tr("Disconnect from seriell device  O"));
        }
        if(ui->animationPlaylistLW->count())
            playAction->setEnabled(true);
        else
            playAction->setDisabled(true);
    }else
    {
        if(openPortAction->text() == "Close port"){
            openPortAction->setText(tr("Open port"));
            openPortAction->setIcon( QIcon( "://images/connect.png"));
            openPortAction->setToolTip(tr("Connect to seriell device  O"));
        }
        playAction->setDisabled(true);
    }
}
/**
 * @brief MainWindow::playAnimationFromList
 */
void MainWindow::playNextAnimation(void)
{
    currentAnimation = ui->animationPlaylistLW->getNextAnimation();
    emit startAnimation(currentAnimation);
}
/**
 * @brief MainWindow::setupAnimationList
 */
void MainWindow::setupAnimationList()
{
    QHash<QString,AnimationStruct> alist;
    AnimationStruct animation;
    animation.name = QString("Firework");
    animation.id = 1;
    animation.speed = 30;
    animation.particle = 20;
    animation.iteration = 10;
    alist.insert(animation.name,animation);
    animation.name = QString("Lift");
    animation.iteration = 5;
    animation.speed = 100;
    animation.delay = 200;
    alist.insert(animation.name,animation);
    animation.name = QString("Wall");
    animation.axis = Y_AXIS;
    animation.direction = FORWARD;
    animation.speed = 50;
    alist.insert(animation.name,animation);
    animation.name = QString("Rain");
    animation.speed = 50;
    animation.iteration = 20;
    alist.insert(animation.name,animation);
    animation.name = QString("Random Z-Axis Lift");
    animation.iteration = 10;
    animation.speed = 40;
    alist.insert(animation.name,animation);
    animation.name = QString("Wire Box Corner Shrink Grow");
    animation.iteration = 8;
    animation.speed = 50;
    alist.insert(animation.name,animation);
    animation.name = QString("Wire Box Center Shrink Grow");
    animation.speed = 50;
    animation.invert = true;
    alist.insert(animation.name,animation);
    animation.name = QString("Axis Nail Wall");
    animation.speed = 50;
    animation.axis = X_AXIS;
    animation.invert = true;
    alist.insert(animation.name,animation);
    animation.name= QString("Loadbar");
    animation.id = 1;
    animation.speed = 20;
    animation.axis = Z_AXIS;
    animation.iteration = 10;
    alist.insert(animation.name,animation);
    animation.name = QString("Random Spark Flash");
    animation.iteration = 5;
    animation.speed =50;
    animation.leds = 20;
    alist.insert(animation.name,animation);
    animation.name = QString("Random Spark");
    animation.leds = 50;
    animation.speed = 20;
    alist.insert(animation.name,animation);
    animation.name = QString("Random Filler");
    animation.speed = 40;
    animation.state = ON;
    alist.insert(animation.name,animation);
    animation.name = QString("String Fly");
    animation.text = QString("LED CUBE");
    animation.speed = 80;
    alist.insert(animation.name,animation);
    QHash<QString,AnimationStruct>::const_iterator i;
    for(i = alist.constBegin(); i!= alist.constEnd(); i++)
        ui->availableAnimationsLW->addItem(i.key());
    ui->availableAnimationsLW->setAnimationList(alist);

}



void MainWindow::openCloseSerialPort(void)  // Open the Serial port
{
    m_port = sdialog->settings();

    if(!serial.isOpen()){ // Get the status of the Serial port
        bool result = openSerialPort();
        if(result)
        {
            ui->statusbar->showMessage (tr("Connected to %1 : %2, %3, %4, %5, %6")
                                        .arg (m_port.name).arg (m_port.stringBaudRate)
                                        .arg (m_port.stringDataBits).arg (m_port.stringParity)
                                        .arg (m_port.stringStopBits).arg (m_port.stringFlowControl));
        }else
        {
            serial.close();
            QMessageBox::warning (this,tr("Error"),
                                  tr("Can't open serial port: %1 - error code: %2\n\n\n"
                                     "Check if device is connected properly!")
                                  .arg (m_port.name).arg (serial.error ()));
            ui->statusbar->showMessage(tr("Open error"),3000);
        }
    }
    else{
        int flag = QMessageBox::information (this,tr("Closing port")
                                             ,tr("Do you really want close the serial port?\n %1")
                                             .arg(m_port.name),QMessageBox::Ok,QMessageBox::Cancel);
        if(flag == QMessageBox::Ok)
            closeSerialPort();

    }
    updateUi ();
}
/**
 * @brief MainWindow::checkPortSettings
 * @return
 */
bool MainWindow::checkPortSettings(void)
{
    if( serial.setBaudRate (m_port.baudRate) && serial.setDataBits (m_port.dataBits)
            && serial.setParity (m_port.parity) && serial.setStopBits (m_port.stopBits)
            && serial.setFlowControl (m_port.flowControl))
    {
        return true;
    }else
    {
        QMessageBox::critical (this,tr("Error"),
                               tr("Can't configure the serial port: %1,\n"
                                  "error code: %2")
                               .arg (m_port.name).arg (serial.error ()));
        return false;
    }
}
/**
 * @brief MainWindow::closeSerialPort
 */
void MainWindow::closeSerialPort(void)
{
    serial.close();
    ui->statusbar->showMessage(tr("Port closed: %1").arg (m_port.name),3000);
}
/**
 * @brief MainWindow::openSerialPort
 * @return
 */
bool MainWindow::openSerialPort(void)
{
    bool result;

    serial.setPortName (m_port.name);

    if(serial.open(QIODevice::ReadWrite))
    {
        result = checkPortSettings();

    }else
    {
        result = false;
    }
    return result;
}
/**
 * @brief MainWindow::readData
 */
void MainWindow::readData()
{
    //        setData (serial->readAll ());
#ifdef DEBUGWINDOW
    debugDockWidget->addLine("Received: " + QString(m_data) + "\n" );
#endif
}

#ifdef DEBUGWINDOW
void MainWindow::sendData()
{
    int i = debugDockWidget->sendString().toInt();
    debugDockWidget->addLine(QString("Sent: %1").arg(i));
    writeData(i);
}
void MainWindow::writeData(const char c) //Function to write data to serial port
{
    qDebug() << "Bytes to write: " + c;
    if(serial->putChar(c))
        qDebug() << "Send successfully";
}
#endif

/**
 * @brief
 *
 */
void MainWindow::connectSignals(void) //Connect Signals
{
    connect (openPortAction,&QAction::triggered,
             this,&MainWindow::openCloseSerialPort);
    connect(quitAction,&QAction::triggered,
            this,&MainWindow::close);
//    connect (quitAction,&QAction::triggered,
//             this,&MainWindow::clearToolButtonClicked);
    connect (aboutAction,&QAction::triggered,
             this,&MainWindow::about);
    connect(clearAction,&QAction::triggered,
            ui->animationPlaylistLW,&AnimationPlayListWidget::clearList);
    connect(settingAction,&QAction::triggered,
            sdialog,&QWidget::show);
#ifdef DEBUGWINDOW
    connect(debugDockWidget,&DebugDockWidget::sendStringChanged,
            this,&MainWindow::sendData);
#endif
    connect(ui->animationPlaylistLW,&AnimationPlayListWidget::updateUi,
            this,&MainWindow::updateUi);
    connect(ui->availableAnimationsLW,&AnimationListWidget::itemsSelected,
            ui->animationPlaylistLW,&AnimationPlayListWidget::newItem);
    connect(shortCutSA,&QShortcut::activated,
            ui->animationPlaylistLW,&AnimationPlayListWidget::selectAllItems);
    connect(shortCutSA,&QShortcut::activated,
            ui->availableAnimationsLW,&AnimationListWidget::selectAllItems);
}

/**
 * @brief
 *
 */
void MainWindow::createActions(void)  // Creat action for the toolbar
{
    quitAction = new QAction( tr( "Quit Ctrl+Q" ), this );
    quitAction->setIcon( QIcon( "://images/application-exit.png" ) );
    quitAction->setShortcut( QKeySequence::Quit );
    quitAction->setStatusTip( tr( "Exit application" ) );

    clearAction = new QAction( tr( "Clear F5" ), this );
    clearAction->setIcon( QIcon( "://images/clear.png" ) );
    clearAction->setShortcut( QKeySequence::Refresh );
    clearAction->setStatusTip( tr( "Clear item list" ) );

    aboutAction = new QAction( tr( "About" ), this );
    aboutAction->setIcon( QIcon( "://images/help-about.png" ) );
//    aboutAction->setShortcut(tr( "Ctrl+A" ));
    aboutAction->setStatusTip( tr( "About Kitchen Scale" ) );

    settingAction = new QAction( tr( "Settings Ctrl+I" ), this );
    settingAction->setIcon( QIcon( "://images/package_settings.png" ) );
    settingAction->setShortcut(tr( "Ctrl+I" ));
    settingAction->setStatusTip( tr( "Configure Kitchen Scale" ) );

    playAction = new QAction( tr("Play Animation R"),this);
    playAction->setIcon(QIcon("://images/media-playback-start-9.png"));
    playAction->setShortcut(tr("R"));
    playAction->setToolTip(tr("Play Animation R"));

    pauseAction = new QAction( tr("Pause Animations P"),this);
    pauseAction->setIcon(QIcon("://images/media-playback-pause-7.png"));
    pauseAction->setShortcut(tr("P"));
    pauseAction->setToolTip(tr("Pause Animations P"));

    openPortAction = new QAction( tr("Open port"), this);
    openPortAction->setIcon( QIcon( "://images/connect.png"));
    openPortAction->setShortcut(tr("O"));
    openPortAction->setToolTip(tr("Connect to seriell device  O"));
}

/**
 * @brief
 *
 */
void MainWindow::createToolbar()
{
    QSize size(32,32);

    mainToolBar = new QToolBar();

    mainToolBar->setObjectName ("Main Toolbar");
    mainToolBar->addAction(quitAction);
    mainToolBar->addSeparator();

    mainToolBar->addAction(openPortAction);
    mainToolBar->addSeparator();

    mainToolBar->addAction(playAction);
    mainToolBar->addAction(pauseAction);
    mainToolBar->addSeparator();

    mainToolBar->addAction (clearAction);
    mainToolBar->addSeparator ();

    mainToolBar->addAction(settingAction);

    mainToolBar->setIconSize (size);
    mainToolBar->setWindowTitle("Main Toolbar");
    this->addToolBar (mainToolBar);

    helpToolBar = new QToolBar( );
    helpToolBar->setObjectName ("Help Toolbar");
    helpToolBar->setLayoutDirection (Qt::RightToLeft);
    helpToolBar->addAction (aboutAction);
    helpToolBar->setIconSize (size);
    helpToolBar->setWindowTitle("Help Toolbar");
    this->addToolBar (Qt::TopToolBarArea,helpToolBar);
    this->helpToolBar->setObjectName ("Hello");
}

/**
 * @brief
 *
 */
void MainWindow::about()
{
    QMessageBox::about(this, tr("About 3D-LED Cube"),
                       tr("<h2> 3D-LED Cube v0.1</h2>"
                          "<p> Copyright &copy; 2014 Christian Schwarzgruber"
                          "<p>The <b>3D-LED Cube</b> program was part of my thesis."
                          "This program lets you rearange the animation in the order you like it, you can even adjust speed,"
                          "delay, iterations and much more."));
}

void MainWindow::on_applyPB_clicked()
{
    qDebug() << "Apply clicked";
}
