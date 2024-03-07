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
#ifndef RANGELINESENSOR_H
#define RANGELINESENSOR_H

#include "linearsensor.h"

/**
    @author Jose Manuel Cuadra Troncoso <jmcuadra@dia.uned.es>
*/
class RangeLineSensor : public LinearSensor
{
public:
    RangeLineSensor(int id, int group_idx, QObject* parent, const char* name);
    
    RangeLineSensor(int id, int group_idx, double x_, double y_, double rot, double orient,
                    QObject* parent, const char* name);
    
    RangeLineSensor(int id, int group_idx, double rad, double rot, double orient,
                    QObject* parent, const char* name);
    
    RangeLineSensor(int id, double rad, double start_ang, double end_ang, int group_idx,
                    QObject* parent, const char* name);

    RangeLineSensor(int id, double rad, double start_ang, double end_ang, int group_idx,
                    double x_, double y_, QObject* parent, const char* name);

    ~RangeLineSensor();

    virtual int sensorSuperType(void);
    virtual int sensorType(void);
    double sample(void);

protected:
    void initParams(void);
    void initMeasure(void);

};

#endif
