//
// C++ Interface: simplemotor
//
// Description: 
//
//
// Author: Javier Garcia Misis, (C) 2010
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef SIMPLEMOTOR_H
#define SIMPLEMOTOR_H

#include "abstractmotor.h"

/**
    @author Javier Garcia Misis
*/
class Robot;

class TurretMotor : public AbstractMotor
{
public:
    TurretMotor(QString motorName , Robot* robot);

    ~TurretMotor();

    virtual void accumOdometry(NDPose odometry, OdometryType type);
    virtual void applyModel();
    virtual void calcAccumOdometry(NDPose& odometry, OdometryType type);
    virtual void reset(bool full);
    virtual void velsToPose(NDPose& velsPose, OdometryType type);

    void setMaxRotVel(double vel){max_rot_ = vel;}

    virtual double getRealVel1() const {
        return vel1*max_rot_;
    }    

private:
    Robot* robot_;
    double max_rot_;
};

#endif
