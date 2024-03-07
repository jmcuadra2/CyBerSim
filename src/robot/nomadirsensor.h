//
// C++ Interface: nomadirsensor
//
// Description: 
//
//
// Author: Javier Garcia Misis <>, (C) 2010
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef NOMADIRSENSOR_H
#define NOMADIRSENSOR_H

#include "nomadsensor.h"

/**
    @author Javier Garcia Misis
*/
class NomadIRSensor : public NomadSensor
{
public:
    NomadIRSensor(int id, int group_idx, QObject* parent, const char* name);
    
    NomadIRSensor(int id, int group_idx, double x_, double y_, double rot,
                  double orient, QObject* parent, const char* name);
    
    NomadIRSensor(int id, int group_idx, double rad, double rot, double orient,
                  QObject* parent, const char* name);
    
    NomadIRSensor(int id, double rad, double start_ang, double end_ang, int group_idx,
                  QObject* parent, const char* name);

    ~NomadIRSensor();

    virtual int sensorType(void){return NOMAD_IR;}

    virtual int getRawReading(bool obtain = true);
    
protected:
    virtual void initMeasure(void);

};

#endif
