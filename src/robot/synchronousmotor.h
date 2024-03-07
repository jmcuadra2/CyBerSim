//
// C++ Interface: synchronousmotor
//
// Description: 
//
//
// Author: Javier Garcia Misis, (C) 2010
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef SYNCHRONOUSMOTOR_H
#define SYNCHRONOUSMOTOR_H

#include "abstractmotor.h"

class Robot;

/**
	@author Javier Garcia Misis
*/
class SynchronousMotor : public AbstractMotor
{
public:
    SynchronousMotor(QString motorName , Robot* robot);

    ~SynchronousMotor();

    virtual void accumOdometry(NDPose odometry, OdometryType type);
    virtual void applyModel();
    virtual void calcAccumOdometry(NDPose& odometry, OdometryType type);
    virtual void velsToPose(NDPose& velsPose, OdometryType type);

    void setModel(double noise1, double noise2, int type_iner, RandomDist randomDist = UNIFORM);

    void setOrient(double dir) {
        orient_ = dir;
    }
    double getOrient() {
        return orient_;
    }

    void setMaxRotVel(double vel){max_rot_ = vel;}

    virtual double getRealVel1() const {
        return vel1*max_vel;
    }    
    virtual double getRealVel2() const {
        return vel2*max_rot_;
    }    


    void reset(bool full);
private:
    double wheelsSeparation;
    Robot* robot;
    double noise1, noise2, m_noise1, m_noise2;
    RandomDist randomDist; 
    int type_iner;
    double inerl, inerr;  // r = right, l = left
    int random_cnt;
    bool generateRand;
    double orient_;
    double max_rot_;
    
};

#endif
