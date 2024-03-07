/***************************************************************************
                          sensorsset.cpp  -  description
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

#include "sensorsset.h"


SensorsSet::SensorsSet(void* robot_client, QObject* parent, char* name) : QObject(parent)
{
    client = robot_client;
}

SensorsSet::~SensorsSet()
{
    QHashIterator<QString, SensorGroup*> itg(sensorGroups);
    while (itg.hasNext()) {
        itg.next();
        delete itg.value();
    }

    //   QListIterator<BaseSimControl*> itc(sensorGroupControls);
    //   while (itc.hasNext())
    //     delete itc.next();
}

void SensorsSet::addSensorGroup(const QString& sg_id, SensorGroup* group)
{

    sensorGroups.insert(sg_id, group);
    QListIterator<AbstractSensor*> it(group->getSensors());
    while(it.hasNext())
        sensors.append(it.next());
}

QList<AbstractSensor*>& SensorsSet::getSensors(const QString& index)
{

    static QList<AbstractSensor*> sen_list;
    if(sensorGroups.contains(index))
        return sensorGroups[index]->getSensors();
    else
        return sen_list;

}

SensorGroup* SensorsSet::getSensorsGroup(const QString& index)
{
    SensorGroup* g_sen = 0;
    if(sensorGroups.contains(index))
        g_sen = sensorGroups[index];
    return g_sen;

}

void SensorsSet::worldCleared(void)
{

    QHashIterator<QString, SensorGroup*> it(sensorGroups);
    while (it.hasNext()) {
        it.next();
        it.value()->worldCleared();
    }
}

void SensorsSet::scaleSensorField(const double& scale)
{

    QHashIterator<QString, SensorGroup*> it(sensorGroups);
    while (it.hasNext()) {
        it.next();
        it.value()->scaleSensorField(scale);
    }

}

void SensorsSet::setRadialFieldSources(const QHash<QString, QList<RadialFieldSource*> >&rf_dict)
{

    QHashIterator<QString, QList<RadialFieldSource*> > itrf(rf_dict);
    QList<RadialFieldSource*> rfs;
    QString rf_name, sg_name;
    SensorGroup* sen_gr;

    while(itrf.hasNext()) {
        itrf.next();
        rf_name = itrf.key();
        rfs = itrf.value();
        QHashIterator<QString, SensorGroup*> it(sensorGroups);
        while ( it.hasNext()) {
            it.next();
            sen_gr = it.value();
            sg_name = sen_gr->sourceName();
            if(sen_gr->sourceName() == rf_name)
                sen_gr->addSources(rfs);
        }
    }

}

void SensorsSet::moveBy(double dx, double dy)
{
    QHashIterator<QString, SensorGroup*> it(sensorGroups);
    while (it.hasNext()) {
        it.next();
        it.value()->moveBy(dx, dy);
    }
}

void SensorsSet::moveBy(double dx, double dy, double ang)
{
    QHashIterator<QString, SensorGroup*> it(sensorGroups);
    while (it.hasNext()) {
        it.next();
        it.value()->moveBy(dx, dy, ang);
    }
}

void SensorsSet::rotate(void)
{
    QHashIterator<QString, SensorGroup*> it(sensorGroups);
    while (it.hasNext()) {
        it.next();
        it.value()->rotate();
    }
}

void SensorsSet::advance(int stage)
{
    QHashIterator<QString, SensorGroup*> it(sensorGroups);
    while (it.hasNext()) {
        it.next();
        it.value()->advance(stage);
    }
}

void SensorsSet::setVisible(bool on) 
{
    QHashIterator<QString, SensorGroup*> it(sensorGroups);
    while (it.hasNext()) {
        it.next();
        it.value()->setVisible(on);
    }
}

void SensorsSet::showField(bool show)
{
    QHashIterator<QString, SensorGroup*> it(sensorGroups);
    while (it.hasNext()) {
        it.next();
        it.value()->showField(show);
    }
}

void SensorsSet::sample(void)
{
    QHashIterator<QString, SensorGroup*> it(sensorGroups);
    while (it.hasNext()) {
        it.next();
        it.value()->sample();
    }
}

void SensorsSet::writeSensors(QDomDocument& doc, QDomElement& e)
{

    QDomElement tag_sensorgr = doc.createElement("Sensors_groups");
    e.appendChild(tag_sensorgr);
    QHashIterator<QString, SensorGroup*> it(sensorGroups);
    while (it.hasNext()) {
        it.next();
        QDomElement tag_sengr = doc.createElement(it.key());
        tag_sensorgr.appendChild(tag_sengr);
        it.value()->writeSensors(doc, tag_sengr);
    }

}

void SensorsSet::showSensors(bool checked, QString const& data)
{
    if(sensorGroups.contains(data))
        sensorGroups[data]->showSensors(checked);
}

void SensorsSet::initSensors(void)
{
    QHashIterator<QString, SensorGroup*> it(sensorGroups);
    while (it.hasNext()) {
        it.next();
        it.value()->initSensors();
    }
}

SensorGroup* SensorsSet::getSensorGroup(const QString& name, bool exact) const
{
    SensorGroup* group = 0;
    QHashIterator<QString, SensorGroup*> it(sensorGroups);
    while (it.hasNext()) {
        it.next();
        if(exact) {
            if(it.key() == name) {
                group = it.value();
                break;
            }
        }
        else {
            if(it.key().contains(name)) {
                group = it.value();
                break;
            }
        }
    }
    return group;
}
