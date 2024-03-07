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

#include <qewdialogfactory.h> 
#include <QMouseEvent>
#include <QEvent>

#include "../neuraldis/xmloperator.h"
#include "../neuraldis/ndmath.h"
//#include "gui/robotkineticstab.h"
#include "gui/totsimulrobotkineticstab.h"
#include "sensorsset.h"
#include "totallysimulatedrobot.h"
#include "differentialmotor.h"

#include <QtGui>

#include <iostream>

// #define COUT_DEBUG

TotallySimulatedRobot::TotallySimulatedRobot(int n_id, QObject* parent, QString name) :
    Robot(n_id, parent, name)
{
    //   inerl = 0.0;
    //   inerr = 0.0;
    noise = 10;
    type_iner = 2;
    sensorMode = KHEPERA_II;
    velUnitsConversion = 1.0;
}

TotallySimulatedRobot::~TotallySimulatedRobot()
{
}

void TotallySimulatedRobot::setNoise(int nois)
{
    noise = nois;
    robot_sprite->noise_ratio = noise/100.0;
} 

void TotallySimulatedRobot::setCanvas(QGraphicsScene* w_canvas)
{
    Robot::setCanvas(w_canvas);
    robot_sprite->setCollisioner(new SpriteCollisioner(robot_sprite));
}

void TotallySimulatedRobot::setSensorMode(int sm, bool load_first)
{

    if((sensorMode == sm) && !load_first) return;
    sensorMode = sm;
    if(sm == KHEPERA_I) {
        if(!load_first)
            sensorsSet->scaleSensorField(0.5);
    }
    else {
        if(!load_first)
            sensorsSet->scaleSensorField(2.0);
    }

}

bool TotallySimulatedRobot::okPosition(void)
{

    bool ok = !(is_crash || is_bump);
    return ok;

}

void TotallySimulatedRobot::advance(int stage)
{  
    if(stage == 0) {
        robot_sprite->advance(0);
        if((prev_contact = robot_sprite->collisioner->getResolvCollision())) {
            lvel = old_left_vel;
            rvel = old_right_vel;
        }
        else if((contact = robot_sprite->collisioner->getResolvCollision())) {
            old_right_vel = 0.0;
            old_left_vel = 0.0;
            mainMotor->setOldVel(0.0, 0.0);
            emit signalBump();
        }
        setVel2(rvel, lvel);
        robot_sprite->setRVel(rvel/robot_scale);
        robot_sprite->setLVel(lvel/robot_scale);
    }
    else {
        robot_sprite->advance(1);
        sensorsSet->advance(1);
    }

}

void TotallySimulatedRobot::outputMotors(void)
{

    if(!prev_contact || driving) {
        mainMotor->applyModel();
        lvel = mainMotor->getRealVel1();
        rvel = mainMotor->getRealVel2();

        old_right_vel = rvel;
        old_left_vel = lvel;
        
        double exactOdomX = mainMotor->getExactOdometry().getPosition().at(0);
        double exactOdomY = mainMotor->getExactOdometry().getPosition().at(1);
        exactOdometry.setCoordinates(exactOdomX, exactOdomY,
                                     mainMotor->getExactOdometry().getOrientation().at(0));

        QPointF p_exact = QPointF(exactOdomX, exactOdomY);
        NDMath::selfRotateRad(p_exact,
                        /*-world->inverseHeadingTransform(1)* */init_rot/NDMath::RAD2GRAD,
                              QPointF());

        robot_sprite->calcAdvance(world->xTransform(p_exact.x() + x),
                            world->yTransform(p_exact.y() + y),
                            world->inverseHeadingTransform(1)*
                            (mainMotor->getExactOdometry().getOrientation().at(0)*
                            NDMath::RAD2GRAD) + init_rot * world->inverseHeadingTransform(1),
                            lvel + rvel >= 0);

        updateOdometries();

#ifdef COUT_DEBUG    
        cout << "---insxxx void TotallySimulatedRobot::outputMotors(void): (prev exact odom / theta in degrees) x y theta: " << getX() << " " << getY() << " " << getRotation() << endl;
        cout << "---insxxx void TotallySimulatedRobot::outputMotors(void): exactOdometry: " << mainMotor->getExactOdometry() << endl;
        cout << "---insxxx void TotallySimulatedRobot::outputMotors(void): slamOdometry : " << mainMotor->getSlamOdometry() << endl;
        cout << "---insxxx void TotallySimulatedRobot::outputMotors(void): rawOdometry  : " << mainMotor->getRawOdometry()  << endl;
#endif


        //     robot_sprite->updateOdometry(world->xTransform(p_raw.x() + x), world->yTransform(p_raw.y() + y));

        //     cout << "Slam " << mainMotor->getSlamOdometry();
        //     cout << "   Exact " << mainMotor->getExactOdometry();
        //     cout << "   Raw " << mainMotor->getRawOdometry() << endl;

    }

}

void TotallySimulatedRobot::setAdvancePeriod(int ap)
{

    Robot::setAdvancePeriod(ap);
    robot_sprite->advance_period = advance_period;
}

bool TotallySimulatedRobot::stop(void)
{
    //   cout << "time_adv0 " << time_adv0 << "time_collsol " << time_collsol << endl;

    prev_contact = false;
    contact = false;
    sensorsVisible(true);
    old_left_vel = 0.0;
    old_right_vel = 0.0;

    //   mainMotor->reset();
    //   x = world->inverseXTransform(robot_sprite->x()); // inicial
    //   y = world->inverseXTransform(robot_sprite->y());
    status_ = false;
    robot_sprite->collisioner->setCollision(false);
    robot_sprite->collisioner->setResolvCollision(false);

    return true;
}  

