//
// C++ Implementation: nomadirsensor
//
// Description: 
//
//
// Author: Javier Garcia Misis <>, (C) 2010
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "../nomad/nomadclientconnector.h"

#include "nomadirsensor.h"

NomadIRSensor::NomadIRSensor(int id, int group_idx, QObject* parent, const char* name) :
                NomadSensor(id, group_idx, parent, name)
{
}

NomadIRSensor::NomadIRSensor(int id, int group_idx, double x_, double y_, double rot,
                             double orient, QObject* parent, const char* name) :
                NomadSensor(id, group_idx, x_, y_, rot, orient, parent, name)
{
}

NomadIRSensor::NomadIRSensor(int id, int group_idx, double rad, double rot,
                             double orient, QObject* parent, const char* name) :
                NomadSensor(id, group_idx, rad, rot, orient, parent, name)
{
}

NomadIRSensor::NomadIRSensor(int id, double rad, double start_ang, double end_ang,
                             int group_idx, QObject* parent, const char* name) :
                NomadSensor(id, rad, start_ang, end_ang, group_idx, parent, name)
{
}

NomadIRSensor::~NomadIRSensor()
{
}

int NomadIRSensor::getRawReading(bool obtain ) 
{
    if(!obtain) return rawReading;

    double rawdata = getConnector()->getStatus()->getInfraredData(getIdLocal());
    
    rawReading = rawdata /* inch */ * 25.4 /* mm/inch */ ;// /* 1.6 /*valor de ajuste*/;

    return NomadSensor::getRawReading(obtain);
}

void NomadIRSensor::initMeasure(void)
{
    vector<size_t> yDimensions(2);
    yDimensions[0] = 1;
    yDimensions[1] = 2;
    sensorPose = NDPose::createPose(NDPose::Pose2D);
    sensorPose->setCoordinates(x_org, y_org, orientation/NDMath::RAD2GRAD);
    measure.initMeasure(NDPose::Pose2D, idChar, "mm", yDimensions);
    measure.setPose(sensorPose);
}
