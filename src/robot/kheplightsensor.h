/***************************************************************************
                          kephlightsensor.h  -  description
                             -------------------
    begin                : Tue Feb 8 2005
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

#ifndef KHEPLIGHTSENSOR_H
#define KHEPLIGHTSENSOR_H

#include "radialfieldsensor.h"
#include "robotposition.h"

class RobotPosition;

/*! \ingroup sensors
    \brief Sensores de luz de Khepera
  */

class KhepLightSensor : public RadialFieldSensor  {

    Q_OBJECT

    friend class SensorFactory;

protected:
    KhepLightSensor(int id, int group_idx, double x_, double y_, double rot,
                    double orient, double arc_, double no_f_val, double satur_val,
                    double normval, QObject *parent = 0, const char *name = 0);
    KhepLightSensor(int id, int group_idx, double rad, double rot, double orient,
                    double arc_, double no_f_val, double satur_val, double normval,
                    QObject *parent = 0, const char *name = 0);
    KhepLightSensor(int id, int group_idx, QObject *parent = 0, const char *name = 0);

    void initMeasure(void);

public:
    ~KhepLightSensor();

    virtual int sensorSuperType(void);
    virtual int sensorType(void);
    void scaleField(const double& scale);
    int getRawReading(bool obtain = true);

};

#endif
