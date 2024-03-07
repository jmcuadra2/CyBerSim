 
/***************************************************************************
                          subnetviewfactory.h  -  description
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

#ifndef SUBNETVIEWFACTORY_H
#define SUBNETVIEWFACTORY_H

#include <QPixmap>
#include <QMap>
#include <QDomDocument>

#include "../neuraldis/dictionarymanager.h"

class SubNet;
class XMLOperator;
class Settings;

/**
  *@author M. Dolores Gomez Tamayo
  */

class SubNetViewFactory {

  protected:
    SubNetViewFactory();

  public:
    static SubNetViewFactory* instance(void);
    virtual ~SubNetViewFactory();

    virtual void assignSubNetPixmap(SubNet* subnet);
    virtual bool readFileConf(void);
    virtual NameClass_Map getNamesMap(void);

  public:
    typedef QMap<QString, QPixmap> SubNetViewPix_Map;

  protected:
    static SubNetViewFactory* factory;
    DictionaryManager* dict_manager;
    SubNetViewPix_Map pix_list;
    SubNetViewPix_Map pix_sel_list;
    NameClass_Map names_list;
    XMLOperator* xml_operator;
    QDomElement subnet_classes;
    Settings* prog_settings;

};

#endif
