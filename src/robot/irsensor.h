/***************************************************************************
                          irsensor.h  -  description
                             -------------------
    begin                : Tue Dec 16 2003
    copyright            : (C) 2003 by Jose M. Cuadra Troncoso
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

#ifndef IRSENSOR_H
#define IRSENSOR_H

#include "pointssampledsensor.h"
#include "../world/lamp.h"

#include <QObject>
#include <QPoint>
#include <QDomDocument>

#define N_IRSPOINTS    15
#define IRS_MAX_VAL    1023

class RobotPosition;

/*! \ingroup sensors 
    \brief Sensores de proximidad de Khepera
  */

class IRSensor : public PointsSampledSensor
{

    Q_OBJECT

    friend class SensorFactory;

protected:
    IRSensor(int id, int group_idx, double x, double y, double rot,
             double orient, QObject *parent = 0, const char* name = 0);
    IRSensor(int id, int group_idx, double rad, double rot, double orient,
             QObject *parent = 0, const char* name = 0);
    IRSensor(int id, int group_idx, QObject *parent = 0, const char* name = 0);
    
    void initMeasure(void);

public:
    ~IRSensor();

    virtual int sensorSuperType(void);
    virtual int sensorType(void);
    double sample(void);
    
};

#endif
