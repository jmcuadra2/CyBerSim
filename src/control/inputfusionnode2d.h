//
// C++ Interface: inputfusionnode2d
//
// Description:
//
//
// Author: Jose Manuel Cuadra Troncoso <jmcuadra@dia.uned.es>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef INPUTFUSIONNODE2D_H
#define INPUTFUSIONNODE2D_H

#include "inputfusionnode.h"

/**
  @author Jose Manuel Cuadra Troncoso <jmcuadra@dia.uned.es>
*/
class InputFusionNode2D : public InputFusionNode
{
  public:
    InputFusionNode2D(AbstractSensor* sensor);

    ~InputFusionNode2D();

    void initialize(void);

  protected:
    void proceessInformation(void);
    void writeInformation(void) {};
    void initResultMeasure(void);
    void initAuxiliarMeasure(void) {};

};

#endif
