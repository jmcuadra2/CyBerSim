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

#ifndef ARSERVEREXTINFOROBOT_H
#define ARSERVEREXTINFOROBOT_H



#include <list>
#include <ArServerInfoRobot.h>
#include "arservermodereactive.h"
#include "../robot/abstractsensor.h"

#define PAN_TOLERANCE  6
#define RESET_TILT  0

class ArServerClient;

using namespace std;
/**
@author jose manuel
*/
class ArServerExtInfoRobot : public ArServerInfoRobot
{

  public:
    ArServerExtInfoRobot(ArServerBase *server, ArRobot *robot, ArVCC4* vcc4 = 0, ArACTS_1_2* acts = 0);

    ~ArServerExtInfoRobot();

    void extUpdate(ArServerClient *client, ArNetPacket *packet);
    void sonarRanges(ArServerClient *client, ArNetPacket* packet );
    void bumperReadings(ArServerClient *client, ArNetPacket* packet );
    void laserRanges(ArServerClient *client, ArNetPacket* packet);
    void getSensorsList(ArServerClient *client, ArNetPacket* packet);
    
    void blobCameraInfo(ArServerClient *client, ArNetPacket* packet);
    
    void setCameraPan(ArServerClient *client, ArNetPacket* packet);
    void setCameraZoom(ArServerClient *client, ArNetPacket* packet);
    void setCameraTiltRel(ArServerClient *client, ArNetPacket* packet);
    void setCameraTilt(ArServerClient *client, ArNetPacket* packet);
    
    void setDeviceMaxRange(ArServerClient *client, ArNetPacket* packet);

    void setVel2(ArServerClient *client, ArNetPacket* packet);
    void stop(ArServerClient *client, ArNetPacket* packet);

    enum LookingState {LOOKING_NONE, LOOKING_MOVE_CAMERA, LOOKING_RETURN_CAMERA, LOOKING_MOVE_ROBOT};
    enum VideoResolution { WIDTH = 320, HEIGHT = 240 };
     
    void setServerMode(ArServerModeReactive* serverMode);

  protected:
    ArFunctor2C<ArServerExtInfoRobot, ArServerClient *, ArNetPacket *> mySensorsListCB;  
    ArFunctor2C<ArServerExtInfoRobot, ArServerClient *, ArNetPacket *> myExtUpdateCB;
    ArFunctor2C<ArServerExtInfoRobot, ArServerClient *, ArNetPacket *> myGetSonarRangesCB;
    ArFunctor2C<ArServerExtInfoRobot, ArServerClient *, ArNetPacket *> myGetBumperReadingsCB;
    ArFunctor2C<ArServerExtInfoRobot, ArServerClient *, ArNetPacket *> myGetSickRangesCB;
    
    ArFunctor2C<ArServerExtInfoRobot, ArServerClient *, ArNetPacket *> setDeviceMaxRangeCB;

    ArFunctor2C<ArServerExtInfoRobot, ArServerClient *, ArNetPacket *> myGetBlobCameraInfoCB;
    ArFunctor2C<ArServerExtInfoRobot, ArServerClient *, ArNetPacket *> setCameraPanCB;
    ArFunctor2C<ArServerExtInfoRobot, ArServerClient *, ArNetPacket *> setCameraZoomCB;
    ArFunctor2C<ArServerExtInfoRobot, ArServerClient *, ArNetPacket *> setCameraTiltRelCB;
    ArFunctor2C<ArServerExtInfoRobot, ArServerClient *, ArNetPacket *> setCameraTiltCB;

    ArFunctor2C<ArServerExtInfoRobot, ArServerClient *, ArNetPacket *> setVel2CB;
    ArFunctor2C<ArServerExtInfoRobot, ArServerClient *, ArNetPacket *> stopCB;

    void lookAround(void);
    bool reachedPan(void);

  protected:
    ArSick* sick;
    uint n_sonars;
    uint n_front_bumpers;
    uint n_rear_bumpers;
    bool* bumper_readings;
    uint n_laser_readings;
    const list<ArSensorReading * > *laser_readings;
    list< ArRangeDevice * > * rangeDevicesList;
    list< AbstractSensor::Types > sensorList;
    ArVCC4* vcc4;
    ArACTS_1_2* acts;
    bool blobFound;
    double panIncrement, actual_incre, maxPan, tilt, maxTilt, aperture;
    double zoom, minBlobSize, pan, panRel, desired_pan, desired_tilt;
    LookingState lookingState;
    bool camera_move_positive;  
    int delayCount,  commandsDelay;
    ArTime myLastTime;

    ArServerModeReactive* serverMode;
    
};

#endif
