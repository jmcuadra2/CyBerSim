//
// C++ Interface: ariarangedevice
//
// Description: 
//
//
// Author: Jose Manuel Cuadra Troncoso <jmcuadra@dia.uned.es>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef ARIARANGEDEVICE_H
#define ARIARANGEDEVICE_H

#include "ariasensor.h"
#include "sensorline.h"
#include "../aria-tools/ndrangedeviceinfowrapper.h"

/**
  @author Jose Manuel Cuadra Troncoso <jmcuadra@dia.uned.es>
*/
class AriaRangeDevice : public AriaSensor
{
  public:
    AriaRangeDevice(int id, int group_idx, NDRangeDeviceInfoWrapper* sensor_inf, QObject* parent, const char* name);
    
    AriaRangeDevice(int id, int group_idx, double rad, double rot, double orient, NDRangeDeviceInfoWrapper* sensor_inf, QObject* parent, const char* name);
    
    AriaRangeDevice(int id, int group_idx, double x_, double y_, double rot, double orient, NDRangeDeviceInfoWrapper* sensor_inf, QObject* parent, const char* name);
    AriaRangeDevice(int id, double rad, double start_ang, double end_ang, int group_idx, NDRangeDeviceInfoWrapper* sensor_inf, QObject *parent = 0, const char *name = 0);    

    ~AriaRangeDevice();

    void advance(int stage);
    void moveBy(double dx, double dy);
    void moveBy(double dx, double dy, double ang);
    void rotate(void);
    void setPosition(double x_, double y_, double rot);
    void setVisible(bool on);
    void showField(bool show);
    double sample(void);
    void setSensorMaxVal ( int maxVal );
    
    int getRawReading(bool obtain = true);
    void writeMeasure(void);    
  
  protected:
    NDRangeDeviceInfoWrapper* sensor_info;
    ArPose encoderPose;
    
  protected:
//     int getRawReading(bool obtain = true);
//     SensorLine::Drawing lineDrawingType(void) { return SensorLine::Rectangle ; };
};

#endif
