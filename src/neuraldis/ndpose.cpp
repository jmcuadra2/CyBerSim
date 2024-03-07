//
// C++ Implementation: ndpose
//
// Description: 
//
//
// Author: Jose Manuel Cuadra Troncoso <jmcuadra@dia.uned.es>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
// 

#include "ndpose2d.hpp"
#include "ndpose3dsph.hpp"
#include "ndpose3dquat.hpp"

NDPose* NDPose::createPose(NDPose::PoseType type)
{
  NDPose* pose = 0;
  switch(type) {
    case NDPose::Pose2D:
      pose = new NDPose2D();
      break;
    case NDPose::Pose3DSph:
      pose = new NDPose3DSph();
      break;
    case NDPose::Pose3DQuat:
      pose = new NDPose3DQuat();
      break;
    default:
      break;
  }
  return pose;
}
