//
// C++ Interface: smgnomadinterface
//
// Description: 
//
//
// Author: Javier Garcia Misis, (C) 2010
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef SMGNOMADINTERFACE_H
#define SMGNOMADINTERFACE_H

#include "smginterface.h"

class DriveNomadControl;

/**
	@author Javier Garcia Misis
*/
class SMGNomadInterface : public SMGInterface
{
public:
    SMGNomadInterface();

    ~SMGNomadInterface();

    AbstractMotor* getMotor() const;
    DriveControl* getDrive() const;
    OutputMotorPlan* getOutputMotorPlan() const;
    Robot* getRobot() const;
    SensorsSet* getSensorsSet() const;
    void setDrive(DriveControl* drive);
    void setMotor(AbstractMotor* motor);
    void setOutputMotorPlan(OutputMotorPlan* outputMotorPlan);
    void setRobot(Robot* robot);
    void setSensorsSet(SensorsSet* sensorsSet);
    void setMotorT(AbstractMotor* motor);
    AbstractMotor* getMotorT() const {return motorT_;}
    OutputMotorPlan* getOutputMotorPlanT() const {return outputMotorPlanT_;}

protected:
    AbstractMotor* motorT_;
    OutputMotorPlan* outputMotorPlanT_;
    DriveNomadControl* driveN_;
};

#endif
