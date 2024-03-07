/***************************************************************************
 *   Copyright (C) 2004 by jose manuel                                     *
 *   jose@pcjose                                                           *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
#include "arserverextinforobot.h"

ArServerExtInfoRobot::ArServerExtInfoRobot(ArServerBase *server,
        ArRobot *robot, ArVCC4* vcc4, ArACTS_1_2* acts) :
        ArServerInfoRobot(server, robot),
        mySensorsListCB(this, &ArServerExtInfoRobot::getSensorsList),
        myExtUpdateCB(this, &ArServerExtInfoRobot::extUpdate),
        myGetSonarRangesCB(this, &ArServerExtInfoRobot::sonarRanges),
        myGetBumperReadingsCB(this, &ArServerExtInfoRobot::bumperReadings),
        myGetSickRangesCB(this, &ArServerExtInfoRobot::laserRanges),
        setDeviceMaxRangeCB(this, &ArServerExtInfoRobot::setDeviceMaxRange),
        myGetBlobCameraInfoCB(this, &ArServerExtInfoRobot::blobCameraInfo),
        setCameraPanCB(this, &ArServerExtInfoRobot::setCameraPan),
        setCameraZoomCB(this, &ArServerExtInfoRobot::setCameraZoom),
        setCameraTiltRelCB(this, &ArServerExtInfoRobot::setCameraTiltRel),
        setCameraTiltCB(this, &ArServerExtInfoRobot::setCameraTilt),
        setVel2CB(this, &ArServerExtInfoRobot::setVel2),
        stopCB(this, &ArServerExtInfoRobot::stop)
{

  if(!myRobot || !myServer)
    return;

  blobFound = false;
  camera_move_positive = true;
  lookingState = LOOKING_NONE;
  pan = 0;
  zoom =1;      
  aperture = 48/zoom;
  panIncrement = 5;
  actual_incre = panIncrement; 
  maxPan = 100;
  minBlobSize = 500;
  tilt = 0;
  desired_pan = 90.0;
  desired_tilt = RESET_TILT;
  delayCount = -1;
  commandsDelay = 3;
  serverMode = 0;

//   myLastTime.setToNow();

  myServer->addData("extUpdate",
		    "gets an update about more robot info",
		    &myExtUpdateCB, "none",
		    "byte2: leftVel, byte2: rightVel, byte1: leftMotorStalled, byte1: rightMotorStalled", "RobotInfo");
  myServer->addData("sensorsList",
        "gets the sensors list",
        &mySensorsListCB, "none",
        "string: sensor1, ...", "RobotInfo");
  myServer->addData("setDeviceMaxRange",
        "sets device max range",
        &setDeviceMaxRangeCB, "none",
        "string: device, byte4: max_range", "RobotInfo");
                    
  myServer->addData("setCameraPan",
        "sets camera pan",
        &setCameraPanCB, "none",
        "string: operation, byte2: pan", "RobotInfo");
  myServer->addData("setCameraZoom",
        "sets camera zoom",
        &setCameraZoomCB, "none",
        "string: operation, byte2: zoom", "RobotInfo");
  myServer->addData("setCameraTiltRel",
        "sets camera relative tilt",
        &setCameraTiltRelCB, "none",
        "string: operation, byte2: relative tilt", "RobotInfo");
  myServer->addData("setCameraTilt",
        "sets camera relative tilt",
        &setCameraTiltCB, "none",
        "string: operation, byte2: absolute tilt", "RobotInfo");

  myServer->addData("setVel2",
        "sets left and righr velocities",
        &setVel2CB, "none",
        "string: operation, byte2: lvel, byte2: rvel", "RobotInfo");
  myServer->addData("stop",
        "stop robot",
        &stopCB, "none",
        "string: operation");

//   robot->lock();

  this->vcc4 = vcc4;
  this->acts = acts;
  if(acts && vcc4)
    sensorList.push_back(AbstractSensor::ARIA_CAMERA);
     
  rangeDevicesList = myRobot->getRangeDeviceList();
  list<ArRangeDevice *>::const_iterator it;
  ArRangeDevice *device;
  sick = 0;
  for (it = rangeDevicesList->begin(); it != rangeDevicesList->end(); ++it) {   
    device = (*it);
    device->lockDevice();    
    if(!strcmp(device->getName(), "laser")) {
      sick = dynamic_cast<ArSick*> (device);
      myServer->addData("laserRanges",
		    "gets an update about laser ranges",
		    &myGetSickRangesCB, "none",
		    "byte2: num readings, byte2: range1 ..., byte2: range_num_lasers", "RobotInfo");
      sensorList.push_back(AbstractSensor::ARIA_SICK);  

    }
    else if(!strcmp(device->getName(), "sonar")) {
      myServer->addData("sonarRanges",
                        "gets an update about sonar ranges",
                        &myGetSonarRangesCB, "none",
                        "byte2: num sonars, byte2: range1 ..., byte2: range_num_sonars", "RobotInfo"); 
      n_sonars = myRobot->getNumSonar();
      sensorList.push_back(AbstractSensor::ARIA_SONAR);                      
    }
    device->unlockDevice();
  }
  
  n_front_bumpers = 0; 
  n_rear_bumpers = 0;   
  if(myRobot->hasFrontBumpers() || myRobot->hasRearBumpers()) {
    ArLog::log(ArLog::Normal, "\nRobot has bumpers\n");
    myServer->addData("bumperReadings",
          "gets an update about collisions",
          &myGetBumperReadingsCB, "none",
          "byte2: num n_front_bumpers, byte2: n_rear_bumpers, byte2: T/F ..., byte2: T/F", "RobotInfo");
    if(myRobot->hasFrontBumpers())
      n_front_bumpers = myRobot->getNumFrontBumpers();
    if(myRobot->hasRearBumpers())
      n_rear_bumpers = myRobot->getNumRearBumpers();
    bumper_readings = new bool[n_front_bumpers + n_rear_bumpers];
/*    if(n_front_bumpers && n_rear_bumpers)
      sensorList.push_back("bumper_both");  
    else if(n_front_bumpers)
      sensorList.push_back("bumper_front");
    else if(n_rear_bumpers)
      sensorList.push_back("bumper_rear");  */     
