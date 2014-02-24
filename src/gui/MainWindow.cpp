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
#include "alt_key.hpp"
#include "aqp.hpp"
#include "DebugDockWidget.hpp"
#include "animations/Draw.hpp"
#include "animations/Lift.hpp"
#include "animations/Firework.hpp"
#include "animations/AxisNailWall.hpp"
#include "animations/WireBoxCenterShrinkGrow.hpp"
#include "animations/WireBoxCornerShrinkGrow.hpp"
#include "animations/Loadbar.hpp"
#include "animations/RandomFiller.hpp"
#include "animations/RandomSpark.hpp"
#include "animations/RandomSparkFlash.hpp"
#include "animations/RandomZLift.hpp"
#include "animations/Wall.hpp"
#include "animations/Rain.hpp"
#include "animations/StringFly.hpp"

#include <QMessageBox>
#include <QCloseEvent>
#include <QSettings>
#include <QMenuBar>
#include <QToolBar>
#include <QByteArray>
#include <QEventLoop>
#include <QSerialPort>
#include <QShortcut>

//#ifdef _DEBUG_
#include <QDebug>
//#endif

using namespace std;

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
//    serial(new QSerialPort),
    shortCutSA(new  QShortcut(QKeySequence(tr("Ctrl+A")),this)),
    createThread(new QThread)
{
    ui->setupUi(this);
#ifdef DEBUGWINDOW
    debugDockWidget = new DebugDockWidget(this);
    addDockWidget(Qt::RightDockWidgetArea,debugDockWidget,Qt::Vertical );
#endif
    //    currentAnimation = new Lift;
    readSettings ();
    createActions ();
    createToolbar ();
    playAction->setDisabled(true);
    pauseAction->setDisabled(true);
    setupAnimationItems();
    currentAnimation = animation.value("Wall");
    AQP::accelerateWidget (this);  //Give each button a accelerater

//    createThread = new QThread;
    qDebug()<< "Main Thread id: " << thread()->currentThread();
    connectSignals();
    timer.setSingleShot(false);
}

/**
 * @brief MainWindow::~MainWindow
 */
