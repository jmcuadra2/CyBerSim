//
// C++ Interface: blobcamerapacketwrapper
//
// Description:
//
//
// Author: Jose Manuel Cuadra Troncoso <jmcuadra@dia.uned.es>, (C) 2009
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef BLOBCAMERAPACKETWRAPPER_H
#define BLOBCAMERAPACKETWRAPPER_H

#include "arianetpacketwrapper.h"

/**
  @author Jose Manuel Cuadra Troncoso <jmcuadra@dia.uned.es>
*/
class BlobCameraPacketWrapper : public AriaNetPacketWrapper
{
  public:
    BlobCameraPacketWrapper();

    ~BlobCameraPacketWrapper();

    virtual bool readPacket(ArNetPacket* packet);

    int getPan() const { return pan; }
    int getTilt() const { return tilt; }
    int getZoom() const { return zoom; }
    
    int getBlobArea() const { return blobArea; }
    int getBlobBottom() const { return blobBottom; }
    int getBlobLeft() const { return blobLeft; }
    int getBlobRight() const { return blobRight; }
    int getBlobTop() const { return blobTop; }
    int getBlobXCG() const { return blobXCG; }
    int getBlobYCG() const { return blobYCG; }

    void setZoom(double zoom);
    void setPan(double ang);
    void setPanRel(double ang);
    void tiltRel(double ang);
    void setTilt(double ang);
    void haltPanTilt(void);

  protected:
    char* packet2_name;
    double pan, tilt, zoom;
    int blobArea, blobBottom, blobLeft, blobRight, blobTop, blobXCG, blobYCG;

};

#endif
