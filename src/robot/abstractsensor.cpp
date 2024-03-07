/***************************************************************************
                          abstractsensor.cpp  -  description
                             -------------------
    begin                : Mon Feb 7 2005
    copyright            : (C) 2005 by Jose M. Cuadra Troncoso
    email                : jmcuadra@dia.uned.es
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "abstractsensor.h"
#include "robotposition.h"
#include "../neuraldis/ndmath.h"

int AbstractSensor::sensorSuperType(void)
{
    return 0 ;
}

int AbstractSensor::sensorType(void)
{
    return 0 ;
}

AbstractSensor::AbstractSensor(int id, int group_idx, double x_, double y_, double rot, double orient, QObject *parent, const char *name ) : InputGenerator(id, parent, name), measure(Measure<>::createMeasure())
{

    ident = id;
    group_index = group_idx;
    x_org = x_;
    y_org = y_;
    QPointF polar = NDMath::cartesianToPolar(x_org, y_org);
    radius = polar.x();
    rotation_org = polar.y();
    start_angle =  0.0;
    end_angle = 0.0;
    polar_mode = false;

}

AbstractSensor::AbstractSensor(int id, int group_idx, double rad,
                               double rot, double orient, QObject *parent,
                               const char *name )
    : InputGenerator(id, parent, name), measure(Measure<>::createMeasure())
{

    ident = id;
    group_index = group_idx;
    radius = rad;
    start_angle =  0.0;
    end_angle = 0.0;
    QPointF p = NDMath::polarToCartesian(radius, rot);
    x_org = p.x();
    y_org = p.y();
    rotation_org = rot;
    while(rotation_org > 360.0) rotation_org -= 360.0;
    while(rotation_org < -360.0) rotation_org += 360.0;
    orientation = orient;
    polar_mode = false;

}


AbstractSensor::AbstractSensor(int id, int group_idx, QObject *parent, const char *name) : InputGenerator(id, parent, name), measure(Measure<>::createMeasure())
{

    x_org = 0.0;
    y_org = 0.0;
    radius = 0.0;
    start_angle =  0.0;
    end_angle = 0.0;
    ident = id;
    group_index = group_idx;
    rotation_org = 0.0;
    orientation = 0.0;
    polar_mode = false;

}

AbstractSensor::AbstractSensor(int id, double rad, double start_ang,
                               double end_ang, int group_idx, QObject *parent, const char *name) : InputGenerator(id, parent, name), measure(Measure<>::createMeasure())
{
    ident = id;
    group_index = group_idx;
    radius = rad;
    start_angle =  start_ang;
    end_angle = end_ang;
    rotation_org = (start_angle + end_angle)/2.0;
    orientation = rotation_org;
    QPointF p = NDMath::polarToCartesian(radius, rotation_org);
    x_org = p.x();
    y_org = p.y();

    polar_mode = true;

}

AbstractSensor::AbstractSensor(int id, double rad, double start_ang,
                               double end_ang, int group_idx, double x_, double y_,
                               QObject *parent, const char *name) : InputGenerator(id, parent, name), measure(Measure<>::createMeasure())
{
    ident = id;
    group_index = group_idx;
    radius = rad;
    start_angle =  start_ang;
    end_angle = end_ang;
    rotation_org = (start_angle + end_angle)/2.0;
    orientation = rotation_org;
    QPointF p = NDMath::polarToCartesian(radius, rotation_org);
    x_org = p.x() + x_;
    y_org = p.y() + y_;

    polar_mode = true;

}

AbstractSensor::~AbstractSensor()
{
    if ( sensorLine ) delete sensorLine;
    if(sensorPose) delete sensorPose;
}

void AbstractSensor::initialize(void)
{
    commonValues();
    initParams();
    initMeasure();
}

void AbstractSensor::commonValues(void)
{
    x = x_org;
    y = y_org;
    rotation = rotation_org;
    robot_position = 0;
    rawReading = -1;
    start_reading = 0; // a inicializar por subclases
    end_reading = 0;
    scale = 1;
    sensorLine = 0;
    x_reading = 0;
    y_reading = 0;
    sensorPose = 0;
    emitSignal = false;
    aperture = 0;
    idealSensor = false;

    strcpy(idChar, QString::number(ident).toLatin1().constData());

}

void AbstractSensor::initParams(void)
{
    sensorMaxVal = 100000;
    sensorWrongVal = 110000;
    sensorCriticVal = -1;

    aperture = 10/NDMath::RAD2GRAD;
}

void AbstractSensor::setPosition(double x_, double y_, double rot)
{

    x = x_;
    y = y_;
    rotation = rot;
    while(rotation > 360.0)
        rotation -= 360.0;
    while(rotation < -360.0)
        rotation += 360.0;


}

void AbstractSensor::write(QDomDocument& doc, QDomElement& e)
{

    //   QDomElement tag_sen = doc.createElement("Sensor");
    //   e.appendChild(tag_sen);
    //   InputGenerator::write(doc, tag_sen);
    //   tag_sen.setAttribute("group_ident", QString::number(getGroupIdent()));
    //
    //   tag_sen.setAttribute("radius", QString::number(radius));
    //   if(polar_mode) {
    //   }
    //   else {
    //     tag_sen.setAttribute("rotation", QString::number(rotation_org));
    //     tag_sen.setAttribute("orientation", QString::number(orientation));
    //   }

}

QString AbstractSensor::toString(void)
{
    QString info;
    QTextStream strm(&info);
    strm << "Identification: " << ident << Qt::endl;
    strm << "Raduis: " << radius << Qt::endl;
    strm << "X: " << x_org << Qt::endl;
    strm << "Y: " << y_org << Qt::endl;
    if(polar_mode) {
        strm << "Start angle: " << start_angle << Qt::endl;
        strm << "End angle: " << end_angle << Qt::endl;
    }
    else {
        strm << "Rotation: " << rotation_org << Qt::endl;
        strm << "Orientation: " << orientation << Qt::endl;
    }
    return info;
}

void AbstractSensor::setRobotPosition(RobotPosition* rob_position)
{
    robot_position = rob_position;
}

void AbstractSensor::setSensorCriticVal ( int criticVal )
{
    sensorCriticVal = criticVal;
}

void AbstractSensor::setSensorMaxVal ( int maxVal )
{
    sensorMaxVal = maxVal;
}

void AbstractSensor::setSensorWrongVal ( int wrongVal )
{
    sensorWrongVal = wrongVal;
}

SensorLine* AbstractSensor::createSensorLine(AbstractWorld* world,
                                             DrawingItem::Drawing drawing_type, const QString& color,
                                             const QString& color2, double angle,bool rotateWithRobot)
{
    if(sensorLine) return sensorLine;
    QPointF p_end = NDMath::rotateGradCanvas(x_org +(sensorMaxVal/scale),
                                             -y_org, x_org, -y_org, -orientation - angle);
    sensorLine = new SensorLine(ident, x_org, -y_org, p_end.x(), p_end.y(),
                                orientation - angle, world, robot_position, color, color2,
                                drawing_type, sensorType(), rotateWithRobot);
    sensorLine->setSensorVals(getSensorMaxVal(), getSensorWrongVal());
    return sensorLine;
}

void AbstractSensor::writeMeasure(void)
{
    measure.setPose(sensorPose);
    measure.setTimeStamp();
    vector<double> mm(2);
    if(rawReading >= sensorWrongVal && !getIdealSensor())
        mm[0] = -1;
    else
        mm[0] = (double)rawReading;
    mm[1] = aperture;
    measure.setY(mm);
}  

void AbstractSensor::monitorClosed(QObject* /*obj*/)
{
    emitSignal = false;
    emit disconnectRecord(group_index + 1);
}

QList<SensorLine*> AbstractSensor::createSensorLines(AbstractWorld* /*world*/, DrawingItem::Drawing /*drawing_type*/, const QString& /*color*/, const QString& /*color2*/, int /*resolution*/)
{
    QList<SensorLine*> list;
    return list;
}
