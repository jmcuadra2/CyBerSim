//
// C++ Implementation: inputfusionnode
//
// Description: 
//
//
// Author: Jose Manuel Cuadra Troncoso <jmcuadra@dia.uned.es>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "inputfusionnode.h"

InputFusionNode::InputFusionNode(AbstractSensor* sensor): AbstractFusionNode(), sensor(sensor)
{
  if(this->sensor) {
    inputMeasures.push_back(sensor->getMeasure());
    resultMeasure = sensor->getMeasure();
  }
}


InputFusionNode::~InputFusionNode()
{
}

void InputFusionNode::readInformation(void)
{
  inputMeasures[0] = sensor->getMeasure();
  
}

void InputFusionNode::setSensor(AbstractSensor* sensor)
{
  if(sensor) {
    if(!this->sensor) {
      this->sensor = sensor;
      inputMeasures.push_back(sensor->getMeasure());
    }
    else
      cerr << "Warning: InputFusionNode::setSensor() pointer already assigned" << endl;
  }
  else
    cerr << "Warning: InputFusionNode::setSensor() null pointer" << endl;
}
