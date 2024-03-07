/***************************************************************************
                          irsensor.cpp  -  description
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

#include "irsensor.h"
#include "robotposition.h"
#include "../neuraldis/ndmath.h"

int IRSensor::sensorSuperType(void)
{
    return POINTS_SAMPLED ;
}

int IRSensor::sensorType(void)
{
    return KHEP_PROXIMITY ;
}

IRSensor::IRSensor(int id, int group_idx, double x, double y, double rot, double orient,
                   QObject *parent, const char* name) :
            PointsSampledSensor(id,group_idx, x, y, rot, orient, parent, name)
{

}

IRSensor::IRSensor(int id, int group_idx, double rad, double rot, double orient,
                   QObject *parent, const char* name) :
            PointsSampledSensor(id, group_idx, rad, rot, orient, parent, name)
{

}

IRSensor::IRSensor(int id, int group_idx, QObject *parent, const char* name) :
            PointsSampledSensor(id, group_idx, parent, name)
{

}

IRSensor::~IRSensor()
{}

double IRSensor::sample(void)
{

    rawReading = 0;
    uint i, j, k;
    QListIterator<SensorPoint*> pnt(points);
    QVector<int> b = QVector<int>(3);
    b.fill(0);
    //for(i = 0; i < n_points; i += 5) {
    for(i = 0; pnt.hasNext(); i += 5){
        for(j = 0; j < 5; j++) {
            k = i+j;
            apointsval[k] = pnt.next()->sample();
            if(apointsval[k] > SensorPoint::SP_NO_SAMPLE) {
                b[i/5] = apointsval[k];
                pnt.toBack();
                break;
            }
        }
    }
    rawReading = b[0] + b[1] + b[2];
    if (rawReading < 7)
        rawReading = rand()%7;
    else if (rawReading > IRS_MAX_VAL)
        rawReading = IRS_MAX_VAL;
    else {
        rawReading = int(rawReading * (0.90 + NDMath::randDouble4(0.0, 2.0)*
                                       robot_position->noiseRatio()));
        if(rawReading > IRS_MAX_VAL)
            rawReading = IRS_MAX_VAL;
    }

    emit getSensorVal(rawReading, group_index);
    if(emitSignal) {
        emit getSensorValue(rawReading);
        emit getSensorFValue((double)rawReading, group_index);
    }
    out = double(rawReading)/double(IRS_MAX_VAL);
    writeMeasure();
    return out;
}

void IRSensor::initMeasure(void)
{
    vector<size_t> yDimensions(2);
    yDimensions[0] = 1;
    yDimensions[1] = 2;
    sensorPose = NDPose::createPose(NDPose::Pose2D);
    sensorPose->setCoordinates(x_org, y_org, orientation/NDMath::RAD2GRAD);
    measure.initMeasure(NDPose::Pose2D, idChar, "mm", yDimensions);
    measure.setPose(sensorPose);
}