//     if(n_front_bumpers + n_rear_bumpers)       
    sensorList.push_back(AbstractSensor::ARIA_BUMP);
  }
  else
    ArLog::log(ArLog::Normal, "\nRobot no has bumpers\n");
//   robot->unlock();

//   if(vcc4 && acts) {
//     myServer->addData("blobCameraInfo",
//           "gets camera and largest blob info",
//           &myGetBlobCameraInfoCB, "none",
//           "byte2: pan, byte2: tilt, byte2: zoom byte4: area, byte2: bottom, byte2: left, byte2: right, byte2: top, byte2: xcg, byte2: ycg", "RobotInfo");
//   }
  
//    // solo para usar en depuración
//   myRobot->setConnectionTimeoutTime(0);
//   sick->setConnectionTimeoutSeconds(0);

}


ArServerExtInfoRobot::~ArServerExtInfoRobot()
{
}

void ArServerExtInfoRobot::extUpdate(ArServerClient *client, ArNetPacket* )
{
  ArNetPacket sending;
  myRobot->lock();
  sending.byte2ToBuf((int)(myRobot->getLeftVel()+0.5));
  sending.byte2ToBuf((int)(myRobot->getRightVel()+0.5));
  sending.byteToBuf((int)myRobot->isLeftMotorStalled());
  sending.byteToBuf((int)myRobot->isRightMotorStalled());
  
  sending.doubleToBuf(myRobot->getEncoderX());
  sending.doubleToBuf(myRobot->getEncoderY()); 
  sending.doubleToBuf(myRobot->getEncoderTh()); 
  
  ArTime t = myRobot->getLastOdometryTime();
  sending.byte4ToBuf(t.getSec());
  sending.byte4ToBuf(t.getMSec()); 
  
  myRobot->unlock();

  client->sendPacketUdp(&sending);
  

}

