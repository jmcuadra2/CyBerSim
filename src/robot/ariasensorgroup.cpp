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

#include <ArClientBase.h>
#include <ArNetPacket.h>

#include "ariasensorgroup.h"
#include "sensorfactory.h"
#include "robotposition.h"
#include "../world/abstractworld.h"
#include "../neuraldis/settings.h"
#include "../neuraldis/ndmath.h"
#include "../neuraldis/xmloperator.h"
#include "sensorline.h"

AriaSensorGroup::AriaSensorGroup(const QDomElement& e, ArClientBase* cl, list<int> * actualSensors): SensorGroup(e)
{
    scalable_field = false;
    client = cl;
    wrapper = 0;
    actualSensorsList = actualSensors;
}


AriaSensorGroup::AriaSensorGroup(int sens_type, int sup_type, ArClientBase* cl, list<int> * actualSensors): SensorGroup(sens_type, sup_type)
{
    client = cl;
    wrapper = 0;
    actualSensorsList = actualSensors;
}

AriaSensorGroup::~AriaSensorGroup()
{
    if(wrapper)
        delete  wrapper;
}

bool AriaSensorGroup::createSensors(void) 
{

    bool ret = false;
    SensorFactory* factory = SensorFactory::instance();
    Settings* settings = Settings::instance();
    AbstractWorld* world = settings->getWorld();
    AriaSensor* sen;

    if(!hasSensor(sensor_type)) return ret;

    wrapper = factory->factoryAriaInfoWrapper(sensor_type, client);
    if(wrapper) {
        for(uint i = 0; i < num_sensors; i++) {
            if(polar_mode) {
                sen = factory->factoryAriaSensor(sensor_type,
                                                 identifications[i], radii[i], group_idents[i],
                                                 start_angles[i], end_angles[i], wrapper);
                if(sen) {
                    sen->initialize();
                    rotations[i] = sen->getRotation();
                    orientations[i] = sen->getOrientation();
                }
            }
            else {
                sen = factory->factoryAriaSensor(sensor_type,
                                                 identifications[i], group_idents[i], radii[i],
                                                 rotations[i], orientations[i], wrapper);
                if(sen)
                    sen->initialize();
            }
            if(!sen) continue;

            if(sensor_max_val >= 0) {
                //         client->addHandler("setSensorMaxRange", new ArFunctor1C<AriaSensorGroup, ArNetPacket*>(this, &AriaSensorGroup::readPacket));

                ArNetPacket sending;
                sending.strToBuf(wrapper->deviceName());

                double ss = sensor_max_val + .5;
                sending.byte4ToBuf(ss);
                //         sending.byte4ToBuf(sensor_max_val);
                client->requestOnce("setDeviceMaxRange", &sending);
                sen->setSensorMaxVal(sensor_max_val);
            }
            if(sensor_wrong_val >= 0)
                sen->setSensorWrongVal(sensor_wrong_val);
            if(sensor_critic_val >= 0)
                sen->setSensorCriticVal(sensor_critic_val);
            sen->setRobotPosition(robot_position);
            sen->setWorldScale(world->getWorldScale());
            sen->createSensorLine(world, drawing_type, drawing_color, drawing_color2);

            aria_sensors.append(sen);
            sensors.append(sen);
        }

        client->addHandler(wrapper->packetName(), new ArFunctor1C<AriaSensorGroup, ArNetPacket*>(this, &AriaSensorGroup::readPacket));
        client->requestOnce(wrapper->packetName());
        ret = true;
    }
    return ret;
}

bool AriaSensorGroup::hasSensor(int type) {
    bool ret = false;
    if(!actualSensorsList) return ret;

    list<int>::const_iterator it;
    for (it = actualSensorsList->begin(); it != actualSensorsList->end(); ++it) {
        if((*it) == type) {
            ret = true;
            break;
        }
    }
    return ret;
}

void AriaSensorGroup::readPacket(ArNetPacket* packet)
{  
    wrapper->readPacket(packet);
}

bool AriaSensorGroup::hasScalableField(void)
{
    return false;
}

void AriaSensorGroup::readSensorsPosId(const QDomElement& e)
{
    SensorGroup::readSensorsPosId(e);
}

void AriaSensorGroup::scaleSensorField(const double& /*scale*/)
{
}

void AriaSensorGroup::setInRobotPosition(void)
{
    SensorGroup::setInRobotPosition();
}


void AriaSensorGroup::write_special(QDomDocument& /*doc*/, QDomElement& /*e*/)
{
}

void AriaSensorGroup::setScalableField(bool)
{
    SensorGroup::setScalableField(false);
}

void AriaSensorGroup::sample(void)
{
    wrapper->setWaiting(true);
    client->requestOnce(wrapper->packetName());

    while(wrapper->isWaiting())
        ArUtil::sleep(10);

    SensorGroup::sample();
}
