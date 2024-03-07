/***************************************************************************
                          sensorgroup.cpp  -  description
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

#include "sensorgroup.h"
#include "robotposition.h"
#include "../neuraldis/settings.h"
#include "../neuraldis/xmloperator.h"

int SensorGroup::individual_radius = -1;

SensorGroup::SensorGroup(int sens_type, int sup_type)
{

    sensor_type = sens_type;
    super_type = sup_type;
    scalable_field = true;
    polar_mode = false;
    xml_operator = XMLOperator::instance();
    x_center = 12.5;
    y_center = 0.0;
    sensor_max_val = -1;
    sensor_wrong_val = -1;
    sensor_critic_val = -1;
    cartesian_org = false;
    equally_spaced = false;
    start_angle = 0.0;
    end_angle = 0.0;
    angle_increment = 0.0;
    drawing_type = DrawingItem::Rectangle;
}

SensorGroup::SensorGroup(const QDomElement& e)
{

    xml_operator = XMLOperator::instance();
    bool ok;
    QDomElement e_super = xml_operator->findTag("sensor_supertype", e);
    QDomElement e_type = xml_operator->findTag("sensor_type", e);
    QDomElement e_file = xml_operator->findTag("sensor_file", e);
    QDomElement e_rad = xml_operator->findTag("radius", e);
    QDomElement e_max = xml_operator->findTag("max_val", e);
    QDomElement e_wrong = xml_operator->findTag("wrong_val", e);
    QDomElement e_critic = xml_operator->findTag("critic_val", e);
    QDomElement e_ring_center = xml_operator->findTag("ring_center", e);
    QDomElement e_scal = xml_operator->findTag("is_scalable", e);
    QDomElement e_drawing_type = xml_operator->findTag("drawing_type", e);
    QDomElement e_drawing_color = xml_operator->findTag("drawing_color", e);
    QDomElement e_drawing_color2 = xml_operator->findTag("drawing_color2", e);
    QDomElement e_sens = xml_operator->findTag("Sensors", e);
    if(e_super.isNull() || e_type.isNull() || e_sens.isNull())
        return;
    super_type = e_super.text().toInt(&ok);
    sensor_type = e_type.text().toInt(&ok);
    file_name = e_file.text();
    radius = e_rad.text().toDouble(&ok);
    if(!e_max.isNull())
        sensor_max_val = e_max.text().toInt(&ok);
    if(!e_wrong.isNull())
        sensor_wrong_val = e_wrong.text().toInt(&ok);
    if(!e_critic.isNull())
        sensor_critic_val = e_critic.text().toInt(&ok);
    if(!e_ring_center.isNull()) {
        x_center = e_ring_center.attribute("x_center").toDouble(&ok);
        y_center = e_ring_center.attribute("y_center").toDouble(&ok);
    }
    else {
        x_center = 0.0;
        y_center = 0.0;
    }
    if(!e_drawing_type.isNull()) {
        int i_drawing_type = e_drawing_type.text().toInt(&ok);
        if(!ok)
            drawing_type = DrawingItem::Rectangle;
        else
            drawing_type = static_cast<DrawingItem::Drawing>(i_drawing_type);
    }
    else
        drawing_type = DrawingItem::Rectangle;
    if(!e_drawing_color.isNull())
        drawing_color = e_drawing_color.text();
    if(!e_drawing_color2.isNull())
        drawing_color2 = e_drawing_color2.text();

    scalable_field = e_scal.text().toInt(&ok);
    polar_mode = false;
    readSensorsPosId(e_sens);

}

SensorGroup::~SensorGroup(){

    QListIterator<AbstractSensor*> it(sensors);
    while (it.hasNext())
        delete it.next();
}

void SensorGroup::readSensorsPosId(const QDomElement& e)
{

    bool ok;
    uint start_id = 1;

    QDomElement e_polar = xml_operator->findTag("Polar_reading", e);
    if(!e_polar.isNull()) {
        polar_mode = true;
        int equal_spc = e_polar.attribute("equally_spaced").toInt(&ok);
        if(ok) equally_spaced = (bool) equal_spc;
        if(equally_spaced) {
            start_angle = e_polar.attribute("start_angle").toDouble(&ok);
            end_angle = e_polar.attribute("end_angle").toDouble(&ok);
            num_sensors = e_polar.attribute("num_sensors").toInt(&ok);
            if(!ok) num_sensors = 1;

            if(end_angle - start_angle >= 360)
                angle_increment = (end_angle - start_angle)/ (num_sensors + 1); // +1 para situar que al situar el sensor en el centro del sector
            else
                angle_increment = (end_angle - start_angle)/ num_sensors;

            start_id = e_polar.attribute("start_id").toInt(&ok);
            if(!ok) start_id = 1;
            rotations.resize(num_sensors);
            orientations.resize(num_sensors);
            start_angles.resize(num_sensors);
            end_angles.resize(num_sensors);
            identifications.resize(num_sensors);
            group_idents.resize(num_sensors);
            radii.fill(radius, num_sensors);
            x_orgs.resize(num_sensors);
            y_orgs.resize(num_sensors);

            for(uint i = 0; i < num_sensors; i++) {
                start_angles[i] = start_angle + i*angle_increment;
                end_angles[i] = start_angle + (i + 1)*angle_increment;
                rotations[i] = (start_angles[i] + end_angles[i])/2;
                orientations[i] = rotations[i];  //revisar debido a center no en (0,0)
                identifications[i] = start_id + i;

                group_idents[i] = int(i*angle_increment + 0.5);
                x_orgs[i] = x_center;
                y_orgs[i] = y_center;
            }
            return;
        }
    }
    QDomElement e_sen = xml_operator->findTag("Sensor", e);
    QDomNode n;
    int id_cartes = 0;
    while(!e_sen.isNull()) {
        double xx = x_center;
        double yy = y_center;
        double rotation = 0.0;
        double orientation = 0.0;
        double sen_radius = e_sen.attribute("radius").toDouble(&ok);
        cartesian_org = false;
//        if(!ok) {
        if(ok) {
            xx = e_sen.attribute("x_org").toDouble(&ok) + x_center;
            if(ok)
                yy = e_sen.attribute("y_org").toDouble(&ok) + y_center;
            if(ok) {
                cartesian_org = true;
                QPointF polar = NDMath::cartesianToPolar(xx, yy);
                sen_radius = polar.x();
                rotation = polar.y();
                x_orgs.resize(id_cartes + 1);
                y_orgs.resize(id_cartes + 1);
                x_orgs[id_cartes] = xx;
                y_orgs[id_cartes] = yy;
                ++id_cartes;
            }
            else
                sen_radius = radius;
        }

        if(polar_mode) {
            start_angle = e_sen.attribute("start_angle").toDouble();
            end_angle = e_sen.attribute("end_angle").toDouble();
        }
        else {
            if(!cartesian_org) {
                rotation = e_sen.attribute("rotation").toDouble();
                while(rotation > 180.0) rotation -= 360.0;
                while(rotation < -180.0) rotation += 360.0;
            }
            //  Realmente  en los grupos sensorio-motores la orientación debía ser relativa al grupo y no al robot (o supergrupos)
            orientation = e_sen.attribute("orientation").toDouble();
            while(orientation > 180.0) orientation -= 360.0;
            while(orientation < -180.0) orientation += 360.0;
        }
        uint identification = e_sen.attribute("identification").toUInt();
        uint group_idx = e_sen.attribute("group_ident").toUInt(&ok);
        int n_p = identifications.count();
        if(!ok)
            group_idx = n_p + 1;

        radii.resize(n_p + 1);
        radii[n_p] = sen_radius;
        rotations.resize(n_p + 1);
        orientations.resize(n_p + 1);
        if(polar_mode) {
            start_angles.resize(n_p + 1);
            start_angles[n_p] = start_angle;
            end_angles.resize(n_p + 1);
            end_angles[n_p] = end_angle;
        }
        else {
            rotations[n_p] = rotation;
            orientations[n_p] = orientation;
        }
        identifications.resize(n_p + 1);
        identifications[n_p] = identification;
        group_idents.resize(n_p + 1);
        group_idents[n_p] = group_idx;

        n = e_sen.nextSibling();
        e_sen = n.toElement();
    }
    num_sensors = identifications.count();

    if(num_sensors) { // ordenando por group_idents
        QVector<uint> agi_copy(num_sensors);
        for(uint i = 0; i < num_sensors; i++)
            agi_copy[i] = group_idents.at(i);
        std::sort(group_idents.begin(), group_idents.end());
        QVector<int> idx;
        idx.fill(-1, num_sensors);
        for(uint i = 0; i < num_sensors; i++) {
            idx[i] = agi_copy.indexOf(group_idents[i]);
            agi_copy[idx[i]] = 100000000;
        }

        for(uint i = 0; i < num_sensors; i++)
            agi_copy[i] = identifications.at(i);
        for(uint i = 0; i < num_sensors; i++)
            identifications[i] = agi_copy[idx[i]];

        QVector<double> adoub_copy(radii.count());
        for(int i = 0; i < radii.count(); i++)
            adoub_copy[i] = radii.at(i);
        for(int i = 0; i < radii.count(); i++)
            radii[i] = adoub_copy[idx[i]];

        for(int i = 0; i < rotations.count(); i++)
            adoub_copy[i] = rotations.at(i);
        for(int i = 0; i < rotations.count(); i++)
            rotations[i] = adoub_copy[idx[i]];

        for(int i = 0; i < orientations.count(); i++)
            adoub_copy[i] = orientations.at(i);
        for(int i = 0; i < orientations.count(); i++)
            orientations[i] = adoub_copy[idx[i]];

        for(int i = 0; i < start_angles.count(); i++)
            adoub_copy[i] = start_angles.at(i);
        for(int i = 0; i < start_angles.count(); i++)
            start_angles[i] = adoub_copy[idx[i]];

        for(int i = 0; i < end_angles.count(); i++)
            adoub_copy[i] = end_angles.at(i);
        for(int i = 0; i < end_angles.count(); i++)
            end_angles[i] = adoub_copy[idx[i]];

        for(int i = 0; i < x_orgs.count(); i++)
            adoub_copy[i] = x_orgs.at(i);
        for(int i = 0; i < x_orgs.count(); i++)
            x_orgs[i] = adoub_copy[idx[i]];

        for(int i = 0; i < y_orgs.count(); i++)
            adoub_copy[i] = y_orgs.at(i);
        for(int i = 0; i < y_orgs.count(); i++)
            y_orgs[i] = adoub_copy[idx[i]];
    }

}

void SensorGroup::setRobotPosition(RobotPosition *rob_pos)
{

    robot_position = rob_pos;
    QListIterator<AbstractSensor*> it(sensors);
    while (it.hasNext())
        it.next()->setRobotPosition(robot_position);

}

void SensorGroup::setInRobotPosition(void)
{

    QListIterator<AbstractSensor*> it(sensors);
    while (it.hasNext())
        it.next()->setPosition(robot_position->x(), robot_position->y(),
                               robot_position->rotation());
}

void SensorGroup::writeSensors(QDomDocument& doc, QDomElement& e)
{    
    if(!num_sensors) return;

    e.appendChild(xml_operator->createTextElement(doc, "radius",
                                                  QString::number(radius)));
    QString relativeName = file_name;
    e.appendChild(xml_operator->createTextElement(doc, "sensor_file",
                  relativeName.remove(Settings::instance()->getRobotDirectory()
                                       + QDir::separator())));
    QDomElement tag_center = doc.createElement("ring_center");
    e.appendChild(tag_center);
    tag_center.setAttribute("x_center", QString::number(x_center));
    tag_center.setAttribute("y_center", QString::number(y_center));

    QDomElement tag_sens = doc.createElement("Sensors");
    e.appendChild(tag_sens);

    if(polar_mode && equally_spaced) {
        QDomElement tag_polar = doc.createElement("Polar_reading");
        tag_sens.appendChild(tag_polar);
        tag_polar.setAttribute("equally_spaced", "1");
        tag_polar.setAttribute("start_angle", QString::number(start_angle));
        tag_polar.setAttribute("end_angle", QString::number(end_angle));
        tag_polar.setAttribute("num_sensors", QString::number(num_sensors));
        tag_polar.setAttribute("start_id", QString::number(identifications[0]));
    }
    else {
        if(polar_mode) {
            QDomElement tag_polar = doc.createElement("Polar_reading");
            tag_sens.appendChild(tag_polar);
            tag_polar.setAttribute("equally_spaced", "0");
        }

        for(uint i = 0; i < num_sensors;  i++) {
            QDomElement tag_sen = doc.createElement("Sensor");
            tag_sens.appendChild(tag_sen);

            tag_sen.setAttribute("identification", QString::number(identifications[i]));
            tag_sen.setAttribute("group_ident", QString::number(group_idents[i]));
            if(polar_mode ) {
                tag_sen.setAttribute("radius", QString::number(radii[i]));
                tag_sen.setAttribute("start_angle", QString::number(start_angles[i]));
                tag_sen.setAttribute("end_angle", QString::number(end_angles[i]));
            }
            else {
                if(cartesian_org) {
                    tag_sen.setAttribute("x_org", QString::number(x_orgs[i]));
                    tag_sen.setAttribute("y_org", QString::number(y_orgs[i]));
                    tag_sen.setAttribute("orientation", QString::number(orientations[i]));
                }
                else {
                    tag_sen.setAttribute("radius", QString::number(radii[i]));
                    tag_sen.setAttribute("rotation", QString::number(rotations[i]));
                    tag_sen.setAttribute("orientation", QString::number(orientations[i]));
                }
            }
        }
    }

}

void SensorGroup::writeSensorsDefs(QDomDocument& doc, QDomElement& e)
{

    e.appendChild(xml_operator->createTextElement(doc, "sensor_supertype",
                                                  QString::number(super_type)));
    e.appendChild(xml_operator->createTextElement(doc, "sensor_type",
                                                  QString::number(sensor_type)));
    e.appendChild(xml_operator->createTextElement(doc, "is_scalable",
                                                  QString::number(scalable_field)));
    e.appendChild(xml_operator->createTextElement(doc, "radius",
                                                  QString::number(radius)));
    e.appendChild(xml_operator->createTextElement(doc, "max_val",
                                                  QString::number(sensor_max_val)));
    e.appendChild(xml_operator->createTextElement(doc, "wrong_val",
                                                  QString::number(sensor_wrong_val)));
    e.appendChild(xml_operator->createTextElement(doc, "critic_val",
                                                  QString::number(sensor_critic_val)));
    e.appendChild(xml_operator->createTextElement(doc, "drawing_type",
                                                  QString::number(drawing_type)));
    e.appendChild(xml_operator->createTextElement(doc, "drawing_color",
                                                  drawing_color));
    e.appendChild(xml_operator->createTextElement(doc, "drawing_color2",
                                                  drawing_color2));

    write_special(doc, e);

}

void SensorGroup::sample(void)
{
//    timeMeasureTaken = TimeStamp::now();
    timeMeasureTaken = TimeStamp::nowMicro();
    QListIterator<AbstractSensor*> it(sensors);
    while (it.hasNext())
        it.next()->sample();
}

void SensorGroup::showSensors(bool on)
{
    QListIterator<AbstractSensor*> it(sensors);
    while (it.hasNext())
        it.next()->showField(on);
}

AbstractSensor* SensorGroup::getSensors(int ident)
{
    AbstractSensor* sensor = 0;
    QListIterator<AbstractSensor*> it(sensors);
    while (it.hasNext()) {
        sensor = it.next();
        if(sensor->getIdent() == ident)
            break;
        else
            sensor = 0;
    }

    return sensor;
}

void SensorGroup::moveBy(double dx, double dy)
{
    QListIterator<AbstractSensor*> it(sensors);
    while (it.hasNext())
        it.next()->moveBy(dx, dy);
}

void SensorGroup::moveBy(double dx, double dy, double ang)
{
    QListIterator<AbstractSensor*> it(sensors);
    while (it.hasNext())
        it.next()->moveBy(dx, dy, ang);
}

void SensorGroup::rotate(void)
{
    QListIterator<AbstractSensor*> it(sensors);
    while (it.hasNext())
        it.next()->rotate();
}

void SensorGroup::advance(int stage)
{

    QListIterator<AbstractSensor*> it(sensors);
    while (it.hasNext())
        it.next()->advance(stage);
}

void SensorGroup::setVisible(bool on)
{

    QListIterator<AbstractSensor*> it(sensors);
    while (it.hasNext())
        it.next()->setVisible(on);
}

void SensorGroup::showField(bool show)
{   
    QListIterator<AbstractSensor*> it(sensors);
    while (it.hasNext())
        it.next()->showField(show);
}

void SensorGroup::initSensors(void)
{
    QListIterator<AbstractSensor*> it(sensors);
    while (it.hasNext())
        it.next()->initSensor();
}
