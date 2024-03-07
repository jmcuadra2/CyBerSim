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
#ifndef DICTIONARYMANAGER_H
#define DICTIONARYMANAGER_H

#include <QMap>
#include <QDomDocument>

class XMLOperator;
class Settings;

typedef QMap<QString, int> NameClass_Map;

/**
@author jose manuel
*/

class DictionaryManager{

  protected:
    DictionaryManager();
      
  public:
    virtual ~DictionaryManager();
    
    static DictionaryManager* instance(void);
    virtual bool readFileConf(const QString& sender, const QString& base_name, QDomElement& elem_classes, const int& super_type_id = -1);
    bool fileConfOk(void);    
    /*typedef QMap<QString, int> NameClass_Map;*/ 
    virtual void getNamesMap(const QDomElement& elem_classes, NameClass_Map&  names_list, bool super_types = false);  
    int chooseClass(NameClass_Map names_map, const QString& comment);  

  protected:
    static DictionaryManager* dict_manager;
    XMLOperator* xml_operator;
    Settings* prog_settings;
    bool file_conf_ok;      
};

#endif
