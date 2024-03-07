//
// C++ Implementation: abstractfusionnode
//
// Description:
//
//
// Author: Jose Manuel Cuadra Troncoso <jmcuadra@dia.uned.es>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "abstractfusionnode.h"

AbstractFusionNode::AbstractFusionNode() :
inputMeasures(), resultMeasure(Measure<>::createMeasure()), auxiliarMeasure(Measure<>::createMeasure())
{
}

AbstractFusionNode::~AbstractFusionNode()
{
}

void AbstractFusionNode::doFusion(void)
{
  readInformation();
  proceessInformation();
  writeInformation();
}

void AbstractFusionNode::initialize(void)
{
  
}

void AbstractFusionNode::readInformation(void)
{
  int numNodes = inputNodesList.size();
  for(int i = 0; i < numNodes; i++)
     inputNodesList[i]->doFusion();
}
