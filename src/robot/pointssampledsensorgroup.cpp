/***************************************************************************
                          pointssampledsensorgroup.cpp  -  description
                             -------------------
    begin                : Wed Feb 23 2005
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

#include "pointssampledsensorgroup.h"
#include "sensorfactory.h"
#include "robotposition.h"
#include "../world/abstractworld.h"
#include "../neuraldis/settings.h"
#include "../neuraldis/ndmath.h"
#include "../neuraldis/xmloperator.h"

#include <QPixmap>

PointsSampledSensorGroup::PointsSampledSensorGroup(int sens_type) :
                    SensorGroup(sens_type, AbstractSensor::POINTS_SAMPLED)
{

    double rad = 15.0;
    uint n_sensors = 0;
    uint n_points;
    visible_color = "default";
    invisible_color = "default";

    if(sensor_type == AbstractSensor::KHEP_PROXIMITY) {
        n_sensors = 8;
        //      radius = diamtr/2.0;
        radius = rad;
        n_points = 15;
        double rots[] = {-160.0, -65.0, -45.0, -15.0, 15.0, 45.0, 65.0, 160.0};
        double orients[] = {180.0, -90.0, -45.0, 0.0, 0.0, 45.0, 90.0, 180.0};
        double pts_x[] = { 2.0, 4.0, 8.0, 13.0, 20.0,
                           6.0, 10.0, 16.0, 22.0, 29.0, 2.0, 4.0, 8.0, 13.0, 20.0};
        double pts_y[] = {-2.0,-4.0,-5.0,-7.0,-9.0,
                          0.0, 0.0, 0.0, 0.0, 0.0, 2.0, 4.0, 5.0, 7.0, 9.0};
        int pts_vals[] = {1023, 800, 600, 400, 60, 900, 750, 650, 160, 40,
                          1023, 800, 600, 400, 60};
        for(uint i = 0; i < n_points; i++) {
            points_sx.resize(i + 1);
            points_sx[i] = pts_x[i];
            points_sy.resize(i + 1);
            points_sy[i] = pts_y[i];
            points_vals.resize(i + 1);
            points_vals[i] = pts_vals[i];
        }
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
    else if(sensor_type == AbstractSensor::KHEP_BUMP) {
        radius = rad;
        double area_ang = 60.0;
        n_sensors = 3;
        n_points = 5;
        visible_color = "#ff00ff";
        double rots[] = {-32.5, 32.5, 180.0};
        double orients[] = {-32.5, 32.5, 180.0};
        double bsensor_rot = area_ang/n_points;
        int separation = 0;
        uint i;
        double ang;
        QPointF p;

        for(i = 0, ang = -area_ang/2.0; i < n_points; i++ ,ang += bsensor_rot) {
            points_sx.resize(i + 1);
            points_sy.resize(i + 1);
            p = NDMath::polarToCartesian(radius + separation, ang);
            points_sx[i] = p.x() - radius;
            points_sy[i] = p.y();
            points_vals.resize(i + 1);
            points_vals[i] = 1;
        }
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


PointsSampledSensorGroup::PointsSampledSensorGroup(const QDomElement& e) : SensorGroup(e)
{

    XMLOperator* xml_operator = XMLOperator::instance();
    QDomElement e_vis = xml_operator->findTag("visible_color", e);
    QDomElement e_invis = xml_operator->findTag("invisible_color", e);
    QDomElement e_pts = xml_operator->findTag("Points", e);

    visible_color = e_vis.text();
    invisible_color = e_invis.text();
    readPointsArrays(e_pts);

}              

PointsSampledSensorGroup::~PointsSampledSensorGroup()
{   
}

void PointsSampledSensorGroup::readPointsArrays(const QDomElement& e)
{

    double sx = 0;
    double sy = 0;
    int val = 0;
    bool ok;
    XMLOperator* xml_operator = XMLOperator::instance();
    QDomElement e_point = xml_operator->findTag("point", e);
    QDomNode n;
    while(!e_point.isNull()){
        sx = e_point.attribute("x").toDouble(&ok);
        sy = e_point.attribute("y").toDouble(&ok);
        val = e_point.attribute("val").toInt(&ok);
        int n_p = points_sx.count();
        points_sx.resize(n_p + 1);
        points_sx[n_p] = sx;
        points_sy.resize(n_p + 1);
        points_sy[n_p] = sy;
        points_vals.resize(n_p + 1);
        points_vals[n_p] = val;

        n = e_point.nextSibling();
        e_point = n.toElement();
    }

}

bool PointsSampledSensorGroup::createSensors(void)
{  

    QVector<double> sx(num_sensors);
    QVector<double> sy(num_sensors);
    QPointF p;
    QColor* pvis_color = 0;
    QColor* pinvis_color = 0;
    SensorFactory* factory = SensorFactory::instance();

    if(invisible_color != "default") {
        invis_color = QColor(invisible_color);
        if(invis_color.isValid())
            pinvis_color = &invis_color;
    }
    if(visible_color != "default") {
        vis_color = QColor(visible_color);
        if(vis_color.isValid())
            pvis_color = &vis_color;
    }
    
    for(uint i = 0; i < num_sensors; i++) {
        p = NDMath::polarToCartesian(radii[i], rotations[i]);
        sx[i] = p.x();
        sy[i] = p.y();
    }
    PointsSampledSensor* sen;
    for(uint i = 0; i < num_sensors; i++) {
        sen = factory->factoryPointsSampledSensor(sensor_type,
                             identifications[i], group_idents[i], radii[i],
                             rotations[i], orientations[i], 0);
        if(sen) {
            sen->initialize();
            points_sampled_sensors.append(sen);
            sensors.append(sen);
        }
    }

    QListIterator<PointsSampledSensor*> it(points_sampled_sensors);
//    points_pix_list = paintSequence(pvis_color, pinvis_color);
    uint i = 0;
    Settings* settings = Settings::instance();
    AbstractWorld* world = settings->getWorld();
    while(it.hasNext()) {
        sen = it.next();
        sen->setRobotPosition(robot_position);
        int n_p = points_sx.count();
        for(int j = 0; j < n_p; j++) {
            QPointF p = NDMath::rotateGrad(points_sx[j],
                                           points_sy[j], sx[i], sy[i], orientations[i]);
            SensorPoint* s_point = new SensorPoint(j + 1, p.x(), p.y(), orientations[i],
                                                   points_vals[j], robot_position,
                                                   visible_color, "red", world);
            sen->addPoint(s_point);
        }
        i++;
    }
    return true;
}

QList<QPixmap> PointsSampledSensorGroup::paintSequence(QColor *vis_color,
                                                       QColor *invis_color, int w, int h)
{

    QPixmap visible_pix(w, h);
    QPixmap invisible_pix(w, h);

    QList<QPixmap> l_pix;

    QPainter pv(&visible_pix);
    if(vis_color) {
        pv.setBrush(*vis_color);
        pv.setPen(Qt::NoPen);
    } else {
        pv.setBrush(Qt::black);
        pv.setPen(Qt::NoPen);
    }
    pv.drawRect(0, 0, w, h);

    QPainter pi(&invisible_pix);
    if(invis_color) {
        pi.setBrush(*invis_color);
        pi.setPen(Qt::NoPen);
    } else {
        Settings* settings = Settings::instance();
        AbstractWorld* world = settings->getWorld();
        pi.setBrush(world->getCanvas()->backgroundBrush());
        pi.setPen(Qt::NoPen);
    }
    pi.drawRect(0, 0, w, h);

    l_pix.append(invisible_pix);
    l_pix.append(visible_pix);

    return l_pix;

}	

void PointsSampledSensorGroup::scaleSensorField(const double& scale)
{

    if(!hasScalableField())
        return;
    if(scale < 0.0)
        return;
    uint n_points = points_sx.count();
    for(uint i = 0; i < n_points; i++) {
        points_sx[i] *= scale;
        points_sy[i] *= scale;

    }

    if(robot_position) {
        PointsSampledSensor* sen;
        QListIterator<PointsSampledSensor*> it(points_sampled_sensors);
        while(it.hasNext()) {
            sen = it.next();
            sen->changePointsPos(points_sx, points_sy, orientations,
                                 robot_position->rotation());
        }
    }

}

void PointsSampledSensorGroup::write_special(QDomDocument& doc, QDomElement& e)
{

    XMLOperator* xml_operator = XMLOperator::instance();

    e.appendChild(xml_operator->createTextElement(doc,
                                                  "visible_color", visible_color));
    e.appendChild(xml_operator->createTextElement(doc,
                                                  "invisible_color", invisible_color));

    QDomElement tag_pts = doc.createElement("Points");
    e.appendChild(tag_pts);
    for(int i = 0; i < points_sx.count(); i++) {
        QDomElement tag_pt_i = doc.createElement("point");
        tag_pts.appendChild(tag_pt_i);
        tag_pt_i.setAttribute("x", QString::number(points_sx[i]));
        tag_pt_i.setAttribute("y", QString::number(points_sy[i]));
        tag_pt_i.setAttribute("val", QString::number(points_vals[i]));
    }
    
}
