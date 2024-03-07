
/***************************************************************************
                          lamp.h  -  description
                             -------------------
    begin                : s� ene 10 2004
    copyright            : (C) 2004 by Jose M. Cuadra Troncoso
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

#ifndef LAMP_H
#define LAMP_H

#include "radialfieldsource.h"
#include <QGraphicsScene>
#include <QtSvg>

/**
  *@author Jose M. Cuadra Troncoso
  */

class Lamp : public RadialFieldSource
{

    friend class World;
    friend class NDAriaWorld;

protected:
    Lamp(int x, int y, int worldScale, const QString& nameId, const QString& lamp_svg, QGraphicsItem * parent = 0);
public:
    ~Lamp();
    
    int type(void) const;
};

#endif
