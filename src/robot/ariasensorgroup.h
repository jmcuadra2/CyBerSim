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
#ifndef ARIASENSORGROUP_H
#define ARIASENSORGROUP_H

#include <list>
#include "sensorgroup.h"
#include "ariasensor.h"

class ArClientBase;
class AriaNetPacketWrapper;
class ArNetPacket;

using namespace std;

/**
    @author jose manuel <jose@pcjose>
*/
class AriaSensorGroup : public SensorGroup
{

public:
    AriaSensorGroup(const QDomElement& e, ArClientBase* cl, list<int> * actualSensors);
    
    AriaSensorGroup(int sens_type, int sup_type, ArClientBase* cl,  list<int> * actualSensors);

    virtual ~AriaSensorGroup();

    bool createSensors(void);
    bool hasScalableField(void);
    void readSensorsPosId(const QDomElement& e);
    void scaleSensorField(const double& scale);
    void setInRobotPosition(void);
    void write_special(QDomDocument& doc, QDomElement& e);
    void setScalableField(bool has);
    void sample(void);
    virtual void readPacket(ArNetPacket* packet);
    
protected:
    bool hasSensor(int type);

protected:
    ArClientBase* client;
    QList<AriaSensor*> aria_sensors;
    AriaNetPacketWrapper* wrapper;
    list<int> * actualSensorsList;

};

#endif