void ArServerExtInfoRobot::sonarRanges(ArServerClient *client, ArNetPacket* )
{
  ArNetPacket sending;
  ArSensorReading *sonar;
  
  myRobot->lock();
  sending.strToBuf("sonarRanges");
  sending.byte2ToBuf(n_sonars);  
  for (uint i = 0; i < n_sonars; i++) {
    sonar = myRobot->getSonarReading(i);
    if (sonar == NULL) {
      ArLog::log(ArLog::Terse, "Have an empty sonar at number %d, there should be %d sonar.", i, n_sonars);
      continue;
    }
    sending.byte4ToBuf(sonar->getRange());
    sending.byte4ToBuf((int)(sonar->getLocalX()+0.5));
    sending.byte4ToBuf((int)(sonar->getLocalY()+0.5));    
    sending.byte4ToBuf((int)(sonar->getX()+0.5));
    sending.byte4ToBuf((int)(sonar->getY()+0.5));
    
    ArPose encodPos = sonar->getEncoderPoseTaken();
    ArTime timePos = sonar->getTimeTaken();
    ArPose sensorPos = sonar->getSensorPosition();
    
    sending.byte4ToBuf(timePos.getSec());
    sending.byte4ToBuf(timePos.getMSec());    
/*    sending.byte4ToBuf(encodPos.getX());
    sending.byte4ToBuf(encodPos.getY()); 
    sending.byte4ToBuf(encodPos.getTh());
    sending.byte4ToBuf(sensorPos.getX());
    sending.byte4ToBuf(sensorPos.getY()); 
    sending.byte4ToBuf(sensorPos.getTh());*/    
//     ArLog::log(ArLog::Terse, "Range %d, x %f, y %f .", sonar->getRange(), sonar->getX(), sonar->getY());
  }
  myRobot->unlock();

  client->sendPacketUdp(&sending);
}

void ArServerExtInfoRobot::bumperReadings(ArServerClient *client, ArNetPacket* )
{
  ArNetPacket sending;
//   int n_front_bumpers = myRobot->getNumFrontBumpers();
//   int n_rear_bumpers = myRobot->getNumRearBumpers();
//   bool* readings = new bool[n_front_bumpers + n_rear_bumpers];
  int val, bit;
  uint i;
  
  myRobot->lock();
  
  val = ((myRobot->getStallValue() & 0xff00) >> 8);
  for (i = 0, bit = 2; i < n_front_bumpers; i++, bit *= 2)
    bumper_readings[i] = val & bit ? true : false;
  
  val = ((myRobot->getStallValue() & 0xff));
  for (i = n_front_bumpers, bit = 2; i < n_front_bumpers + n_rear_bumpers; i++, bit *= 2)
    bumper_readings[i] = val & bit ? true : false;
  
  myRobot->unlock();
  
  sending.strToBuf("bumperReadings");
  sending.byte2ToBuf(n_front_bumpers);
  sending.byte2ToBuf(n_rear_bumpers);
  for (uint i = 0; i < n_front_bumpers + n_rear_bumpers; i++) {
    sending.byte4ToBuf(bumper_readings[i]);
    sending.byte4ToBuf(0);
    sending.byte4ToBuf(0);
  }

  client->sendPacketUdp(&sending);  
  
}

