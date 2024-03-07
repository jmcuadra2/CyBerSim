//
// C++ Implementation: ndariabumper
//
// Description: 
//
//
// Author: Jose Manuel Cuadra Troncoso <jmcuadra@dia.uned.es>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//

#include "ndariabumper.h"

int NDAriaBumper::sensorSuperType(void)
{
    return AbstractSensor::ARIA_SENSOR;
}

int NDAriaBumper::sensorType(void )
{
    return AbstractSensor::ARIA_BUMP;
}

NDAriaBumper::NDAriaBumper(int id, double rad, double start_ang, double end_ang,
                           int group_idx, BumperInfoWrapper* bumper_inf, QObject* parent,
                           const char* name) :
            AriaSensor(id, rad, start_ang, end_ang, group_idx, bumper_inf, parent, name)
{
    bumper_info = bumper_inf;
}


NDAriaBumper::NDAriaBumper(int id, int group_idx, BumperInfoWrapper* bumper_inf,
                           QObject* parent, const char* name) :
            AriaSensor(id, group_idx, bumper_inf, parent, name)
{
    bumper_info = bumper_inf;
}

NDAriaBumper::NDAriaBumper(int id, int group_idx, double rad, double rot,
                           double orient, BumperInfoWrapper* bumper_inf, QObject* parent,
                           const char* name) :
            AriaSensor(id, group_idx, rad, rot, orient, bumper_inf, parent, name)
{
    bumper_info = bumper_inf;
}


NDAriaBumper::NDAriaBumper(int id, int group_idx, double x_, double y_, double rot,
                           double orient, BumperInfoWrapper* bumper_inf, QObject* parent,
                           const char* name) :
            AriaSensor(id, group_idx, x_, y_, rot, orient, bumper_inf, parent, name)
{
    bumper_info = bumper_inf;
}


NDAriaBumper::~NDAriaBumper()
{
}


/*!
    \fn NDAriaBumper::sample(void)
 */

double NDAriaBumper::sample(void)
{
    int p_raw = rawReading;
    rawReading = getRawReading();
    if(p_raw == 1 || rawReading == 1) {
        if(emitSignal) {
            emit getSensorValue(rawReading);
            emit getSensorFValue((double)rawReading, group_index + 1);
        }
    }
    out = (double)rawReading;
    writeMeasure();
    return out;
} 

int NDAriaBumper::getRawReading(bool obtain)
{
    if(!obtain) return rawReading;
    return bumper_info->getCollision(group_index);
}

void NDAriaBumper::initMeasure(void)
{
    vector<size_t> yDimensions(2);
    yDimensions[0] = 1;
    yDimensions[1] = 2;
    sensorPose = NDPose::createPose(NDPose::Pose2D);
    sensorPose->setCoordinates(x_org, y_org, orientation/NDMath::RAD2GRAD);
    measure.initMeasure(NDPose::Pose2D, idChar, "logical", yDimensions);
    measure.setPose(sensorPose);
}
