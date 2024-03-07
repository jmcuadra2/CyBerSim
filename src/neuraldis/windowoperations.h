/***************************************************************************
                          windowoperations.h  -  description
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

#ifndef WINDOWOPERATIONS_H
#define WINDOWOPERATIONS_H

#include <QDomDocument>

class QWidget;

/**
  *@author Jose M. Cuadra Troncoso
  */    

class WindowOperations {

  protected:
    WindowOperations();
  public:
    static WindowOperations* instance(void);	
  	virtual ~WindowOperations();
    virtual void setPosSize(QWidget* win, const QDomElement& e);
    virtual void writePosSize(QWidget* win, QDomElement& e);

  protected:
    static WindowOperations* w_oper;
    
};

#endif
