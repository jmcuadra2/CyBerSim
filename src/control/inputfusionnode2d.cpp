//
// C++ Implementation: inputfusionnode2d
//
// Description: 
//
//
// Author: Jose Manuel Cuadra Troncoso <jmcuadra@dia.uned.es>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "inputfusionnode2d.h"

InputFusionNode2D::InputFusionNode2D(AbstractSensor* sensor): InputFusionNode(sensor)
{
}

InputFusionNode2D::~InputFusionNode2D()
{
}

void InputFusionNode2D::initialize(void)
{
  initResultMeasure();  
}

void InputFusionNode2D::proceessInformation(void)
{
  Measure<> measure = inputMeasures.at(0);
  NDPose* pose = measure.getPose();
  vector<double> yValue = measure.getY();
  vector<double> y(4);
  y[0] = yValue.at(0);
  y[1] = yValue.at(1);

  /*double y0 = y[0] < 0 ? 1000 : y[0];*/
  double y0 = y[0] < 0 ? sensor->getSensorMaxVal() : y[0];
  
  y[2] = pose->getPosition().at(0) + y0*cos(pose->getOrientation().at(0));  // coord x
  y[3] = pose->getPosition().at(1) + y0*sin(pose->getOrientation().at(0));  // coord y
  resultMeasure.setPose(pose);
  resultMeasure.setY(y);
  resultMeasure.setTimeStamp(measure.getTimeStamp());
}

// void InputFusionNode2D::writeInformation(void)
// {
//   resultMeasure = inputMeasures.at(0);
//   resultMeasure.setTimeStamp();
//   resultMeasure.setY(sensor->Out());
// }

void InputFusionNode2D::initResultMeasure(void)
{
  resultMeasure = Measure<>::createMeasure(NDPose::Pose2D);  
  resultMeasure.setEntity("Cartesian2D");
  resultMeasure.setUnit("mm");
  vector<size_t> yDim(2, 2);
  resultMeasure.setYDimensions(yDim);  
  if(inputMeasures.size())
   proceessInformation();    
}
 
// void initAuxiliarMeasure(void)
// {
// }
