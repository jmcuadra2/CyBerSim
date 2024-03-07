/***************************************************************************
                          lamp.cpp  -  description
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
#include "lamp.h"
#include "rtti.h"
#include <QSvgWidget>

Lamp::Lamp(int x, int y, int worldScale,  const QString& nameId, const QString& lamp_svg,
           QGraphicsItem * parent)
    : RadialFieldSource(x, y, worldScale, nameId, lamp_svg, parent)

{

}

Lamp::~Lamp() {}       

int Lamp::type(void) const
{
    return LAMP_RTTI;
}
