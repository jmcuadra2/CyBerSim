//
// C++ Interface: netinputfusionnode
//
// Description:
//
//
// Author: Jose Manuel Cuadra Troncoso <jmcuadra@dia.uned.es>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef NETINPUTFUSIONNODE_H
#define NETINPUTFUSIONNODE_H

#include "inputfusionnode.h"

/**
  @author Jose Manuel Cuadra Troncoso <jmcuadra@dia.uned.es>
*/
class NetInputFusionNode : public InputFusionNode
{
  public:
    NetInputFusionNode(AbstractSensor* sensor);

    ~NetInputFusionNode();

  protected:
    void proceessInformation(void){};
    void writeInformation(void);
    void initResultMeasure(void) {};
    void initAuxiliarMeasure(void) {};

};

#endif
