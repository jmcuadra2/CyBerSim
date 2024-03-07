//
// C++ Implementation: linearsensor
//
// Description:
//
//
// Author: Jose Manuel Cuadra Troncoso <jmcuadra@dia.uned.es>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "sensorline.h"
#include "linearsensor.h"

LinearSensor::LinearSensor ( int id, double rad, double start_ang, double end_ang,
                             int group_idx, QObject* parent, const char* name ) :
            AbstractSensor ( id, rad, start_ang, end_ang, group_idx, parent, name )
{
    showBeam = false;
    beamResolution = 0;
}


LinearSensor::LinearSensor ( int id, int group_idx, double rad, double rot, double orient,
                             QObject* parent, const char* name ) :
            AbstractSensor ( id, group_idx, rad, rot, orient, parent, name )
{
    showBeam = false;
    beamResolution = 0;
}


LinearSensor::LinearSensor ( int id, int group_idx, double x_, double y_, double rot,
                             double orient, QObject* parent, const char* name ) :
            AbstractSensor ( id, group_idx, x_, y_, rot, orient, parent, name )
{
    showBeam = false;
    beamResolution = 0;
}


LinearSensor::LinearSensor ( int id, int group_idx, QObject* parent, const char* name ) :
            AbstractSensor ( id, group_idx, parent, name )
{
    showBeam = false;
    beamResolution = 0;
}

LinearSensor::LinearSensor ( int id, double rad, double start_ang, double end_ang,
                             int group_idx, double x_, double y_, QObject* parent,
                             const char* name ) :
            AbstractSensor ( id, rad, start_ang, end_ang, group_idx, x_, y_, parent, name )
{
    showBeam = false;
    beamResolution = 0;
}


LinearSensor::~LinearSensor()
{
}

int LinearSensor::getRawReading (bool obtain)
{
    if(!obtain)
        return rawReading;

    rawReading = sensorLine->sample();
    x_reading = sensorLine->getGlobalIntersectionPoint().x();
    y_reading = sensorLine->getGlobalIntersectionPoint().y();

    return rawReading;
}

void LinearSensor::advance ( int stage )
{
    if ( sensorLine )
        sensorLine->advance ( stage );
}

void LinearSensor::moveBy ( double dx, double dy )
{
    //   if (sensorLine) sensorLine->moveBy(dx,dy);
}

void LinearSensor::moveBy ( double dx, double dy, double ang )
{
    if ( sensorLine )
        sensorLine->rotate ( dx,dy,ang );
}

void LinearSensor::rotate ( void )
{
    if ( sensorLine )
        sensorLine->rotate();
}

void LinearSensor::setPosition ( double x_, double y_, double rot )
{
    AbstractSensor::setPosition ( x_, y_, rot );
    moveBy ( x_,y_,rot );
}

void LinearSensor::setVisible ( bool on )
{
    sensorLine->setVisible ( on );
}

void LinearSensor::showField ( bool show )
{
    sensorLine->setShowColor ( show );
}

QList<SensorLine*> LinearSensor::createSensorLines(AbstractWorld* world,
                            DrawingItem::Drawing drawing_type, const QString& color,
                            const QString& color2, const QString& beam_color,
                            int resolution, bool showBeam)
{

    this->showBeam = showBeam;

    if(resolution == 0) {
        createSensorLine(world, drawing_type, color, color2);
        sensorLine->setProbabilisticParams(sigma);
        sensorLines.append(sensorLine);
    }
    else{
        double angleGrowth = aperture/resolution;
        double angleStart = -aperture/2;
        double angleEnd = aperture/2;
        for( double angle = angleStart; angle <= angleEnd ; angle += angleGrowth){
            if(showBeam)
                createSensorLine(world, drawing_type, beam_color, color2, angle);
            else
                createSensorLine(world, DrawingItem::NoDrawing, beam_color,color2, angle);
            sensorLine->setProbabilisticParams(sigma);
            sensorLines.append(sensorLine);
            sensorLine = 0;
        }

        rawReadings.resize(sensorLines.size());
        xReadings.resize(sensorLines.size());
        yReadings.resize(sensorLines.size());
        sensorLine = createSensorLine(world, drawing_type, color, color2);
    }
    return sensorLines;
}

void LinearSensor::setProbabilisticParams(double sigma)
{
    this->sigma = sigma;
}
