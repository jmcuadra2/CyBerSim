/***************************************************************************
        xmloperationslog.cpp  -  description
           -------------------
    begin    : July 24 2005
    copyright    : (C) 2007 by Mº Dolores Gómez Tamayo
    email    : mgomez4@alumnos.uned.es
 ***************************************************************************/

/***************************************************************************
 *                     *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.           *
 *                     *
 ***************************************************************************/

#include <iostream>
#include <QTextStream>

#include "xmloperationslog.h"
#include "../neuraldis/settings.h"

XMLOperationsLog* XMLOperationsLog::xml_log = 0;

XMLOperationsLog* XMLOperationsLog::instance ( void )
{

    if(xml_log == 0 )
        xml_log = new XMLOperationsLog;
    return xml_log;

}

// constructor
XMLOperationsLog::XMLOperationsLog()
{

    // inicializar DOM
    QDomDocument log ( "log");
    this->log = log;
    QDomElement root = log.createElement ( "log");
    this->root = root;
    log.appendChild ( root );

    defaultValues();

}

// destructor
XMLOperationsLog::~XMLOperationsLog()
{
}

void XMLOperationsLog::defaultValues(void)
{
    // inicializar contador identificadores operaciones
    id = 0;

    // inicializar variables de grupo (para guardar en el historico)
    active_group = false;
    group = 0;
    group_order = 0;
    undo_group = 0;
    // inicializar variable de estado para operaciones undo
    undoMode = false;

    // inicializar otros datos
    current_op_name = "";
    prev_op_name = "";
    current_elem = QDomElement();
    current_op = QDomElement();
    prev_elem = QDomElement();

}

void XMLOperationsLog::clear(void)
{
    defaultValues();
    undoStack.clear();
    redoStack.clear();
    XMLOperator* xml_operator = XMLOperator::instance();
    xml_operator->removeChildren(root, 0);
}

// insertar una nueva operacion en el historico
// el metodo se sobrecarga para cada tipo de objeto
void XMLOperationsLog::insertLogOperation ( Synapse* synapse, const QString& operation)
{

    // un grupo activo indica que las operaciones que tienen lugar
    // las procesara la operacion principal por si misma

    // crear una nueva operacion
    QDomElement tagOp = logOperation ( operation );

    // incluir la informacion del objeto dentro de esa operacion
    synapse->write(log, tagOp);

    // borrar - solo test
    Settings* settings = Settings::instance();
    saveLogFile (settings->getAppDirectory() + "fichero_undo.txt");

}

// instertLogOperation para Neuron
void XMLOperationsLog::insertLogOperation ( Neuron* neuron, const QString& operation )
{

    // un grupo activo indica que las operaciones que tienen lugar
    // las procesara la operacion principal por si misma

    // crear una nueva operacion
    QDomElement tagOp = logOperation ( operation );

    // incluir la informacion del objeto dentro de esa operacion
    neuron->write ( log, tagOp );

    // borrar - solo test
    Settings* settings = Settings::instance();
    saveLogFile (settings->getAppDirectory() + "fichero_undo.txt");

}

void XMLOperationsLog::insertLogOperation ( SubNet* subnet, const QString& operation )
{

    // un grupo activo indica que las operaciones que tienen lugar
    // las procesara la operacion principal por si misma

    // crear una nueva operacion
    QDomElement tagOp = logOperation ( operation );

    // incluir la informacion del objeto dentro de esa operacion
    subnet->write ( log, tagOp );

    // borrar - solo test
    Settings* settings = Settings::instance();
    saveLogFile (settings->getAppDirectory() + "fichero_undo.txt");

}

// inserta comentario en la estructura DOM (solo debug)
void XMLOperationsLog::insertComment ( const QString& comment )
{

    QDomElement tagOp = log.createElement ( "comment");
    root.appendChild ( tagOp );
    QDomText comm = log.createTextNode ( comment );
    tagOp.appendChild ( comm );

    // borrar - solo test
    Settings* settings = Settings::instance();
    saveLogFile (settings->getAppDirectory() + "fichero_undo.txt");

}

