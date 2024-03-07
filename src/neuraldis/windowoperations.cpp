/***************************************************************************
                          windowoperations.cpp  -  description
                             -------------------
    begin                : Mon May 2 2005
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

#include "windowoperations.h"

#include <QWidget>

WindowOperations* WindowOperations::w_oper = 0;

WindowOperations::WindowOperations(){
}
WindowOperations::~WindowOperations(){
}

WindowOperations* WindowOperations::instance(void)
{

  if(w_oper == 0)
    w_oper = new WindowOperations();
  return w_oper;

}

void WindowOperations::setPosSize(QWidget* win, const QDomElement& e)
{

  if(e.tagName().contains("geometry")) {
    bool ok;
    int x  = e.attribute("x").toInt(&ok);
    int y  = e.attribute("y").toInt(&ok);
    int width  = e.attribute("width").toInt(&ok);
    int height  = e.attribute("height").toInt(&ok);
    QSize s(width, height);
  	QPoint p(x, y);
    win->move(p);
    win->resize(s);
  }

}
    
void WindowOperations::writePosSize(QWidget* win, QDomElement& e)
{
  
  if(win) {
    QWidget* w_parent = win->parentWidget();
    QPoint b_pos;
    if(w_parent)
      b_pos = w_parent->pos();
    else
      b_pos = win->pos();
    QSize b_size = win->size();
    e.setAttribute("x", QString::number(b_pos.x()));
    e.setAttribute("y", QString::number(b_pos.y()));
    e.setAttribute("width", QString::number(b_size.width()));
    e.setAttribute("height", QString::number(b_size.height()));;
  }
  else {
    e.setAttribute("x", QString::number(0));
    e.setAttribute("y", QString::number(0));
    e.setAttribute("width", QString::number(500));
    e.setAttribute("height", QString::number(500));
  }

}
 
