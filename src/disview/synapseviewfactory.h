/***************************************************************************
                          synapseviewfactory.h  -  description
                             -------------------
    begin                : Mon Jul 18 2005
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

#ifndef SYNAPSEVIEWFACTORY_H
#define SYNAPSEVIEWFACTORY_H

#include <QPixmap>
#include <QMap>
#include <QDomDocument>

#include "../neuraldis/dictionarymanager.h"

class Synapse;
class XMLOperator;

/**
  *@author Jose M. Cuadra Troncoso
  */

class SynapseViewFactory {

  protected:
  	SynapseViewFactory();
   
  public:
  	virtual ~SynapseViewFactory();

    static SynapseViewFactory* instance(void);
    virtual bool readFileConf(void);
//     bool fileConfOk(void);
    virtual NameClass_Map getNamesMap(void);

    
  protected:  
    static SynapseViewFactory* factory;
    DictionaryManager* dict_manager;
    NameClass_Map names_list;
    XMLOperator* xml_operator;
    QDomElement synapse_classes;         
};

#endif
