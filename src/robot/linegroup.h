//
// C++ Interface: linegroup
//
// Description:
//
//
// Author: Jose Manuel Cuadra Troncoso <jmcuadra@dia.uned.es>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef LINEGROUP_H
#define LINEGROUP_H

#include "sensorgroup.h"
#include "linearsensor.h"
#include <QRect>

/**
    @author Jose Manuel Cuadra Troncoso <jmcuadra@dia.uned.es>
*/
class LineGroup : public SensorGroup
{
public:
    LineGroup(const QDomElement& e);

    LineGroup(int sens_type, int sup_type);

    ~LineGroup();

    bool hasScalableField(void);
    QString getGroupName() const;
    bool createSensors(void);
    const QString& sourceName(void);
    QList<AbstractSensor *> &getSensors(void);
    void addSources(QList<RadialFieldSource*> sources);
    void sample(void);
    void scaleSensorField(const double& scale);
    void setInRobotPosition(void);
    void setRobotPosition(RobotPosition* rob_pos);
    void worldCleared(void);
    void writeSensors(QDomDocument& doc, QDomElement& e);
    void setGroupName(const QString& group_name);
    void setScalableField(bool has);
    void writeSensorsDefs(QDomDocument& doc, QDomElement& e);

    void showSensors(bool on);

    void moveBy(double dx, double dy, double ang);
    void rotate(void);
    void advance(int stage);
    void setVisible(bool on);

protected:
    void readSensorsPosId(const QDomElement& e);
    void write_special(QDomDocument& doc, QDomElement& e);
    void initProbabilityParams(void);

protected:
    QList<LinearSensor*> linear_sensors;
    bool idealSensor, showBeam;
    double sigma;
    QString beam_color;
    QRect boundRect;

};

#endif
