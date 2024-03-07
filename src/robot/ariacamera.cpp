//
// C++ Implementation: ariacamera
//
// Description: 
//
//
// Author: Jose Manuel Cuadra Troncoso <jmcuadra@dia.uned.es>, (C) 2009
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "ariacamera.h"
#include <Aria.h>

AriaCamera::AriaCamera(int id, double rad, double start_ang, double end_ang, int group_idx, BlobCameraPacketWrapper* sensor_info, QObject* parent, const char* name): AriaSensor(id, rad, start_ang, end_ang, group_idx, sensor_info, parent, name)
{
  this->sensor_info = sensor_info;
}


AriaCamera::AriaCamera(int id, int group_idx, BlobCameraPacketWrapper* sensor_info, QObject* parent, const char* name): AriaSensor(id, group_idx, sensor_info, parent, name)
{
  this->sensor_info = sensor_info;
}


AriaCamera::AriaCamera(int id, int group_idx, double rad, double rot, double orient, BlobCameraPacketWrapper* sensor_info, QObject* parent, const char* name): AriaSensor(id, group_idx, rad, rot, orient, sensor_info, parent, name)
{
  this->sensor_info = sensor_info;
}


AriaCamera::AriaCamera(int id, int group_idx, double x_, double y_, double rot, double orient, BlobCameraPacketWrapper* sensor_info, QObject* parent, const char* name): AriaSensor(id, group_idx, x_, y_, rot, orient, sensor_info, parent, name)
{
  this->sensor_info = sensor_info;
}


AriaCamera::~AriaCamera()
{
}

int AriaCamera::sensorSuperType(void)
{
  return AbstractSensor::ARIA_SENSOR;
}

int AriaCamera::sensorType(void )
{
  return AbstractSensor::ARIA_CAMERA;
}

void AriaCamera::initParams(void)
{
  sensorMaxVal = WIDTH*HEIGHT+1;
  sensorWrongVal = WIDTH*HEIGHT+1;
  sensorCriticVal = 0;

  pan = 0;
  zoom = 1;
  aperture = 48/zoom;

  panIncrement = 5;
  maxPan = 100;
  minBlobSize = 500;
  tilt = 0;
  blobFound = false;
  maxTilt = 30;
  delayCount = -1;
  camera_move_positive = true;
  desired_pan = 0.0;
  desired_tilt = RESET_TILT;
  lookingState = LOOKING_NONE;
}

void AriaCamera::initSensor(void)
{
  orientation = 0;
  setPan(orientation);
  setZoom(1);
//   tiltRel(-tilt);
  setTilt(RESET_TILT);

  lookingState = LOOKING_NONE;
  ArUtil::sleep(1000);
  delayCount = -1;
  // mandar mensaje para espera 1 s.
}

void AriaCamera::initMeasure(void)
{
  vector<size_t> yDimensions(2);
  yDimensions[0] = 1;
  yDimensions[1] = 7; // blob area, bottom, left, right, top, xcg, ycg
  sensorPose = NDPose::createPose(NDPose::Pose3DSph);
  vector<double> cam_position(3);
  cam_position[0] = x_org;
  cam_position[1] = y_org;
  cam_position[2] = 0; // por ahora
  vector<double> cam_orientation(3);
  cam_orientation[0] = orientation;  // pan
  cam_orientation[1] = tilt;  // titl
  cam_orientation[2] = zoom;  // zoom
  sensorPose->setCoordinates(cam_position, cam_orientation);
  measure.initMeasure(NDPose::Pose3DSph, idChar, "pixel", yDimensions);
  measure.setPose(sensorPose);
}

void AriaCamera::writeMeasure(void)
{
  measure.setPose(sensorPose);
  measure.setTimeStamp();
  vector<double> mm(7);
  mm[0] = out;
  // faltan los 6 elementos restantes del vector
  measure.setY(mm);
}

void AriaCamera::setPan(double ang)
{
  pan = ang;
  sensor_info->setPan(pan);
  sensorLine->rotateTo(pan);
}

void AriaCamera::setPanRel(double ang)
{
  delayCount++;
  pan += ang;
  if((pan <= - maxPan) || (pan >= maxPan)) {
    if(pan >= 0) {
      pan = maxPan;
      camera_move_positive = false;
    }
    else {
      pan = -maxPan;
      camera_move_positive = true;
    }      
    ang = 0;
  }
  sensor_info->setPanRel(ang);
  sensorLine->rotateTo(pan);
}

void AriaCamera::setZoom(double zoom)
{
  zoom = zoom < 1 ? 1 : zoom;
  zoom = zoom > 10 ? 10 : zoom;  // crear variable maxZomm (10)
  sensor_info->setZoom(zoom);
  this->zoom = zoom;
  aperture = 48/zoom;
}

