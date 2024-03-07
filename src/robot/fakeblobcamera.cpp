//
// C++ Implementation: fakeblobcamera
//
// Description: 
//
//
// Author: Jose Manuel Cuadra Troncoso <jmcuadra@dia.uned.es>, (C) 2009
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "fakeblobcamera.h"

FakeBlobCamera::FakeBlobCamera(int id, double rad, double start_ang, double end_ang, int group_idx, QObject* parent, const char* name): RangeBeamSensor(id, rad, start_ang, end_ang, group_idx, parent, name)
{
}


FakeBlobCamera::FakeBlobCamera(int id, int group_idx, double rad, double rot, double orient, QObject* parent, const char* name): RangeBeamSensor(id, group_idx, rad, rot, orient, parent, name)
{
}


FakeBlobCamera::FakeBlobCamera(int id, int group_idx, double x_, double y_, double rot, double orient, QObject* parent, const char* name): RangeBeamSensor(id, group_idx, x_, y_, rot, orient, parent, name)
{
}


FakeBlobCamera::FakeBlobCamera(int id, int group_idx, QObject* parent, const char* name): RangeBeamSensor(id, group_idx, parent, name)
{
}


FakeBlobCamera::~FakeBlobCamera()
{
}

int FakeBlobCamera::getRawReading(bool obtain)
{ 
  if(!obtain) return rawReading;
  
  int numReadings = rawReadings.size();
  int correctRawReadings = 0;
  int requiredNumReadings = int(numReadings*minApertureFraction + .5) + 1;
  requiredNumReadings = !requiredNumReadings ? 1 : requiredNumReadings;
  
  mapCollisionSolver.clear();
  blobs.clear();
  
  for(int i = 0; i < numReadings; i++) {    
    int auxRawReading = sensorLines.at(i)->sample(false);
    CollisionSolver* solver = sensorLines.at(i)->getSolver();

    addReadingToSolver(i, auxRawReading, solver);
    paintLine(i, auxRawReading);
  }

  QList<CollisionSolver*> keys = mapCollisionSolver.keys();
  rawReading = getSensorWrongVal(); //inicializo al max

  QVector<int> sizes;
  QVector<int> readings;

  for( int k = 0; k < keys.size() ; k++ ){  //for each e_k
    QVector<QPoint> readings_zstar = mapCollisionSolver.value(keys[k]);
    correctRawReadings = 0;
    int numElementReadings = readings_zstar.size();
    int minReading = sensorWrongVal;
    
    for (int j = 0; j < numElementReadings; j++) { //for j in e_k
      double reading = readings_zstar.at(j).x();
      if(reading < sensorWrongVal) {
        if(reading < minReading)
          minReading = reading;
        correctRawReadings++;
      }
    }
    sizes.append(correctRawReadings);
    readings.append(minReading);
  }
  int maxSize = 0;
  int maxIdx = -1;
  for(int k = 0; k < sizes.size(); k++) {
    if(maxSize < sizes.at(k)) {
      maxIdx = k;
      maxSize = sizes.at(k);
    }
  }

  if(maxIdx > -1) {
    rawReading = readings.at(maxIdx);
    largestBlob = blobs[keys.at(maxIdx)];
  }
  else
      largestBlob.clear();
      
  return rawReading;
}

double FakeBlobCamera::sample(void)
{
  double pan;
//   bool blobFound = false;
    
  rawReading = getRawReading();
  if(rawReading >= sensorWrongVal) 
    sensorLine->setValidReading(false);
  else 
    sensorLine->setValidReading(true);
    
  sensorLine->setLength(rawReading);
  x_reading = sensorLine->getGlobalIntersectionPoint().x();
  y_reading = sensorLine->getGlobalIntersectionPoint().y();

  int blobSize = largestBlob.size();
  out = double(blobSize)/rawReadings.size();
//   out *= out*160*120; // out = blob area, 160x120 pixeles total

  // Esto deberia ir a un control
  pan = orientation;
  if(blobSize > 1) {
    if(!blobFound) {
      setZoom(1);
      blobFound = true;
    }
    double blobXCG = (largestBlob.at(blobSize - 1) + largestBlob.at(0))/2.0;
    double xCenter = rawReadings.size()/2.0;
//     pan += (xCenter - blobXCG)/2.0;
    pan -= (xCenter - blobXCG)/2.0;
    if(pan > maxPan)
      pan = maxPan;
    else if(pan < -maxPan)
      pan = -maxPan;      
  }
  else {
    if(blobFound) {
      setZoom(2);
      blobFound = false;
    }  
    out = 0;
    pan += panIncrement;
    if(panIncrement >= 0) {
      if(pan > maxPan) {
        pan = maxPan;
        panIncrement = -panIncrement;
      }
    }
    else {
      if(pan < -maxPan) {
        pan = -maxPan;
        panIncrement = -panIncrement;
      }
    }
  }
  setPan(-pan);
//   setPan(pan);
  
  vector<double> orient = sensorPose->getOrientation();
  orient[0] = orientation;
  orient[2] = zoom;
  sensorPose->setOrientation(orient);
  prev_out = out;
  writeMeasure();
  orientation = pan;
  return out; 
}

