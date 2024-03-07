//
// C++ Implementation: rangelinesensor
//
// Description: 
//
//
// Author: Jose Manuel Cuadra Troncoso <jmcuadra@dia.uned.es>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "rangelinesensor.h"

RangeLineSensor::RangeLineSensor(int id, int group_idx, QObject* parent, const char* name) :
    LinearSensor(id, group_idx, parent, name)
{
    initParams();
}


RangeLineSensor::RangeLineSensor(int id, int group_idx, double x_, double y_, double rot,
                                 double orient, QObject* parent, const char* name) :
    LinearSensor(id, group_idx, x_, y_, rot, orient, parent, name)
{
    initParams();
}


RangeLineSensor::RangeLineSensor(int id, int group_idx, double rad, double rot,
                                 double orient, QObject* parent, const char* name) :
    LinearSensor(id, group_idx, rad, rot, orient, parent, name)
{
    initParams();
}

RangeLineSensor::RangeLineSensor(int id, double rad, double start_ang, double end_ang,
                                 int group_idx, QObject* parent, const char* name) :
    LinearSensor(id, rad, start_ang, end_ang, group_idx, parent, name)
{
    initParams();
}

RangeLineSensor::RangeLineSensor(int id, double rad, double start_ang, double end_ang,
                                 int group_idx, double x_, double y_, QObject* parent,
                                 const char* name) :
    LinearSensor(id, rad, start_ang, end_ang, group_idx, x_, y_, parent, name)
{
    initParams();
}


RangeLineSensor::~RangeLineSensor()
{
}

void RangeLineSensor::initParams(void)
{
    sensorMaxVal = 5000;
    sensorWrongVal = 5000;
    sensorCriticVal = 100;

    aperture = 1/NDMath::RAD2GRAD;

    sigma = 5;
}

int RangeLineSensor::sensorSuperType(void)
{
    return LINEAR_SENSOR;
}

int RangeLineSensor::sensorType(void)
{
    return RANGE_LINE_SENSOR;
}

double RangeLineSensor::sample(void)
{
    rawReading = getRawReading();
    if(rawReading >= sensorWrongVal) {
        if(prev_out < sensorCriticVal)
            prev_out = 0.9*prev_out;
        out = prev_out;
        sensorLine->setValidReading(false);
        sensorLine->scale(xReading(), yReading());
        if(emitSignal) {
            emit getSensorValue(-1);
            emit getSensorFValue(-1.0, ident);
        }
    }
    else {
        if(rawReading >= sensorMaxVal)
            rawReading = sensorMaxVal;
        out = 1.0 - (double) rawReading/sensorMaxVal;
        prev_out = out;
        sensorLine->setValidReading(true);
        sensorLine->scale(xReading(), yReading());
        if(emitSignal) {
            emit getSensorValue(rawReading);
            emit getSensorFValue((double)rawReading, ident);
        }
    }
    writeMeasure();
    return out;
}

void RangeLineSensor::initMeasure(void)
{
    vector<size_t> yDimensions(2);
    yDimensions[0] = 1;
    yDimensions[1] = 2;
    sensorPose = NDPose::createPose(NDPose::Pose2D);
    sensorPose->setCoordinates(x_org, y_org, orientation/NDMath::RAD2GRAD);
    measure.initMeasure(NDPose::Pose2D, idChar, "mm", yDimensions);
    measure.setPose(sensorPose);
}
