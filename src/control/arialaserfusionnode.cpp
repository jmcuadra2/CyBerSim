//
// C++ Implementation: arialaserfusionnode
//
// Description: 
//
//
// Author: Jose Manuel Cuadra Troncoso <jmcuadra@dia.uned.es>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "arialaserfusionnode.h"

AriaLaserFusionNode::AriaLaserFusionNode(AbstractSensor* sensor): InputFusionNode(sensor)
{
}

AriaLaserFusionNode::~AriaLaserFusionNode()
{
}

void AriaLaserFusionNode::initialize(void)
{
  initResultMeasure();  
}

void AriaLaserFusionNode::proceessInformation(void)
{
//  Measure<> measure = inputMeasures.at(0);
  NDPose* pose = inputMeasures.at(0).getPose();
//  vector<double> yValue = measure.getY();
//  vector<double> y(9);
//  y[0] = yValue.at(0); // range
//  y[1] = yValue.at(1); // no usado

//  /*double y0 = y[0] < 0 ? 1000 : y[0];*/
//  double y0 = y[0] < 0 ? sensor->getSensorMaxVal() : y[0];
  
//  y[2] = yValue.at(2);  // coord x
//  y[3] = yValue.at(3);  // coord y

//  y[4] = yValue.at(4); // secs.
//  y[5] = yValue.at(5); // milisecs.
//  y[6] = yValue.at(6); // encoder x
//  y[7] = yValue.at(7); // encoder y
//  y[8] = yValue.at(8); // encoder th

  resultMeasure.setPose(pose);
  resultMeasure.setY(inputMeasures.at(0).getY());
  resultMeasure.setTimeStamp(inputMeasures.at(0).getTimeStamp());
}

void AriaLaserFusionNode::initResultMeasure(void)
{
  resultMeasure = Measure<>::createMeasure(NDPose::Pose2D);  
  resultMeasure.setEntity("Cartesian2D");
  resultMeasure.setUnit("mm");
  vector<size_t> yDim(1, 9);
//  yDimensions[0] = 1;
//  yDimensions[1] = 7;
  resultMeasure.setYDimensions(yDim);  
  if(inputMeasures.size())
    proceessInformation();
}
 

