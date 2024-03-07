//
// C++ Implementation: abstractmotor
//
// Description: 
//
//
// Author: Jose Manuel Cuadra Troncoso <jmcuadra@dia.uned.es>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "abstractmotor.h"

AbstractMotor::AbstractMotor(QString motorName) : motorName(motorName), vel1(0.0), vel2(0.0), init_vel1(0.0), init_vel2(0.0), prev_vel1(0.0), prev_vel2(0.0)
{
//   message.setSender(motorName);
  rawOdometry = 0;
  exactOdometry = 0;
  slamOdometry = 0;
  velsPose = 0;
  advance_period = 0;
  max_vel = 1;
}

AbstractMotor::~ AbstractMotor()
{
  if(rawOdometry)
    delete rawOdometry;
  if(exactOdometry)
    delete exactOdometry;    
}

void AbstractMotor::reset(bool /*full*/)
{
  vel1 = 0;vel2 = 0;init_vel1 = 0;init_vel2 = 0;prev_vel1 = 0;prev_vel2 = 0;
  // ¿se deberia resetear la odometria?, se deja para las subclases
}

void AbstractMotor::setOldVel(double prev_vel1, double prev_vel2)
{
  this->prev_vel1 = prev_vel1;
  this->prev_vel2 = prev_vel2;
}

void AbstractMotor::setSlamOdometry(NDPose slamOdometry) {
  this->slamOdometry->setCoordinates(slamOdometry.getPosition().at(0), slamOdometry.getPosition().at(1), slamOdometry.getOrientation().at(0));
}

bool AbstractMotor::isStalled(void)
{
  bool ret = false;
  int siz = stallValues.size();
  for(int i = 0; i < siz; i++)
    ret = ret || stallValues.at(i);

  return ret;
}
