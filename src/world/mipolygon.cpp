/***************************************************************************
 *   Copyright (C) 2004 by jose manuel                                     *
 *   jose@pcjose                                                           *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#include <QPainter>

#include "rtti.h"
#include "collisionsolver.h"
#include "mipolygon.h"
#include <QGraphicsItem>

MiPolygon::MiPolygon(QPolygonF pa, uint thickness, QColor pen_color,
                     QColor fill_color, float depth,  int area_fill)
    : QGraphicsPolygonItem(), SolidObject()
{

    setZValue(1000 - depth);
    setPoints(pa);
    pol_points = polygon();


    QPen pen;
    QBrush brush;
    pen.setWidth(thickness);
    pen.setColor(pen_color);

    if(area_fill == -1)
        brush.setStyle(Qt::NoBrush);
    else
        brush.setStyle(Qt::SolidPattern);
    brush.setColor(fill_color);
    setPen(pen);
    setBrush(brush);

    show();

}

MiPolygon::~MiPolygon()
{

}

int MiPolygon::type(void) const
{   
    return POLYGON_RTTI;
}

QPolygonF MiPolygon::getPoints(void)
{
    return pol_points;
}

void MiPolygon::setPoints(const QPolygonF& polygon)
{
    QGraphicsPolygonItem::setPolygon(polygon.toPolygon());
}
