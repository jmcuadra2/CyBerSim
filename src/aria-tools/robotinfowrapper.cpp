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

#include <ArNetworking.h>
#include <string.h>

#include "robotinfowrapper.h"

RobotInfoWrapper::RobotInfoWrapper(): AriaNetPacketWrapper()
{
  sensorsListReaded = false;
  leftMotorStalled = false;
  rightMotorStalled = false;
  
}


RobotInfoWrapper::~RobotInfoWrapper()
{
}

//      *  <li>Status (Null-terminated string)</li>
//      *  <li>Mode (Null-terminated string)</li>
//      *  <li>Battery voltage (times 10) (2 Byte integer)</li>
//      *  <li>X position (4 byte floating point number)</li>
//      *  <li>Y position (4 byte floating point number)</li>
//      *  <li>Theta (2-byte floating point number)</li>
//      *  <li>Forward velocity (2-byte number)</li>
//      *  <li>Rotation velocity (2-byte number)</li>


/*!
    \fn RobotInfoWrapper::readPacket(void)
 */
bool RobotInfoWrapper::readPacket(ArNetPacket* packet)
{
 
  packet->bufToStr(robot_status, 63);
  packet->bufToStr(robot_mode, 63);
  battery = packet->bufToByte2();
  x_robot = packet->bufToByte4();
  y_robot = packet->bufToByte4();
  th_robot = packet->bufToByte2();
  linvel_robot = packet->bufToByte2();
  radvel_robot = packet->bufToByte2();

  return true;

}

/*!
    \fn RobotInfoWrapper::readExtPacket(void)
 */
bool RobotInfoWrapper::readExtPacket(ArNetPacket* packet)
{

  left_vel_robot = packet->bufToByte2();
  right_vel_robot = packet->bufToByte2();
  leftMotorStalled = (bool)packet->bufToByte();
  rightMotorStalled = (bool)packet->bufToByte();
//   char c[20];
//   packet->bufToData(c, 16);
  
  char *saved_locale;
  saved_locale = setlocale(LC_NUMERIC, "C");
 
  x_encoder = packet->bufToDouble();
  y_encoder = packet->bufToDouble();
  th_encoder = packet->bufToDouble();  
  
  setlocale(LC_NUMERIC, saved_locale); 
  
  secs = packet->bufToByte4();
  mSecs = packet->bufToByte4();  

  timeTaken.setSec(secs);
  timeTaken.setMSec(mSecs);
  timeGet.setToNow();
  return true;

}

/*!
    \fn RobotInfoWrapper::readSensorsList(void)
 */
bool RobotInfoWrapper::readSensorsList(ArNetPacket* packet)
{
  sensorsListReaded = false;
  sensorsList.clear();
  char packet_name[64];
  packet->bufToStr(packet_name, 63);
  if(!strcmp(packet_name, "sensorsList")) {
    int type = packet->bufToByte2();
    while(type) {
      sensorsList.push_back(type);
      type = packet->bufToByte2();     
    }
    sensorsListReaded = true;
  }
  return sensorsListReaded;
}

bool RobotInfoWrapper::hasSensorType(int type)
{
  bool ret = false;
  list<int>::const_iterator it;
  for (it = sensorsList.begin(); it != sensorsList.end(); ++it) {
    if((*it) == type) {
      ret = true;
      break;
    }
  }
  return ret;
}

void RobotInfoWrapper::setVel2(int lvel, int rvel)
{
//     char s[20];
//     sprintf(s, "%d %d %d", 32, lvel, rvel);

//     QString s;
//     if(realRobot) // Ñapa por que el comando de microcontrolador
//                   // VEL2 de Aria gira bien en MobileSim
//                   // y mal con los robots reales
//       s = QString::number(32) + " " + QString::number(NDMath::roundInt(lvel/diffConvFactor)) + " " + QString::number(NDMath::roundInt(rvel/diffConvFactor));
//     else
//       s = QString::number(32) + " " + QString::number(NDMath::roundInt(rvel/diffConvFactor)) + " " + QString::number(NDMath::roundInt(lvel/diffConvFactor));
      
// //     s = QString::number(32) + " " + QString::number(int(lvel)) + " " + QString::number(int(rvel));  // solo prueba en MobileSim con Amigo

//     ArNetPacket packet;
//     packet.strToBuf(s);
//     client->requestOnce("MicroControllerCommand", &packet);


  ArNetPacket sending;
  sending.strToBuf("Robot vel2");
  sending.byte2ToBuf(lvel*10);
  sending.byte2ToBuf(rvel*10);
  client->requestOnce("setVel2", &sending);
    
// //     client->request("MicroControllerCommand", 50, &packet);
}

void RobotInfoWrapper::stop(void)
{
  ArNetPacket sending;
  sending.strToBuf("Robot stop");
  client->requestOnce("stop", &sending);  
}
