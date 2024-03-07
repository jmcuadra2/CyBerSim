//
// C++ Implementation: blobcamerapacketwrapper
//
// Description: 
//
//
// Author: Jose Manuel Cuadra Troncoso <jmcuadra@dia.uned.es>, (C) 2009
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "blobcamerapacketwrapper.h"
#include <ArNetworking.h>

#include "../neuraldis/ndmath.h"

BlobCameraPacketWrapper::BlobCameraPacketWrapper(): AriaNetPacketWrapper()
{
  packet_name = "blobCameraInfo";
  device_name = "camera";
}


BlobCameraPacketWrapper::~BlobCameraPacketWrapper()
{
}


bool BlobCameraPacketWrapper::readPacket(ArNetPacket* packet)
{
  char info_packet[RangeHeaderLenght];
  packet->bufToStr(info_packet, RangeHeaderLenght - 1);
  if(std::string(info_packet) != std::string(packet_name)) {
    std::string message = std::string("It isn't a camera packet");
    ArLog::log(ArLog::Terse, message.c_str());
    return false;
  }

  pan = packet->bufToByte4()/100.0;
  tilt = packet->bufToByte4()/100.0;
  zoom = packet->bufToByte4()/100.0;
  blobArea = packet->bufToByte4();
  printf("blobArea = %d", blobArea);
  blobBottom = packet->bufToByte2();
  blobLeft = packet->bufToByte2();
  blobRight = packet->bufToByte2();
  blobTop = packet->bufToByte2();
  blobXCG = packet->bufToByte2();
  blobYCG = packet->bufToByte2();
  return true;
}

void BlobCameraPacketWrapper::setZoom(double zoom)
{
  ArNetPacket sending;
  sending.strToBuf("Camera zoom");
//   sending.byte4ToBuf(zoom);
  sending.doubleToBuf(zoom);
  client->requestOnce("setCameraZoom", &sending);
}

void BlobCameraPacketWrapper::setPan(double ang) // quizás *0.1125
{
  ArNetPacket sending;
  sending.strToBuf("Camera pan");
//   sending.byte4ToBuf(ang);
  sending.doubleToBuf(NDMath::roundInt(ang));
  client->requestOnce("setCameraPan", &sending);
}

void BlobCameraPacketWrapper::setPanRel(double ang) // quizás *0.1125
{
  ArNetPacket sending;
  sending.strToBuf("Camera relative pan");
//   sending.byte4ToBuf(ang);
  sending.doubleToBuf(NDMath::roundInt(ang));
  client->requestOnce("setCameraPanRel", &sending);
}

void BlobCameraPacketWrapper::tiltRel(double ang) // quizás *0.1125
{
  ArNetPacket sending;
  sending.strToBuf("Camera relative tilt");
//   sending.byte4ToBuf(ang);
  sending.doubleToBuf(ang);;
  client->requestOnce("setCameraTiltRel", &sending);
}

void BlobCameraPacketWrapper::setTilt(double ang) // quizás *0.1125
{
  ArNetPacket sending;
  sending.strToBuf("Camera absolute tilt");
//   sending.byte4ToBuf(ang);
  sending.doubleToBuf(ang);
  client->requestOnce("setCameraTilt", &sending);
}

void BlobCameraPacketWrapper::haltPanTilt(void)
{
  ArNetPacket sending;
  sending.strToBuf("Halt Pan Tilt");
  client->requestOnce("haltPanTilt", &sending);  
}
