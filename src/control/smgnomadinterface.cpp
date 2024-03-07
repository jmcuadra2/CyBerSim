//
// C++ Implementation: smgnomadinterface
//
// Description: 
//
//
// Author: Javier Garcia Misis, (C) 2010
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "smgnomadinterface.h"
#include "drivenomadcontrol.h"

SMGNomadInterface::SMGNomadInterface(): SMGInterface()
{
    motorT_ = 0;
    outputMotorPlanT_ = 0;
}


SMGNomadInterface::~SMGNomadInterface()
{
    if(outputMotorPlanT_)
        delete outputMotorPlanT_;
    
    outputMotorPlanT_ = 0;

}


AbstractMotor* SMGNomadInterface::getMotor() const
{
    return SMGInterface::getMotor();
}

DriveControl* SMGNomadInterface::getDrive() const
{
    return SMGInterface::getDrive();
}

OutputMotorPlan* SMGNomadInterface::getOutputMotorPlan() const
{
    return SMGInterface::getOutputMotorPlan();
}

Robot* SMGNomadInterface::getRobot() const
{
    return SMGInterface::getRobot();
}


void SMGNomadInterface::setDrive(DriveControl* drive)
{
    SMGInterface::setDrive(drive);
}

void SMGNomadInterface::setMotor(AbstractMotor* motor)
{
  if(!this->motor) {
    this->motor = motor;
    driveN_ = new DriveNomadControl();
    drive = driveN_;
    drive->setMotor(motor);
    drive->setControlName("Drive" + motor->getMotorName());
    outputMotorPlan = new OutputMotorPlan(motor);
  }  
  else
    cerr << "Warning: SMGInterface::setSensorsSet(), motor already assigned" << endl;    
}

void SMGNomadInterface::setMotorT(AbstractMotor* motor)
{
  if(motorT_ == 0) {
    motorT_ = motor;
    driveN_->setMotorT(motorT_);
    outputMotorPlanT_ =  new OutputMotorPlan(motorT_);
  }  
  else
    cerr << "Warning: SMGInterface::setSensorsSet(), motor already assigned" << endl;    
}


void SMGNomadInterface::setOutputMotorPlan(OutputMotorPlan* outputMotorPlan)
{
    SMGInterface::setOutputMotorPlan(outputMotorPlan);
}

void SMGNomadInterface::setRobot(Robot* robot)
{
    if(!this->robot) {
        this->robot = robot;
        setSensorsSet(robot->getSensorsSet());
        setMotor(robot->getMainMotor());
        setMotorT(robot->getMotors().value ("Turret",0));
    
        QObject::connect(robot, SIGNAL(sendKeyEvent(QKeyEvent*, bool )), drive, SLOT(getKeyEvent(QKeyEvent* , bool )));
        QObject::connect(robot, SIGNAL(sendDrived( bool )), drive, SLOT(activate( bool )));
    }
}


SensorsSet* SMGNomadInterface::getSensorsSet() const
{
    return SMGInterface::getSensorsSet();
}

void SMGNomadInterface::setSensorsSet(SensorsSet* sensorsSet)
{
    SMGInterface::setSensorsSet(sensorsSet);
}

