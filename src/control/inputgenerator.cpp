/***************************************************************************
                          inputgenerator.cpp  -  description
                             -------------------
    begin                : Fri Apr 29 2005
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

#include "inputgenerator.h"

InputGenerator::InputGenerator(int id, QObject *parent, const char *name)
                  : QObject(parent)
{

  ident = id;
  out = 0.0;
  prev_out = 0.0;
  
}
InputGenerator::~InputGenerator()
{
}

void InputGenerator::write(QDomDocument& , QDomElement& e)
{

  e.setAttribute("identification", QString::number(ident));

}  

