//
// C++ Interface: rangelinesensor
//
// Description:
//
//
// Author: Jose Manuel Cuadra Troncoso <jmcuadra@dia.uned.es>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef BUMPLINESENSOR_H
#define BUMPLINESENSOR_H

#include "linearsensor.h"

/**
  @author Jose Manuel Cuadra Troncoso <jmcuadra@dia.uned.es>
*/
class BumpLineSensor : public LinearSensor
{
  public:
    BumpLineSensor(int id, int group_idx, QObject* parent, const char* name);

    BumpLineSensor(int id, int group_idx, double x_, double y_, double rot, double orient, QObject* parent, const char* name);

    BumpLineSensor(int id, int group_idx, double rad, double rot, double orient, QObject* parent, const char* name);

    BumpLineSensor(int id, double rad, double start_ang, double end_ang, int group_idx, QObject* parent, const char* name);

    ~BumpLineSensor();

    virtual int sensorSuperType(void);
    virtual int sensorType(void);
    double sample(void);

  protected:
    void initParams(void);
    void initMeasure(void);

};

#endif
