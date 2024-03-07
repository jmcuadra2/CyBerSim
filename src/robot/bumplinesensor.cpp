//
// C++ Implementation: bumplinesensor
//
// Description: 
//
//
// Author: Jose Manuel Cuadra Troncoso <jmcuadra@dia.uned.es>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "bumplinesensor.h"

BumpLineSensor::BumpLineSensor(int id, int group_idx, QObject* parent, const char* name): LinearSensor(id, group_idx, parent, name)
{
  initParams();
}


BumpLineSensor::BumpLineSensor(int id, int group_idx, double x_, double y_, double rot, double orient, QObject* parent, const char* name): LinearSensor(id, group_idx, x_, y_, rot, orient, parent, name)
{
  initParams();
}


BumpLineSensor::BumpLineSensor(int id, int group_idx, double rad, double rot, double orient, QObject* parent, const char* name): LinearSensor(id, group_idx, rad, rot, orient, parent, name)
{
  initParams();
}


BumpLineSensor::BumpLineSensor(int id, double rad, double start_ang, double end_ang, int group_idx, QObject* parent, const char* name): LinearSensor(id, rad, start_ang, end_ang, group_idx, parent, name)
{
  initParams();
}


BumpLineSensor::~BumpLineSensor()
{
}

void BumpLineSensor::initParams(void)
{
  sensorMaxVal = 100;
  sensorWrongVal = 0;
  sensorCriticVal = 0;
  aperture = 10/NDMath::RAD2GRAD;
}

int BumpLineSensor::sensorSuperType(void)
{
    return LINEAR_SENSOR;
}

int BumpLineSensor::sensorType(void)
{
    return BUMP_LINE_SENSOR;
}

double BumpLineSensor::sample(void)
{

  rawReading = getRawReading();
  if(rawReading > sensorWrongVal) {
    rawReading = 1;
    out = 1;
    sensorLine->setValidReading(true);
    sensorLine->scale(xReading(), yReading());
  }
  else {
    rawReading = 0;
    out = 0;
    prev_out = out;  
    sensorLine->setValidReading(true);  
    sensorLine->scale(xReading(), yReading());    
  }
//   if(p_raw == 1 || rawReading == 1) {
// //     emit getSensorVal(rawReading, group_index);
//     if(emitSignal) {
//       emit getSensorValue(rawReading);
//       emit getSensorFValue((double)rawReading, group_index);
//     } 
//   }
  if(emitSignal) {
    emit getSensorValue(rawReading);
    emit getSensorFValue((double)rawReading, group_index + 1);
  }
  writeMeasure();
  return out;
  
}

void BumpLineSensor::initMeasure(void)
{

  vector<size_t> yDimensions(2);
  yDimensions[0] = 1;
  yDimensions[1] = 2;
  sensorPose = NDPose::createPose(NDPose::Pose2D);
  sensorPose->setCoordinates(x_org, y_org, orientation/NDMath::RAD2GRAD);
  measure.initMeasure(NDPose::Pose2D, idChar, "logical", yDimensions);
  measure.setPose(sensorPose);  
}
