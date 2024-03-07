//
// C++ Implementation: simplemotor
//
// Description: 
//
//
// Author: Javier Garcia Misis, (C) 2010
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "turretmotor.h"
#include "robot.h"
#include "../slam/scanmatchingalgo.h"

TurretMotor::TurretMotor(QString motorName , Robot* robot)
 : AbstractMotor(motorName)
{
    robot_ = robot;
    advance_period = robot->getAdvancePeriod();
    max_vel = robot->getMaxVel();
    max_rot_ = 60;


    rawOdometry = new NDPose2D();
    exactOdometry = new NDPose2D();
    slamOdometry = new NDPose2D();
    velsPose = new NDPose2D();
}


TurretMotor::~TurretMotor()
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



void TurretMotor::applyModel()
{
    NDPose2D deltaOdometry;
    velsToPose(deltaOdometry, AbstractMotor::EXACT);

    accumOdometry(deltaOdometry, AbstractMotor::RAW);

    //add noise!!!!!

    accumOdometry(deltaOdometry, AbstractMotor::EXACT);

}


void TurretMotor::velsToPose(NDPose& velsPose, OdometryType type)
{

    vector<double> v_th(1);
    vector<double> v_d(2);
    //indicamos lo que ha girado en radianes.
    v_th[0] = vel1 * max_rot_/NDMath::RAD2GRAD * advance_period / 1000;

    v_d[0] = 0;
    v_d[1] = 0;

    velsPose.setCoordinates(v_d, v_th);

}


void TurretMotor::accumOdometry(NDPose odometry, OdometryType type)
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


void TurretMotor::calcAccumOdometry(NDPose& odometry, OdometryType type)
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


void TurretMotor::reset(bool full)
{
    AbstractMotor::reset(full);
    if (full) {
        rawOdometry->setCoordinates(0, 0, 0);
        exactOdometry->setCoordinates(0, 0, 0);
        slamOdometry->setCoordinates(0, 0, 0);
    }

}


