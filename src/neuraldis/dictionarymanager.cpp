/***************************************************************************
 *   Copyright (C) 2004 by jose manuel                                     *
 *   jose@pcjose                                                           *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#include <QCursor>
#include <QLabel>

#include "dictionarymanager.h"
#include "../neuraldis/xmloperator.h"
#include "../neuraldis/settings.h"
#include "../neuraldis/neuraldisdoc.h"
#include "gui/dictclasschooser.h"

DictionaryManager* DictionaryManager::dict_manager = 0;

DictionaryManager::DictionaryManager()
{

  file_conf_ok = false;
  xml_operator = XMLOperator::instance();
  prog_settings = Settings::instance();
  
}

DictionaryManager::~DictionaryManager()
{
}

DictionaryManager* DictionaryManager::instance(void)
{

  if(!dict_manager) 
    dict_manager = new DictionaryManager();
  return dict_manager;     
  
}

bool DictionaryManager::readFileConf(const QString& sender, const QString& base_name, QDomElement& elem_classes, const int& super_type_id)
{

  NeuralDisDoc* doc = NeuralDisDoc::instance();
  QString file_conf_name = prog_settings->fillPath("src/neuraldis/classesdict.conf", Settings::instance()->getAppDirectory());
  //QString file_conf_name = Settings::instance()->getAppDirectory() + "/src/neuraldis/classesdict.conf";
  QDomDocument classes_doc = doc->loadXML(file_conf_name, "", sender);
  if(!classes_doc.isNull()) {
    QDomElement e_root = classes_doc.firstChild().toElement();
    if(!e_root.isNull()) {
      QDomElement e_base = xml_operator->findParentTag("base_name", e_root, base_name);
      if(!e_base.isNull()) {
        if(super_type_id >= 0) {
          QDomElement e_super = xml_operator->
                    findParentTag("super_type_id", e_base, super_type_id);
          if(!e_super.isNull()) {
            elem_classes = xml_operator->findTag("Subclasses", e_super);
            if(!elem_classes.isNull()) { 
              if(elem_classes.hasChildNodes())
                file_conf_ok = true;
            }
          }
        }
        else {
          QDomElement e_classes = xml_operator->findTag("Super_type_definition", e_base);
          if(e_classes.isNull())
            elem_classes = xml_operator->findTag("Subclasses", e_base);
          else
            elem_classes = e_base;
          if(!elem_classes.isNull()) {
            if(elem_classes.hasChildNodes())
              file_conf_ok = true;
          }        
        }
      }
    }
  }
  return file_conf_ok;

}

bool DictionaryManager::fileConfOk(void)
{

  return file_conf_ok;

}

// typedef QMap<QString, int> Class_Map;
void DictionaryManager::getNamesMap(const QDomElement& elem_classes, NameClass_Map& names_list, bool super_types)
{

//   if(!file_conf_ok) return names_list;
//   if(names_list.count()) return names_list; // ya estaba creado

  bool ok = false;
  QString s_name;
  int type;  
//   NameClass_Map names_list;
  QDomNode n = elem_classes.firstChild();
  while(!n.isNull()) {
    QDomElement e_class = n.toElement();
    if(e_class.isNull()) {
      n = n.nextSibling();
      continue;
    }
    if(!super_types) {
      QDomElement e_type = xml_operator->findTag("type", e_class);
      QDomElement e_text_name = xml_operator->findTag("text_name", e_class);
      s_name = e_text_name.attribute("capital");
      type = e_type.text().toInt(&ok);    
    }
    else {
      QDomElement e_type = xml_operator->findTag("super_type_id", e_class);
      QDomElement e_text_name = xml_operator->findTag("super_type_name", e_class);
      s_name = e_text_name.text();
      type = e_type.text().toInt(&ok);
    }
    if(!s_name.isEmpty() && ok)
      names_list[s_name] = type;
    n = n.nextSibling();
  }

}

int DictionaryManager::chooseClass(NameClass_Map names_map, const QString& comment)
{

  dictClassChooser* class_chooser = new dictClassChooser();
  class_chooser->setClassMap(names_map);
  class_chooser->setComment(comment); 
  class_chooser->adjustSize();
  int ret = -1;
  if(class_chooser->exec() == QDialog::Accepted)
    ret = class_chooser->getClassId();
  delete class_chooser;
  return ret;
  
}

