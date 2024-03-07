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
 
#include "bumperinfowrapper.h"

BumperInfoWrapper::BumperInfoWrapper(): AriaNetPacketWrapper()
{
  packet_name = "bumperReadings";
}


BumperInfoWrapper::~BumperInfoWrapper()
{
}


bool BumperInfoWrapper::readPacket(ArNetPacket* packet)
{
  char info_packet[RangeHeaderLenght];
  packet->bufToStr(info_packet, RangeHeaderLenght - 1);
  if(std::string(info_packet) != std::string(packet_name)) {
      ArLog::log(ArLog::Terse, "It isn't a bumpers info packet");
      return false;
  }
  uint n_front_bumpers = packet->bufToByte2();
  uint n_rear_bumpers = packet->bufToByte2();
  uint n_bumpers = n_front_bumpers + n_rear_bumpers;
  
  if(collisions.size() != n_bumpers)
  collisions.resize(n_bumpers);
  for (uint i = 0; i < n_bumpers; i++) 
    collisions[i] = bool(packet->bufToByte2());
  return true;
}


bool BumperInfoWrapper::getCollision(uint n_bumper)
{
  uint num_bumpers = collisions.size();  
  bool ret = n_bumper < num_bumpers ? collisions[n_bumper] : false; 
  return ret;
}
