/***************************************************************************
                          bumpsensor.cpp  -  description
                             -------------------
    begin                : sábado ene 24 2004
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

#include "robotposition.h" 
#include "bumpsensor.h"

int BumpSensor::sensorSuperType(void)
{

    return POINTS_SAMPLED ;

}

int BumpSensor::sensorType(void)
{

    return KHEP_BUMP ;

}

BumpSensor::BumpSensor(int id, int group_idx, double x_, double y_, double rot, double orient, QObject *parent, const char* name)
    : PointsSampledSensor(id, group_idx, x_, y_ , rot, orient, parent, name)
{
}

BumpSensor::BumpSensor(int id, int group_idx, double rad, double rot, double orient, QObject *parent, const char* name)
    : PointsSampledSensor(id, group_idx, rad, rot, orient, parent, name)
{
}


BumpSensor::BumpSensor(int id, int group_idx, QObject *parent, const char* name) : PointsSampledSensor(id, group_idx, parent, name)
{
}


BumpSensor::~BumpSensor()
{}

double BumpSensor::sample(void)
{
    double p_out = out;
    PointsSampledSensor::sample();
    if(p_out > 0.0 || out > 0.0) {
        emit getSensorVal(NDMath::roundInt(out), group_index);
        if(emitSignal) {
            emit getSensorValue(NDMath::roundInt(out));
            emit getSensorFValue((double)NDMath::roundInt(out), group_index);
        }
    }
    writeMeasure();
    return out;

}

void BumpSensor::initMeasure(void)
{
    vector<size_t> yDimensions(2);
    yDimensions[0] = 1;
    yDimensions[1] = 2;
    sensorPose = NDPose::createPose(NDPose::Pose2D);
    sensorPose->setCoordinates(x_org, y_org, orientation/NDMath::RAD2GRAD);
    measure.initMeasure(NDPose::Pose2D, idChar, "logical", yDimensions);
    measure.setPose(sensorPose);
}

