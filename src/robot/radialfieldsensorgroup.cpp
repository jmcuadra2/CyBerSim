/***************************************************************************
                          radialfieldsensorgroup.cpp  -  description
                             -------------------
    begin                : Thu Feb 24 2005
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

#include "radialfieldsensorgroup.h"
#include "sensorfactory.h"
#include "../world/world.h"
#include "../neuraldis/ndmath.h"
#include "../neuraldis/xmloperator.h"

RadialFieldSensorGroup::RadialFieldSensorGroup(int sens_type)
    : SensorGroup(sens_type, AbstractSensor::RADIAL_FIELD)
{

    double rad = 15.0;
    uint n_sensors = 0;
    double arc_ = 120;
    double no_field_val = 250;
    double saturation_val = 50;
    double norm_val = 275;

    if(sensor_type == AbstractSensor::KHEP_LIGHT) {
        n_sensors = 8;
        radius = rad;

        arc_ = 120;
        no_field_val = 250;
        saturation_val = 50;
        norm_val = 275.0;

        arc = arc_;
        no_field_value = no_field_val;
        saturation_value = saturation_val;
        norm_value = norm_val;

        double rots[] = {-160.0, -65.0, -45.0, -15.0, 15.0, 45.0, 65.0, 160.0};
        double orients[] = {180.0, -90.0, -45.0, 0.0, 0.0, 45.0, 90.0, 180.0};
        for(uint i = 0; i < n_sensors; i++) {
            rotations.resize(i + 1);
            rotations[i] = rots[i];
            while(rotations[i] > 360.0) rotations[i] -= 360.0;
            while(rotations[i] < -360.0) rotations[i] += 360.0;
            orientations.resize(i + 1);
            orientations[i] = orients[i];
            while(orientations[i] > 360.0) orientations[i] -= 360.0;
            while(orientations[i] < -360.0) orientations[i] += 360.0;
        }
    }
    num_sensors = n_sensors;

}

RadialFieldSensorGroup::RadialFieldSensorGroup(const QDomElement& e)
    : SensorGroup(e)
{

    bool ok;
    XMLOperator* xml_operator = XMLOperator::instance();
    QDomElement e_arc = xml_operator->findTag("arc", e);
    QDomElement e_no_f = xml_operator->findTag("no_field_value", e);
    QDomElement e_sat = xml_operator->findTag("saturation_value", e);
    QDomElement e_norm = xml_operator->findTag("normalization_value", e);
    QDomElement e_src = xml_operator->findTag("source_name", e);

    arc = e_arc.text().toDouble(&ok);
    arc = arc <= 0.0 ? 120.0 : arc;
    no_field_value = e_no_f.text().toDouble(&ok);
    saturation_value = e_sat.text().toDouble(&ok);
    no_field_value = no_field_value <= saturation_value ?
                saturation_value + 450.0 : no_field_value;
    norm_value = e_norm.text().toDouble(&ok);
    norm_value = norm_value <= no_field_value ?
                no_field_value + 25 : norm_value;
    source_name = e_src.text();
    num_sensors = identifications.count();

}

RadialFieldSensorGroup::~RadialFieldSensorGroup()
{}

bool RadialFieldSensorGroup::createSensors(void)
{  
    SensorFactory* factory = SensorFactory::instance();
    RadialFieldSensor* sen;
    for(uint i = 0; i < num_sensors; i++) {
        sen = factory->factoryRadialFieldSensor(sensor_type,
                                                identifications[i], group_idents[i], radii[i],
                                                rotations[i], orientations[i],
                                                arc/NDMath::RAD2GRAD, no_field_value,
                                                saturation_value, norm_value);
        if(sen) {
            sen->initialize();
            radial_field_sensors.append(sen);
            sensors.append(sen);
            sen->setRobotPosition(robot_position);
        }
    }
    return true;
}

void RadialFieldSensorGroup::scaleSensorField(const double& scale)
{

    if(scale > 0.0) {
        no_field_value *= scale;
        norm_value = no_field_value + saturation_value/2.0;
    }
    QListIterator<RadialFieldSensor*> it(radial_field_sensors);
    while (it.hasNext())
        it.next()->scaleField(scale);
}

void RadialFieldSensorGroup::worldCleared(void)
{

    QListIterator<RadialFieldSensor*> it(radial_field_sensors);
    while (it.hasNext())
        it.next()->getSources().clear();
}  

void RadialFieldSensorGroup::addSources(QList<RadialFieldSource*> sources)
{

    QListIterator<RadialFieldSensor*> it(radial_field_sensors);
    RadialFieldSensor* sen;
    while (it.hasNext()) {
        sen = it.next();
        QListIterator<RadialFieldSource*> it_s(sources);
        while(it_s.hasNext())
            sen->addSource(it_s.next());
    }

}

void RadialFieldSensorGroup::write_special(QDomDocument& doc, QDomElement& e)
{

    XMLOperator* xml_operator = XMLOperator::instance();
    e.appendChild(xml_operator->createTextElement(doc,
                                                  "arc", QString::number(arc)));
    e.appendChild(xml_operator->createTextElement(doc,
                                    "no_field_value", QString::number(no_field_value)));
    e.appendChild(xml_operator->createTextElement(doc,
                                    "saturation_value", QString::number(saturation_value)));
    e.appendChild(xml_operator->createTextElement(doc,
                                    "normalization_value", QString::number(norm_value)));
    e.appendChild(xml_operator->createTextElement(doc,
                                                  "source_name", source_name));

}
