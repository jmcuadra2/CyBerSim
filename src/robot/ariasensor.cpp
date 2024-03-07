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
 
#include "ariasensor.h"
#include "../aria-tools/arianetpacketwrapper.h"

AriaSensor::AriaSensor(int id, int group_idx, double x_, double y_, double rot, double orient, AriaNetPacketWrapper* range_info, QObject* parent, const char* name): AbstractSensor(id, group_idx, x_, y_, rot, orient, parent, name)
{
}

AriaSensor::AriaSensor(int id, int group_idx, double rad, double rot, double orient, AriaNetPacketWrapper* sensor_info, QObject* parent, const char* name): AbstractSensor(id, group_idx, rad, rot, orient, parent, name)
{
}

AriaSensor::AriaSensor(int id, int group_idx, AriaNetPacketWrapper* range_info, QObject* parent, const char* name): AbstractSensor(id, group_idx, parent, name)
{
}

AriaSensor::AriaSensor(int id, double rad, double start_ang, double end_ang, int group_idx, AriaNetPacketWrapper* sensor_info, QObject* parent, const char* name): AbstractSensor(id, rad, start_ang, end_ang, group_idx, parent, name)
{
}

AriaSensor::~AriaSensor()
{
}

void AriaSensor::advance(int stage)
{
}

void AriaSensor::moveBy(double dx, double dy)
{
}

void AriaSensor::moveBy(double dx, double dy, double ang)
{
}

void AriaSensor::rotate(void)
{
}

void AriaSensor::showField(bool show)
{
}


