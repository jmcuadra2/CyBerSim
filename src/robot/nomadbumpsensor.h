//
// C++ Interface: nomadbumpsensor
//
// Description: 
//
//
// Author: Javier Garcia Misis <>, (C) 2010
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef NOMADBUMPSENSOR_H
#define NOMADBUMPSENSOR_H

#include "nomadsensor.h"
//#include "linearsensor.h"

/**
    @author Javier Garcia Misis
*/

class NomadBumpSensor : public NomadSensor
        //class NomadBumpSensor : public LinearSensor
{
public:
    NomadBumpSensor(int id, int group_idx, QObject* parent, const char* name);
    
    NomadBumpSensor(int id, int group_idx, double x_, double y_, double rot,
                    double orient, QObject* parent, const char* name);
    
    NomadBumpSensor(int id, int group_idx, double rad, double rot, double orient,
                    QObject* parent, const char* name);
    
    NomadBumpSensor(int id, double rad, double start_ang, double end_ang, int group_idx,
                    QObject* parent, const char* name);

    ~NomadBumpSensor();

    virtual int sensorType(void){ return NOMAD_BUMP;}

    virtual double sample(void);

    int getRawReading(bool obtain = true);


    virtual SensorLine* createSensorLine(AbstractWorld* world, DrawingItem::Drawing drawing_type = DrawingItem::Rectangle, const QString& color = "", const QString& color2 = "", double angle = 0, bool rotateWithRobot = true);

protected:
    void initMeasure(void);

};

#endif
