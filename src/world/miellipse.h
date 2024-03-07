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
#ifndef MIELLIPSE_H
#define MIELLIPSE_H

#include <QGraphicsScene>
#include "solidobject.h"
#include <QGraphicsItem>

/**
@author jose manuel
*/

class MiEllipse : public QGraphicsEllipseItem, public SolidObject
{

  friend class AbstractWorld;
  friend class World;
  
  protected:
    MiEllipse(QPolygonF pa, uint thickness, QColor pen_color, QColor fill_color, float depth, int area_fill);
    ~MiEllipse(void);
    void setParams(double center_x, double center_y, double radius_x, double radius_y, double angle);
        
  public:             
    int type(void) const;
    bool contains(const double& x_p, const double& y_p) const;
    QString getTypeName(void) const { return "solid_ellipse" ; }
    QPolygonF getPoints(void);
    void setPoints(const QPolygonF& polygon);
    int getRtti(void) const { return type() ; }
    void paint(QPainter &painter,
               const QStyleOptionGraphicsItem * option,
               QWidget * widget);    

  private:
    double a2, b2, a2b, rot; // robot_rad;
    double x_center, y_center, x_radius, y_radius;
};

inline bool MiEllipse::contains(const double& x_p, const double& y_p) const
{
  double x_c = x_p - x_center;
  double y_c = y_p - y_center;
  return x_c*x_c*a2 + y_c*y_c*b2 - x_c*y_c*a2b <= 1 ? true : false;

};

#endif
