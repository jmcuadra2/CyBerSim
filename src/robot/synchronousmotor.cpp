//
// C++ Implementation: synchronousmotor
//
// Description: 
//
//
// Author: Javier Garcia Misis, (C) 2010
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "synchronousmotor.h"
#include "robot.h"
#include "../slam/scanmatchingalgo.h"

SynchronousMotor::SynchronousMotor(QString motorName, Robot* robot)
 : AbstractMotor(motorName)
{
  this->robot = robot;
  wheelsSeparation = this->robot->getRealWheelsSep();
  orient_ = 0;
  inerl = 0.0;
  inerr = 0.0;
  noise1 = 0.1;
  noise2 = 0.1;
  m_noise1 = 1;
  m_noise2 = 1;
  type_iner = 2;
  advance_period = this->robot->getAdvancePeriod();
  max_vel = robot->getMaxVel();
  max_rot_ = 45;

  random_cnt = 0;
  generateRand = true;

  rawOdometry = new NDPose2D();
  exactOdometry = new NDPose2D();
  slamOdometry = new NDPose2D();
  velsPose = new NDPose2D();



}


SynchronousMotor::~SynchronousMotor()
{
    delete rawOdometry;
    delete exactOdometry;
    delete slamOdometry;
    delete velsPose;

    rawOdometry = 0;
    exactOdometry = 0;
    slamOdometry = 0;
    velsPose = 0;
}

void SynchronousMotor::applyModel()
{
                   
    orient_ = ScanMatchingAlgo::norm_a(orient_ + (vel2 * max_rot_/NDMath::RAD2GRAD * advance_period / 1000));

    NDPose2D deltaOdometry;
    velsToPose(deltaOdometry, AbstractMotor::EXACT);

    accumOdometry(deltaOdometry, AbstractMotor::RAW);

    //addInertia();
    generateRand = true;
    if (randomDist == UNIFORM) {
        if (generateRand) {
            m_noise1 = 1 + NDMath::randDouble4(-1.0, 1.0) * noise1 / 100.0;
            m_noise2 = 1 + NDMath::randDouble4(-1.0, 1.0) * noise2 / 100.0;
            m_noise1 = m_noise1 > 2 ? 2 : m_noise1; // noise no pasa de 100%
            m_noise1 = m_noise1 < 0 ? 0 : m_noise1;
            m_noise2 = m_noise2 > 2 ? 2 : m_noise2;
            m_noise2 = m_noise2 < 0 ? 0 : m_noise2;
        }

        vel1 *= m_noise1;
        vel2 *= m_noise2;
    } else if (randomDist == GAUSS) {
        if (generateRand) {
            m_noise1 = 1 + NDMath::randGaussian() * noise1 / 100 / 3; // 3, 99% confidence
            m_noise2 = 1 + NDMath::randGaussian() * noise2 / 100.0 / 3;
            m_noise1 = m_noise1 > 2 ? 2 : m_noise1; // noise no pasa de 100%
            m_noise1 = m_noise1 < 0 ? 0 : m_noise1;
            m_noise2 = m_noise2 > 2 ? 2 : m_noise2;
            m_noise2 = m_noise2 < 0 ? 0 : m_noise2;
        }

        vel1 *= m_noise1;
        vel2 *= m_noise2;
/*
        vel1 = (k2 * v_lin + k1 * dth) / (2 * k1 * k2);
        vel2 = (v_lin - k1 * vel1) / k1;
*/
    }

//  prev_vel1 = vel1;
//  prev_vel2 = vel2;

//  NDPose2D deltaOdometry;
//  velsToPose(deltaOdometry, AbstractMotor::EXACT);
    accumOdometry(deltaOdometry, AbstractMotor::EXACT);
}

void SynchronousMotor::accumOdometry(NDPose odometry, OdometryType type)
{
  QPointF newOdom(odometry.getPosition().at(0), odometry.getPosition().at(1));
  double rot = odometry.getOrientation().at(0);
//  if (fabs(rot) > 0.01)
//    int i = 0;

  NDPose2D* odometryP;
  if (type == RAW)
    odometryP = dynamic_cast<NDPose2D*>(rawOdometry);
  else if (type == EXACT)
    odometryP = dynamic_cast<NDPose2D*>(exactOdometry);
  else if (type == SLAM)
    odometryP = dynamic_cast<NDPose2D*>(slamOdometry);

//  NDMath::selfRotateRad(newOdom, -odometryP->getOrientation().at(0), QPointF());  // para poder sumar abajo

  odometryP->setCoordinates(odometryP->getPosition().at(0) + newOdom.x(), odometryP->getPosition().at(1) + newOdom.y(), ScanMatchingAlgo::norm_a(odometryP->getOrientation().at(0) + rot));
}


void SynchronousMotor::calcAccumOdometry(NDPose& odometry, OdometryType type)
{
  QPointF newOdom(odometry.getPosition().at(0), odometry.getPosition().at(1));
  double rot = odometry.getOrientation().at(0);

  NDPose* odometryP;
  if (type == RAW)
    odometryP = rawOdometry;
  else if (type == EXACT)
    odometryP = exactOdometry;
  else if (type == SLAM)
    odometryP = slamOdometry;

  vector<double> v_th(1);
  v_th[0] = ScanMatchingAlgo::norm_a(odometryP->getOrientation().at(0) + rot);
  vector<double> v_d(2);
  v_d[0] = odometryP->getPosition().at(0) + newOdom.x();
  v_d[1] = odometryP->getPosition().at(1) + newOdom.y();
  odometry.setCoordinates(v_d, v_th);

}

void SynchronousMotor::velsToPose(NDPose& velsPose, OdometryType type)
{
    double advance = vel1 /*potencia -1 y 1*/
                   * max_vel /* velocidad maxima mm/s*/
                   * advance_period / 1000 /*segundos*/;

    QPointF d  = NDMath::polarToCartesian(advance, orient_ * NDMath::RAD2GRAD);


    vector<double> v_th(1);
    vector<double> v_d(2);
    v_th[0] = 0;

    v_d[0] = d.x();
    v_d[1] = d.y();

    velsPose.setCoordinates(v_d, v_th);

}

void SynchronousMotor::reset(bool full)
{
  AbstractMotor::reset(full);
  inerl = 0.0;
  inerr = 0.0;
  orient_ = 0;
  if (full) {
    rawOdometry->setCoordinates(0, 0, 0);
    exactOdometry->setCoordinates(0, 0, 0);
    slamOdometry->setCoordinates(0, 0, 0);
  }
}

void SynchronousMotor::setModel(double noise1, double noise2, int type_iner, RandomDist randomDist)
{

  this->noise1 = noise1;
  this->noise2 = noise2;
  this->type_iner = type_iner;
  this->randomDist = randomDist;
}

