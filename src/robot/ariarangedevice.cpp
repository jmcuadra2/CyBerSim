//
// C++ Implementation: ariarangedevice
//
// Description: 
//
//
// Author: Jose Manuel Cuadra Troncoso <jmcuadra@dia.uned.es>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "ariarangedevice.h"

AriaRangeDevice::AriaRangeDevice(int id, int group_idx, NDRangeDeviceInfoWrapper* sensor_inf, QObject* parent, const char* name): AriaSensor(id, group_idx, sensor_info, parent, name)
{
  sensor_info = sensor_inf;
}


AriaRangeDevice::AriaRangeDevice(int id, int group_idx, double rad, double rot, double orient, NDRangeDeviceInfoWrapper* sensor_inf, QObject* parent, const char* name): AriaSensor(id, group_idx, rad, rot, orient, sensor_info, parent, name)
{
  sensor_info = sensor_inf;
}

AriaRangeDevice::AriaRangeDevice(int id, int group_idx, double x_, double y_, double rot, double orient, NDRangeDeviceInfoWrapper* sensor_inf, QObject* parent, const char* name): AriaSensor(id, group_idx, x_, y_, rot, orient, sensor_info, parent, name)
{
  sensor_info = sensor_inf;
}

AriaRangeDevice::AriaRangeDevice(int id, double rad, double start_ang, double end_ang, int group_idx, NDRangeDeviceInfoWrapper* sensor_inf, QObject* parent, const char* name): AriaSensor(id, rad, start_ang, end_ang, group_idx, sensor_inf, parent, name)
{
  sensor_info = sensor_inf;
}

AriaRangeDevice::~AriaRangeDevice()
{
}

void AriaRangeDevice::advance(int stage)
{
    if (sensorLine) sensorLine->advance(stage);
}

void AriaRangeDevice::moveBy(double dx, double dy)
{
//    if (sensorLine) sensorLine->moveBy(dx,dy);
}

void AriaRangeDevice::moveBy(double dx, double dy, double ang)
{
  if (sensorLine) sensorLine->rotate(dx,dy,ang);
}

void AriaRangeDevice::rotate(void)
{
  if (sensorLine) sensorLine->rotate();
}

void AriaRangeDevice::setPosition(double x_, double y_, double rot)
{
    AbstractSensor::setPosition(x_, y_, rot);
    moveBy(x_,y_,rot);
}

void AriaRangeDevice::setVisible(bool on)
{
    sensorLine->setVisible(on);
}

void AriaRangeDevice::showField(bool show)
{
    sensorLine->setShowColor(show);
}

double AriaRangeDevice::sample(void)
{
  rawReading = getRawReading();
  if(rawReading >= sensorWrongVal) {
//     if(prev_out < sensorCriticVal)
//       prev_out = 0.9*prev_out;
    out = prev_out;
    sensorLine->setValidReading(false);
    sensorLine->scale(xReading(), yReading());
  }
  else {
    if(rawReading >= sensorMaxVal)
      rawReading = sensorMaxVal;
    out = 1.0 - double(rawReading)/sensorMaxVal;
    prev_out = out;
    sensorLine->setValidReading(true);  
    sensorLine->scale(xReading(), yReading());    
  }
  writeMeasure();
  return out;
}

int AriaRangeDevice::getRawReading(bool obtain ) 
{
  if(!obtain) return rawReading;
  rawReading = sensor_info->getRange(group_index);
  rawReading = rawReading < 0 ? sensorWrongVal : rawReading;
  rawReading = rawReading > sensorWrongVal ? sensorWrongVal : rawReading;

  sensorLine->setLength(rawReading);
  x_reading = sensorLine->getGlobalIntersectionPoint().x();
  y_reading = sensorLine->getGlobalIntersectionPoint().y();

  encoderPose = sensor_info->getEncoderPose(group_index);
  return rawReading;
}



void AriaRangeDevice::setSensorMaxVal(int maxVal)
{
  sensorMaxVal = maxVal;  // TODO
}

void AriaRangeDevice::writeMeasure(void) {
  AriaSensor::writeMeasure();
  measure.setTimeStamp(TimeStamp(sensor_info->getSecs(group_index), sensor_info->getMSecs(group_index)*1000, false));
 
}
