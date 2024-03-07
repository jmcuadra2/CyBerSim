/***************************************************************************
                          synapseviewfactory.cpp  -  description
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

#include "synapseviewfactory.h"
 #include "../net/neuralnetwork.h"
 #include "../neuraldis/xmloperator.h"
 #include "../neuraldis/neuraldisdoc.h"

SynapseViewFactory* SynapseViewFactory::factory = 0;

SynapseViewFactory::SynapseViewFactory()
{

  xml_operator = XMLOperator::instance();
  dict_manager = DictionaryManager::instance();

}

SynapseViewFactory::~SynapseViewFactory(){
}

SynapseViewFactory* SynapseViewFactory::instance(void)
{

  if(!factory)
    factory = new SynapseViewFactory();
  return factory;

} 

bool SynapseViewFactory::readFileConf(void)
{

  bool ret = dict_manager->readFileConf("SynapseViewFactory", "NeuralConnection", synapse_classes, NeuralConnection::SYNAPSE);
  if(ret)
    dict_manager->getNamesMap(synapse_classes, names_list);

//   NeuralDisDoc* doc = NeuralDisDoc::instance();
//   QDomDocument classes_doc = doc->loadXML("src/neuraldis/classesdict.conf", "",
//                               "SynapseViewFactory");
//   if(!classes_doc.isNull()) {
//     QDomElement e_root = classes_doc.firstChild().toElement();
//     if(!e_root.isNull()) {
//       QDomElement e_base = xml_operator->findParentTag("base_name",
//                                             e_root, "NeuralConnection");
//       if(!e_base.isNull()) {
//         QDomElement e_super = xml_operator->
//                   findParentTag("super_type_id", e_base,
//                       NeuralConnection::SYNAPSE);
//         if(!e_super.isNull()) {
//           synapse_classes = xml_operator->findTag("Subclasses", e_super);
//           if(!synapse_classes.isNull()) { 
//             if(synapse_classes.hasChildNodes())
//               file_conf_ok = true;
//           }
//         }
//       }
//     }
//   }
  return ret;

}

//QMap<QString, int> SynapseViewFactory::getNamesMap(void)
// typedef QMap<QString, int> SynapseClass_Map;
NameClass_Map SynapseViewFactory::getNamesMap(void)
{

//   if(names_list.count()) return names_list; // ya estaba creado
//   
//   bool ok = false;
//   QDomNode n = synapse_classes.firstChild();
//   while(!n.isNull()) {
//     QDomElement e_class = n.toElement();
//     if(e_class.isNull()) {
//       n = n.nextSibling();
//       continue;
//     }
//     QDomElement e_type = xml_operator->findTag("type", e_class);
//     QDomElement e_text_name = xml_operator->findTag("text_name", e_class);
//     QString s_name = e_text_name.attribute("capital");
//     int type = e_type.text().toInt(&ok);
//     if(!s_name.isEmpty() && ok)
//       names_list[s_name] = type;
//     n = n.nextSibling();
//   }
  return names_list;
} 
