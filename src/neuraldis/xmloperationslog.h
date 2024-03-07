/***************************************************************************
              xmloperationslog.h  -  description
                 -------------------
    begin		 : July 24 2005
    copyright		 : (C) 2007 by Mº Dolores Gómez Tamayo
    email		 : mgomez4@alumnos.uned.es
 ***************************************************************************/

/***************************************************************************
 *									   *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or	   *
 *   (at your option) any later version.				   *
 *									   *
 ***************************************************************************/

#ifndef XMLOPERATIONSLOG_H
#define XMLOPERATIONSLOG_H



/**
  *@author María Dolores Gómez Tamayo
  */

#include <iostream>
#include <fstream>
#include <QFile>

#include <QDomDocument>      
#include <QTextStream>       
#include <QStack>

#include "../net/neuron.h"
#include "../net/synapse.h"      
#include "../net/subnet.h"
#include "../neuraldis/xmloperator.h"

using namespace std;

class XMLOperationsLog{

protected:
    XMLOperationsLog();
    void defaultValues(void);
    
public:
    virtual ~XMLOperationsLog();

    static XMLOperationsLog* instance(void);
    
    // operaciones
    virtual QDomElement logOperation(const QString& operation);
    virtual void clear(void);
    // las operaciones recursivas forman parte de un grupo
    virtual int openGroup ();
    virtual int getGroup();
    virtual int getGroupOrder();
    virtual int closeGroup();
    virtual bool activeGroup();
    
    virtual void openUndo();
    virtual void closeUndo();
    virtual void insertLogOperation(Synapse* synapse, const QString& operacion);
    virtual void insertLogOperation (Neuron* neuron, const QString& operation);
    // nuevo
    virtual void insertLogOperation (SubNet* subnet, const QString& operation);
    virtual void insertComment(const QString& comment);
    virtual void removeLastEditOperation(void);

    // metodos para undo / redo
    virtual bool undo();
    virtual bool next_undo();
    virtual bool redo();
    virtual bool next_redo();
    
    virtual QDomElement getCurrentElement();
    virtual QString getCurrentOperationName();

    // Solo testing para ver estructura del historico
    // virtual bool readLogFile(const QString& log_file);
    virtual bool saveLogFile(const  QString& log_file);
    
protected:
    static XMLOperationsLog* xml_log;

private:
    QDomDocument log;
    QDomElement root;

    int id;
    int group;
    bool active_group;
    int group_order;

    int undo_group;

    bool undoMode;

    QStack<int> undoStack;
    QStack<int> redoStack;

    QString current_op_name, prev_op_name;
    QDomElement current_elem, prev_elem;
    QDomElement current_op;

};

#endif
