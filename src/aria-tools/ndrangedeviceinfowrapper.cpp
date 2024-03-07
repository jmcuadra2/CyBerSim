//
// C++ Implementation: ndrangedeviceinfowrapper
//
// Description: 
//
//
// Author: Jose Manuel Cuadra Troncoso <jmcuadra@dia.uned.es>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "ndrangedeviceinfowrapper.h"


#include <ArNetworking.h>

NDRangeDeviceInfoWrapper::NDRangeDeviceInfoWrapper()
 : AriaNetPacketWrapper()
{
}


NDRangeDeviceInfoWrapper::~NDRangeDeviceInfoWrapper()
{
}

/*!
    \fn NDRangeDeviceInfoWrapper::readPacket(ArNetPacket* packet)
 */
bool NDRangeDeviceInfoWrapper::readPacket(ArNetPacket* packet)
{
  char info_packet[RangeHeaderLenght];
  packet->bufToStr(info_packet, RangeHeaderLenght - 1);
  if(std::string(info_packet) != std::string(packet_name)) {
    std::string message = std::string("It isn't a ") + packet_name + std::string(" packet");
    ArLog::log(ArLog::Terse, message.c_str());
    return false;
  }
  
  uint num_devices = packet->bufToByte2();
  if(num_devices) {
    if(ranges.size() != num_devices)
      ranges.resize(num_devices);

//       ArLog::log(ArLog::Terse, "Device %s", packet_name);
    
    for (uint i = 0; i < num_devices; i++) {
      ranges[i].setValue(packet->bufToByte4());
      ranges[i].setXLocal(packet->bufToByte4());
      ranges[i].setYLocal(packet->bufToByte4());
      ranges[i].setXGlobal(packet->bufToByte4());
      ranges[i].setYGlobal(packet->bufToByte4());
      
      ranges[i].setSecs(packet->bufToByte4());
      ranges[i].setMSecs(packet->bufToByte4());
      ranges[i].setEncodPosX(packet->bufToDouble());
      ranges[i].setEncodPosY(packet->bufToDouble());
      ranges[i].setEncodPosTh(packet->bufToDouble());
      ranges[i].setSensorPosX(packet->bufToByte4());
      ranges[i].setSensorPosY(packet->bufToByte4());
      ranges[i].setSensorPosTh(packet->bufToByte4());
      
//      ArLog::log(ArLog::Terse, "Time = %lu.%lu EncoderPose = (%f, %f, %f) SensorPose = (%f, %f, %f)\n", ranges[i].getSecs(), ranges[i].getMSecs()
//        , ranges[i].getEncodPosX(), ranges[i].getEncodPosY(), ranges[i].getEncodPosTh(), ranges[i].getSensorPosX(), ranges[i].getSensorPosY(), ranges[i].getSensorPosTh());
//       ArLog::log(ArLog::Terse, "%d Time = %lu.%lu Value = %f", i, ranges[i].getSecs(), ranges[i].getMSecs(), ranges[i].value());  
    }
    ArLog::log(ArLog::Terse, "Time = %lu.%lu EncoderPose = (%f, %f, %f) SensorPose = (%f, %f, %f)\n", ranges[0].getSecs(), ranges[0].getMSecs()
      , ranges[0].getEncodPosX(), ranges[0].getEncodPosY(), ranges[0].getEncodPosTh(), ranges[0].getSensorPosX(), ranges[0].getSensorPosY(), ranges[0].getSensorPosTh());

    timeGet.setToNow();
    waiting = false;
  }

  return true;

}

int NDRangeDeviceInfoWrapper::getRange(uint n_device) 
{
  uint num_devices = ranges.size();  
  int ret = n_device < num_devices ? ranges[n_device].value() : -1; 
  return ret;
}

double NDRangeDeviceInfoWrapper::getXGlobal(uint n_device)
{
  uint num_devices = ranges.size();  
  double ret = n_device < num_devices ? ranges[n_device].getXGlobal() : 0; 
  return ret;
}

double NDRangeDeviceInfoWrapper::getYGlobal(uint n_device)
{
  uint num_devices = ranges.size();  
  double ret = n_device < num_devices ? ranges[n_device].getYGlobal() : 0; 
  return ret;
}

double NDRangeDeviceInfoWrapper::getXLocal(uint n_device)
{
  uint num_devices = ranges.size();  
  double ret = n_device < num_devices ? ranges[n_device].getXLocal() : 0; 
  return ret;
}

double NDRangeDeviceInfoWrapper::getYLocal(uint n_device)
{
  uint num_devices = ranges.size();  
  double ret = n_device < num_devices ? ranges[n_device].getYLocal() : 0; 
  return ret;
}

ArPose NDRangeDeviceInfoWrapper::getEncoderPose(uint n_device)
{
    ArPose ret;
    uint num_devices = ranges.size();
    if(n_device < num_devices)
        ret.setPose(ranges[n_device].getEncodPosX(), ranges[n_device].getEncodPosY(),
                ranges[n_device].getEncodPosTh());
    return ret;
}

ulong NDRangeDeviceInfoWrapper::getSecs(uint n_device)
{
  uint num_devices = ranges.size();  
  ulong ret = n_device < num_devices ? ranges[n_device].getSecs() : 0; 
  return ret;
}

ulong NDRangeDeviceInfoWrapper::getMSecs(uint n_device)
{
  uint num_devices = ranges.size();  
  ulong ret = n_device < num_devices ? ranges[n_device].getMSecs() : 0; 
  return ret;
}
