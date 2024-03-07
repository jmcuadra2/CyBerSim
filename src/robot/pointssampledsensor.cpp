/***************************************************************************
                          pointssampledsensor.cpp  -  description
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

#include "pointssampledsensor.h"
#include "../neuraldis/ndmath.h"

#include <QPainter>

PointsSampledSensor::PointsSampledSensor(int id, int group_idx, double x_, double y_,
                           double rot, double orient, QObject *parent, const char *name) :
                    AbstractSensor(id, group_idx, x_, y_, rot, orient, parent, name)
{
}

PointsSampledSensor::PointsSampledSensor(int id, int group_idx, double rad, double rot,
                                         double orient, QObject *parent, const char *name) :
                    AbstractSensor(id, group_idx, rad, rot, orient, parent, name)
{
}

PointsSampledSensor::PointsSampledSensor(int id, int group_idx, QObject *parent,
                                         const char *name) :
                    AbstractSensor(id, group_idx, parent, name)
{
}

PointsSampledSensor::~PointsSampledSensor()
{
    QListIterator<SensorPoint*> it(points);
    while(it.hasNext())
        delete it.next();

}

void PointsSampledSensor::changePointsPos(QVector<double> points_sx, QVector<double> points_sy,
                                          QVector<double> , double robot_rot)
{

    QListIterator<SensorPoint*> it(points);
    SensorPoint *sp;
    int i = 0;
    QPointF p;
    while(it.hasNext()) {
        sp = it.next();
        p = NDMath::rotateGrad(points_sx[i], points_sy[i], x_org, y_org,
                               orientation);
        p = NDMath::rotateGrad(p.x(), p.y(), robot_position->x(), robot_position->y(), robot_rot);
        sp->changeRelativePos(p.x(), p.y());
        ++i;
    }

}

void PointsSampledSensor::addPoint(SensorPoint* s_point)
{

    points.append(s_point);
    apointsval.resize(points.count());
    apointsval[points.count() - 1] = 0;

}

void PointsSampledSensor::advance(int stage)
{

    QListIterator<SensorPoint*> pnt(points);
    while(pnt.hasNext())
        pnt.next()->advance(stage);
}

void PointsSampledSensor::rotate(void)
{

    QListIterator<SensorPoint*> pnt(points);
    while(pnt.hasNext())
        pnt.next()->rotate();
}

double PointsSampledSensor::sample(void)
{

    int i = 0;
    QListIterator<SensorPoint*> pnt(points);
    if(!pnt.hasNext())
        out = pnt.previous()->sample();
    else  {
        while(pnt.hasNext()) {
            apointsval[i] = pnt.next()->sample();
            ++i;
        }
        out = getRawReading();
    }
    prev_out = out;
    return out;

}

int PointsSampledSensor::getRawReading(bool obtain)
{
    if(!obtain)
        return rawReading;

    uint i;
    uint n_points = points.count();
    rawReading = 0;
    for(i=0; i < n_points; i++) {
        if (apointsval[i] > SensorPoint::SP_NO_SAMPLE) {
            rawReading= apointsval[i];
            break;
        }
    }
    return rawReading;

}

void PointsSampledSensor::showField(bool show)
{

    QListIterator<SensorPoint*> pnt(points);
    SensorPoint *sp;
    while(pnt.hasNext()) {
        sp = pnt.next();
        if(show) {
            sp->setVisible(true);
        }
        else {
            sp->setVisible(false);
        }
    }
}

void PointsSampledSensor::setPosition(double x_, double y_, double rot)
{

    AbstractSensor::setPosition(x_, y_, rot);
    moveBy(x_, y_, rot);

}

void PointsSampledSensor::moveBy(double dx, double dy)
{

//    QListIterator<SensorPoint*> pnt(points);
//    while(pnt.hasNext())
//        pnt.next()->moveBy(dx,dy);
    
}

void PointsSampledSensor::moveBy(double dx, double dy, double ang)
{

    QListIterator<SensorPoint*> pnt(points);
    while(pnt.hasNext())
        pnt.next()->rotate(dx,dy,ang);
    
}

void PointsSampledSensor::setVisible(bool on)
{

    QListIterator<SensorPoint*> pnt(points);
    while(pnt.hasNext())
        pnt.next()->setVisible(on);

}
