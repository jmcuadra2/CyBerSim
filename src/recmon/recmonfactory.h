/***************************************************************************
                          recmonfactory.h  -  description
                             -------------------
    begin                : Sat May 21 2005
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

#ifndef RECMONFACTORY_H
#define RECMONFACTORY_H

#include <QDomDocument>

class BoxMonitor;
class GraphicMonitor;
class FileRecorder;
class XMLOperator;

/**
  *@author Jose M. Cuadra Troncoso
  */

class RecMonFactory {

  protected:
    RecMonFactory();

  public:
  	virtual ~RecMonFactory();

    static RecMonFactory* instance(void);
    virtual BoxMonitor* factoryBoxMonitor(QDomElement& e);
    virtual GraphicMonitor* factoryGraphicMonitor(QDomElement& e,
                                  BoxMonitor* box_mon = 0);
    virtual FileRecorder* factoryRecorder(QDomElement& e);
    
  protected:
    static RecMonFactory* factory;
    XMLOperator* xml_operator;
};
#endif
