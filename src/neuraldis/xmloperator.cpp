/***************************************************************************
                          xmloperator.cpp  -  description
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

#include <iostream>

#include "xmloperator.h"
#include "../neuraldis/neuraldisdoc.h"

XMLOperator* XMLOperator::xml_operator = 0;

XMLOperator* XMLOperator::instance(void)
{

  if(xml_operator == 0)
    xml_operator = new XMLOperator;
  return xml_operator;

}

XMLOperator::XMLOperator(){
}
XMLOperator::~XMLOperator(){
}

QDomElement XMLOperator::findTag(const QString& tag_name,
                           const QDomElement& elem, bool direct_child)
{

  QDomElement e_tag, ret_tag;
  QDomNode n;
  if(!direct_child) {
    QDomNodeList n_tag = elem.elementsByTagName(tag_name);
    n = n_tag.item(0);
    if(!n.isNull())
      ret_tag = n.toElement();
  }
  else {
    n = elem.firstChild();
    while(!n.isNull()) {
      e_tag = n.toElement();
      QString s_e = e_tag.tagName();
      if(e_tag.tagName() == tag_name) {
        ret_tag = e_tag;
        break;
      }
      n = n.nextSibling();
    }
  }
  return ret_tag;
}

QDomElement XMLOperator::findTagIdent(const QString& tag_name,
                     const QDomElement& elem, const QString& tag_id,
                              bool exact)
{

  QDomNodeList n_tag = elem.elementsByTagName(tag_name);
  QDomElement e_tag;
  QDomElement ret_tag;
  int cnt_tags = n_tag.count();
  for(int i = 0; i < cnt_tags; ++i) {
    QDomNode n = n_tag.item(i);
    e_tag = n.toElement();
    if(exact) {
      if(e_tag.text() == tag_id) {
         ret_tag = e_tag;
         break;
      }
    }
    else {
      if(e_tag.text().compare(tag_id) > 0) {
        ret_tag = e_tag;
        break;
      }
    }
  }
  return ret_tag;
}

QDomElement XMLOperator::findTagIdent(const QString& tag_name,
                     const QDomElement& elem, const int& tag_id,
                     bool exact)
{
  bool ok;
  QDomNodeList n_tag = elem.elementsByTagName(tag_name);
  QDomElement e_tag;
  QString e_t = elem.tagName();
  QDomElement ret_tag;
  int cnt_tags = n_tag.count();
  for(int i = 0; i < cnt_tags; ++i) {
    QDomNode n = n_tag.item(i);
    e_tag = n.toElement();
    if(exact) {
      if(e_tag.text().toInt(&ok) == tag_id) {
         ret_tag = e_tag;
         break;
      }
    }
    else {
      if(e_tag.text().toInt(&ok) > tag_id) {
        ret_tag = e_tag;
        break;
      }
    }
  }
  return ret_tag;
}

QDomElement XMLOperator::findParentTag(const QString& tag_name,
                 const QDomElement& elem, const int& tag_id, bool exact)
{

  QDomElement e_id;
  QDomElement e_parent;
  e_id = findTagIdent(tag_name, elem, tag_id, exact);
  if(!e_id.isNull())
    e_parent = e_id.parentNode().toElement();
  return e_parent;

}

QDomElement XMLOperator::findParentTag(const QString& tag_name,
                 const QDomElement& elem, const QString& tag_id, bool exact)
{

  QDomElement e_id;
  QDomElement e_parent;
  e_id = findTagIdent(tag_name, elem, tag_id, exact);
  if(!e_id.isNull())
    e_parent = e_id.parentNode().toElement();
  return e_parent;

}


QDomElement XMLOperator::findElementById (const QString& tag_name,
              const QDomElement& elem, const QString& id, const QString& id_name)
{

  QDomNodeList n_tag = elem.elementsByTagName(tag_name);
  QDomElement e_tag;
  QDomElement ret_tag;
  int cnt_tags = n_tag.count();
  for(int i = 0; i < cnt_tags; ++i) {
    QDomNode n = n_tag.item(i);
    e_tag = n.toElement();
    
    if(e_tag.attribute(id_name) == id) {
      ret_tag = e_tag;
      break;
    }
  }
  return ret_tag;
}


void XMLOperator::removeChildren(QDomNode& n_parent, int first,
                                    int last)
{

  QDomNodeList nodes = n_parent.childNodes();
  last = last < 0 ? nodes.count() : last;
  for(int i = first; i < last; i++)
    n_parent.removeChild(nodes.item(first));

}

void XMLOperator::deleteElementsByTagName(const QString& tagname,
                              QDomElement& e)
{

  QDomNodeList l_elems = e.elementsByTagName(tagname);
  while(l_elems.count()) {
    QDomNode n_parent = l_elems.item(0).parentNode();
    n_parent.removeChild(l_elems.item(0));
  }

}

void XMLOperator::deleteParentsByTagIdent(const QString& tagname,
                              QDomElement& e, const QString& ident)
{

  QDomNodeList l_oper_mons = e.elementsByTagName(tagname);
  for(int i = 0; i < int(l_oper_mons.count()); i++) {
    if(l_oper_mons.item(i).toElement().text() == ident) {
      QDomNode n_parent = l_oper_mons.item(i).parentNode();
      QDomNode g_parent = n_parent.parentNode();
      g_parent.removeChild(n_parent);
      --i;
    }
  }

}

void XMLOperator::deleteParentsByTagIdent(const QString& tagname,
                              QDomElement& e, const int& ident)
{

  QDomNodeList l_oper_mons = e.elementsByTagName(tagname);
  for(int i = 0; i < int(l_oper_mons.count()); i++) {
    if(l_oper_mons.item(i).toElement().text().toInt() == ident) {
      QDomNode n_parent = l_oper_mons.item(i).parentNode();
      QDomNode g_parent = n_parent.parentNode();
      g_parent.removeChild(n_parent);
      --i;
    }
  }

}

void XMLOperator::insertChildByTagName(const QString& tagname,
                               QDomElement& e_parent, QDomElement& e,
                               bool tagname_no_child)
{

  bool ok;
  QDomElement e_post = findTag(tagname, e_parent,
                                    tagname_no_child);
  QString str_ident;
  if(!tagname_no_child)
    str_ident = findTag(tagname, e).text();
  else
    str_ident = e.text();
  int num_ident = str_ident.toInt(&ok);
  e_parent.appendChild(e);

  if(!e_post.isNull()) {
    if(ok) {
      if(!tagname_no_child)
        e_post = findParentTag(tagname,
                            e_parent, num_ident, false);
      else
        e_post = findTagIdent(tagname,
                            e_parent, num_ident, false);
    }
    else {
      if(!tagname_no_child)
        e_post = findParentTag(tagname,
                            e_parent, str_ident, false);
      else
        e_post = findTagIdent(tagname,
                            e_parent, str_ident, false);
    }
    if(!e_post.isNull())
      e_parent.insertBefore(e, e_post);
  }

}

QDomElement XMLOperator::createTextElement(QDomDocument& doc,
                      const QString& tag_name, const QString& tag_text)
{
  QDomElement tag = doc.createElement(tag_name);  
  QDomText text = doc.createTextNode(tag_text);
  tag.appendChild(text);
  return tag;  
}

bool XMLOperator::insertDocContent(QDomElement& e_parent,
                    QString& file_name, const QString& doctype_name,
                    const QString& sender)
{
  bool ret = false;
  NeuralDisDoc* doc_manager = NeuralDisDoc::instance();
  QDomDocument doc = doc_manager->loadXML(file_name, doctype_name, sender);
  if(doc.isNull())    
    return ret;
  QDomNode n = doc.firstChild().firstChild();
  if(!n.isNull())
    ret = true;
  while(!n.isNull()) {
    e_parent.appendChild(n.cloneNode());
    n = n.nextSibling();
  }

  return ret;
}

void XMLOperator::changeTextInElement(QDomElement& elem, const QString& new_text)
{
  
  QDomNode e_parent = elem.parentNode();
  QDomDocument doc = elem.ownerDocument();
  QString tag_name = elem.tagName();
  e_parent.replaceChild(xml_operator->createTextElement(doc, tag_name, new_text), elem);
  
}
