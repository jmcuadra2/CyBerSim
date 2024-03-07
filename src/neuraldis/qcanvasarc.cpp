/***************************************************************************
        qcanvasarc.h  -  description
           -------------------
    begin    : July 24 2007
    copyright    : (C) 2007 by María Dolores Gómez Tamayo
    email    : mgomez4@alumnos.uned.es
 ***************************************************************************/

/***************************************************************************
 *                     *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.           *
 *                     *
 ***************************************************************************/

#include "qcanvasarc.h"

QCanvasArc::QCanvasArc(qreal x, qreal y, qreal width, qreal height, int a, int alen) :
    QAbstractGraphicsShapeItem()
{
    setPos(x,y);
    m_width = width;
    m_height = height;
    this->a = a;
    this->alen = alen;
}

QCanvasArc::~QCanvasArc()
{
    hide(); // Required to avoid segfault - see docs
}

void QCanvasArc::setWidth(qreal width)
{
    m_width = width;
}

qreal QCanvasArc::getWitdth()
{
    return m_width;
}

void QCanvasArc::setHeight(qreal height)
{
    m_height = height;
}

qreal QCanvasArc::getHeight()
{
    return m_height;
}

void QCanvasArc::paint(QPainter* p, const QStyleOptionGraphicsItem *, QWidget *)
{
    p->setPen(pen());
    p->drawArc(0, 0, m_width, m_height, a, alen);
}
