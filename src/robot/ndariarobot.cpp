/***************************************************************************
 *   Copyright (C) 2004 by jose manuel                                     *
 *   jose@pcjose                                                           *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#include <QString>
#include <QMouseEvent>
#include <QMessageBox>
#include <Aria.h>
#include <ArRobot.h>

#include <qewdialogfactory.h> 
#include <QDateTime>
#include <QElapsedTimer>

#include "../slam/scanmatchingalgo.h"
#include "gui/ndariarobotkineticstab.h"
#include "gui/robotcommandsdialog.h"
#include "ndariarobot.h"

#include <ArNetworking.h>

NDAriaRobot::NDAriaRobot(int n_id, QObject* parent, QString name) :
    Robot(n_id, parent, name) //,advanceCB(this, &NDAriaRobot::advance)
{
    client = 0;
    clientConnector = 0;
    advance_period = 100;
    stopping = false;
    //   realRobot = false; // MobileSim
    velUnitsConversion = 10.0;
    diffConvFactor = 1.0; // 2.0 para Amigo

}

NDAriaRobot::~NDAriaRobot()
{
    if(client) {
        if(client->isConnected())
            client->disconnect();
        //     delete client;
    }
    if(clientConnector)
        delete clientConnector;
    Aria::shutdown();
}

void NDAriaRobot::setAriaRobot(ArClientBase* r_client,
                               ArClientSimpleConnector* r_clientConnector, bool realRobot)
{
    this->realRobot = realRobot;
    client = r_client;
    clientConnector = r_clientConnector;
    client->addHandler("update", new ArFunctor1C<NDAriaRobot,
                       ArNetPacket*>(this, &NDAriaRobot::handleRobotUpdate));
    client->addHandler("extUpdate", new ArFunctor1C<NDAriaRobot,
                       ArNetPacket*>(this, &NDAriaRobot::handleRobotExtUpdate));
    client->addHandler("sensorsList", new ArFunctor1C<NDAriaRobot,
                       ArNetPacket*>(this, &NDAriaRobot::handleSensorsList));

    client->runAsync();
    client->requestOnce("update");
    client->requestOnce("extUpdate");
    client->requestOnce("sensorsList");
    actualSensorsList = readActualSensorList();

    robot_info.setClient(r_client);
}

//      *  <li>Status (Null-terminated string)</li>
//      *  <li>Mode (Null-terminated string)</li>
//      *  <li>Battery voltage (times 10) (2 Byte integer)</li>
//      *  <li>X position (4 byte floating point number)</li>
//      *  <li>Y position (4 byte floating point number)</li>
//      *  <li>Theta (2-byte floating point number)</li>
//      *  <li>Forward velocity (2-byte number)</li>
//      *  <li>Rotation velocity (2-byte number)</li>

void NDAriaRobot::handleRobotUpdate(ArNetPacket* packet)
{

    robot_info.readPacket(packet);
    //   x = NDMath::roundInt(robot_info.getXRobot());
    //   y = NDMath::roundInt(robot_info.getYRobot());
    //   init_rot = robot_info.getHeadingRobot();
    vlin = robot_info.getLinVelRobot()/velUnitsConversion;
    vrad = robot_info.getRadVelRobot()/velUnitsConversion;
    //   while(init_rot < 0.0) init_rot += 360.0;
    //   while(init_rot >= 360.0) init_rot -= 360.0;
    //
    //   if(mainMotor) {
    //     mainMotor->getRawOdometryPointer()->setCoordinates(robot_info.getXRobot(), robot_info.getYRobot(), robot_info.getHeadingRobot() / NDMath::RAD2GRAD);
    // //     cout << "RAW_RAW " << mainMotor->getRawOdometry() << endl;
    //
    //     double dx = robot_info.getXRobot() - prevX;
    //     double dy = robot_info.getYRobot() - prevY;
    //     double dth = robot_info.getHeadingRobot() - prevHead;
    //     double slamX = mainMotor->getSlamOdometry().getPosition().at(0);
    //     double slamY = mainMotor->getSlamOdometry().getPosition().at(1);
    //     double slamHead = mainMotor->getSlamOdometry().getOrientation().at(0);
    //
    //     prevX = robot_info.getXRobot();
    //     prevY = robot_info.getYRobot();
    //     prevHead = robot_info.getHeadingRobot();
    //
    //     mainMotor->getSlamOdometryPointer()->setCoordinates(slamX + dx, slamY + dy, slamHead + dth / NDMath::RAD2GRAD );
    //
    // //     cout << "SLAM_SLAM " << mainMotor->getSlamOdometry() << endl;
    //   }

}

void NDAriaRobot::handleRobotExtUpdate(ArNetPacket* packet)
{

    robot_info.readExtPacket(packet);
    //   setVel2(robot_info.getRightVelRobot(), robot_info.getLeftVelRobot());
    x = NDMath::roundInt(robot_info.getXEncoder());
    y = NDMath::roundInt(robot_info.getYEncoder());
    init_rot = robot_info.getHeadingEncoder();

    //  QPointF newOdom(x, y);
    //  NDMath::selfRotateGrad(newOdom, rotInit, QPointF());
    //  x = NDMath::roundInt(newOdom.x()) + xInit;
    //  y = NDMath::roundInt(newOdom.y()) + yInit;
    //  init_rot = robot_info.getHeadingEncoder() + rotInit;
    timePoseTaken.fromArTime(robot_info.getTimeTaken());
    //   vlin = robot_info.getLinVelRobot()/velUnitsConversion;
    //   vrad = robot_info.getRadVelRobot()/velUnitsConversion;
    //   while(init_rot < 0.0) init_rot += 360.0;
    //   while(init_rot >= 360.0) init_rot -= 360.0;

    if(mainMotor) {
        mainMotor->getRawOdometryPointer()->setCoordinates(x, y, init_rot / NDMath::RAD2GRAD);

        //     char s[80];
        //     sprintf(s,"RAW_RAW Taken %lu.%03lu   Get %lu.%03lu", timePoseTaken.seconds(), timePoseTaken.microseconds()/1000, robot_info.getTimeGet().getSec(), robot_info.getTimeGet().getMSec());
        //     cout << s << " " << mainMotor->getRawOdometry() << endl;

        double dx = x - prevX;
        double dy = y - prevY;
        double dth = init_rot - prevHead;
        double slamX = mainMotor->getSlamOdometry().getPosition().at(0);
        double slamY = mainMotor->getSlamOdometry().getPosition().at(1);
        double slamHead = mainMotor->getSlamOdometry().getOrientation().at(0);

        prevX = x;
        prevY = y;
        prevHead = init_rot;

        mainMotor->getSlamOdometryPointer()->setCoordinates(slamX + dx, slamY + dy,
                                                            ScanMatchingAlgo::norm_a(slamHead + dth / NDMath::RAD2GRAD) );

        //    cout << "SLAM_SLAM " << mainMotor->getSlamOdometry() << endl;
    }
}

void NDAriaRobot::handleSensorsList(ArNetPacket* packet)
{
    robot_info.readSensorsList(packet);
}

void NDAriaRobot::setXY(double x_, double y_)
{
    QPointF newOdom(x_, y_);
    NDMath::selfRotateGrad(newOdom, rotInit, QPointF());
    x_ = NDMath::roundInt(newOdom.x()) + xInit;
    y_ = NDMath::roundInt(newOdom.y()) + yInit;

    double x_sprite = world->xTransform(x_);
    double y_sprite = world->yTransform(y_);
    double mov_x = x_sprite - robot_sprite->x();
    double mov_y = y_sprite - robot_sprite->y();
    robot_sprite->setX(x_sprite, false); // false para que no borre el path
    robot_sprite->setY(y_sprite, false);
    sensorsSet->moveBy(mov_x, mov_y);
}

void NDAriaRobot::rotation(double )
{
    double rot_sprite = world->headingTransform(init_rot + rotInit);
    robot_sprite->setRotation(rot_sprite);
    sensorsSet->rotate();
}

void NDAriaRobot::initSim(bool reset)
{
    initSensors();
    robot_sprite->clearPath(reset);
}

void NDAriaRobot::setStatus(bool on)
{

    if(status_ == on) return;
    status_ = on;
    if(status_) {

        ArNetPacket packet;
        //     ArUtil::sleep(20000);
        /*    packet.strToBuf("28 1"); // enable sonar
    client->requestOnce("MicroControllerCommand", &packet);
    client->requestOnce("reactive_wander");
    client->requestOnce("safeDrivingDisable");
    packet.empty();
    packet.strToBuf("12 -135");*/
        client->request("update", 100);
        client->request("extUpdate", 100);

        // para felix videos, quitar
        client->requestOnce("reactive_wander"); // activar

        //     ArUtil::sleep(10000);
        //     client->request("outputMotors", 100);
        //     client->requestOnce("wander");
        //     client->addCycleCallback(&advanceCB);
    }
    else {
        client->requestStop("update");
        client->requestStop("extUpdate");
        client->requestStop("MicroControllerCommand");
        // para felix videos, quitar
        client->requestOnce("reactive_wander");// desactivar

        //     client->requestStop("outputMotors");
        //     client->remCycleCallback(&advanceCB);
    }

}