void ArServerExtInfoRobot::laserRanges(ArServerClient *client, ArNetPacket* )
{
  if(!sick) return;
  
  ArNetPacket sending;
  ArTime now;
  sick->lockDevice();  
  laser_readings = sick->getRawReadings();
  sick->unlockDevice();
  //////////////

  n_laser_readings = laser_readings->size(); 
//   ArLog::log(ArLog::Terse, "num_sensors = %d", n_laser_readings); 
  sending.strToBuf("laserRanges");
  sending.byte2ToBuf(n_laser_readings);
  
  ArPose encodPos;
  ArTime timePos;

  ArPose sensorPos;
  myRobot->lock();
  list<ArSensorReading *>::const_iterator it;
  for (it = laser_readings->begin(); it != laser_readings->end(); ++it) {
    sending.byte4ToBuf((*it)->getRange());
    sending.byte4ToBuf((int)((*it)->getLocalX()+0.5));
    sending.byte4ToBuf((int)((*it)->getLocalY()+0.5));    
    sending.byte4ToBuf((int)((*it)->getX()+0.5));
    sending.byte4ToBuf((int)((*it)->getY()+0.5));
    
//    encodPos = (*it)->getEncoderPoseTaken();
    timePos = (*it)->getTimeTaken();
    myRobot->getEncoderPoseInterpPosition(timePos, &encodPos);
    sensorPos = (*it)->getSensorPosition();
    
    sending.byte4ToBuf(timePos.getSec());
    sending.byte4ToBuf(timePos.getMSec());    
    sending.doubleToBuf(encodPos.getX());
    sending.doubleToBuf(encodPos.getY());
    sending.doubleToBuf(encodPos.getTh());
    sending.byte4ToBuf(sensorPos.getX());
    sending.byte4ToBuf(sensorPos.getY()); 
    sending.byte4ToBuf(sensorPos.getTh());

/*    ArLog::log(ArLog::Terse, "Time = %lu.%lu EncoderPose = (%f, %f, %f) SensorPose = (%f, %f, %f)\n", timePos.getSec(), timePos.getMSec()
	       , encodPos.getX(), encodPos.getY(), encodPos.getTh(), sensorPos.getX(), sensorPos.getY(), sensorPos.getTh());  */ 
  }
  myRobot->unlock();

      ArLog::log(ArLog::Terse, "Now = %lu.%03lu Time = %lu.%03lu EncoderPose = (%f, %f, %f) SensorPose = (%f, %f, %f)\n", now.getSec(), now.getMSec()
             , timePos.getSec(), timePos.getMSec(), encodPos.getX(), encodPos.getY(), encodPos.getTh(), sensorPos.getX(), sensorPos.getY(), sensorPos.getTh());

/*    ArLog::log(ArLog::Terse, "Time = %lu.%03lu EncoderTh = %f", timePos.getSec(), timePos.getMSec(), encodPos.getTh());  */   
  client->sendPacketUdp(&sending);

}

void ArServerExtInfoRobot::getSensorsList(ArServerClient *client, ArNetPacket* )
{
    ArNetPacket sending;
    sending.strToBuf("sensorsList");
    list<AbstractSensor::Types>::const_iterator it;
    for (it = sensorList.begin(); it != sensorList.end(); ++it) 
       sending.byte2ToBuf((*it));
       
    client->sendPacketUdp(&sending);
}

void ArServerExtInfoRobot::setDeviceMaxRange(ArServerClient */*client*/, ArNetPacket* packet)
{
  char device_type[64];
  packet->bufToStr(device_type, 63);  
  /*int max_range =  packet->bufToByte4();*/
  double max_range =  packet->bufToByte4();
  
  list<ArRangeDevice *>::const_iterator it;
  ArRangeDevice *device;
  for (it = rangeDevicesList->begin(); it != rangeDevicesList->end(); ++it) {
    device = (*it);
    device->lockDevice(); 
    if(!strcmp(device->getName(), device_type))
      device->setMaxRange(max_range); 
    device->unlockDevice();
  }
}

void ArServerExtInfoRobot::blobCameraInfo(ArServerClient *client, ArNetPacket* /*packet*/)
{
  ArNetPacket sending;
  if(vcc4) {
//     vcc4->getRealPanTilt();
//     vcc4->getRealZoomPos();
    sending.strToBuf("blobCameraInfo");
    pan = vcc4->getPan();
    sending.byte4ToBuf(pan*100);
    sending.byte4ToBuf(vcc4->getTilt()*100);
    sending.byte4ToBuf(vcc4->getZoom()*100);
  }
  if(serverMode) {
    sending.byte4ToBuf(serverMode->getBlobArea());
    sending.byte2ToBuf(0);
    sending.byte2ToBuf(0);
    sending.byte2ToBuf(0);
    sending.byte2ToBuf(0);
    sending.byte2ToBuf(0);
    sending.byte2ToBuf(0);
  }
  else {
    sending.byte4ToBuf(-1);
    sending.byte2ToBuf(0);
    sending.byte2ToBuf(0);
    sending.byte2ToBuf(0);
    sending.byte2ToBuf(0);
    sending.byte2ToBuf(0);
    sending.byte2ToBuf(0);
  }

  client->sendPacketUdp(&sending); 
}

