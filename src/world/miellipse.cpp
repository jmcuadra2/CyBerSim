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
#include <QtGui>

#include "rtti.h"
#include "../neuraldis/ndmath.h"
#include "collisionsolver.h"
#include "miellipse.h"

MiEllipse::MiEllipse(QPolygonF pa, uint thickness, QColor pen_color, QColor fill_color,
                     float depth, int area_fill)
    : QGraphicsEllipseItem(), SolidObject()
{
    setPoints(pa);

    QBrush brush;

    if(area_fill == -1)
        brush.setStyle(Qt::NoBrush);
    else
        brush.setStyle(Qt::SolidPattern);
    brush.setColor(fill_color);

    setBrush(brush);
    setZValue(1000 - depth);
    // precedencia delante-detras
    // es al reves, y va de 0-999

    show();

}

MiEllipse::~MiEllipse()
{    
}

int MiEllipse::type(void) const
{
    return ELLIPSE_RTTI;
}

void MiEllipse::setParams(double center_x, double center_y, double radius_x, double radius_y, double angle)
{

    x_radius = radius_x;
    y_radius = radius_y;
    x_center = double(center_x);
    y_center = double(center_y);
    rot = angle;
    a2 = cos(rot)*cos(rot)/(x_radius*x_radius)+sin(rot)*sin(rot)/(y_radius*y_radius);
    b2 = sin(rot)*sin(rot)/(x_radius*x_radius)+cos(rot)*cos(rot)/(y_radius*y_radius);
    a2b = 2*(-sin(rot)*cos(rot)/(x_radius*x_radius) + sin(rot)*cos(rot)/(y_radius*y_radius));

    setRect(x_center - x_radius, y_center - y_radius, 2 * radius_x, 2 * radius_y);

}

QPolygonF MiEllipse::getPoints(void)
{
    QPolygonF pol_points;
    pol_points.append(QPointF(x_center, y_center));
    pol_points.append(QPointF(x_radius, y_radius));
    pol_points.append(QPointF(rot, 0)); // 0 no se usa
    return pol_points;
}

void MiEllipse::setPoints(const QPolygonF& polygon)
{
    if(polygon.size() == 3)
        setParams(polygon.at(0).x(), polygon.at(0).y(), polygon.at(1).x() ,polygon.at(1).y(), polygon.at(2).x());
    // polygon.at(2).y() no se usa

    // hay que mirar esto cuando se usen elipses en los aria world
}

void MiEllipse::paint(QPainter &painter,
                      const QStyleOptionGraphicsItem * option,
                      QWidget * widget)
{

    painter.save();

    painter.setPen(QPen(QColor(Qt::blue)));
    painter.setBrush(QBrush(QColor(Qt::blue)));

    painter.translate(NDMath::roundInt(x_center), NDMath::roundInt(y_center));
    painter.rotate(rot*NDMath::RAD2GRAD);
    painter.drawEllipse(NDMath::roundInt( -x_radius) + 1, NDMath::roundInt(-y_radius) + 1,
                        NDMath::roundInt(2*x_radius)-2, NDMath::roundInt(2*y_radius)-2);

    painter.restore();

}
