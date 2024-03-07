//
// C++ Interface: abstractmotor
//
// Description:
//
//
// Author: Jose Manuel Cuadra Troncoso <jmcuadra@dia.uned.es>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef ABSTRACTMOTOR_H
#define ABSTRACTMOTOR_H

#include <QString>
#include <QVector>
#include "../neuraldis/measure.hpp"

/**
  @author Jose Manuel Cuadra Troncoso <jmcuadra@dia.uned.es>
*/
class AbstractMotor
{
  public:
    AbstractMotor(QString motorName);

    virtual ~AbstractMotor();

    enum RandomDist {UNIFORM, GAUSS};

    enum OdometryType {RAW, EXACT, SLAM};

    const QString& getMotorName(void) {
      return motorName ;
    }
    virtual void setVel1(double vel1) {
      this->vel1 = vel1;
    }
    virtual double getVel1() const {
      return vel1;
    }
    virtual double getRealVel1() const {
      return vel1*max_vel;
    }    
    virtual  void setVel2(double vel2) {
      this->vel2 = vel2;
    }
    virtual double getVel2() const {
      return vel2 ;
    }
    virtual double getRealVel2() const {
      return vel2*max_vel;
    }     
    virtual void setInitVel1(double init_vel1) {
      this->init_vel1 = init_vel1;
    }
    virtual double getInitVel1() const {
      return init_vel1 ;
    }
    virtual  void setInitVel2(double init_vel2) {
      this->init_vel2 = init_vel2;
    }
    virtual double getInitVel2() const {
      return init_vel2 ;
    }
    virtual void applyModel(void) = 0;
    virtual void reset(bool full);
    virtual void setOldVel(double prev_vel1, double prev_vel2);

    void setRawOdometry(NDPose odometry) {
      *(this->rawOdometry) = odometry;
    }

    NDPose getRawOdometry() const {
      return *rawOdometry;
    }
    
    NDPose* getRawOdometryPointer() {
      return rawOdometry;
    }  
    
    NDPose* getSlamOdometryPointer() {
      return slamOdometry;
    } 
    
    void setExactOdometry(NDPose exactOdometry) {
      *(this->exactOdometry) = exactOdometry;
    }

    NDPose getExactOdometry() const {
      return *exactOdometry;
    }
       
//     void setSlamOdometry(NDPose slamOdometry) {
//       *(this).slamOdometry = slamOdometry;
//     }
    void setSlamOdometry(NDPose slamOdometry);
    NDPose getSlamOdometry() const {
      return *slamOdometry;
    }
          
    virtual void accumOdometry(NDPose odometry, OdometryType type) = 0;
    virtual void calcAccumOdometry(NDPose& odometry, OdometryType type) = 0;
    virtual void velsToPose(NDPose& velsPose, OdometryType type) = 0;

    void setMaxVel(double max_vel) { this->max_vel = max_vel; }    
    double getMaxVel() const { return max_vel; }
    void setAdvancePeriod(int advance_period) { this->advance_period = advance_period; }   
    int getAdvancePeriod() const { return advance_period; }
    
    bool isStalled(void);
          

  protected:
    QString motorName;
    double vel1, vel2, init_vel1, init_vel2, prev_vel1, prev_vel2; // 1 left or linear, 2 right or angular
    NDPose* rawOdometry;
    NDPose* exactOdometry;
    NDPose* slamOdometry;
    NDPose* velsPose;
    int advance_period;
    double max_vel;
    QVector<bool> stallValues; // uno para dch. y otro para izq. p. ej

};

#endif
