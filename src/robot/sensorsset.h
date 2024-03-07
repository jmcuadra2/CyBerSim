/***************************************************************************
                          sensorsset.h  -  description
                             -------------------
    begin                : Wed Mar 2 2005
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

#ifndef SENSORSSET_H
#define SENSORSSET_H

#include <list>
#include <QHash>
#include <QList>
#include <QDomDocument>

#include "sensorgroup.h"
#include "abstractsensor.h"
#include "../control/basesimcontrolfactory.h"

using namespace std;

/*! \ingroup sensors 
    sensores
    \brief Esta clase define un proxy a traves del cual el cliente (el robot)
    accede a la interfaz comun definida en AbstractSensor.
  */

class SensorsSet : public QObject{

    Q_OBJECT
public:
    SensorsSet(void* robot_client = 0, QObject* parent = 0, char* name = 0);
    ~SensorsSet();

    void addSensorGroup(const QString& sg_id, SensorGroup* group);
    QList<AbstractSensor *> &getSensors(const QString& index);
    QList<AbstractSensor*>& getSensors(void) { return sensors; }
    virtual void worldCleared(void);
    virtual void moveBy(double dx, double dy);
    virtual void moveBy(double dx, double dy, double ang);
    virtual void rotate(void);
    virtual void advance(int stage);
    virtual void setVisible(bool on);
    virtual void showField(bool show);
    virtual void sample(void);
    virtual void scaleSensorField(const double& scale);
    SensorGroup* getSensorsGroup(const QString& index);
    void writeSensors(QDomDocument& doc, QDomElement& e);
    void* getClient(void) { return client; };
    void setActualSensorsList ( list< int >* sensorsList) { realSensorsList = sensorsList; }
    list< int > * getActualSensorsList() const { return realSensorsList; }
    
    QHash<QString, SensorGroup*> getSensorGroups(void) const { return sensorGroups ; }
    SensorGroup* getSensorGroup(const QString& name, bool exact = false) const;

    void showSensors(bool checked, QString const& data);

    void initSensors(void);

public slots:
    virtual void setRadialFieldSources(const QHash<QString, QList<RadialFieldSource*> >& rf_dict);

public:
    int scale;
    
private:
    QHash<QString, SensorGroup*> sensorGroups;
    QList<AbstractSensor*> sensors;
    void* client;
    list<int> *realSensorsList;

};

#endif
