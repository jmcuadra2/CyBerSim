/***************************************************************************
              qcanvasarc.h  -  description
                 -------------------
    begin		 : July 24 2007
    copyright		 : (C) 2007 by M� Dolores G�mez Tamayo
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

#ifndef QCANVASARC_H
#define QCANVASARC_H

#include <QPainter>
#include <QPainterPath>
#include <QAbstractGraphicsShapeItem>

class QCanvasArc : public QAbstractGraphicsShapeItem
{
public:
    QCanvasArc(qreal x, qreal y, qreal width, qreal height, int a, int alen);
    
    ~QCanvasArc();

    QRectF boundingRect() const { return QRectF(-1,-1, m_width + 1, m_height + 1) ; }
    void paint(QPainter*p, const QStyleOptionGraphicsItem*, QWidget*);

    void setWidth(qreal width);
    qreal getWitdth();

    void setHeight(qreal height);
    qreal getHeight();

private:
    qreal m_width;
    qreal m_height;
    int a;
    int alen;
};

#endif
