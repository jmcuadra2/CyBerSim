/***************************************************************************
              qcanvasarc.h  -  description
                 -------------------
    begin		 : July 24 2007
    copyright		 : (C) 2007 by Mº Dolores Gómez Tamayo
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

#ifndef QCANVASARROW_H
#define QCANVASARROW_H

#include <QGraphicsScene>
#include <QGraphicsPolygonItem>
#include <QPainter>
#include <math.h>

class QCanvasArrow : public QGraphicsPolygonItem
{
public:
    QCanvasArrow( double size, double ang, QGraphicsScene *c );

    ~QCanvasArrow();

protected:
    void drawShape( QPainter &p );

private:
    double x0, y0, x2, y2, x3, y3;
    double size;
    double ang;
    bool calculated;
};

#endif
