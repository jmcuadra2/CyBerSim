//
// C++ Interface: rangebeamsensor
//
// Description:
//
//
// Author: Jose Manuel Cuadra Troncoso <jmcuadra@dia.uned.es>, (C) 2009
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef RANGEBEAMSENSOR_H
#define RANGEBEAMSENSOR_H

#include "rangelinesensor.h"

/**
  @author Jose Manuel Cuadra Troncoso <jmcuadra@dia.uned.es>
*/
class RangeBeamSensor : public RangeLineSensor
{
public:
    RangeBeamSensor(int id, int group_idx, QObject* parent, const char* name);

    RangeBeamSensor(int id, int group_idx, double x_, double y_, double rot, double orient,
                    QObject* parent, const char* name);

    RangeBeamSensor(int id, int group_idx, double rad, double rot, double orient,
                    QObject* parent, const char* name);

    RangeBeamSensor(int id, double rad, double start_ang, double end_ang, int group_idx,
                    QObject* parent, const char* name);

    ~RangeBeamSensor();

    void setVisible(bool on);
    void showField(bool show);
    void rotate(void);
    void advance(int stage);
    void moveBy(double dx, double dy);
    void moveBy(double dx, double dy, double ang);

    virtual int sensorSuperType(void);
    virtual int sensorType(void);
    double sample(void);

    int getRawReading(bool obtain = true);

protected:
    void initParams(void);

protected:
    double minApertureFraction;
    void paintLine(int idx, int auxRawReading);
    virtual void addReadingToSolver(int idx, int reading, CollisionSolver* solver);
    
protected:
    QMap<CollisionSolver*, QVector<QPoint> > mapCollisionSolver;

};

#endif
