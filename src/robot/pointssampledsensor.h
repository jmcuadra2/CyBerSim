/***************************************************************************
                          pointssampledsensor.h  -  description
                             -------------------
    begin                : Wed Jan 21 2004
    copyright            : (C) 2004 by Jose M. Cuadra Troncoso
    email                : jmcuadra@dia.uned.es
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef POINTSSAMPLEDSENSOR_H
#define POINTSSAMPLEDSENSOR_H

#include "sensorpoint.h"
#include "abstractsensor.h"

#include <QDomDocument>

class RobotPosition;
class SensorFactory;

/*! \ingroup Sensores Los sensores basados en puntos
    \brief renombrar PointedSampledSensor
  */

class PointsSampledSensor : public AbstractSensor
{
    Q_OBJECT

protected:
    PointsSampledSensor(int id, int group_idx, double x_, double y_, double rot,
                        double orient, QObject *parent = 0, const char *name = 0);
    PointsSampledSensor(int id, int group_idx, double rad, double rot, double orient,
                        QObject *parent = 0, const char *name = 0);
    PointsSampledSensor(int id, int group_idx, QObject *parent = 0, const char *name = 0);
    
public:
    ~PointsSampledSensor();

    int getRawReading(bool obtain = true);

    void advance(int stage);
    void rotate(void);
    void showField(bool show);
    void moveBy(double dx, double dy);
    void moveBy(double dx, double dy, double ang);

    virtual void changePointsPos(QVector<double> points_sx, QVector<double> points_sy,
                                 QVector<double> orientations, double robot_rot);
    double sample(void);
    void setPosition(double x_, double y_, double rot);
    void setVisible(bool on);
    virtual void addPoint(SensorPoint* s_point);

protected:
    QList<SensorPoint*> points;
    QVector<int> apointsval;

};

#endif