bool NDAriaRobot::okPosition(void)
{
    return true;
}

const QString NDAriaRobot::type(void)
{
    return "AriaRobot";
}

QList< QewExtensibleDialog* > NDAriaRobot::createKineticsDialog(QewExtensibleDialog* parent)
{
    QList<QewExtensibleDialog*> add_dialogs;
    NDAriaRobotKineticsTab* robkin_tab = new NDAriaRobotKineticsTab(parent);
    RobotCommandsDialog* commands_tab = new RobotCommandsDialog(parent);
    if(robkin_tab->setUpDialog()) {
        robkin_tab->init(this);
        add_dialogs.append(robkin_tab);
    }
    else
        delete robkin_tab;

    if(commands_tab->setUpDialog()) {
        commands_tab->init(this);
        add_dialogs.append(commands_tab);
    }
    else
        delete commands_tab;

    return add_dialogs;

}

void NDAriaRobot::advance(int stage)
{
    if(stage == 0) {
        // para felix videos, quitar
        setVel2(rvel, lvel);
    }
    else if(stage == 1) {
        //     setX(x);
        //     setY(y);

        setXY(x, y);
        rotation(init_rot);
        robot_sprite->updatePath();
        recordRobot(3, 1, 4, 1, 5, 9, false); // no se usan los parametros
    }
}

