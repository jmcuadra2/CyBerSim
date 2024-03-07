/***************************************************************************
                          radialfieldsensorgroup.h  -  description
                             -------------------
    begin                : Thu Feb 24 2005
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

#ifndef RADIALFIELDSENSORGROUP_H
#define RADIALFIELDSENSORGROUP_H

#include "sensorgroup.h"
#include "radialfieldsensor.h"

class SensorFactory;  

/*! \ingroup sensors
    \brief Crea grupos de sensores tomando como subclase base
    RadialFieldSensor

    Los situa respecto del robot y mantiene una lista de las fuentes de campo
  */

class RadialFieldSensorGroup : public SensorGroup  {

public:
    RadialFieldSensorGroup(int sens_type);
    RadialFieldSensorGroup(const QDomElement& e);
    ~RadialFieldSensorGroup();

    bool createSensors(void);
    void scaleSensorField(const double& scale);
    void setMaxDistance(double max_dist) { no_field_value = max_dist ; }
    void worldCleared(void);
    void addSources(QList<RadialFieldSource*> sources);

protected:
    void write_special(QDomDocument& doc, QDomElement& e);

protected:
    double arc;
    double no_field_value;
    double saturation_value;
    double norm_value;

private:
    QList<RadialFieldSensor*> radial_field_sensors;
    
};

#endif
