/***************************************************************************
                          neuronviewfactory.cpp  -  description
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

#include "neuronviewfactory.h"
#include "../net/neuron.h"
#include "../net/neuralnetwork.h"
#include "../neuraldis/xmloperator.h"
#include "../neuraldis/neuraldisdoc.h"
#include "../neuraldis/settings.h"

#include <QPixmap>

NeuronViewFactory* NeuronViewFactory::factory = 0;

NeuronViewFactory::NeuronViewFactory()
{

  xml_operator = XMLOperator::instance();
  dict_manager = DictionaryManager::instance();
  prog_settings = Settings::instance();  
      
}

NeuronViewFactory::~NeuronViewFactory(){
}

NeuronViewFactory* NeuronViewFactory::instance(void)
{

  if(!factory)
    factory = new NeuronViewFactory();
  return factory;

}

bool NeuronViewFactory::readFileConf(void)
{

  bool ret = dict_manager->readFileConf("NeuronViewFactory", "Neuron", neuron_classes);

  if(ret)
    dict_manager->getNamesMap(neuron_classes, names_list);
  return ret;

}

void NeuronViewFactory::assignNeuronPixmap(Neuron* neuron)
{

  QDomElement e_pix_file;
  QDomElement e_pix_sel_file;
  QString pix_file;
  QString pix_sel_file;
  int neuron_type = neuron->rtti_dev();
  
  QDomElement e_class = xml_operator->findParentTag("type",
                                            neuron_classes, neuron_type);
  if(e_class.isNull()) return;
  if(neuron_type == Neuron::THRESHOLD_DEVICE &&
            neuron->getLayer() == NeuralNetwork::LAYER_OUTPUT) {
    e_pix_file = xml_operator->findTag("pixmap_file2", e_class);
    e_pix_sel_file = xml_operator->findTag("pixmap_selected_file2", e_class);
  } else {
    e_pix_file = xml_operator->findTag("pixmap_file", e_class);
    e_pix_sel_file = xml_operator->findTag("pixmap_selected_file", e_class);
  }
  if(e_pix_file.isNull() || e_pix_sel_file.isNull()) return;
  QString app_dir = prog_settings->getAppDirectory() + "/";
  pix_file = app_dir + e_pix_file.text();
  pix_sel_file = app_dir + e_pix_sel_file.text();
  if(pix_list.find(pix_file) == pix_list.end()) {
    pix_list[pix_file] = QPixmap(pix_file);
    pix_list[pix_sel_file] = QPixmap(pix_sel_file);
  }
  neuron->setNeuronPix(&pix_list[pix_file]);
  neuron->setNeuronSelPix(&pix_list[pix_sel_file]);

}

NameClass_Map NeuronViewFactory::getNamesMap(void)
{
  return names_list;
}

