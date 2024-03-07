//
// C++ Interface: fakeblobcamera
//
// Description:
//
//
// Author: Jose Manuel Cuadra Troncoso <jmcuadra@dia.uned.es>, (C) 2009
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef FAKEBLOBCAMERA_H
#define FAKEBLOBCAMERA_H

#include "rangebeamsensor.h"

/**
  @author Jose Manuel Cuadra Troncoso <jmcuadra@dia.uned.es>
*/
class FakeBlobCamera : public RangeBeamSensor
{
  public:
    FakeBlobCamera(int id, double rad, double start_ang, double end_ang, int group_idx, QObject* parent, const char* name);

    FakeBlobCamera(int id, int group_idx, double rad, double rot, double orient, QObject* parent, const char* name);

    FakeBlobCamera(int id, int group_idx, double x_, double y_, double rot, double orient, QObject* parent, const char* name);

    FakeBlobCamera(int id, int group_idx, QObject* parent, const char* name);

    ~FakeBlobCamera();

    double sample(void);
    int getRawReading(bool obtain = true);
    
    int sensorSuperType(void);
    int sensorType(void);
    void initSensor(void);

    void setZoom(double zoom);
    void setPan(double ang);

  protected:
    void initParams(void);    
    void initMeasure(void);
    void writeMeasure(void);
    void addReadingToSolver(int idx, int reading, CollisionSolver* solver);

  protected:
    double panIncrement, maxPan;
    QMap<CollisionSolver*, QVector <int> > blobs;
    QVector<int> largestBlob;
    double zoom;
    bool blobFound;
};

#endif