// insertar una nueva operacion en el historico
QDomElement XMLOperationsLog::logOperation ( const QString& operation )
{

    QDomElement tagOp = log.createElement ( "operation");

    // este valor debe autoincrementarse con cada operaci�n
    id++;

    // actualizar la pila de undo / redo
    if(undoMode ) {
        // se trata de una operacion realizada a traves de un undo -
        // pasa directamente a la cola de redo
        insertComment ( "REDO_STACK [ "+QString::number ( id ) +" ]");
        redoStack.push ( id );
    }
    else {
        // se trata de una operacion normal -
        // pasa a la cola de undo
        insertComment ( "UNDO_STACK [ "+QString::number ( id ) +" ]");
        undoStack.push ( id );
    }

    QDomAttr a = log.createAttribute ( "id");
    a.setValue ( QString::number ( id ) );
    tagOp.setAttributeNode ( a );

    QDomAttr a2 = log.createAttribute ( "type");
    a2.setValue ( operation );
    tagOp.setAttributeNode ( a2 );

    if(active_group) {
        group_order++;
    }
    else {
        group++;
        group_order=1;
    }

    // hay que guardar informacion del grupo
    // identificador del grupo
    a = log.createAttribute ( "group");
    a.setValue ( QString::number ( group ) );
    tagOp.setAttributeNode ( a );

    // orden dentro del grupo
    a = log.createAttribute ( "group_order");
    a.setValue ( QString::number ( group_order ) );
    tagOp.setAttributeNode ( a );

    // las operaciones se insertan dentro de la marca <log>
    root.appendChild ( tagOp );
    current_op = tagOp;
    current_op_name = operation;
    return tagOp;

}

// crea un nuevo identificador de grupo para
// operaciones recursivas
int XMLOperationsLog::openGroup()
{

    active_group = true;
    group++;
    group_order = 0;
    return group;

}

bool XMLOperationsLog::activeGroup()
{
    return active_group;
}

// devuelve el identificador de grupo actual
int XMLOperationsLog::getGroup()
{    
    return group;
}

int XMLOperationsLog::getGroupOrder()
{    
    return group_order;

}

// cierra el grupo actual
// una vez cerrado, las operaciones
int XMLOperationsLog::closeGroup()
{

    active_group = false;
    group_order = 0;

    return group;

}

void XMLOperationsLog::openUndo()
{
    undoMode = true;
}

void XMLOperationsLog::closeUndo()
{
    undoMode =  false;
}

bool XMLOperationsLog::saveLogFile ( const QString& log_file )
{

    QFile fichero ( log_file );

    if(!fichero.open ( QIODevice::WriteOnly)) {
        cout << "Failed to open file." << endl;
        //    ha habido un error al escribir el fichero
        return false;

    }
    else {
        QTextStream ts ( &fichero );
        log.save ( ts, 4 );
        fichero.close();
    }

    return true;
}

bool XMLOperationsLog::undo()
{
    // inicializar una nueva sesión undo
    undo_group = 0;
    return true;
}

