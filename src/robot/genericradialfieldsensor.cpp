//
// C++ Implementation: genericradialfieldsensor
//
// Description:
//
//
// Author: Jose Manuel Cuadra Troncoso <jmcuadra@dia.uned.es>, (C) 2009
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "genericradialfieldsensor.h"

int GenericRadialFieldSensor::sensorSuperType(void)
{    
    return RADIAL_FIELD ;
}

int GenericRadialFieldSensor::sensorType(void)
{  
    return GENERIC_RADIAL_FIELD ;
}

GenericRadialFieldSensor::GenericRadialFieldSensor(int id, int group_idx, QObject* parent,
                         const char* name): RadialFieldSensor(id, group_idx, parent, name)
{
}


GenericRadialFieldSensor::GenericRadialFieldSensor(int id, int group_idx, double rad,
                         double rot, double orient, double arc_, double no_f_val,
                         double satur_val, double normval, QObject* parent, const char* name) :
                    RadialFieldSensor(id, group_idx, rad, rot, orient, arc_, no_f_val,
                                            satur_val, normval, parent, name)
{
}


GenericRadialFieldSensor::GenericRadialFieldSensor(int id, int group_idx, double x_,
                         double y_, double rot, double orient, double arc_,
                         double no_f_val, double satur_val, double normval,
                         QObject* parent, const char* name) :
                    RadialFieldSensor(id, group_idx, x_, y_, rot, orient, arc_,
                                              no_f_val, satur_val, normval, parent, name)
{
}


GenericRadialFieldSensor::~GenericRadialFieldSensor()
{
}


int GenericRadialFieldSensor::getRawReading(bool obtain)
{
    if (!obtain)
        return rawReading;
    double ang, d, dx, dy, rx, ry;

    RadialFieldSource *src;
    QListIterator<RadialFieldSource*> it(sources);
    rawReading = int(no_field_value);   // maxima distancia a la que se percibe

    rx = robot_position->x();
    ry = robot_position->y();

    while (it.hasNext()) {
        src = it.next();
        if (src->isOn()) {
            ang = robot_position->rotation()/NDMath::RAD2GRAD;
            double xx, yy;
            xx = src->xCenter();
            yy = src->yCenter();

            dx = xx - (rx+x_org*cos(ang) + y_org*sin(ang)); // el cambio de signo de NDAriaWorld en y
            dy = yy - (ry+x_org*sin(ang) - y_org*cos(ang));
            dy *= -1;
            d = sqrt(dx*dx + dy*dy);
            if(d <= saturation_value)
                rawReading = (int)saturation_value;
            else {
                d = (no_field_value - sqrt(dx*dx + dy*dy))/no_field_value;
                if (d > 0) {
                    ang = ang + atan2(dy,dx) - rotation/NDMath::RAD2GRAD;
                    while (ang > M_PI) ang -= 2*M_PI;
                    while (ang < -M_PI) ang += 2*M_PI;
                    if ((ang > -arc/2.0)&&(ang < arc/2.0))
                        rawReading -= (int)(radialFunction(d*d*d, ang));
                }
            }
        }
    }
    return rawReading;
}

void GenericRadialFieldSensor::initMeasure(void)
{
    vector<size_t> yDimensions(2);
    yDimensions[0] = 1;
    yDimensions[1] = 2;
    sensorPose = NDPose::createPose(NDPose::Pose2D);
    sensorPose->setCoordinates(x_org, y_org, orientation/NDMath::RAD2GRAD);
    measure.initMeasure(NDPose::Pose2D, idChar, "mm", yDimensions);
    measure.setPose(sensorPose);
}
