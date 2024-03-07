/***************************************************************************
                          sensorpoint.cpp  -  description
                             -------------------
    begin                : vie feb 6 2004
    copyright            : (C) 2004 by Jose M. Cuadra Troncoso
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

#include "sensorpoint.h"
#include "../world/rtti.h"
#include "../world/world.h"
#include "../world/mirectangle.h"
#include "../world/miellipse.h"
#include "../world/mipolygon.h"
#include "../world/myline.h"

class QPen;
class QBrush;

SensorPoint::SensorPoint(int id, double x_org, double y_org, double org_rotation, int val,
                         RobotPosition* rob_pos, const QString& visible_color,
                         const QString& second_color, AbstractWorld* world,
                         bool rotateWithRobot)
{
    robot_position = rob_pos;
    ident = id;
    this->val = val;
    this->world = world;
    size = 2;
    this->rotateWithRobot = rotateWithRobot;
    x_old_trans = x_org - robot_position->x();
    y_old_trans = y_org - robot_position->y();

    x_org_old_trans = x_org - robot_position->x();
    y_org_old_trans = y_org - robot_position->y();

    drawing = dynamic_cast<DrawingItem*> (
                DrawingsFactory::factoryDrawingItem(DrawingItem::Rectangle,
                world->getCanvas(), visible_color, second_color));

    if(drawing) {
//        world->addSensorLine(this, drawing->canvasItem());
        drawing->setValidReading(true);
        drawing->setSize(size, size);
//        drawing->setGeometry(NDMath::roundInt(x_org) - size/2, NDMath::roundInt(y_org) - size/2,
//                      NDMath::roundInt(x_org) + size/2, NDMath::roundInt(y_org) + size/2);
//        drawing->setGeometry(0, 0, NDMath::roundInt(x_org), NDMath::roundInt(y_org));
                drawing->setGeometry(0, 0, 0, 0);

//        drawing->setReadingCoords(NDMath::roundInt(world->xRobotTransform(x_org)),
//                                  NDMath::roundInt(world->yRobotTransform(y_org)));
//        drawing->setReadingCoords(NDMath::roundInt(x_org), NDMath::roundInt(x_org));
    }

    originalRotation = org_rotation;
    if (rotateWithRobot)
        rotation  = originalRotation + robot_position->rotation();
    else
        rotation  = originalRotation;
    solver = 0;
    calculador = world->getBresenhamCalculador();
    qTree = getQuadtree();

//    setX(x_);
//    setY(y_);
//    setZValue(1000);
//    showInWorld(false);
//    setVisible(true);
//    setRect(0, 0, 4, 4);
//    show();

}

SensorPoint::~SensorPoint()
{
    if(drawing)
        delete drawing;
}

void SensorPoint::advance(int stage)
{

    switch ( stage ) {
    case 0: {
        break;
    }
    case 1:
        rotate();
        break;
    }
}

void SensorPoint::rotate()
{
    QPointF n_org_p;
    if (rotateWithRobot) {
        n_org_p = robot_position->rotateWithRobot(x_old_trans, y_old_trans);
//        n_org_p = robot_position->rotateWithRobot(100,
//                                                  0);
        rotation  = originalRotation + robot_position->rotation();
    }
    else {
        n_org_p.setX(robot_position->x() + x_old_trans);
        n_org_p.setY(robot_position->y() + y_old_trans);
    }

    if(drawing) {
//        drawing->setGeometry(NDMath::roundInt(n_org_p.x()) - size/2,
//                             NDMath::roundInt(n_org_p.y()) - size/2,
//                             NDMath::roundInt(n_org_p.x()) + size/2,
//                             NDMath::roundInt(n_org_p.y()) + size/2);
        xx = NDMath::roundInt(world->xRobotTransform(n_org_p.x()));
        yy = NDMath::roundInt(world->xRobotTransform(n_org_p.y()));
        drawing->setReadingCoords(xx, yy);
//        drawing->setReadingCoords(n_org_p.x(), n_org_p.y());
    }
}

void SensorPoint::moveBy(qreal dx, qreal dy)
{
    drawing->canvasItem()->moveBy(dx, dy);
}

//void SensorPoint::showInWorld(bool show)
//{
//    if(show == true)
//        setBrush(Qt::SolidPattern);
//    else
//        setBrush(Qt::NoBrush);
//}

int SensorPoint::sample(void)
{

    bool cnt = false;
    QList<QGraphicsItem *> l = drawing->canvasItem()->collidingItems(Qt::IntersectsItemBoundingRect);
    for (QList<QGraphicsItem *>::Iterator it=l.begin();it!=l.end(); ++it) {
        QGraphicsItem *hit = *it;
        if( hit->type() / 100 != RADIALFIELDSOURCE_RTTI && hit->type() != ROBOT_RTTI &&
                hit->type() != SPOINT_RTTI   && hit->type() != DRAWING_RTTI) {
            if(collidesWithItem(hit)) {
                cnt = true;
                break;
            }
        }
    }
    return cnt ? val : SP_NO_SAMPLE;

}

bool SensorPoint::collidesWithItem(const QGraphicsItem* i) const
{
    QRegion region = i->boundingRegion(QTransform());
    if(i->type() == ELLIPSE_RTTI) {
        MiEllipse* ellip = (MiEllipse*) i;
        return ellip->contains(xx, yy);
    }
    else if(i->type() == RECTANGLE_RTTI) {
        MiRectangle* rectangle = (MiRectangle*) i;
        return rectangle->contains(QPointF(xx, yy));
    }
    else if(i->type() == POLYGON_RTTI) {
        MiPolygon* poly = (MiPolygon*) i;
        return poly->contains(QPointF(xx, yy));
    }
    else {
        return i->boundingRect().contains(QPoint(xx, yy));
    }

}

void SensorPoint::changeRelativePos(double x_, double y_)
{
    setX(x_);
    setY(y_);
    QPointF p = NDMath::rotateGrad(x_ - robot_position->x(),
                                   y_ - robot_position->y(), 0.0, 0.0,
                                   -robot_position->rotation());
    x_old_trans = p.x();
    y_old_trans = p.y();
    rotate();

}

QuadTree* SensorPoint::getQuadtree()
{
    return world->getQuadTree();
}
