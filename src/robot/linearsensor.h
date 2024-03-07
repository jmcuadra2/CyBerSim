//
// C++ Interface: linearsensor
//
// Description: 
//
//
// Author: Jose Manuel Cuadra Troncoso <jmcuadra@dia.uned.es>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef LINEARSENSOR_H
#define LINEARSENSOR_H

#include "abstractsensor.h"

/**
  @author Jose Manuel Cuadra Troncoso <jmcuadra@dia.uned.es>
*/

class LinearSensor : public AbstractSensor
{
public:
    LinearSensor(int id, int group_idx, double rad, double rot, double orient, QObject* parent, const char* name);
    
    LinearSensor(int id, int group_idx, double x_, double y_, double rot, double orient, QObject* parent, const char* name);
    
    LinearSensor(int id, int group_idx, QObject* parent, const char* name);
    LinearSensor(int id, double rad, double start_ang,
                 double end_ang, int group_idx, QObject *parent = 0,
                 const char *name = 0);
    LinearSensor(int id, double rad, double start_ang,
                 double end_ang, int group_idx, double x_, double y_, QObject *parent = 0,
                 const char *name = 0);

    ~LinearSensor();

    void advance(int stage);
    void moveBy(double dx, double dy);
    void moveBy(double dx, double dy, double ang);
    void rotate(void);
    void setPosition(double x_, double y_, double rot);
    void setVisible(bool on);
    void showField(bool show);

    QList<SensorLine*>  getSensorLines() const { return sensorLines ; }
    virtual QList<SensorLine*> createSensorLines(AbstractWorld* world,
                               DrawingItem::Drawing drawing_type = DrawingItem::Rectangle,
                               const QString& color = "", const QString& color2 = "",
                               const QString& beam_color = "", int resolution = 0,
                               bool showBeam = false);
    
    int getRawReading(bool obtain = true);
    void setProbabilisticParams(double sigma);
    int getBeamResolution() const { return beamResolution; }

protected:
    QVector<int> rawReadings;
    QVector<double> xReadings;
    QVector<double> yReadings;
    QList<SensorLine*> sensorLines;
    double sigma;
    bool showBeam;
    int beamResolution;

};

#endif