void NDAriaRobot::initMotors(void)
{
    Robot::initMotors();
    ArNetPacket packet;
    packet.strToBuf("28 1"); // enciende sonar
    // para felix videos, quitar
    client->requestOnce("MicroControllerCommand", &packet);
}

void NDAriaRobot::initRecord(void)
{
    emit velMonitor(vlin, -1);
    emit rotMonitor(vrad, -1);
    emit sendToRecorder(x, 1);
    emit sendToRecorder(y, 2);
    emit sendToRecorder(init_rot, 3);
    emit sendToRecorder(vlin, 4);
    emit sendToRecorder(vrad, 5);
    emit sendToRecorder(lvel/diffConvFactor, 6);
    emit sendToRecorder(rvel/diffConvFactor, 7);
}

void NDAriaRobot::outputMotors(void)
{
    if(!stopping) {
        //     lvel = max_vel/robot_scale*(motors_output[0] - motors_output[1])*1;
        //     rvel = max_vel/robot_scale*(motors_output[3] - motors_output[2]);

        //     if(driving)
        //       drive();
        //     BaseSimControl* motorcontrol = motorControls.at(0);
        //     motorcontrol->sampleInputs();
        //     motorcontrol->sendOutputs();
        //
        //     QString ctrl_name = motorControls.at(0)->getControlName();
        //     AbstractMotor* motor = motors[ctrl_name.remove("Control")];
        lvel = mainMotor->getRealVel1();
        rvel = mainMotor->getRealVel2();
        //     lvel = max_vel*(motors_output[0] - motors_output[1]);
        //     rvel = max_vel*(motors_output[3] - motors_output[2]);
        old_right_vel = rvel;
        old_left_vel = lvel;


        // para felix videos, quitar
        //     if(realRobot) // Ñapa por que el comando de microcontrolador
        //                   // VEL2 de Aria gira bien en MobileSim
        //                   // y mal con los robots reales
        // Ya arreglado
        robot_info.setVel2(NDMath::roundInt(lvel/diffConvFactor), NDMath::roundInt(rvel/diffConvFactor));
        //     else
        //       robot_info.setVel2(NDMath::roundInt(rvel/diffConvFactor), NDMath::roundInt(lvel/diffConvFactor));

    }
    updateOdometries();
}

