/***************************************************************************
                          pointssampledsensorgroup.h  -  description
                             -------------------
    begin                : Wed Feb 23 2005
    copyright            : (C) 2005 by Jose M. Cuadra Troncoso
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

#ifndef POINTSSAMPLEDSENSORGROUP_H
#define POINTSSAMPLEDSENSORGROUP_H

#include "sensorgroup.h"
#include "pointssampledsensor.h"

#include <QColor>
#include <QPixmap>

/*! \ingroup sensors 
    \brief Crea grupos de sensores tomando como subclase base
    PointedSampledSensor y los situa respecto del robot
  */

class PointsSampledSensorGroup : public SensorGroup  {

public:
    PointsSampledSensorGroup(int sens_type);
    PointsSampledSensorGroup(const QDomElement& e);
    virtual ~PointsSampledSensorGroup();

    bool createSensors(void);
    void scaleSensorField(const double& scale);
    static QList<QPixmap> paintSequence(QColor *vis_color = 0, QColor *invis_color = 0,
                                        int w = 2, int h = 2);

protected:
    void write_special(QDomDocument& doc, QDomElement& e);
    void readPointsArrays(const QDomElement& e);

    
protected:
    QVector<double> points_sx;
    QVector<double> points_sy;
    QVector<int> points_vals;
    QList<QPixmap> points_pix_list;
    QColor vis_color, invis_color;
    QString visible_color, invisible_color;

private:
    QList<PointsSampledSensor*> points_sampled_sensors;
    
};

#endif
