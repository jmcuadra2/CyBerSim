//
// C++ Interface: drivecontrol
//
// Description:
//
//
// Author: Jose Manuel Cuadra Troncoso <jmcuadra@dia.uned.es>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef DRIVECONTROL_H
#define DRIVECONTROL_H

#include "motorcontrol.h"
#include <QObject>

class AbstractWorld;

/**
  @author Jose Manuel Cuadra Troncoso <jmcuadra@dia.uned.es>
*/
class DriveControl : /*public QObject ,*/ public MotorControl
{
  Q_OBJECT
  public:
    DriveControl();

    virtual ~DriveControl();

    int getType(void);
    void sendOutputs(void);
    void connectDrive(bool on);
    void setMotor(AbstractMotor* motor);
//     void setDefaultCommand(MotorCommand const& motorCommand);
//     virtual void drive(void);
    
  public slots:
    virtual void getKeyEvent(QKeyEvent* event, bool key_on);
    void activate(bool on);

/*  signals:
    void sendToRecorder(double, int); */   
    
  protected:
    MotorCommand writeCommand(MotorCommand::MotorCommands type, double priority, double commandParam1, double commandParam2, double commandParam3);  
//     MotorCommand defaultCommand;

};

#endif
