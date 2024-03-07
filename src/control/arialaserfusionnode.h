//
// C++ Interface: arialaserfusionnode
//
// Description:
//
//
// Author: Jose Manuel Cuadra Troncoso <jmcuadra@dia.uned.es>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef ARIALASERFUSIONNODE_H
#define ARIALASERFUSIONNODE_H

#include "inputfusionnode.h"

/**
  @author Jose Manuel Cuadra Troncoso <jmcuadra@dia.uned.es>
*/
class AriaLaserFusionNode : public InputFusionNode
{
  public:
    AriaLaserFusionNode(AbstractSensor* sensor);

    ~AriaLaserFusionNode();

    void initialize(void);

  protected:
    void proceessInformation(void);
    void writeInformation(void) {};
    void initResultMeasure(void);
    void initAuxiliarMeasure(void) {};

};

#endif