void AriaCamera::setTilt(double ang)
{
  sensor_info->setTilt(ang);
}

void AriaCamera::tiltRel(double tilt_r)
{
  double l_tilt = tilt + tilt_r;
  if(fabs(l_tilt) <= maxTilt)
    tilt = l_tilt;
  else
    tilt_r = 0;
  sensor_info->tiltRel(tilt_r);
}

int AriaCamera::getRawReading(bool obtain)
{ 
  if(!obtain) return rawReading;

  rawReading = sensor_info->getBlobArea();

//   rawReading = rawReading < 0 ? sensorWrongVal : rawReading;
  rawReading = (rawReading > sensorMaxVal) ? sensorMaxVal : rawReading;

  sensorLine->setLength(rawReading);
  x_reading = sensorLine->getGlobalIntersectionPoint().x();
  y_reading = sensorLine->getGlobalIntersectionPoint().y();
      
  rawReading = (rawReading >= sensorMaxVal) ? -1 : rawReading;
//   ArLog::log(ArLog::Normal, "\nBlobArea %d %f\n", rawReading, pan);
  return rawReading;
}

double AriaCamera::sample(void)
{

  rawReading = getRawReading();
  if(rawReading >= sensorMaxVal || rawReading <= 0)
    sensorLine->setValidReading(false);
  else 
    sensorLine->setValidReading(true);

  orientation = sensor_info->getPan();
  pan = orientation;
  zoom = sensor_info->getZoom();
  tilt = sensor_info->getTilt();
  
  if(rawReading >= minBlobSize) 
    out = double(rawReading)/(WIDTH*HEIGHT);
  else
    out = 0;
  
  vector<double> orient(3);
  orient[0] = orientation;
  orient[1] = tilt;
  orient[2] = zoom;
  sensorPose->setOrientation(orient);
  prev_out = out;
  writeMeasure();
  return out; 
}

void AriaCamera::lookAround(void)
{

  if(reachedPan()) {
    if(camera_move_positive) {
      desired_pan = -90.0;
      camera_move_positive = false;
    }
    else {
      desired_pan = 90.0;
      camera_move_positive = true;
    }
  }
  else {
    if(camera_move_positive) {
      setPanRel(panIncrement);
    }
    else {
      setPanRel(-panIncrement);
    }
  }
}

// void AriaCamera::lookAround(void)
// {
// //   ++delayCount;
// //   
// //   if(delayCount%3) return;
//   
//   if(lookingState == LOOKING_NONE) {    
//     ArLog::log(ArLog::Terse, 
//         "\nCamera will now look for targets.\n");
//     if(pan >= 0) {
// //       desired_pan = 90.0;
//       desired_pan = pan + panIncrement;
//       desired_tilt = RESET_TILT;
//       setPan(desired_pan);
//       setTilt(desired_tilt);
//       camera_move_positive = true;
// //       camera_move_positive = randomCameraPositive();
//     }
//     else {
// //       desired_pan = -90.0;
//       desired_pan = pan - panIncrement;
//       desired_tilt = RESET_TILT;
//       setPan(desired_pan);
//       setTilt(desired_tilt);
//       camera_move_positive = false;
// //       camera_move_positive = randomCameraPositive();
//     }
//     zoom = 2;
//     setZoom(zoom);
//     lookingState = LOOKING_MOVE_CAMERA;
//   }
//   else if(lookingState == LOOKING_MOVE_CAMERA) { 
//     if(reachedPan()) {
//       if(camera_move_positive) {
// //         desired_pan = -90.0;
//         desired_pan += panIncrement;
//         if(desired_pan >= maxPan-1) {
//           desired_pan -= panIncrement*2;
//           desired_tilt = RESET_TILT;          
//           setTilt(desired_tilt);
//           camera_move_positive = false;          
//         }
//         setPan(desired_pan);
// //         desired_tilt = RESET_TILT;
// //         setPan(desired_pan);
// //         setTilt(desired_tilt);
// //         camera_move_positive = false;
//       }
//       else {
//         desired_pan -= panIncrement;
//         if(desired_pan <= -maxPan+1) {
//           desired_pan += panIncrement*2;
//           desired_tilt = RESET_TILT;          
//           setTilt(desired_tilt);
//           camera_move_positive = true;          
//         }
//         setPan(desired_pan);     
// //         desired_pan = 90.0;
// //         desired_tilt = RESET_TILT;
// //         setPan(desired_pan);
// //         setTilt(desired_tilt);
// //         camera_move_positive = true;
//       }
//     }
//   } 
// }

bool AriaCamera::reachedPan(void)
{
  bool ret = false;
  
  ret = (fabs(pan - desired_pan) < PAN_TOLERANCE) || (fabs(pan) >= maxPan);
  
  return ret;
}
