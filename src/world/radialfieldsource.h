/***************************************************************************
                          radialfieldsource.h  -  description
                             -------------------
    begin                : Mon Feb 7 2005
    copyright            : (C) 2005 by Jose M. Cuadra Troncoso
    email                : jmcuadra@dia.uned.es
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef RADIALFIELDSOURCE_H
#define RADIALFIELDSOURCE_H

#include <QGraphicsScene>
#include <QGraphicsItem>
#include <QGraphicsSvgItem>

/**
  *@author Jose M. Cuadra Troncoso
  */

class RadialFieldSource : public QGraphicsSvgItem
{

    friend class World;
    friend class NDAriaWorld;
    friend class AbstractWorld;

protected:
    RadialFieldSource(int x, int y, int scale, const QString& nameId, const QString& str_radialfieldsource_svg, QGraphicsItem * parent = 0);


public:
    ~RadialFieldSource();

    virtual bool isOn(void);
    virtual void setOn(bool on_);
    double xCenter(void);
    double yCenter(void);
    int type(void) const;
    QString getNameId() const { return nameId; }

protected:
    QString nameId;

private:
    bool on;

};

#endif
