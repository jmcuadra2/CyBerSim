/***************************************************************************
                          xmloperator.h  -  description
                             -------------------
    begin                : Sat Jun 4 2005
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

#ifndef XMLOPERATOR_H
#define XMLOPERATOR_H


/**
  *@author Jose M. Cuadra Troncoso
  */

#include <QDomDocument>  

class XMLOperator {

  protected:
    XMLOperator();
    
  public:    
    virtual ~XMLOperator();

    static XMLOperator* instance(void);
    virtual QDomElement findTag(const QString& tag_name,
                                const QDomElement& elem,
                                bool direct_child = true);
    virtual QDomElement findTagIdent(const QString& tag_name,
                        const QDomElement& elem, const QString& tag_id,
                              bool exact = true);
    virtual QDomElement findTagIdent(const QString& tag_name,
                              const QDomElement& elem, const int& tag_id,
                              bool exact = true);
    virtual QDomElement findParentTag(const QString& tag_name,
                              const QDomElement& elem, const int& tag_id,
                               bool exact = true);
    virtual QDomElement findParentTag(const QString& tag_name,
                              const QDomElement& elem,
                              const QString& tag_id, bool exact = true);

    virtual QDomElement findElementById (const QString& tag_name,
			      const QDomElement& elem, const QString& id, const QString& id_name = "id");

    virtual void removeChildren(QDomNode& n_parent, int first = 0,
                                    int last = -1);
    virtual void deleteElementsByTagName(const QString& tagname,
                              QDomElement& e);
    virtual void deleteParentsByTagIdent(const QString& tagname,
                              QDomElement& e, const QString& ident);
    virtual void deleteParentsByTagIdent(const QString& tagname,
                              QDomElement& e, const int& ident);
    virtual void insertChildByTagName(const QString& tagname,
                               QDomElement& e_parent, QDomElement& e,
                               bool tagname_no_child = true);
    virtual QDomElement createTextElement(QDomDocument& doc,
                    const QString& tag_name, const QString& tag_text);
    virtual bool insertDocContent(QDomElement& e_parent,
                    QString& file_name, const QString& doc_name,
                    const QString& sender);                    
    virtual void changeTextInElement(QDomElement& elem, const QString& new_text);

  protected:
    static XMLOperator* xml_operator;                    
};

#endif
