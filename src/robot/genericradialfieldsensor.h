//
// C++ Interface: genericradialfieldsensor
//
// Description:
//
//
// Author: Jose Manuel Cuadra Troncoso <jmcuadra@dia.uned.es>, (C) 2009
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef GENERICRADIALFIELDSENSOR_H
#define GENERICRADIALFIELDSENSOR_H

#include "radialfieldsensor.h"

/**
  @author Jose Manuel Cuadra Troncoso <jmcuadra@dia.uned.es>
*/
class GenericRadialFieldSensor : public RadialFieldSensor
{
public:
    GenericRadialFieldSensor(int id, int group_idx, QObject* parent, const char* name);

    GenericRadialFieldSensor(int id, int group_idx, double rad, double rot, double orient, double arc_, double no_f_val, double satur_val, double normval, QObject* parent, const char* name);

    GenericRadialFieldSensor(int id, int group_idx, double x_, double y_, double rot, double orient, double arc_, double no_f_val, double satur_val, double normval, QObject* parent, const char* name);

    ~GenericRadialFieldSensor();

    int getRawReading(bool obtain);

    virtual int sensorSuperType(void);
    virtual int sensorType(void);

protected:
    void initMeasure(void);

};

#endif
