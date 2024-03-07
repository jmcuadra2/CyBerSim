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
#ifndef MIRECTANGLE_H
#define MIRECTANGLE_H

#include "solidobject.h"

#include <QGraphicsScene>
#include <QGraphicsItem>

/**
@author jose manuel
*/

class MiRectangle : public QGraphicsRectItem, public SolidObject
{

    friend class AbstractWorld;
    friend class World;

protected:
    MiRectangle(QPolygon pa, uint thickness, QColor pen_color,
                QColor fill_color, float depth, int area_fill);

public:
    ~MiRectangle();
    int type(void) const;
    QString getTypeName(void) const { return "solid_rectangle" ; }
    QPolygonF getPoints(void);
    void setPoints(const QPolygonF& polygon);
    int getRtti(void) const { return type() ; }

protected:
    QRect rectangle;
};

#endif
