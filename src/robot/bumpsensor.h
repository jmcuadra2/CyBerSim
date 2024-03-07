/***************************************************************************
                          bumpsensor.h  -  description
                             -------------------
    begin                : s� ene 24 2004
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

#ifndef BUMPSENSOR_H
#define BUMPSENSOR_H

#include "pointssampledsensor.h"

/*! \ingroup sensors
    \brief Sensores de colision de Khepera
  */

class BumpSensor : public PointsSampledSensor
{

    Q_OBJECT

    friend class SensorFactory;

protected:
    BumpSensor(int id, int group_idx, double x_, double y_, double rot, double orient,
               QObject *parent = 0, const char* name = 0);
    BumpSensor(int id, int group_idx, double rad, double rot, double orient,
               QObject *parent = 0, const char* name = 0);
    BumpSensor(int id, int group_idx, QObject *parent = 0, const char* name = 0);

    void initMeasure(void);

public:
    ~BumpSensor();

    virtual int sensorSuperType(void);
    virtual int sensorType(void);
    double sample(void);

};

#endif