void TotallySimulatedRobot::initSim(bool reset_)
{

    setVel2(init_right_vel/robot_scale, init_left_vel/
            robot_scale); // antes de rotation, 2 escala a real
    //   rotation(init_rot);
    initSensors();
    readSensors();
    robot_sprite->clearPath(reset_);
    if(reset_) {
        x = world->inverseXTransform(robot_sprite->x()); // iniciales
        y = world->inverseYTransform(robot_sprite->y());
        init_rot = world->inverseHeadingTransform(robot_sprite->rotation);
    }
    mainMotor->reset(reset_);
}

void TotallySimulatedRobot::save_special(QDomElement& elem)
{
    Robot::save_special(elem);
}

void TotallySimulatedRobot::write_special(QDomDocument& doc, QDomElement& e)
{
    XMLOperator* xml_operator = XMLOperator::instance();
    QDomElement tag_kinet = xml_operator->findTag("Kinetics", e, false);
    if(!tag_kinet.isNull()) {
        tag_kinet.appendChild(xml_operator->createTextElement(doc, "inertia", QString::number(type_iner)));
        tag_kinet.appendChild(xml_operator->createTextElement(doc, "noise", QString::number(noise)));
    }

    e.appendChild(xml_operator->createTextElement(doc, "sensorMode", QString::number(sensorMode)));
}

void TotallySimulatedRobot::initRecord(void)
{

    if(!type_iner) {
        getLinearVel();
        getRadialVel();
    } else {
        vlin = 0.0;
        vrad = 0.0;
    }
    Robot::initRecord();

}    

void TotallySimulatedRobot::recordRobot(double x, double y, double rot, double vx,
                                        double vy, double vrot, bool forw)
{

    if(forw)
        vlin =  sqrt(vx*vx + vy*vy)*robot_scale;
    else
        vlin =  -sqrt(vx*vx + vy*vy)*robot_scale;
    if(is_monitoring) {
        emit velMonitor(vlin, -1);
        emit rotMonitor(vrot, -1);
    }
    if(is_recording) {
//        emit sendToRecorder(world->inverseXTransform(x), 1);
//        emit sendToRecorder(world->inverseYTransform(y), 2);
//        emit sendToRecorder(world->inverseHeadingTransform(rot), 3);
        emit sendToRecorder(x, 1);
        emit sendToRecorder(y, 2);
        emit sendToRecorder(rot, 3);
        emit sendToRecorder(vlin, 4);
        emit sendToRecorder(vrot, 5);
        emit sendToRecorder(0, 6); // RobotSprite::add_rotation
        emit sendToRecorder(forw, 7); // RobotSprite::forw
    }

}

void TotallySimulatedRobot::getMouseEvent(QMouseEvent* e)
{
    if(status_) {
        e->ignore();
        return;
    }
    // mirar las tranformaciones para situar al robot
    if(e->type() == QEvent::MouseButtonDblClick) {
        setX(world->inverseXTransform(e->position().x()));
        setY(world->inverseYTransform(e->position().y()));
        rotation(world->inverseHeadingTransform(robot_sprite->getRotation()));
        readSensors();
    }
    else
        Robot::getMouseEvent(e);

}

QList<QewExtensibleDialog*> TotallySimulatedRobot::createKineticsDialog(QewExtensibleDialog* parent)
{

    QList<QewExtensibleDialog*> add_dialogs = Robot::createKineticsDialog(parent);
    TotSimulRobotKineticsTab* robkin_tab = new TotSimulRobotKineticsTab(parent);
    if(robkin_tab->setUpDialog()) {
        robkin_tab->init(this);
        add_dialogs.append(robkin_tab);
    }
    else
        delete robkin_tab;

    return add_dialogs;
}

void TotallySimulatedRobot::setStatus(bool on)
{
    Robot::setStatus(on);
}

double TotallySimulatedRobot::getY(void)
{
    return world->inverseYTransform(robot_sprite->y());
}

double  TotallySimulatedRobot::getX(void)
{
    return world->inverseXTransform(robot_sprite->x());
}

double TotallySimulatedRobot::getRotation(void)
{
    return world->inverseHeadingTransform(robot_sprite->getRotation());
}

double TotallySimulatedRobot::getLinearVel(void)
{
    double vx, vy;
    vx = robot_sprite->xVelocity()/(0.01*advance_period);
    vy = robot_sprite->yVelocity()/(0.01*advance_period);
    if(robot_sprite->forw)
        vlin =  sqrt(vx*vx + vy*vy)*robot_scale;
    else
        vlin =  -sqrt(vx*vx + vy*vy)*robot_scale;
    return vlin;
}

double TotallySimulatedRobot::getRadialVel(void)
{
    vrad = (lvel - rvel)*10*NDMath::RAD2GRAD/realWheelsSep;
    return vrad;
}

void TotallySimulatedRobot::setMainMotor(void) {
    // por implementar leyendo xml
    if(!mainMotor) {
        DifferentialMotor* motor = new DifferentialMotor("MainMotor", this);
        //     motor->setWheelsSeparation(getRealWheelsSep());

        //     motor->setModel(noise, noise, type_iner, DifferentialMotor::UNIFORM);
        motor->setModel(noise, noise, type_iner, DifferentialMotor::GAUSS);

        mainMotor = motor;
        addMotor("MainMotor", mainMotor);
    }
    else
        cerr << "Warning: Robot::setMainMotor() MainMotor already assigned" << endl;

}