// void ArServerExtInfoRobot::lookAround(void)
// {
//   ++delayCount;
//   if(!delayCount) {
//     vcc4-> pan(desired_pan);
//     return;
//   }
//   if(reachedPan()) {
//     if(camera_move_positive) {
//       desired_pan = -90.0;
//       camera_move_positive = false;      
//     }
//     else {
//       desired_pan = 90.0;
//       camera_move_positive = true;
//     }
//    vcc4-> pan(desired_pan);
//   }
// //   else {
// //     if(!(delayCount%5)) {
// //       if(camera_move_positive) {
// //         vcc4->panRel(panIncrement);
// //       }
// //       else {
// //         vcc4->panRel(-panIncrement);
// //       }
// //     }
// //   }
// }

void ArServerExtInfoRobot::lookAround(void)
{
  if(vcc4) {
    ++delayCount;
    if(delayCount < 150) 
      return;
    if(!(delayCount%commandsDelay)) {
      double l_pan = vcc4->getPan();
      ArLog::log(ArLog::Terse, "getPan: %f", l_pan);
      if(l_pan <= vcc4->getMaxNegPan())
        actual_incre = panIncrement;
      else if(l_pan >= vcc4->getMaxPosPan())
        actual_incre = -panIncrement;
      vcc4->panRel(actual_incre);
      ArLog::log(ArLog::Terse, "Time: %d", myLastTime.mSecSince ());
//       myLastTime.setToNow();
    }
  }
}

bool ArServerExtInfoRobot::reachedPan(void)
{
  bool ret = false;

  pan = vcc4->getPan();  
  ret = (fabs(pan - desired_pan) < PAN_TOLERANCE) /*|| (fabs(pan) >= maxPan)*/;
  ArLog::log(ArLog::Terse, "desired_pan = %f pan = %f", desired_pan, pan);
  
  return ret;
}

void ArServerExtInfoRobot::setCameraPan(ArServerClient */*client*/, ArNetPacket* packet)
{
  char device_type[64];
  packet->bufToStr(device_type, 63);  
  double l_pan =  packet->bufToDouble();
  vcc4->pan(l_pan);
}

void ArServerExtInfoRobot::setCameraZoom(ArServerClient */*client*/, ArNetPacket* packet)
{
  char device_type[64];
  packet->bufToStr(device_type, 63);  
  double l_zoom =  packet->bufToDouble();
  vcc4->zoom(l_zoom);
}

void ArServerExtInfoRobot::setCameraTiltRel(ArServerClient */*client*/, ArNetPacket* packet)
{
  char device_type[64];
  packet->bufToStr(device_type, 63);  
  double tiltrel =  packet->bufToDouble();
  vcc4->tiltRel(tiltrel);
}

void ArServerExtInfoRobot::setCameraTilt(ArServerClient */*client*/, ArNetPacket* packet)
{
  char device_type[64];
  packet->bufToStr(device_type, 63);  
  double l_tilt =  packet->bufToDouble();
  vcc4->tilt(l_tilt);
}

void ArServerExtInfoRobot::setVel2(ArServerClient */*client*/, ArNetPacket* packet)
{
  char device_type[64];
  packet->bufToStr(device_type, 63);
  int lvel =  packet->bufToByte2();
  int rvel =  packet->bufToByte2();
  myRobot->setVel2(lvel, rvel);
}

void ArServerExtInfoRobot::stop(ArServerClient */*client*/, ArNetPacket* /*packet*/)
{
  myRobot->stop();
}

void ArServerExtInfoRobot::setServerMode(ArServerModeReactive* serverMode)
{
  this->serverMode = serverMode;
  if(vcc4 && acts) {
    myServer->addData("blobCameraInfo",
          "gets camera and largest blob info",
          &myGetBlobCameraInfoCB, "none",
          "byte2: pan, byte2: tilt, byte2: zoom byte4: area, byte2: bottom, byte2: left, byte2: right, byte2: top, byte2: xcg, byte2: ycg", "RobotInfo");
  }  
}
