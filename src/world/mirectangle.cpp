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

#include "rtti.h" 
#include "../neuraldis/ndmath.h"
#include "collisionsolver.h"
#include "mirectangle.h"
#include <QGraphicsItem>

MiRectangle::MiRectangle(QPolygon pa, uint thickness, QColor pen_color,
                         QColor fill_color, float depth,  int area_fill)
    : QGraphicsRectItem(), SolidObject()
{

    setZValue(1000 - depth); // ver MiEllipse::MiEllipse
    QPen pen;
    QBrush brush;
    setPoints(pa);

    pen.setWidth(thickness);
    pen.setColor(pen_color);

    setPen(pen);
    if(area_fill == -1)
        brush.setStyle(Qt::NoBrush);
    else
        brush.setStyle(Qt::SolidPattern);
    brush.setColor(fill_color);
    setBrush(brush);

    show();

}

MiRectangle::~MiRectangle()
{

}


int MiRectangle::type(void) const
{    
    return RECTANGLE_RTTI;
}

QPolygonF MiRectangle::getPoints(void)
{
    QPolygonF pol_points(rect());
    return pol_points;
}

void MiRectangle::setPoints(const QPolygonF& polygon)
{
    int xx, yy, ww, hh, n_points;
    QPolygon pol = polygon.toPolygon();
    xx = 1000000000;
    yy = 1000000000;
    ww = 0;
    hh = 0;
    n_points = polygon.count();
    for(int i = 0; i < n_points; i++) {
        xx = pol.at(i).x() < xx ? pol.at(i).x() : xx;
        yy = pol.at(i).y() < yy ? pol.at(i).y() : yy;
    }

    for(int i = 0; i < n_points; i++) {
        ww = abs(pol.at(i).x() - xx) > ww ? abs(pol.at(i).x() - xx) : ww;
        hh = abs(pol.at(i).y() - yy) > hh ? abs(pol.at(i).y() - yy) : hh;
    }
    setRect(xx, yy, ww, hh);
}
