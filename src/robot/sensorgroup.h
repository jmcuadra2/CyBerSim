/***************************************************************************
                          sensorgroup.h  -  description
                             -------------------
    begin                : Tue Feb 22 2005
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

#ifndef SENSORGROUP_H
#define SENSORGROUP_H

#include <QDomDocument>
#include "abstractsensor.h"

class RobotPositon;
class XMLOperator;

/*! \ingroup sensors  
    \brief Clase base abstracta para las factorias de sensores.
  */

class SensorGroup {

public:
    SensorGroup(int sens_type, int sup_type);
    SensorGroup(const QDomElement& e);
    virtual ~SensorGroup();

    virtual bool createSensors(void) = 0;
    
    virtual void setRobotPosition(RobotPosition *rob_pos);
    virtual void setInRobotPosition(void);
    
    virtual void scaleSensorField(const double& scale) = 0;
    bool hasScalableField(void) { return scalable_field; }
    void setScalableField(bool has) { scalable_field = has ; }
    virtual void worldCleared(void) { }
    virtual void addSources(QList<RadialFieldSource*> ) { }
    
    QList<AbstractSensor*>& getSensors(void) { return sensors ; }
    AbstractSensor* getSensors(int ident);
    
    virtual void writeSensors(QDomDocument& doc, QDomElement& e);
    void writeSensorsDefs(QDomDocument& doc, QDomElement& e);
    
    virtual const QString& sourceName(void) { return source_name ; }
    
    virtual void sample(void);
    void setGroupName ( const QString& group_name ) { groupName = group_name; }
    QString getGroupName() const { return groupName ; }

    virtual void showSensors(bool on);
    virtual void moveBy(double dx, double dy);
    virtual void moveBy(double dx, double dy, double ang);
    virtual void rotate(void);
    virtual void advance(int stage);
    virtual void setVisible(bool on);
    virtual void showField(bool show);
    virtual void initSensors(void);
    
    void setTimeMeasureTaken(TimeStamp timeMeasureTaken)
    { this->timeMeasureTaken = timeMeasureTaken ;}
    TimeStamp getTimeMeasureTaken(void) const { return timeMeasureTaken ;}

protected:
    virtual void write_special(QDomDocument& doc, QDomElement& e) = 0;
    virtual void readSensorsPosId(const QDomElement& e);

protected:
    static int individual_radius;
    double radius;
    QVector<double> radii;
    QVector<double> rotations;
    QVector<double> orientations;
    QVector<uint> identifications;
    QVector<double> start_angles;
    QVector<double> end_angles;
    QVector<uint> group_idents;
    QVector<double> x_orgs;
    QVector<double> y_orgs;
    bool polar_mode;
    uint num_sensors;
    int sensor_type;
    int super_type;
    RobotPosition *robot_position;
    bool scalable_field;
    QList<AbstractSensor*> sensors;
    QString source_name, file_name, groupName;
    XMLOperator* xml_operator;
    double x_center, y_center;
    int sensor_max_val, sensor_wrong_val, sensor_critic_val;
    DrawingItem::Drawing drawing_type;
    QString drawing_color, drawing_color2;
    bool cartesian_org;
    bool equally_spaced;
    double start_angle;
    double end_angle;
    double angle_increment;
    TimeStamp timeMeasureTaken;
    
};

#endif
