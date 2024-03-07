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
#ifndef ROBOTINFOWRAPPER_H
#define ROBOTINFOWRAPPER_H

#include <list>
#include <Aria.h>

#include "arianetpacketwrapper.h"

using namespace std;

/**
@author jose manuel
*/
class RobotInfoWrapper : public AriaNetPacketWrapper
{
public:
    RobotInfoWrapper();

    ~RobotInfoWrapper();
    bool readPacket(ArNetPacket* packet);
    bool readExtPacket(ArNetPacket* packet);
    bool readSensorsList(ArNetPacket* packet);
    const double& getXRobot(void) { return x_robot ; };
    const double& getYRobot(void) { return y_robot ; };
    const double& getLinVelRobot(void) { return linvel_robot ; };
    const double& getRadVelRobot(void) { return radvel_robot ; };
    const double& getLeftVelRobot(void) { return left_vel_robot ; };
    const double& getRightVelRobot(void) { return right_vel_robot ; };
    const double& getHeadingRobot(void) { return th_robot ; };
    const char* getRobotStatus(void) { return robot_status ; };
    bool isSensorsListReaded() const { return sensorsListReaded; };
    bool hasSensorType(int type);
    list< int > * getSensorsList() { return &sensorsList; };

    void setVel2(int lvel, int rvel);
    void stop(void);
    
    const double& getXEncoder(void) { return x_encoder ; };
    const double& getYEncoder(void) { return y_encoder ; };    
    const double& getHeadingEncoder(void) { return th_encoder ; };
    const ArTime& getTimeTaken(void) { return timeTaken ; };
    const ArTime& getTimeGet(void) { return timeGet ; };
    
  protected:
    char robot_status[64];
    char robot_mode[64];
    int battery;
    double x_robot;
    double y_robot;
    double th_robot;
    double linvel_robot;
    double radvel_robot;
    double left_vel_robot;
    double right_vel_robot;
    list< int > sensorsList;
    bool sensorsListReaded;
    bool leftMotorStalled, rightMotorStalled;
    
    double x_encoder;
    double y_encoder;
    double th_encoder;    

    ulong secs;
    ulong mSecs;
    ArTime timeTaken;
    ArTime timeGet;
};

#endif
