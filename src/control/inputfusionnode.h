//
// C++ Interface: inputfusionnode
//
// Description: 
//
//
// Author: Jose Manuel Cuadra Troncoso <jmcuadra@dia.uned.es>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef INPUTFUSIONNODE_H
#define INPUTFUSIONNODE_H



#include "../robot/abstractsensor.h"
#include "abstractfusionnode.h"

/**
	@author Jose Manuel Cuadra Troncoso <jmcuadra@dia.uned.es>
*/
class InputFusionNode : public AbstractFusionNode
{
public:
    InputFusionNode(AbstractSensor* sensor = 0);

    ~InputFusionNode();

    virtual void setSensor(AbstractSensor* sensor);

protected:
    virtual void readInformation(void);

  protected:
    AbstractSensor* sensor;

};

#endif
