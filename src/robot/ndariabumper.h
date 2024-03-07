//
// C++ Interface: ndariabumper
//
// Description: 
//
//
// Author: Jose Manuel Cuadra Troncoso <jmcuadra@dia.uned.es>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef NDARIABUMPER_H
#define NDARIABUMPER_H

#include "ariasensor.h"
#include "../aria-tools/bumperinfowrapper.h"

/**
    @author Jose Manuel Cuadra Troncoso <jmcuadra@dia.uned.es>
*/
class NDAriaBumper : public AriaSensor
{
public:
    NDAriaBumper(int id, double rad, double start_ang, double end_ang, int group_idx,
                 BumperInfoWrapper* bumper_inf, QObject* parent = 0, const char* name = 0);
    
    NDAriaBumper(int id, int group_idx, BumperInfoWrapper* bumper_inf, QObject* parent = 0,
                 const char* name = 0);
    
    NDAriaBumper(int id, int group_idx, double rad, double rot, double orient,
                 BumperInfoWrapper* bumper_inf, QObject* parent = 0, const char* name = 0);
    
    NDAriaBumper(int id, int group_idx, double x_, double y_, double rot, double orient,
                 BumperInfoWrapper* bumper_inf, QObject* parent = 0, const char* name = 0);

    ~NDAriaBumper();
    
    virtual int sensorSuperType(void);
    virtual int sensorType(void);
    double sample(void);
    
    int getRawReading(bool obtain = true);
    
protected:
    BumperInfoWrapper* bumper_info;

protected:
    void initMeasure(void);

};

#endif
