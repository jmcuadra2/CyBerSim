//
// C++ Interface: ndrangedeviceinfowrapper
//
// Description: 
//
//
// Author: Jose Manuel Cuadra Troncoso <jmcuadra@dia.uned.es>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef NDRANGEDEVICEINFOWRAPPER_H
#define NDRANGEDEVICEINFOWRAPPER_H

#include <vector>
#include <Aria.h>
#include "arianetpacketwrapper.h"

using namespace std;

class NDSensorReading
{
  public:
//     class NDRangeDeviceInfoWrapper;
//     NDSensorReading(); 
    NDSensorReading(int value = 0, double x_local = 0.0, double y_local = 0.0, double x_global = 0.0, double y_global = 0.0) : value_(value), xLocal(x_local), yLocal(y_local), xGlobal(x_global), yGlobal(y_global) { }
    
    const int& value(void) { return value_ ; }
    const double& getXGlobal(void) { return xGlobal ; }
    const double& getYGlobal(void) { return yGlobal ; }
    void setValue(const int& value) { value_ = value ; }
    void setXGlobal(const double& x) { xGlobal = x ; }
    void setYGlobal(const double& y) { yGlobal = y ; }
    const double& getXLocal(void) { return xLocal ; }
    const double& getYLocal(void) { return yLocal ; }
    void setXLocal(const double& x) { xLocal = x ; }
    void setYLocal(const double& y) { yLocal = y ; }
    
    const ulong& getSecs(void) { return secs ; }
    const ulong& getMSecs(void) { return mSecs ; }
    void setSecs(const ulong& s) { secs = s ; }
    void setMSecs(const ulong& ms) { mSecs = ms ; }
    const double& getEncodPosX(void) { return encodPosX ; }
    void setEncodPosX(const double& posX) { encodPosX = posX ; }
    const double& getEncodPosY(void) { return encodPosY ; }
    void setEncodPosY(const double& posY) { encodPosY = posY ; }
    const double& getEncodPosTh(void) { return encodPosTh ; }
    void setEncodPosTh(const double& posTh) { encodPosTh = posTh ; }
    const double& getSensorPosX(void) { return sensorPosX ; }
    void setSensorPosX(const double& posX) { sensorPosX = posX ; }
    const double& getSensorPosY(void) { return sensorPosY ; }
    void setSensorPosY(const double& posY) { sensorPosY = posY ; }
    const double& getSensorPosTh(void) { return sensorPosTh ; }
    void setSensorPosTh(const double& posTh) { sensorPosTh = posTh ; }
    
  private:
    int value_;
    double xLocal;
    double yLocal;    
    double xGlobal;
    double yGlobal; 
    ulong secs, mSecs;
    double encodPosX, encodPosY, encodPosTh;
    double sensorPosX, sensorPosY, sensorPosTh;
    
};

/**
	@author Jose Manuel Cuadra Troncoso <jmcuadra@dia.uned.es>
*/
class NDRangeDeviceInfoWrapper : public AriaNetPacketWrapper
{
  public:
    NDRangeDeviceInfoWrapper();

    ~NDRangeDeviceInfoWrapper();
    int getRange(uint n_device);
    double getXGlobal(uint n_device);
    double getYGlobal(uint n_device);
    double getXLocal(uint n_device);
    double getYLocal(uint n_device);
    ArPose getEncoderPose(uint n_device);
    ulong getSecs(uint n_device);
    ulong getMSecs(uint n_device);   
    bool readPacket(ArNetPacket* packet);
    const ArTime& getTimeGet(void) { return timeGet ; };    

  protected:
    vector<NDSensorReading> ranges;
    ArTime timeGet;
};

#endif
