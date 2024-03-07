/***************************************************************************
                          radialfieldsensor.cpp  -  description
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

#include "radialfieldsensor.h"
#include "robotposition.h"

RadialFieldSensor::RadialFieldSensor(int id, int group_idx, double x_,
                            double y_, double rot, double orient, double arc_,
                            double no_f_val, double satur_val, double normval,
                                     QObject *parent, const char *name) :
                AbstractSensor(id, group_idx, x_, y_, rot, orient, parent, name)
{

    arc = arc_;
    no_field_value = no_f_val;
    saturation_value = satur_val;
    norm_value  = normval;

}

RadialFieldSensor::RadialFieldSensor(int id, int group_idx, double rad, double rot,
                            double orient, double arc_, double no_f_val, double satur_val,
                            double normval, QObject *parent, const char *name) :
                AbstractSensor(id, group_idx, rad, rot, orient, parent, name)
{

    arc = arc_;
    no_field_value = no_f_val;
    saturation_value = satur_val;
    norm_value  = normval;

}

RadialFieldSensor::RadialFieldSensor(int id, int group_idx, QObject *parent,
                                     const char *name) :
                AbstractSensor(id, group_idx, parent, name)
{

    arc = 120;
    no_field_value = 500;
    saturation_value = 50;
    norm_value  = 525;

}

RadialFieldSensor::~RadialFieldSensor()
{
}

void RadialFieldSensor::initParams(void)
{
    AbstractSensor::initParams();
    sensorMaxVal = no_field_value;
    sensorWrongVal = sensorMaxVal;
    sensorCriticVal = saturation_value;

}
double RadialFieldSensor::sample(void)
{
    getRawReading();
    if(rawReading >= no_field_value) {
        out = 0;
        prev_out = out;
        emit getSensorVal(-1, group_index);
        if(emitSignal) {
            emit getSensorValue(-1);
            emit getSensorFValue(-1.0, group_index);
        }
    }
    else {
        if (rawReading < saturation_value)
            rawReading = int(saturation_value);
        out = normalizeOut(rawReading);
        prev_out = out;
        emit getSensorVal(rawReading, group_index);
        if(emitSignal) {
            emit getSensorValue(rawReading);
            emit getSensorFValue((double)rawReading, group_index);
        }
    }
    writeMeasure();
    return out;
}

int RadialFieldSensor::getRawReading(bool obtain) 
{
    if(!obtain) return rawReading;
    double ang, d, dx, dy, rx, ry;
    RadialFieldSource *src;
    QListIterator<RadialFieldSource*> it(sources);
    rawReading = int(no_field_value);   // maxima distancia a la que se percibe

    rx = robot_position->x();
    ry = robot_position->y();

    while (it.hasNext()) {
        src = it.next();
        if(src->isOn()) {
            ang = robot_position->rotation()/NDMath::RAD2GRAD;
            dx = src->xCenter() - (rx+x_org*cos(ang) - y_org*sin(ang));
            dy = src->yCenter() - (ry+x_org*sin(ang) + y_org*cos(ang));
            d = sqrt(dx*dx + dy*dy);
            d = (no_field_value - sqrt(dx*dx + dy*dy))/no_field_value;
            if (d > 0) {
                ang = -ang + atan2(dy,dx) - rotation/NDMath::RAD2GRAD;
                while(ang > M_PI) ang -= 2*M_PI;
                while(ang < -M_PI) ang += 2*M_PI;
                if ((ang > -arc/2.0)&&(ang < arc/2.0))
                    rawReading -= (int)(radialFunction(d, ang));
            }
        }
    }
    return rawReading;
}

void RadialFieldSensor::scaleField(const double& scale)
{

    if(scale > 0.0) {
        no_field_value *= scale;
        norm_value = no_field_value + saturation_value/2.0;
    }

}


void RadialFieldSensor::addSource(RadialFieldSource* src)
{   
    sources.append(src);
}