void NDAriaRobot::save_special(QDomElement& e)
{
    Robot::save_special(e);
}

void NDAriaRobot::sensorsVisible(bool on)
{
    Robot::sensorsVisible(on);
}

void NDAriaRobot::setCanvas(QGraphicsScene* w_canvas)
{
    Robot::setCanvas(w_canvas);
}

void NDAriaRobot::setIntrinsicVars(const QDomElement& e_size)
{
    Robot::setIntrinsicVars(e_size);
}

void NDAriaRobot::setVel2(double r_vel, double l_vel)
{
    rvel = r_vel;
    lvel = l_vel;
    emit rVel(robot_info.getRightVelRobot()); // 10 mm = 1 cm
    emit lVel(robot_info.getLeftVelRobot());
}

bool NDAriaRobot::stop(void)
{
    //   ArNetPacket packet;
    //   if(!stopping) {
    //     stopping = true;
    //     packet.strToBuf("32 0 0");
    //     client->requestOnce("MicroControllerCommand", &packet);
    //     client->requestOnce("stop");
    //     status_ = false;
    //   }
    //   stopping = (QString(robot_info.getRobotStatus()) != "Stopped");
    //   status_ = stopping;
    //   return !stopping;
    //
    ArNetPacket packet;
    //   client->requestStop("MicroControllerCommand");
    //   packet.strToBuf("32 0 0");
    //   client->requestOnce("MicroControllerCommand", &packet);
    robot_info.stop();
    stopping = !((vlin > -2) && (vlin < 2) && (vrad > -2) && (vrad < 2));
    if(!stopping)
        packet.strToBuf("28 0");
    //   status_ = stopping;

    x = world->inverseXTransform(robot_sprite->x()); // inicial
    y = world->inverseXTransform(robot_sprite->y());
    
    return !stopping;
}

void NDAriaRobot::viewSensors(bool show)
{
    Robot::viewSensors(show);
}

void NDAriaRobot::write_special(QDomDocument& doc, QDomElement& e)
{
    Robot::write_special(doc, e);
}

void NDAriaRobot::setOldVel(double r_vel, double l_vel)
{
    Robot::setOldVel(r_vel, l_vel);
}

void NDAriaRobot::getMouseEvent(QMouseEvent* e)
{
    //   if(status_) {
    //     e->ignore();
    //     return;
    //   }
    //   if(e->type() == QEvent::MouseButtonPress) {
    //     if(e->button() == Qt::LeftButton) {
    //       gotoPose(e->x(), e->y());
    //     }
    //     else
    //       Robot::getMouseEvent(e);
    //   }
    //   else
    //     e->ignore();
    Robot::getMouseEvent(e);
}

void NDAriaRobot::recordRobot(double , double , double , double , double , double , bool )
{
    if(is_monitoring) {
        emit velMonitor(vlin, -1);
        emit rotMonitor(vrad, -1);
    }
    if(is_recording) {
        emit sendToRecorder(x, 1);
        emit sendToRecorder(y, 2);
        emit sendToRecorder(init_rot, 3);
        emit sendToRecorder(vlin, 4);
        emit sendToRecorder(vrad, 5);

        emit sendToRecorder(lvel/diffConvFactor, 6);
        emit sendToRecorder(rvel/diffConvFactor, 7);
    }
}

void NDAriaRobot::setXYInit(double x, double y)
{
    xInit = x;
    yInit = y;
}

void NDAriaRobot::setRotInit(double rot)
{

    while(rot < 0.0) rot += 360.0;
    while(rot > 360.0) rot -= 360.0;
    rotInit = rot;
    rot = world->headingTransform(rotInit);
    robot_sprite->setRotation(rot);
    sensorsSet->rotate();
}


double NDAriaRobot::getY(void)
{
    return double(y);
}

double  NDAriaRobot::getX(void)
{
    return double(x);
}

double NDAriaRobot::getRotation(void)
{
    return init_rot;
}