bool XMLOperationsLog::next_undo()
{

    if(undoStack.isEmpty() )
        return false;

    int id_temp;
    // vemos la siguiente operacion (sin extraer de la pila)
    id_temp = undoStack.top();

    // localizar esa operacion en el DOM de log
    XMLOperator* xml_operator = XMLOperator::instance();
    current_op = xml_operator->findElementById ( "operation",root, QString::number ( id_temp ) );
    QString operacion = current_op.attribute ( "type");

    int temp_group = current_op.attribute ( "group").toInt();

    if(undo_group == 0 ) {
        // es la primera operacion del grupo
        undo_group = temp_group;
        openGroup();
    }
    else {
        // comprobar si forma parte de este grupo de operaciones
        if(undo_group != temp_group )
        {
            closeGroup();
            return false;
        }
    }

    // la operacion es valida
    // se ajusta la pila
    undoStack.pop();

    // determinar que operacion hay que hacer para restaurar
    // el sistema (deshacer la ultima operacion)
    // la operacion tiene que ser la contraria

    if(operacion=="add_synapse") {
        // operacion undo: del_synapse
        prev_op_name = current_op_name;
        current_op_name = "del_synapse";

        this->insertComment ( "DEBUG:: PILA UNDO :: id = "+QString::number ( id_temp ) +" :: operacion: "+operacion );

        QDomNode n = current_op.firstChild();
        QDomElement e = n.toElement();
        prev_elem = current_elem;
        current_elem = e;
    }
    else if(operacion=="del_synapse") {
        // operacion undo: add_synapse
        current_op_name = "add_synapse";

        this->insertComment ( "DEBUG:: PILA UNDO :: id = "+QString::number ( id_temp ) +" :: operacion: "+operacion );

        QDomNode n = current_op.firstChild();
        QDomElement e = n.toElement();
        prev_elem = current_elem;
        current_elem = e;

    }
    else if(operacion=="edit_synapse") {

        // operacion undo: edit_synapse
        current_op_name = "edit_synapse";

        this->insertComment ( "DEBUG:: PILA UNDO :: id = "+QString::number ( id_temp ) +" :: operacion: "+operacion );

        QDomNode n = current_op.firstChild();
        QDomElement e = n.toElement();
        prev_elem = current_elem;
        current_elem = e;
    }
    else if(operacion=="add_neuron") {

        // operacion undo: edit_synapse
        current_op_name = "del_neuron";

        this->insertComment ( "DEBUG:: PILA UNDO :: id = "+QString::number ( id_temp ) +" :: operacion: "+operacion );

        QDomNode n = current_op.firstChild();
        QDomElement e = n.toElement();
        prev_elem = current_elem;
        current_elem = e;
    }
    else if(operacion=="del_neuron") {

        // operacion undo: edit_synapse
        current_op_name = "add_neuron";

        this->insertComment ( "DEBUG:: PILA UNDO :: id = "+QString::number ( id_temp ) +" :: operacion: "+operacion );

        QDomNode n = current_op.firstChild();
        QDomElement e = n.toElement();
        prev_elem = current_elem;
        current_elem = e;
    }
    else if(operacion=="edit_neuron") {
        // operacion undo: edit_synapse
        current_op_name = "edit_neuron";

        this->insertComment ( "DEBUG:: PILA UNDO :: id = "+QString::number ( id_temp ) +" :: operacion: "+operacion );

        QDomNode n = current_op.firstChild();
        QDomElement e = n.toElement();
        prev_elem = current_elem;
        current_elem = e;
    }
    else if(operacion=="add_subnet") {
        // operacion undo: del_subnet
        current_op_name = "del_subnet";

        this->insertComment ( "DEBUG:: PILA UNDO :: id = "+QString::number ( id_temp ) +" :: operacion: "+operacion );

        QDomNode n = current_op.firstChild();
        QDomElement e = n.toElement();
        prev_elem = current_elem;
        current_elem = e;
    }
    else if(operacion=="del_subnet") {
        // operacion undo: add_subnet
        current_op_name = "add_subnet";

        this->insertComment ( "DEBUG:: PILA UNDO :: id = "+QString::number ( id_temp ) +" :: operacion: "+operacion );

        QDomNode n = current_op.firstChild();
        QDomElement e = n.toElement();
        prev_elem = current_elem;
        current_elem = e;
    }
    else if(operacion=="edit_subnet") {

        // operacion undo: edit_synapse
        current_op_name = "edit_subnet";

        this->insertComment ( "DEBUG:: PILA UNDO :: id = "+QString::number ( id_temp ) +" :: operacion: "+operacion );

        QDomNode n = current_op.firstChild();
        QDomElement e = n.toElement();
        prev_elem = current_elem;
        current_elem = e;
    }
    return true;
}

bool XMLOperationsLog::redo()
{
    // inicializar una nueva sesi�n redo
    undo_group = 0;
    return true;
}

