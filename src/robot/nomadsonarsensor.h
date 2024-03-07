//
// C++ Interface: nomadsonarsensor
//
// Description: 
//
//
// Author: Javier Garcia Misis <>, (C) 2010
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef NOMADSONARSENSOR_H
#define NOMADSONARSENSOR_H

#include "nomadsensor.h"

/**
    @author Javier Garcia Misis
*/
class NomadSonarSensor : public NomadSensor
{
public:
    NomadSonarSensor(int id, int group_idx, QObject* parent, const char* name);
    
    NomadSonarSensor(int id, int group_idx, double x_, double y_, double rot, double orient,
                     QObject* parent, const char* name);
    
    NomadSonarSensor(int id, int group_idx, double rad, double rot, double orient,
                     QObject* parent, const char* name);
    
    NomadSonarSensor(int id, double rad, double start_ang, double end_ang, int group_idx,
                     QObject* parent, const char* name);

    ~NomadSonarSensor();

    virtual int sensorType(void){return NOMAD_SONAR;}

    virtual int getRawReading(bool obtain = true);

protected:
    virtual void initMeasure(void);

};

#endif
