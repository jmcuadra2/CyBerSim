//
// C++ Interface: ndariasick
//
// Description: 
//
//
// Author: Jose Manuel Cuadra Troncoso <jmcuadra@dia.uned.es>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef NDARIASICK_H
#define NDARIASICK_H

#include "ariarangedevice.h"
#include "../aria-tools/sickinfowrapper.h"

/**
    @author Jose Manuel Cuadra Troncoso <jmcuadra@dia.uned.es>
*/
class NDAriaSick : public AriaRangeDevice
{

    Q_OBJECT

public:
    NDAriaSick(int id, int group_idx, double x_, double y_, double rot, double orient, SickInfoWrapper* sick_inf, QObject* parent = 0, const char* name = 0);
    
    NDAriaSick(int id, int group_idx, double rad, double rot, double orient, SickInfoWrapper* sick_inf, QObject* parent = 0, const char* name = 0);
    
    NDAriaSick(int id, int group_idx, SickInfoWrapper* sick_inf, QObject* parent = 0, const char* name = 0);
    
    NDAriaSick(int id, double rad, double start_ang, double end_ang, int group_idx, SickInfoWrapper* sick_inf, QObject* parent = 0, const char* name = 0);

    ~NDAriaSick();
    
    virtual int sensorSuperType(void);
    virtual int sensorType(void);

    double sample(void);

protected:
    void initParams(void);
    void initMeasure(void);
    void writeMeasure(void);
    const QString lineColor(void) { return "blue" ; };
    
protected:
    SickInfoWrapper* sick_info;
    
};

#endif