int FakeBlobCamera::sensorSuperType(void)
{
    return LINEAR_SENSOR;
}

int FakeBlobCamera::sensorType(void)
{
    return FAKE_BLOB_CAMERA;
}

void FakeBlobCamera::initParams(void)
{
  sensorMaxVal = 50000;
  sensorWrongVal = 50000;
  sensorCriticVal = 100;

  aperture = 48;
  minApertureFraction = 1.0/48.0;
  zoom = 1;

  panIncrement = 5.0/3.0;
  maxPan = 70;

  beamResolution = 40;
  blobFound = false;
}

void FakeBlobCamera::initSensor(void)
{
  orientation = 0;
  setPan(orientation);
}

void FakeBlobCamera::initMeasure(void)
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
  cam_orientation[1] = 0;  // titl
  cam_orientation[2] = zoom;  // zoom
  sensorPose->setCoordinates(cam_position, cam_orientation);
  measure.initMeasure(NDPose::Pose3DSph, idChar, "pixel", yDimensions);
  measure.setPose(sensorPose);
}

void FakeBlobCamera::writeMeasure(void)
{
  measure.setPose(sensorPose);
  measure.setTimeStamp();
  vector<double> mm(7);
  if(rawReading >= sensorWrongVal && !getIdealSensor())
    mm[0] = -1;
  else
    mm[0] = out;
  // faltan los 6 elementos restantes del vector
  measure.setY(mm);
}

void FakeBlobCamera::addReadingToSolver(int idx, int reading, CollisionSolver* solver)
{
  if(!solver) return;
  
  if(mapCollisionSolver.contains(solver)) {
    mapCollisionSolver[solver].append(QPoint(reading, sensorLines.at(idx)->getZStar()));
    if(solver && (reading < sensorWrongVal)) 
      blobs[solver].append(idx);
  }
  else {
    mapCollisionSolver[solver].append(QPoint(reading, sensorLines.at(idx)->getZStar()));
    if(solver && (reading < sensorWrongVal)) {
      QVector<int> blob;
      blob.append(idx);
      blobs.insert(solver, blob);
    }
  }
}

// void FakeBlobCamera::setPan(double ang)
// {
//   for(int i = 0; i < sensorLines.size(); i++){
//     sensorLines.at(i)->rotate(ang);
//   }
//   sensorLine->rotate(ang);
// }

void FakeBlobCamera::setPan(double ang)
{
  for(int i = 0; i < sensorLines.size(); i++){
    sensorLines.at(i)->rotateTo(ang + sensorLines.at(i)->getOriginalRotation() - sensorLine->getOriginalRotation());
  }
  sensorLine->rotateTo(ang);
}

void FakeBlobCamera::setZoom(double zoom)
{
  zoom = zoom < 1 ? 1 : zoom;
  zoom = zoom > 10 ? 10 : zoom;  // crear variable maxZomm (10)
  double ratio = this->zoom/zoom;
  if(ratio < 0.99 || ratio > 1.01) {
    for(int i = 0; i < sensorLines.size(); i++){
      sensorLines.at(i)->rotateTo(sensorLine->getOriginalRotation() + (sensorLines.at(i)->getOriginalRotation() - sensorLine->getOriginalRotation())*ratio);
    }
    this->zoom = zoom;
  }
}