MainWindow::~MainWindow(void) //Deinit MainWindow
{
    foreach (Animation *a, animation)
        delete a;
    animation.clear();
    delete createThread;
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
void MainWindow::playNextAnimation(const QString &a)
{
    currentAnimation = animation.value(a);
    connect(createThread,&QThread::started,currentAnimation,&Animation::createAnimation);
    connect(currentAnimation, &Animation::done, createThread, &QThread::quit);
    //    connect(worker, SIGNAL(finished()), worker, SLOT(deleteLater()));
    //    connect(createThread , &QThread::finished, createThread, &QThread::deleteLater);
    connect(currentAnimation,&Animation::done,this,&MainWindow::animationDone);
//    qDebug()<< "Main thread id: " << thread()->currentThread();
    timer.start(2);
    createThread->start();

}

void MainWindow::playAnimations()
{
    static u_int8_t row;
    if(row < ui->animationPlaylistLW->count()){
        qDebug() << ui->animationPlaylistLW->item(row)->text();
        playNextAnimation(ui->animationPlaylistLW->item(row++)->text());
    }
    if(row >= ui->animationPlaylistLW->count())
        row = 0;
}

void MainWindow::animationDone()
{
    disconnect(createThread,&QThread::started,currentAnimation,&Animation::createAnimation);
    disconnect(currentAnimation, &Animation::done, createThread, &QThread::quit);
    disconnect(currentAnimation,&Animation::done,this,&MainWindow::animationDone);
    createThread->wait();
    Q_EMIT playAnimations();
//    timer.stop();
    //    createThread->wait();
    //    delete createThread;
}

void MainWindow::sendAnimation()
{
    serial.putChar(0xFF);
    serial.waitForBytesWritten(1000);
    serial.putChar(0x00);
    serial.waitForBytesWritten(1000);
    for (u_int8_t z = 0; z < CUBE_SIZE; z++) {
        for (u_int8_t y = 0; y < CUBE_SIZE; y++) {
            serial.putChar(currentAnimation->cubeFrame[z][y]);
            serial.waitForBytesWritten(1000);
            if(currentAnimation->cubeFrame[z][y] == 0xFF){
                serial.putChar(0xFF);
                serial.waitForBytesWritten(1000);
            }
        }
    }
    qDebug()<< "SendAnimation executed!!!";
}

void MainWindow::updateAnimation(const Draw::AnimationOptions *animationOptions)
{
    QList<QListWidgetItem*> items = ui->animationPlaylistLW->selectedItems();
    if(!items.isEmpty())
    {
        QListWidgetItem *item = items.first();
        QString aString =item->text();
        Animation *a = animation.value(aString);
        if(aString.compare("Lift") == 0){
            dynamic_cast<Lift*>(a)->setDelay(animationOptions->delay);
            dynamic_cast<Lift*>(a)->setIterations(animationOptions->iteration);
            dynamic_cast<Lift*>(a)->setSpeed(animationOptions->speed);
        }else if(aString.compare("Rain") == 0){
            dynamic_cast<Rain*>(a)->setSpeed(animationOptions->speed);
            dynamic_cast<Rain*>(a)->setIterations(animationOptions->iteration);
        }else if(aString.compare("String Fly") == 0){
            dynamic_cast<StringFly*>(a)->setSToDisplay(animationOptions->text);
            dynamic_cast<StringFly*>(a)->setSpeed(animationOptions->speed);
        }else if(aString.compare("Wall") == 0){
            dynamic_cast<Wall*>(a)->setSpeed(animationOptions->speed);
            dynamic_cast<Wall*>(a)->setAxis(animationOptions->axis);
            dynamic_cast<Wall*>(a)->setDirection(animationOptions->direction);
        }else if(aString.compare("Firework") == 0){
            dynamic_cast<Firework*>(a)->setSpeed(animationOptions->speed);
            dynamic_cast<Firework*>(a)->setParticles(animationOptions->particle);
            dynamic_cast<Firework*>(a)->setIterations(animationOptions->iteration);
        }else if(aString.compare("Random Spark Flash") == 0){
            dynamic_cast<RandomSparkFlash*>(a)->setSpeed(animationOptions->speed);
            dynamic_cast<RandomSparkFlash*>(a)->setIterations(animationOptions->iteration);
            dynamic_cast<RandomSparkFlash*>(a)->setLeds(animationOptions->leds);
        }else if(aString.compare("Random Spark") == 0){
            dynamic_cast<RandomSpark*>(a)->setSpeed(animationOptions->speed);
            dynamic_cast<RandomSpark*>(a)->setSparks(animationOptions->leds);
        }else if(aString.compare("Random Filler") == 0){
            dynamic_cast<RandomFiller*>(a)->setSpeed(animationOptions->speed);
            dynamic_cast<RandomFiller*>(a)->setState(animationOptions->state);
        }else if(aString.compare("Axis Nail Wall") == 0){
            dynamic_cast<AxisNailWall*>(a)->setSpeed(animationOptions->speed);
            dynamic_cast<AxisNailWall*>(a)->setAxis(animationOptions->axis);
            dynamic_cast<AxisNailWall*>(a)->setInvert(animationOptions->invert == 0 ? false : true);
        }else if(aString.compare("Loadbar") == 0){
            dynamic_cast<Loadbar*>(a)->setSpeed(animationOptions->speed);
            dynamic_cast<Loadbar*>(a)->setAxis(animationOptions->axis);
        }else if(aString.compare("Wire Box Center Shrink Grow") == 0){
            dynamic_cast<WireBoxCenterShrinkGrow*>(a)->setSpeed(animationOptions->speed);
            dynamic_cast<WireBoxCenterShrinkGrow*>(a)->setCenterStart(animationOptions->invert == 0 ? false : true);
        }else if(aString.compare("Wire Box Corner Shrink Grow") == 0){
            dynamic_cast<WireBoxCornerShrinkGrow*>(a)->setSpeed(animationOptions->speed);
        }else if(aString.compare("Random Z-Axis Lift") == 0){
            dynamic_cast<RandomZLift*>(a)->setSpeed(animationOptions->speed);
        }
        updateAnimationItemToolTip(aString,item);
    }
}

void MainWindow::updateAnimationItemToolTip(const QString &a, QListWidgetItem *item)
{
    if(item == Q_NULLPTR)
        return;
    QHash<QString,Animation*>::iterator iter = animation.find(a);
    QString itemToolTip = QString(tr("<p style='white-space:pre'><font color=#00FFFF><b>%1 Animation</b></font><br>"
                                     "Speed: %2<br>"))
            .arg(iter.key())
            .arg(iter.value()->getSpeed());
    if(a.compare("Lift") == 0){
        itemToolTip.append(QString("Delay: %1<br>"
                                   "Iterations: %2")
                           .arg(dynamic_cast<Lift*>(iter.value())->getDelay())
                           .arg(dynamic_cast<Lift*>(iter.value())->getIterations()));
    }else if(a.compare("String Fly") == 0){
        itemToolTip.append(QString("Current Text: "
                                   + dynamic_cast<StringFly*>(iter.value())->getSToDisplay().toLatin1()));
    }else if(a.compare("Random Spark Flash") == 0){
        itemToolTip.append(QString("LEDs: %1<br>"
                                   "Iterations: %2")
                           .arg(dynamic_cast<RandomSparkFlash*>(iter.value())->getLeds())
                           .arg(dynamic_cast<RandomSparkFlash*>(iter.value())->getIterations()));
    }else if(a.compare("Random Spark") == 0){
        itemToolTip.append(QString("Sparks: %1<br>")
                           .arg(dynamic_cast<RandomSpark*>(iter.value())->getSparks()));
    }else if(a.compare("Random Filler") == 0){
        QString tmp;

        tmp  = dynamic_cast<RandomFiller*>(iter.value())->getState() == Draw::ON ? "ON" : "OFF";

        itemToolTip.append(QString("Start State: " + tmp));
    }else if(a.compare("Loadbar") == 0){
        QString tmp;

        if(dynamic_cast<Loadbar*>(iter.value())->getAxis() == Draw::X_AXIS)
            tmp = "X-Axis";
        else if(dynamic_cast<Loadbar*>(iter.value())->getAxis() == Draw::Y_AXIS)
            tmp = "Y-Axis";
        else
            tmp = "Z-Axis";

        itemToolTip.append(QString("Axis: " + tmp));
    }else if(a.compare("Axis Nail Wall") == 0){
        QString tmp;

        if(dynamic_cast<AxisNailWall*>(iter.value())->getAxis() == Draw::X_AXIS)
            tmp = "X-Axis<br>";
        else if(dynamic_cast<AxisNailWall*>(iter.value())->getAxis() == Draw::Y_AXIS)
            tmp = "Y-Axis<br>";
        else
            tmp = "Z-Axis<br>";

        tmp.append(dynamic_cast<AxisNailWall*>(iter.value())->getInvert() ? "Invert: Yes" : "Invert: No");

        itemToolTip.append(QString("Axis: " + tmp));
    }else if(a.compare("Wire Box Center Shrink Grow") == 0){
        QString tmp;

        tmp  = dynamic_cast<WireBoxCenterShrinkGrow*>(iter.value())->getCenterStart() ? "YES" : "NO";

        itemToolTip.append(QString("Start in center: " + tmp));
    }else if(a.compare("Wire Box Corner Shrink Grow") == 0){
        itemToolTip.append(QString("Iterations: %1")
                           .arg(dynamic_cast<WireBoxCornerShrinkGrow*>(iter.value())->getIterations()));
    }else if(a.compare("Random Z-Axis Lift") == 0){
        itemToolTip.append(QString("Iterations: %1")
                           .arg(dynamic_cast<RandomZLift*>(iter.value())->getIterations()));
    }else if(a.compare("Rain") == 0){
        itemToolTip.append(QString("Iterations: %1")
                           .arg(dynamic_cast<Rain*>(iter.value())->getIterations()));
    }else if(a.compare("Wall") == 0){
        QString tmp;

        tmp = dynamic_cast<Wall*>(iter.value())->getDirection() == Draw::FORWARD ? "Direction: Forward<br>" : "Direction: Backward<br>";

        if(dynamic_cast<Wall*>(iter.value())->getAxis() == Draw::X_AXIS)
            tmp = "Axis: X-Axis";
        else if(dynamic_cast<Wall*>(iter.value())->getAxis() == Draw::Y_AXIS)
            tmp = "Axis: Y-Axis";
        else
            tmp = "Axis: Z-Axis";

        itemToolTip.append(tmp);
    }else if(a.compare("Firework") == 0){
        itemToolTip.append(QString("Iterations: %1<br>"
                                   "Particles: %2")
                           .arg(dynamic_cast<Firework*>(iter.value())->getIterations())
                           .arg(dynamic_cast<Firework*>(iter.value())->getParticles()));
    }
    item->setToolTip(itemToolTip);
}

void MainWindow::setupAnimationItems()
{
    animation.insert("Lift",new Lift);
    animation.insert("String Fly",new StringFly);
    animation.insert("Random Spark Flash",new RandomSparkFlash);
    animation.insert("Random Spark",new RandomSpark);
    animation.insert("Random Filler",new RandomFiller);
    animation.insert("Loadbar",new Loadbar);
    animation.insert("Axis Nail Wall",new AxisNailWall);
    animation.insert("Wire Box Center Shrink Grow",new WireBoxCenterShrinkGrow);
    animation.insert("Wire Box Corner Shrink Grow",new WireBoxCornerShrinkGrow);
    animation.insert("Random Z-Axis Lift",new RandomZLift);
    animation.insert("Rain",new Rain);
    animation.insert("Wall",new Wall);
    animation.insert("Firework",new Firework);

    QHash<QString,Animation*>::const_iterator iter = animation.constBegin();
    while(iter != animation.constEnd()){
        QListWidgetItem *item = new QListWidgetItem(iter.key(),ui->availableAnimationsLW);
        updateAnimationItemToolTip(iter.key(),item);
        iter.value()->moveToThread(createThread);
//        connect(iter.value(),&Animation::sendData,this,&MainWindow::sendAnimation);
        iter++;
    }
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
    connect(ui->animationAdjustGB,&AnimationOptionsGroupBox::optionsReady,
            this,&MainWindow::updateAnimation);
    connect(playAction,&QAction::triggered,this,&MainWindow::playAnimations);

    connect(&timer,&QTimer::timeout,this,&MainWindow::sendAnimation);

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
