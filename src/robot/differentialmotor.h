//
// C++ Interface: differentialmotor
//
// Description:
//
//
// Author: Jose Manuel Cuadra Troncoso <jmcuadra@dia.uned.es>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef DIFFERENTIALMOTOR_H
#define DIFFERENTIALMOTOR_H

#include "abstractmotor.h"

class Robot;

/**
  @author Jose Manuel Cuadra Troncoso <jmcuadra@dia.uned.es>
*/
class DifferentialMotor : public AbstractMotor
{
  public:

    DifferentialMotor(QString motorName , Robot* robot);

    ~DifferentialMotor();
    
    void setWheelsSeparation(double wheelsSeparation) { this->wheelsSeparation = wheelsSeparation;  }
    double getWheelsSeparation() const { return wheelsSeparation ; }

    void setModel(double noise1, double noise2, int type_iner, RandomDist randomDist = UNIFORM);
    
    void applyModel(void);
    void reset(bool full);
    void accumOdometry(NDPose odometry, OdometryType type);
    void calcAccumOdometry(NDPose& odometry, OdometryType type);
    void velsToPose(NDPose& velsPose, OdometryType type);

  protected:
    void addInertia(void);

  protected:
    double wheelsSeparation;
    Robot* robot;
    double noise1, noise2, m_noise1, m_noise2;
    RandomDist randomDist; 
    int type_iner;
    double inerl, inerr;  // r = right, l = left
    int random_cnt;
    bool generateRand;
    bool turning;
};

#endif
