//
// C++ Implementation: nomadbumpsensor
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
#include "nomadbumpsensor.h"

NomadBumpSensor::NomadBumpSensor(int id, int group_idx, QObject* parent, const char* name) :
                NomadSensor(id, group_idx, parent, name)
{
}


NomadBumpSensor::NomadBumpSensor(int id, int group_idx, double x_, double y_, double rot,
                                 double orient, QObject* parent, const char* name) :
                NomadSensor(id, group_idx, x_, y_, rot, orient, parent, name)
{
}

NomadBumpSensor::NomadBumpSensor(int id, int group_idx, double rad, double rot,
                                 double orient, QObject* parent, const char* name) :
                NomadSensor(id, group_idx, rad, rot, orient, parent, name)
{
}

NomadBumpSensor::NomadBumpSensor(int id, double rad, double start_ang, double end_ang,
                                 int group_idx, QObject* parent, const char* name):
                NomadSensor(id, rad, start_ang, end_ang, group_idx, parent, name)
{
}

NomadBumpSensor::~NomadBumpSensor()
{
}

double NomadBumpSensor::sample(void){
    int p_raw = rawReading;
    rawReading = getRawReading();

    if(rawReading != 0) {
        rawReading = 1;
        out = 1;
        sensorLine->setValidReading(true);
        sensorLine->scale(xReading(), yReading());
        sensorLine->setColor(QColor("red"));
    }
    else {
        rawReading = 0;
        out = 0;
        prev_out = out;
        sensorLine->setValidReading(true);
        sensorLine->scale(xReading(), yReading());
        sensorLine->setColor(QColor("green"));
    }

    if(p_raw == 1 || rawReading == 1) {
        if(emitSignal) {
            emit getSensorValue(rawReading);
            emit getSensorFValue((double)rawReading, group_index + 1);
        }
    }

    writeMeasure();
    return out;
}

int NomadBumpSensor::getRawReading(bool obtain){
    if(!obtain) return rawReading;

    rawReading = getConnector()->getStatus()->getBumperData(getIdLocal());

    sensorLine->sampleSimple();

    x_reading = sensorLine->getGlobalIntersectionPoint().x();
    y_reading = sensorLine->getGlobalIntersectionPoint().y();

    return rawReading;
}

void NomadBumpSensor::initMeasure(void){
    vector<size_t> yDimensions(2);
    yDimensions[0] = 1;
    yDimensions[1] = 2;
    sensorPose = NDPose::createPose(NDPose::Pose2D);
    sensorPose->setCoordinates(x_org, y_org, orientation/NDMath::RAD2GRAD);
    measure.initMeasure(NDPose::Pose2D, idChar, "logical", yDimensions);
    measure.setPose(sensorPose);
}


SensorLine* NomadBumpSensor::createSensorLine(AbstractWorld* world,
                    DrawingItem::Drawing drawing_type, const QString& color,
                    const QString& color2, double angle, bool )
{
    return NomadSensor::createSensorLine(world, drawing_type, color, color2, angle, false );
}
