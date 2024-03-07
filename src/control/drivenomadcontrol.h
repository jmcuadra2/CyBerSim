//
// C++ Interface: drivenomadcontrol
//
// Description: 
//
//
// Author: Javier Garcia Misis <>, (C) 2010
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef DRIVENOMADCONTROL_H
#define DRIVENOMADCONTROL_H

#include "drivecontrol.h"

/**
	@author Javier Garcia Misis
*/
class DriveNomadControl : public DriveControl
{
public:
    DriveNomadControl();

    ~DriveNomadControl();

    void connectDrive(bool on);
    void setMotor(AbstractMotor* motor);
    void setMotorT(AbstractMotor* motor);
    AbstractMotorPlan* getMotorPlanT(void) { return motorPlanT_ ; }
    void setDefaultCommandT(NDPose* const commandValues);

    virtual void getKeyEvent(QKeyEvent* event, bool key_on);

protected:
    MotorCommand writeCommand(MotorCommand::MotorCommands type, double priority, double commandParam1, double commandParam2, double commandParam3);

    AbstractMotor* motorT_;
    AbstractMotorPlan* motorPlanT_;
    MotorCommand defaultCommandT_;

};

#endif
