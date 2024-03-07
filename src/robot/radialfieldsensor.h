/***************************************************************************
                          radialfieldsensor.h  -  description
                             -------------------
    begin                : Mon Feb 7 2005
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

#ifndef RADIALFIELDSENSOR_H
#define RADIALFIELDSENSOR_H

#include "abstractsensor.h"
#include "../neuraldis/ndmath.h"

/*! \ingroup sensors 
    \brief Subclase base de los sensores de campo radial
  */

class RadialFieldSensor : public AbstractSensor  {
    Q_OBJECT

protected:
    RadialFieldSensor(int id, int group_idx, double x_, double y_, double rot,
                      double orient, double arc_, double no_f_val, double satur_val,
                      double normval, QObject *parent = 0, const char *name = 0);
    RadialFieldSensor(int id, int group_idx, double rad, double rot, double orient,
                      double arc_, double no_f_val, double satur_val, double normval,
                      QObject *parent = 0, const char *name = 0);
    RadialFieldSensor(int id, int group_idx, QObject *parent = 0, const char *name = 0);

public:
    ~RadialFieldSensor();

    int getRawReading(bool obtain = true);

    double sample(void);
    void advance(int )  {}
    void rotate(void) {}
    void showField(bool) {}
    void moveBy(double, double) {}
    void moveBy(double, double, double) {}
    void setPosition(double, double, double) {}
    void setSources(QList<RadialFieldSource*> srcs) {sources = srcs ; }
    QList<RadialFieldSource*>& getSources(void) {return sources ; }
    void addSource(RadialFieldSource* src);
    void scaleField(const double& scale);

protected:
    virtual double radialFunction(double d, double ang);
    virtual double normalizeOut(int val);
    void initParams(void);

protected:
    double arc;
    QList<RadialFieldSource*> sources;
    double norm_value;
    double no_field_value, saturation_value;

};

inline double RadialFieldSensor::normalizeOut(int p_out)
{  
    return (out = 1.0 - p_out/norm_value);
}

inline double RadialFieldSensor::radialFunction( double d, double ang)
{ 
    return (int)(cos(ang*(M_PI/arc))*d*(no_field_value - saturation_value)) ;
}

#endif
