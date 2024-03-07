/***************************************************************************
                          neuronviewfactory.h  -  description
                             -------------------
    begin                : Sun Jul 10 2005
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

#ifndef NEURONVIEWFACTORY_H
#define NEURONVIEWFACTORY_H

#include <QPixmap>
#include <QMap>
#include <QDomDocument>

#include "../neuraldis/dictionarymanager.h"

class Neuron;
class XMLOperator;
class Settings;

/**
  *@author Jose M. Cuadra Troncoso
  */

class NeuronViewFactory {
  
  protected:
    NeuronViewFactory();

  public:
    static NeuronViewFactory* instance(void);  
  	virtual ~NeuronViewFactory();

    virtual void assignNeuronPixmap(Neuron* neuron);
    virtual bool readFileConf(void);
//     bool fileConfOk(void);
//     typedef QMap<QString, int> NeuronClass_Map;
    virtual NameClass_Map getNamesMap(void);    

  public:
    typedef QMap<QString, QPixmap> NeuronPix_Map;

  protected:  
    static NeuronViewFactory* factory;
    DictionaryManager* dict_manager;
    NeuronPix_Map pix_list;
    NeuronPix_Map pix_sel_list;
    NameClass_Map names_list;    
    XMLOperator* xml_operator;
    QDomElement neuron_classes;
    Settings* prog_settings;    
//     bool file_conf_ok;
      
};

#endif
