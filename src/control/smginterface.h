//
// C++ Interface: smginterface
//
// Description:
//
//
// Author: Jose Manuel Cuadra Troncoso <jmcuadra@dia.uned.es>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef SMGINTERFACE_H
#define SMGINTERFACE_H



#include "outputmotorplan.h"
#include "drivecontrol.h"
#include "../robot/robot.h"

/**
  @author Jose Manuel Cuadra Troncoso <jmcuadra@dia.uned.es>
*/
class SMGInterface
{
  public:
    SMGInterface();

    virtual ~SMGInterface();

  void setSensorsSet(SensorsSet* sensorsSet);
  SensorsSet* getSensorsSet() const { return sensorsSet; }

  void setOutputMotorPlan(OutputMotorPlan* outputMotorPlan);
  OutputMotorPlan* getOutputMotorPlan() const { return outputMotorPlan; }

  void setMotor(AbstractMotor* motor);
  AbstractMotor* getMotor() const { return motor; }

  void setDrive(DriveControl* drive);
  DriveControl* getDrive() const { return drive; }

  bool isInitialized() { return (sensorsSet && outputMotorPlan && motor && drive) ; }

  virtual void setRobot(Robot* robot);
  Robot* getRobot() const { return robot ; }
  

  protected:
    SensorsSet* sensorsSet;
    OutputMotorPlan* outputMotorPlan;
    AbstractMotor* motor;
    DriveControl* drive;
    Robot* robot;
};

#endif
