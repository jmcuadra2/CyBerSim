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

#include "ndariasonar.h"

int NDAriaSonar::sensorSuperType(void)
{
    return AbstractSensor::ARIA_SENSOR;
}

int NDAriaSonar::sensorType(void )
{
    return AbstractSensor::ARIA_SONAR;
}

NDAriaSonar::NDAriaSonar(int id, int group_idx, SonarInfoWrapper* sonar_inf,
                         QObject* parent, const char* name) :
                AriaRangeDevice(id, group_idx, sonar_inf, parent, name)
{
    sonar_info = sonar_inf;
    initParams();
}

NDAriaSonar::NDAriaSonar(int id, int group_idx, double x_, double y_, double rot,
                         double orient, SonarInfoWrapper* sonar_inf, QObject* parent,
                         const char* name) :
            AriaRangeDevice(id, group_idx, x_, y_, rot, orient, sonar_inf, parent, name)
{
    sonar_info = sonar_inf;
    initParams();
}

NDAriaSonar::NDAriaSonar(int id, int group_idx, double rad, double rot, double orient,
                         SonarInfoWrapper* sonar_inf, QObject* parent, const char* name) :
            AriaRangeDevice(id, group_idx, rad, rot, orient, sonar_inf, parent, name)
{
    sonar_info = sonar_inf;
    initParams();
}

NDAriaSonar::NDAriaSonar(int id, double rad, double start_ang, double end_ang,
                         int group_idx, SonarInfoWrapper* sonar_inf, QObject* parent,
                         const char* name) :
            AriaRangeDevice(id, rad, start_ang, end_ang, group_idx, sonar_inf, parent, name)
{
    sonar_info = sonar_inf;
    initParams();
}

NDAriaSonar::~NDAriaSonar()
{
}

/*!
    \fn NDAriaSonar::sample(void)
 */

double NDAriaSonar::sample(void)
{
    AriaRangeDevice::sample();
    if(sensorLine->isValidReading()) {
        if(emitSignal) {
            emit getSensorValue(rawReading);
            emit getSensorFValue((double)rawReading, group_index + 1);
        }
    }
    else {
        if(emitSignal) {
            emit getSensorValue(-1);
            emit getSensorFValue(-1.0, group_index + 1);
        }
    }
    writeMeasure();
    return out;
}

/*!
    \fn NDAriaSonar::initParams(void)
 */
void NDAriaSonar::initParams(void)
{
    sensorMaxVal = 4000;
    sensorWrongVal = 5000;
    sensorCriticVal = 100;
    aperture = 15/NDMath::RAD2GRAD;
    idealSensor = true;
}

void NDAriaSonar::initMeasure(void)
{
    vector<size_t> yDimensions(2);
    yDimensions[0] = 1;
    yDimensions[1] = 2;
    sensorPose = NDPose::createPose(NDPose::Pose2D);
    sensorPose->setCoordinates(x_org, y_org, orientation/NDMath::RAD2GRAD);
    measure.initMeasure(NDPose::Pose2D, idChar, "mm", yDimensions);
    measure.setPose(sensorPose);
}
