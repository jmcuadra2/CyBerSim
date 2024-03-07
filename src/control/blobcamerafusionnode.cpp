//
// C++ Implementation: blobcamerafusionnode
//
// Description: 
//
//
// Author: Jose Manuel Cuadra Troncoso <jmcuadra@dia.uned.es>, (C) 2009
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "blobcamerafusionnode.h"

BlobCameraFusionNode::BlobCameraFusionNode(AbstractSensor* sensor)
 : InputFusionNode(sensor)
{
}


BlobCameraFusionNode::~BlobCameraFusionNode()
{
}

void BlobCameraFusionNode::initialize(void)
{
  initResultMeasure();
}

void BlobCameraFusionNode::proceessInformation(void)
{
  resultMeasure.setPose(inputMeasures.at(0).getPose());
  resultMeasure.setY(inputMeasures.at(0).getY());
}

void BlobCameraFusionNode::initResultMeasure(void)
{
  resultMeasure = Measure<>::createMeasure(NDPose::Pose3DSph);  
  resultMeasure.setEntity("BlobCamera");
  resultMeasure.setUnit("pixel");

  vector<size_t> yDimensions(2);
  yDimensions[0] = 1;
  yDimensions[1] = 7; // blob area, bottom, left, right, top, xcg, ycg

  resultMeasure.setYDimensions(yDimensions);  
  if(inputMeasures.size())
   proceessInformation();    
}
