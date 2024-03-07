//
// C++ Implementation: myline
//
// Description: 
//
//
// Author: Jose Manuel Cuadra Troncoso <jmcuadra@dia.uned.es>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//

#include "rtti.h"
#include "myline.h"
#include "collisionsolver.h"
#include "material.h"

MyLine::MyLine() : QGraphicsLineItem(), SolidObject()
{

}

MyLine::~MyLine()
{

}

int MyLine::type(void) const
{   
    return LINE_RTTI;
}

QPolygonF MyLine::getPoints(void)
{
    QPolygonF pol_points;
    pol_points.append(QPoint(line().x1(), line().y1()));
    pol_points.append(QPoint(line().x2(), line().y2()));
    return pol_points;
}

void MyLine::setPoints(const QPolygonF& polygon)
{
    if(polygon.size() == 2) {
        QPolygon pol = polygon.toPolygon();
        QGraphicsLineItem::setLine(pol.at(0).x(), pol.at(0).y(), pol.at(1).x(),
                                   pol.at(1).y());
    }
}
