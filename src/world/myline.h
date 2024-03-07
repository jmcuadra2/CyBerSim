//
// C++ Interface: myline
//
// Description: 
//
//
// Author: Jose Manuel Cuadra Troncoso <jmcuadra@dia.uned.es>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef MYLINE_H
#define MYLINE_H

#include <QGraphicsScene>
#include "solidobject.h"
#include <QGraphicsItem>

/**
  @author Jose Manuel Cuadra Troncoso <jmcuadra@dia.uned.es>
*/

class MyLine : public QGraphicsLineItem, public SolidObject
{
    friend class AbstractWorld;

protected:
    MyLine();

public:
    int type(void) const;
    ~MyLine();
    QString getTypeName(void) const { return "solid_line" ; }
    QPolygonF getPoints(void);
    void setPoints(const QPolygonF& polygon);
    int getRtti(void) const { return type() ; }

};

#endif