bool NDAriaRobot::hasSensorType(int type)
{
//    QTime time;
//    time.start();
//    while(!robot_info.isSensorsListReaded() || time.elapsed() > 1000) {};
    QElapsedTimer time;
    time.start();
    while(!robot_info.isSensorsListReaded() || time.elapsed() > 1000) {};
    if(robot_info.isSensorsListReaded())
        return robot_info.hasSensorType(type);
    else
//        QMessageBox::information(nullptr, tr("NDAriaRobot::hasSensorType()"), tr("Cannot read sensors list"), tr("&Ok"));
        QMessageBox::critical(nullptr, tr("NDAriaRobot::hasSensorType()"), tr("Cannot read sensors list"), QMessageBox::Ok);
    return false;
}

list< int > * NDAriaRobot::readActualSensorList() {

//    QTime time;
//    time.start();
//    while(!robot_info.isSensorsListReaded() || time.elapsed() < 1000) {};
    QElapsedTimer timer;
    timer.start();
    while(!robot_info.isSensorsListReaded() && timer.elapsed() < 1000) {
//        std::cout << timer.elapsed() << "milliseconds" << std::endl;
    }
    if(robot_info.isSensorsListReaded())
        return robot_info.getSensorsList();
    else
//        QMessageBox::information(nullptr, tr("NDAriaRobot::readActualSensorList()"), tr("Cannot read sensors list"), tr("&Ok"));
       QMessageBox::critical(nullptr, tr("NDAriaRobot::readActualSensorList()"), tr("Cannot read sensors list"), QMessageBox::Ok);
    return 0;
}

void NDAriaRobot::gotoPose(double x, double y, double th)
{
    // Goto action at lower priority
    ArNetPacket packet;
    packet.empty();
    packet.byte4ToBuf((int)x);
    packet.byte4ToBuf((int)y);
    packet.byte2ToBuf((int)th);
    client->requestOnce("gotoPose", &packet);

    /*  ArActionGoto gotoPoseAction("goto");
  robot.addAction(&gotoPoseAction, 50);
  gotoPoseAction.setGoal(ArPose(x, y, th));
  ArLog::log(ArLog::Normal, "Going to next goal at %.0f %.0f with heading %.0f", gotoPoseAction.getGoal().getX(), gotoPoseAction.getGoal().getY(), gotoPoseAction.getGoal().getTh());*/
}

void NDAriaRobot::setMainMotor(void) {
    // por implementar leyendo xml
    if(!mainMotor) {
        DifferentialMotor* motor = new DifferentialMotor("MainMotor", this);
        //     motor->setWheelsSeparation(getRealWheelsSep());
        mainMotor = motor;
        addMotor("MainMotor", mainMotor);
        mainMotor->setMaxVel(max_vel);
        mainMotor->setAdvancePeriod(advance_period);
    }
    else
        cerr << "Warning: Robot::setMainMotor() MainMotor already assigned" << endl;

}

void NDAriaRobot::updateOdometries(bool paintRaw, bool paintSlam)
{
    double odomX = mainMotor->getRawOdometry().getPosition().at(0);
    double  odomY = mainMotor->getRawOdometry().getPosition().at(1);
    rawOdometry.setCoordinates(odomX, odomY, mainMotor->getRawOdometry().getOrientation().at(0));

    double slamOdomX = mainMotor->getSlamOdometry().getPosition().at(0);
    double slamOdomY = mainMotor->getSlamOdometry().getPosition().at(1);
    slamOdometry.setCoordinates(slamOdomX, slamOdomY, mainMotor->getSlamOdometry().getOrientation().at(0));

    if(paintRaw) {
        QPointF p_raw = QPointF(odomX, odomY);
        robot_sprite->updateRawOdometry(world->xTransform(p_raw.x() + xInit),
                                        world->yTransform(p_raw.y() + yInit));
        //        std::cout << "updateRawOdometry " << p_raw.x() + x + xInit << " " << p_raw.y() + y + yInit << endl;
    }
    if(paintSlam) {
        QPointF p_slam = QPointF(slamOdomX, slamOdomY);
        //        NDMath::selfRotateRad(p_slam, /*-world->inverseHeadingTransform(1)* */
        //                              slamOdometry.getOrientation().at(0)-rawOdometry.getOrientation().at(0), QPointF());
        robot_sprite->updateSlamOdometry(world->xTransform(p_slam.x() + xInit),
                                         world->yTransform(p_slam.y() + yInit));
    }

}
