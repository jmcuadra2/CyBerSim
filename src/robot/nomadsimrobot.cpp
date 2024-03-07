//
// C++ Implementation: nomadsimrobot
//
// Description: 
//
//
// Author: Javier Garcia Misis, (C) 2010
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "nomadsimrobot.h"

#include "synchronousmotor.h"
#include "turretmotor.h"
#include "nomadsprite.h"

QString NomadSimRobot::TYPE_NAME = "SimulatedNomadRobot";


NomadSimRobot::NomadSimRobot(int n_id, QObject* parent, const char* name)
    : Robot(n_id, parent, name)
{
    noise = 10;
    type_iner = 2;
    sensorMode = KHEPERA_II;
    velUnitsConversion = 1.0;
}



NomadSimRobot::~NomadSimRobot()
{
}

void NomadSimRobot::setCanvas(QGraphicsScene* w_canvas)
{

    robot_sprite->setX(0);
    robot_sprite->setY(0);
    robot_sprite->diameter = NDMath::roundInt(diamtr);
    robot_sprite->wheelsSep = 1;
    robot_sprite->rotation = rot;
    robot_sprite->parent();

    Robot::setCanvas(w_canvas);
    robot_sprite->setCollisioner(new SpriteCollisioner(robot_sprite));

}

void NomadSimRobot::setNoise(int nois)
{
    noise = nois;
    robot_sprite->noise_ratio = noise/100.0;
} 

void NomadSimRobot::setSensorMode(int sm, bool load_first)
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


bool NomadSimRobot::okPosition()
{
    return !(is_crash || is_bump);
}

bool NomadSimRobot::stop()
{
    prev_contact = false;
    contact = false;
    sensorsVisible(true);
    old_left_vel = 0.0;
    old_right_vel = 0.0;

    status_ = false;
    robot_sprite->collisioner->setCollision(false);
    robot_sprite->collisioner->setResolvCollision(false);

    return true;
}

const QString NomadSimRobot::type()
{
    return NomadSimRobot::TYPE_NAME;
}

double NomadSimRobot::getRotation()
{
    return world->inverseHeadingTransform(robot_sprite->getRotation());
}

double NomadSimRobot::getX()
{
    return world->inverseXTransform(robot_sprite->x());
}

double NomadSimRobot::getY()
{
    return world->inverseYTransform(robot_sprite->y());
}

void NomadSimRobot::initSim(bool reset)
{
    setVel2(init_right_vel/robot_scale, init_left_vel/
            robot_scale); // antes de rotation, 2 escala a real

    initSensors();
    readSensors();
    robot_sprite->clearPath(reset);
    if(reset) {
        x = world->inverseXTransform(robot_sprite->x()); // iniciales
        y = world->inverseYTransform(robot_sprite->y());
        init_rot = world->inverseHeadingTransform(robot_sprite->rotation);
    }
    mainMotor->reset(reset);
}

void NomadSimRobot::outputMotors()
{
    //   cout << "NomadSimRobot::outputMotors " << endl;
    TurretMotor * motorT = dynamic_cast<TurretMotor *>(motors.value("Turret",0));
    SynchronousMotor * motor = dynamic_cast<SynchronousMotor *>(mainMotor);

    if(motor == 0 || motorT==0){
        //No estan definidos uno o varios motores
        return;
    }

    if(!prev_contact || driving) {
        motor->applyModel();
        motorT->applyModel();

        double vel = motor->getVel1();
        double ori = motor->getOrient();

        double odomX = motor->getRawOdometry().getPosition().at(0);
        double  odomY = motor->getRawOdometry().getPosition().at(1);
        rawOdometry.setCoordinates(odomX, odomY, motorT->getRawOdometry().getOrientation().at(0));
        
        double exactOdomX = motor->getExactOdometry().getPosition().at(0);
        double exactOdomY = motor->getExactOdometry().getPosition().at(1);
        exactOdometry.setCoordinates(exactOdomX, exactOdomY, motorT->getExactOdometry().getOrientation().at(0));

        double slamOdomX = motor->getSlamOdometry().getPosition().at(0);
        double slamOdomY = motor->getSlamOdometry().getPosition().at(1);
        slamOdometry.setCoordinates(slamOdomX, slamOdomY, motorT->getSlamOdometry().getOrientation().at(0));

        QPointF p_raw = QPointF(odomX, odomY);
        NDMath::selfRotateRad(p_raw, /*-world->inverseHeadingTransform(1)* */init_rot/NDMath::RAD2GRAD, QPointF());
        
        QPointF p_exact = QPointF(exactOdomX, exactOdomY);
        NDMath::selfRotateRad(p_exact, /*-world->inverseHeadingTransform(1)* */init_rot/NDMath::RAD2GRAD, QPointF());

        QPointF p_slam = QPointF(slamOdomX, slamOdomY);
        NDMath::selfRotateRad(p_slam, /*-world->inverseHeadingTransform(1)* */init_rot/NDMath::RAD2GRAD, QPointF());

        robot_sprite->calcAdvance(world->xTransform(p_exact.x() + x),
                                  world->yTransform(p_exact.y() + y),
                                  (motorT->getExactOdometry().getOrientation().at(0)*NDMath::RAD2GRAD-init_rot),
                                  vel >= 0);
        
        robot_sprite->updateSlamOdometry(world->xTransform(p_slam.x() + x), world->yTransform(p_slam.y() + y));

        robot_sprite->updateRawOdometry(world->xTransform(p_raw.x() + x), world->yTransform(p_raw.y() + y));
        
        NomadSprite * nsprite = dynamic_cast<NomadSprite *>(robot_sprite);

        nsprite->updatePathLine(ori);
    }
}

void NomadSimRobot::rotation(double rot_)
{
    Robot::rotation(rot_);
}

void NomadSimRobot::setAdvancePeriod(int ap)
{

    Robot::setAdvancePeriod(ap);
    robot_sprite->advance_period = advance_period;
}

void NomadSimRobot::setMainMotor()
{
    if(!mainMotor) {
        SynchronousMotor* motor = new SynchronousMotor("MainMotor", this);
        motor->setMaxRotVel(max_rot_);
        mainMotor = motor;
        addMotor("MainMotor", mainMotor);
    }
    else
        cerr << "Warning: Robot::setMainMotor() MainMotor already assigned" << endl;
    
    //añadimos el motor adicional que controla la torreta
    if (motors.value("Turret",0)==0){
        TurretMotor * motor = new TurretMotor("TurretMotor",this);
        motor->setMaxRotVel(max_rot_);
        addMotor("Turret",motor);
    }
    else
        cerr << "Warning: Robot::setMainMotor() TurretMotor already assigned" << endl;

}


void NomadSimRobot::advance(int stage){
    //   cout << "NomadSimRobot::advance " << stage << endl;
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

        //     sensorsSet->advance(0);   no hace nada por ahora
    }
    else {
        robot_sprite->advance(1);
        sensorsSet->advance(1);
    }
}

void * NomadSimRobot::getClient(void){
    return 0;
}


void NomadSimRobot::getKeyEvent(QKeyEvent* event, bool key_on)
{
    Robot::getKeyEvent(event, key_on);
}

