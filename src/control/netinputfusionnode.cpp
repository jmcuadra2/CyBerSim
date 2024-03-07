//
// C++ Implementation: netinputfusionnode
//
// Description: 
//
//
// Author: Jose Manuel Cuadra Troncoso <jmcuadra@dia.uned.es>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "netinputfusionnode.h"

NetInputFusionNode::NetInputFusionNode(AbstractSensor* sensor): InputFusionNode(sensor)
{
}

NetInputFusionNode::~NetInputFusionNode()
{
}

void NetInputFusionNode::writeInformation(void)
{
  resultMeasure = inputMeasures.at(0);
  resultMeasure.setTimeStamp();
  resultMeasure.setY(sensor->Out());
}
