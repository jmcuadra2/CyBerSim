/***************************************************************************
                          subnetviewfactory.cpp  -  description
                             -------------------
    begin                : Sun Jul 10 2007
    copyright            : (C) 2007 by M. Dolores Gomez Tamayo
    email                : mgomez4@alumno.uned.es
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
#include <QMessageBox>
#include <QPixmap>

#include "subnetviewfactory.h"
#include "../net/subnet.h"
#include "../net/neuralnetwork.h"
#include "../neuraldis/xmloperator.h"
#include "../neuraldis/neuraldisdoc.h"
#include "../neuraldis/settings.h"

SubNetViewFactory* SubNetViewFactory::factory = 0;

SubNetViewFactory::SubNetViewFactory()
{

  xml_operator = XMLOperator::instance();
  dict_manager = DictionaryManager::instance();
  prog_settings = Settings::instance();  
      
}

SubNetViewFactory::~SubNetViewFactory(){
}

SubNetViewFactory* SubNetViewFactory::instance(void)
{

  if(!factory)
    factory = new SubNetViewFactory();
  return factory;

}

bool SubNetViewFactory::readFileConf(void)
{

bool ret = dict_manager->readFileConf("SubNetViewFactory", "SubNet", subnet_classes);
  if(ret)
    dict_manager->getNamesMap(subnet_classes, names_list);
  return ret;

}

void SubNetViewFactory::assignSubNetPixmap(SubNet* subnet)
{

  QString pix_file;
  QString pix_sel_file;
/*
  QString app_dir = prog_settings->getAppDirectory() + "/";
  pix_file = app_dir + "src/disview/imagenes/subnetneuron.xpm";
  pix_sel_file = app_dir + "src/disview/imagenes/subnetneuronsel.xpm"; */
  
  QDomElement e_pix_file;
  QDomElement e_pix_sel_file;
  
  int subnet_type = subnet->rtti_dev();
  
  QDomElement e_class = xml_operator->findParentTag("type",
                                            subnet_classes, subnet_type);
  if(e_class.isNull()) return;
  // se trata del tipo de 
  if(subnet_type == 1) {
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

  // se pasan los paths de los pixmaps  
  subnet->setPix(pix_file, pix_sel_file);

}

NameClass_Map SubNetViewFactory::getNamesMap(void)
{
  return names_list;
}

 
