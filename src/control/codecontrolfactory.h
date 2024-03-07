/***************************************************************************
                          codecontrolfactory.h  -  description
                             -------------------
    begin                : Sat Apr 30 2005
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

#ifndef CODECONTROLFACTORY_H
#define CODECONTROLFACTORY_H

#include "basesimcontrolfactory.h"

class NullSimControl;
class BaseSimControl;

/**
  *@author Jose M. Cuadra Troncoso
  */

class CodeControlFactory : public BaseSimControlFactory  {

  public:
  	CodeControlFactory();
  	~CodeControlFactory();

    BaseSimControl* construct(int control_type, const QDomElement& e, const QString& control_filename);
    BaseSimControl* construct(void);
//    virtual void construct(NullSimControl* ctrl, const QDomElement& e);
    
  protected:
    NullSimControl* null_control;
    
};

#endif
