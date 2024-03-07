//
// C++ Interface: ariacamera
//
// Description: 
//
//
// Author: Jose Manuel Cuadra Troncoso <jmcuadra@dia.uned.es>, (C) 2009
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef ARIACAMERA_H
#define ARIACAMERA_H

#include "ariasensor.h"
#include "../aria-tools/blobcamerapacketwrapper.h"

#define RESET_TILT -10
#define PAN_TOLERANCE  6

/**
	@author Jose Manuel Cuadra Troncoso <jmcuadra@dia.uned.es>
*/
class AriaCamera : public AriaSensor
{
  public:
    AriaCamera(int id, double rad, double start_ang, double end_ang, int group_idx, BlobCameraPacketWrapper* sensor_info, QObject* parent, const char* name);
    
    AriaCamera(int id, int group_idx, BlobCameraPacketWrapper* sensor_info, QObject* parent, const char* name);
    
    AriaCamera(int id, int group_idx, double rad, double rot, double orient, BlobCameraPacketWrapper* sensor_info, QObject* parent, const char* name);
    
    AriaCamera(int id, int group_idx, double x_, double y_, double rot, double orient, BlobCameraPacketWrapper* sensor_info, QObject* parent, const char* name);

    ~AriaCamera();

    double sample(void);
    int getRawReading(bool obtain = true);
    
    int sensorSuperType(void);
    int sensorType(void);
    void initSensor(void);

    void setZoom(double zoom);
    void setPan(double ang);
    void setPanRel(double ang);
    void tiltRel(double tilt_r);
    void setTilt(double ang);

    public:
      enum VideoResolution { WIDTH = 320, HEIGHT = 240 };
      enum LookingState {LOOKING_NONE, LOOKING_MOVE_CAMERA, LOOKING_RETURN_CAMERA, LOOKING_MOVE_ROBOT};      

  protected:
    void initParams(void);
    void initMeasure(void);
    void writeMeasure(void);
    void lookAround(void);
    bool reachedPan(void);
     
  protected:
    double panIncrement, maxPan, tilt, maxTilt;
    double zoom, minBlobSize, pan, panRel, desired_pan, desired_tilt;
    BlobCameraPacketWrapper* sensor_info;
    bool blobFound;
    int delayCount;
    double aperture;
    bool camera_move_positive;
    LookingState lookingState;

};

#endif