bool XMLOperationsLog::next_redo()
{

    if(redoStack.isEmpty() )
        return false;

    int id_temp;

    // vemos la siguiente operacion (sin extraer de la pila)
    id_temp = redoStack.top();

    // localizar esa operacion en el DOM de log
    XMLOperator* xml_operator = XMLOperator::instance();
    QDomElement current_op = xml_operator->findElementById ( "operation",root, QString::number ( id_temp ) );
    QString operacion = current_op.attribute ( "type");

    int temp_group = current_op.attribute ( "group").toInt();

    if(undo_group == 0) {
        // es la primera operacion del grupo
        undo_group = temp_group;
        openGroup();
    }
    else {
        // comprobar si forma parte de este grupo de operaciones
        if(undo_group != temp_group )
        {
            closeGroup();
            return false;
        }
    }

    // la operacion es valida
    // se ajusta la pila
    redoStack.pop();


    // determinar que operacion hay que hacer para restaurar
    // el sistema (rehacer la operacion que indica redo)
    if(operacion=="add_synapse") {
        // operacion redo: add_synapse
        current_op_name = "del_synapse";

        this->insertComment ( "DEBUG:: PILA REDO :: id = "+QString::number ( id_temp ) +" :: operacion: "+operacion );

        QDomNode n = current_op.firstChild();
        QDomElement e = n.toElement();
        prev_elem = current_elem;
        current_elem = e;

    }
    else if(operacion=="del_synapse") {
        // operacion redo: del_synapse
        current_op_name = "add_synapse";

        this->insertComment ( "DEBUG:: PILA REDO :: id = "+QString::number ( id_temp ) +" :: operacion: "+operacion );

        QDomNode n = current_op.firstChild();
        QDomElement e = n.toElement();
        prev_elem = current_elem;
        current_elem = e;

    }
    else if(operacion=="edit_synapse") {
        // operacion redo: edit_synapse
        current_op_name = "edit_synapse";

        this->insertComment ( "DEBUG:: PILA REDO :: id = "+QString::number ( id_temp ) +" :: operacion: "+operacion );

        QDomNode n = current_op.firstChild();
        QDomElement e = n.toElement();
        prev_elem = current_elem;
        current_elem = e;
    }
    else if(operacion=="add_neuron") {
        // operacion redo: add_neuron
        current_op_name = "del_neuron";

        this->insertComment ( "DEBUG:: PILA REDO :: id = "+QString::number ( id_temp ) +" :: operacion: "+operacion );

        QDomNode n = current_op.firstChild();
        QDomElement e = n.toElement();
        prev_elem = current_elem;
        current_elem = e;
    }
    else if(operacion=="del_neuron") {
        // operacion redo: del_neuron
        current_op_name = "add_neuron";

        this->insertComment ( "DEBUG:: PILA REDO :: id = "+QString::number ( id_temp ) +" :: operacion: "+operacion );

        QDomNode n = current_op.firstChild();
        QDomElement e = n.toElement();
        prev_elem = current_elem;
        current_elem = e;

    }
    else if(operacion=="edit_neuron") {
        // operacion redo: edit_neuron
        current_op_name = "edit_neuron";

        this->insertComment ( "DEBUG:: PILA REDO :: id = "+QString::number ( id_temp ) +" :: operacion: "+operacion );

        QDomNode n = current_op.firstChild();
        QDomElement e = n.toElement();
        prev_elem = current_elem;
        current_elem = e;
    }
    else if(operacion=="add_subnet") {
        // operacion redo: add_subnet
        current_op_name = "del_subnet";

        this->insertComment ( "DEBUG:: PILA REDO :: id = "+QString::number ( id_temp ) +" :: operacion: "+operacion );

        QDomNode n = current_op.firstChild();
        QDomElement e = n.toElement();
        prev_elem = current_elem;
        current_elem = e;
    }
    else if(operacion=="del_subnet") {
        // operacion redo: del_subnet
        current_op_name = "add_subnet";

        this->insertComment ( "DEBUG:: PILA REDO :: id = "+QString::number ( id_temp ) +" :: operacion: "+operacion );

        QDomNode n = current_op.firstChild();
        QDomElement e = n.toElement();
        prev_elem = current_elem;
        current_elem = e;

    }
    else if(operacion=="edit_subnet") {
        // operacion redo: edit_subnet
        current_op_name = "edit_subnet";

        this->insertComment ( "DEBUG:: PILA REDO :: id = "+QString::number ( id_temp ) +" :: operacion: "+operacion );

        QDomNode n = current_op.firstChild();
        QDomElement e = n.toElement();
        prev_elem = current_elem;
        current_elem = e;
    }

    return true;
}

QDomElement XMLOperationsLog::getCurrentElement()
{    
    return current_elem;
}

QString XMLOperationsLog::getCurrentOperationName()
{    
    return current_op_name;
}

void XMLOperationsLog::removeLastEditOperation ( void )
{
    if(current_op_name.contains("edit")) {
        QDomNode n = current_op.previousSibling();
        QDomElement e;
        root.removeChild (current_op);
        undoStack.pop();
        --id;
        current_op_name = prev_op_name;
        current_elem = prev_elem;
        while(!n.isNull()) {
            e = n.toElement();
            if(e.tagName() != "comment")
                break;
            n=n.previousSibling();
            root.removeChild (e);
        }
        
        Settings* settings = Settings::instance();
        saveLogFile (settings->getAppDirectory() + "fichero_undo.txt");

    }
}
