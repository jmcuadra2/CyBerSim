//
// C++ Implementation: smginterface
//
// Description: 
//
//
// Author: Jose Manuel Cuadra Troncoso <jmcuadra@dia.uned.es>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "smginterface.h"

SMGInterface::SMGInterface() : sensorsSet(0), outputMotorPlan(0), motor(0), drive(0), robot(0)
{
}


SMGInterface::~SMGInterface()
{
  if(drive)
    delete drive;
  if(outputMotorPlan)
    delete outputMotorPlan;
}

void SMGInterface::setSensorsSet(SensorsSet* sensorsSet)
{
  if(!this->sensorsSet)
    this->sensorsSet = sensorsSet;
  else
    cerr << "Warning: SMGInterface::setSensorsSet(), sensorsSet already assigned" << endl;
}


void SMGInterface::setOutputMotorPlan(OutputMotorPlan* outputMotorPlan)
{
  this->outputMotorPlan = outputMotorPlan;
}


void SMGInterface::setMotor(AbstractMotor* motor)
{
  if(!this->motor) {
    this->motor = motor;
    drive = new DriveControl();
    drive->setMotor(motor);
    drive->setControlName("Drive" + motor->getMotorName());
    outputMotorPlan = new OutputMotorPlan(motor);
  }  
  else
    cerr << "Warning: SMGInterface::setSensorsSet(), motor already assigned" << endl;    
}

void SMGInterface::setDrive(DriveControl* drive)
{
  if(!this->drive)
    this->drive = drive;
  else
    cerr << "Warning: SMGInterface::setSensorsSet(), driver already assigned" << endl;
}


void SMGInterface::setRobot(Robot* robot)
{
  if(!this->robot) {
    this->robot = robot;
    setSensorsSet(robot->getSensorsSet());
    setMotor(robot->getMainMotor());
    QObject::connect(robot, SIGNAL(sendKeyEvent(QKeyEvent*, bool )), drive, SLOT(getKeyEvent(QKeyEvent* , bool )));
    QObject::connect(robot, SIGNAL(sendDrived( bool )), drive, SLOT(activate( bool )));
  }
}
