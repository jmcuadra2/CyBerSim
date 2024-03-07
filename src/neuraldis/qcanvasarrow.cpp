/***************************************************************************
              qcanvasarc.h  -  description
                 -------------------
    begin		 : July 24 2007
    copyright		 : (C) 2007 by María Dolores Gómez Tamayo
    email		 : mgomez4@alumnos.uned.es
 ***************************************************************************/

/***************************************************************************
 *									   *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or	   *
 *   (at your option) any later version.				   *
 *									   *
 ***************************************************************************/

#include "qcanvasarrow.h"

QCanvasArrow::QCanvasArrow(double size, double ang, QGraphicsScene *c) :
    QGraphicsPolygonItem( /*c*/ )
{
    this->size = size;
    this->ang = ang;
}


QCanvasArrow::~QCanvasArrow()
{
    hide(); // Required to avoid segfault - see docs
}

void QCanvasArrow::drawShape( QPainter &p )
{
    double rad = M_PI / 180.0;

    x0 = x();
    y0 = y();

    double ang2 = ang-150.0;
    x2= size * cos(ang2*rad);
    y2= size * sin(ang2*rad);

    double ang3 = ang+150.0;
    x3= size * cos(ang3*rad);
    y3= size * sin(ang3*rad);

    p.drawLine ((int)x(), (int)y(), (int)x()+(int)x2, (int)y()+(int)y2);
    p.drawLine ((int)x(), (int)y(), (int)x()+(int)x3, (int)y()+(int)y3);
    p.drawLine ((int)x()+(int)x2, (int)y()+(int)y2, (int)x()+(int)x3, (int)y()+(int)y3);

}

