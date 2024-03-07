/***************************************************************************
                          kephlightsensor.cpp  -  description
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

#include "kheplightsensor.h"

int KhepLightSensor::sensorSuperType(void)
{ 
    return RADIAL_FIELD ;
}

int KhepLightSensor::sensorType(void)
{   
    return KHEP_LIGHT ;
}

KhepLightSensor::KhepLightSensor(int id, int group_idx, double x_, double y_, double rot,
                                 double orient, double arc_, double no_f_val,
                                 double satur_val, double normval, QObject *parent,
                                 const char *name) :
    RadialFieldSensor(id, group_idx, x_, y_, rot, orient, arc_,
                      no_f_val, satur_val, normval, parent, name)
{
}

KhepLightSensor::KhepLightSensor(int id, int group_idx, double rad,  double rot,
                                 double orient, double arc_, double no_f_val,
                                 double satur_val, double normval, QObject *parent,
                                 const char *name) :
    RadialFieldSensor(id, group_idx, rad, rot, orient, arc_, no_f_val,
                      satur_val, normval, parent, name)
{
}

KhepLightSensor::KhepLightSensor(int id, int group_idx, QObject *parent, const char *name) :
    RadialFieldSensor(id, group_idx, parent, name)
{
}

KhepLightSensor::~KhepLightSensor(){
}

int KhepLightSensor::getRawReading(bool obtain) 
{
    if(!obtain) return rawReading;
    RadialFieldSensor::getRawReading();

    rawReading -= int(rawReading*NDMath::randDouble4(-0.5, 0.5)*
                      robot_position->noiseRatio()); /* noise = noise->ratio/2 */
    return rawReading;
}

void KhepLightSensor::initMeasure(void)
{
    vector<size_t> yDimensions(2);
    yDimensions[0] = 1;
    yDimensions[1] = 2;
    sensorPose = NDPose::createPose(NDPose::Pose2D);
    sensorPose->setCoordinates(x_org, y_org, orientation/NDMath::RAD2GRAD);
    measure.initMeasure(NDPose::Pose2D, idChar, "mm", yDimensions);
    measure.setPose(sensorPose);
}
