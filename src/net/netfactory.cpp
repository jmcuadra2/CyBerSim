/***************************************************************************
                          netfactory.cpp  -  description
                             -------------------
    begin                : Wed Mar 16 2005
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

#include "netfactory.h"
#include "inputneuron.h"
#include "hebbiansynapse.h"
#include "learninglink.h"
#include "subnet.h"
// nuevo include para neurona comando motor
#include "motorcommandneuron.h"
// nuevo include para sinapsis entre subredes
#include "subnetsynapse.h"
#include "../neuraldis/xmloperator.h"
#include "../net/neuralnetwork.h"

#include <QMessageBox>
#include <QList>

NetFactory* NetFactory::factory = nullptr;

NetFactory::NetFactory()
{

    xml_operator = XMLOperator::instance();

}

NetFactory::~NetFactory(){
}

NetFactory* NetFactory::instance(void)
{

    if(factory == 0)
        factory = new NetFactory;
    return factory;

}

Neuron* NetFactory::factoryNeuron(const int& type, const ulong& n_i,
                                  const QDomElement& e)
{

    Neuron * neuron = nullptr;
    switch(type) {
    case Neuron::THRESHOLD_DEVICE:
        neuron = new Neuron(n_i);
        break;
    case Neuron::ISO_DEVICE:
        neuron = new ISODevice(n_i);
        break;
    case Neuron::INPUT_NEURON:
        neuron = new InputNeuron(n_i);
        break;
    case Neuron::ICO_DEVICE:
        neuron = new ICODevice(n_i);
        /* código nuevo para llamar al constructor de netview */
        // case Neuron::NET_VIEW:
        //  neuron = new NetView(n_i);
        // break;
        /* AQUÍ DEBO LLAMAR AL CONSTRUCTOR DE NETVIEW */
    case Neuron::MOTOR_COMMAND_NEURON:
        neuron = new MotorCommandNeuron(n_i);
        break;


    default :
        //	QMessageBox::warning(nullptr, "loadNet",
        //            QObject::tr("Neuron class not defined.\nUpcasting to threshold device"),
        //            QObject::tr("&Ok"));
        neuron = new Neuron(n_i);
        break;

    }
    if(neuron)
        neuron->setParams(e);
    
    return neuron;

}

Neuron* NetFactory::factoryNeuron(const int& type, const ulong& n_i,
                                  const int& lay)
{

    Neuron * neuron = 0;
    switch(type) {
    case Neuron::THRESHOLD_DEVICE:
        neuron = (Neuron *)new Neuron(n_i, lay);  // constructor basico
        break;
    case Neuron::ISO_DEVICE:               // constructor basico
        neuron = (Neuron *)new ISODevice(n_i, NeuralNetwork::LAYER_HIDDEN);
        break;
    case Neuron::INPUT_NEURON:
        neuron = (Neuron *)new InputNeuron(n_i, lay);  // constructor basico
        break;
    case Neuron::ICO_DEVICE:               // constructor basico
        neuron = (Neuron *)new ICODevice(n_i, NeuralNetwork::LAYER_HIDDEN);
        break;
    case Neuron::MOTOR_COMMAND_NEURON:
        // esto hay que redefinirlo
        neuron = (Neuron *)new MotorCommandNeuron(n_i,lay);  // constructor basico
        break;

    default :

        neuron = new Neuron(n_i, lay);  // constructor basico
        break;

    }
    return neuron;

}

Synapse* NetFactory::factorySynapse(const int& type, const ulong& n_i)
{

    Synapse * synapse = nullptr;
    switch(type) {
    case Synapse::FIXED:
        synapse = new Synapse(n_i);
        break;
    case Synapse::ISO_INPUT:
        synapse = (Synapse *)new ISOInputSynapse(n_i);
        break;
    case Synapse::LEARNING_LINK:
        synapse = (Synapse *)new LearningLink(n_i);
        break;
    case Synapse::HEBBIAN:
        synapse = (Synapse *)new HebbianSynapse(n_i);
        break;
        // nuevo caso para sinapsis entre subredes
        //case Synapse::SUBNET:
        //        synapse = new SubNetSynapse(n_i);
        //	break;
    default :
        //   		QMessageBox::warning(nullptr, "loadNet",
        //                            QObject::tr("Synapse class not defined.\nUpcasting to synapse base"),
        //                            QObject::tr("&Ok"));
        synapse = new Synapse(n_i);
        break;
    }
    return synapse;

}


Synapse* NetFactory::factorySynapse(const int& type, const ulong& n_i,
                                    const QDomElement& e)
{

    Synapse * synapse = 0;
    switch(type) {
    case Synapse::FIXED:
        synapse = new Synapse(n_i);
        break;
    case Synapse::ISO_INPUT:
        synapse = new ISOInputSynapse(n_i);
        break;
    case Synapse::LEARNING_LINK:
        synapse = new LearningLink(n_i);
        break;
    case Synapse::HEBBIAN:
        synapse = new HebbianSynapse(n_i);
        break;
    default :
        //   		QMessageBox::warning(nullptr, "loadNet",
        //                            QObject::tr("Synapse class not defined.\nUpcasting to synapse"),
        //                            QObject::tr("&Ok"));
        synapse = new Synapse(n_i);
        break;
    }
    if(synapse)
        synapse->setParams(e);
    return synapse;

}

Resonator* NetFactory::factoryResonator(const int& type, const ulong& n_i,
                                        const QDomElement& e)
{

    Resonator* resonator = nullptr;

    if(type == Resonator::RESONATOR)
        resonator = new Resonator(n_i);
    else  {
        resonator = new Resonator(n_i);
    }
    if(resonator)
        resonator->setParams(e);
    return resonator;

}

Resonator* NetFactory::factoryResonator(const int& type, const ulong& n_i)
{

    Resonator* resonator = 0;

    if(type == Resonator::RESONATOR)
        resonator = new Resonator(n_i);
    else  {
        //   	QMessageBox::warning(nullptr, "loadNet",
        //                            QObject::tr("Resonator class not defined.\nUpcasting to synapse"),
        //                            QObject::tr("&Ok"));
        resonator = new Resonator(n_i);
    }
    return resonator;

}

bool NetFactory::readSubObject(int& type, int& num_id,
                               QDomElement& e_construct, const QDomElement& e)
{

    bool ret = false;
    bool ok;
    XMLOperator* xml_operator = XMLOperator::instance();

    QDomElement e_type = xml_operator->findTag("type", e);
    QDomElement e_num_id = xml_operator->findTag("num_id", e);
    e_construct = xml_operator->findTag("constructor", e);
    if(e_type.isNull() || e_num_id.isNull() || e_construct.isNull())
        return ret;
    type =  e_type.text().toInt(&ok);
    if(!ok) type = Resonator::RESONATOR;
    num_id = e_num_id.text().toInt(&ok);
    ret = true;
    return ret;

}

void NetFactory::buildResonators(QList<Resonator*> &sublist, const QDomElement& e_list)
{

    bool ok;
    int super_type, type;
    QDomNode n;
    int num_id;
    Resonator* res;
    
    QDomElement e_suptype = xml_operator->findTag("super_type", e_list);
    if(!e_suptype.isNull()) {
        super_type =  e_suptype.text().toInt(&ok);
        n = e_suptype.nextSibling();
    }
    else {
        super_type = NeuralConnection::RESONATOR;
        n = e_list.firstChild();
    }
    if(super_type != NeuralConnection::RESONATOR)
        return;

    while(!n.isNull()) {
        QDomElement e_res = n.toElement();
        QDomElement e_construct;
        if(readSubObject(type, num_id, e_construct, e_res)) {
            res = factoryResonator(type, num_id, e_construct);
            if(res)
                sublist.append(res);
        }
        n = n.nextSibling();
    }

}

void NetFactory::buildAxon(QList<Synapse*> &sublist, const QDomElement& e_list)
{

    bool ok;
    int super_type, type;
    QDomNode n;
    int num_id;
    Synapse* syn;

    QDomElement e_suptype = xml_operator->findTag("super_type", e_list);
    if(!e_suptype.isNull()) {
        super_type =  e_suptype.text().toInt(&ok);
        n = e_suptype.nextSibling();
    }
    else {
        super_type = NeuralConnection::SYNAPSE;
        n = e_list.firstChild();
    }
    if(super_type != NeuralConnection::SYNAPSE)
        return;

    while(!n.isNull()) {
        QDomElement e_syn = n.toElement();
        QDomElement e_construct;
        if(readSubObject(type, num_id, e_construct, e_syn)) {
            if(super_type == NeuralConnection::SYNAPSE) {
                syn = factorySynapse(type, num_id, e_construct);
                if(syn)
                    sublist.append(syn);
            }
        }
        n = n.nextSibling();
    }

}          

SubNet* NetFactory::factorySubNet( const ulong& n_i, const QString& path,
                                   const QString& filename)
{

    SubNet * subnet = nullptr;
    subnet = new SubNet(n_i,0,path,filename);  // constructor basico
    // queda redefinir el constructor de subred para que guarde lo que queremos: numero de identificador, path y fichero
    return subnet;

}


// nuevo método para construir axones para subredes
void NetFactory::buildAxon(QList<SubNetSynapse*> &sublist,
                           const QDomElement& e_list)
{

    bool ok;
    //  QString type_deriv;
    int super_type, type;
    QDomNode n;
    int num_id;
    QString to;
    QString from;
    SubNetSynapse* syn;

    QDomElement e_suptype = xml_operator->findTag("super_type", e_list);
    if(!e_suptype.isNull()) {
        super_type =  e_suptype.text().toInt(&ok);
        n = e_suptype.nextSibling();
    }
    else {
        n = e_list.firstChild();
    }

    while(!n.isNull()) {
        QDomElement e_syn = n.toElement();
        QDomElement e_construct;
        if(readSubObjectSubNetSynapse(type, num_id,from, to, e_construct, e_syn)) {
            syn = factorySubNetSynapse(type, num_id,from,to, e_construct);
            if(syn)
                sublist.append(syn);

        }
        n = n.nextSibling();
    }

}

SubNetSynapse* NetFactory::factorySubNetSynapse(const int& /*type*/, const ulong& n_i,const QString& from, const QString& to,const QDomElement& /*e*/)
{
    QString numneuronfrom;
    QString numsubnetfrom;
    ulong idneuronfrom = 0;
    ulong idneuronto =  0 ;
    ulong idsubnetfrom = 0 ;
    ulong idsubnetto = 0;

    SubNetSynapse * subnetsynapse = nullptr;


    QString subnet_id_tmp;
    QString neuron_id_tmp;


    // vemos si From es una neurona o una subnet
    int pos_separador = from.lastIndexOf(":");
    if (pos_separador > 0){
        // se trata de una subred
        subnet_id_tmp = from.mid(0, pos_separador);
        idsubnetfrom = subnet_id_tmp.toInt();

        neuron_id_tmp = from.mid(pos_separador+1, from.length());
        idneuronfrom = neuron_id_tmp.toInt();

        // vemos ahora el destino (to)
        pos_separador = to.lastIndexOf(":");
        if (pos_separador > 0) {
            // el origen es una subred y el destino es una subred
            subnet_id_tmp = to.mid(0, pos_separador);
            idsubnetto = subnet_id_tmp.toInt();

            neuron_id_tmp = to.mid(pos_separador+1, to.length());
            idneuronto = neuron_id_tmp.toInt();

        }
        else {
            // el origen es una subred y el destino es una neurona
            idneuronto = to.toInt();
        }
    }
    else {
        // el origen es una neurona y el destino es una subred

        idneuronfrom = from.toInt();

        pos_separador = to.lastIndexOf(":");
        if (pos_separador > 0) {
            // el origen es una subred y el destino es una subred
            subnet_id_tmp = to.mid(0, pos_separador);
            idsubnetto = subnet_id_tmp.toInt();

            neuron_id_tmp = to.mid(pos_separador+1, to.length());
            idneuronto = neuron_id_tmp.toInt();
        }
        else {
            // se ha producido algun error

        }

    }

    // crear la sinapse
    subnetsynapse = new SubNetSynapse(n_i,  idsubnetfrom, idneuronfrom, idsubnetto, idneuronto);

    return subnetsynapse;

}

bool NetFactory::readSubObjectSubNetSynapse(int& type, int& num_id, QString& from, QString& to,
                                            QDomElement& e_construct, const QDomElement& e)
{

    bool ret = false;
    bool ok;
    XMLOperator* xml_operator = XMLOperator::instance();

    QDomElement e_type = xml_operator->findTag("type", e);
    QDomElement e_num_id = xml_operator->findTag("num_id", e);
    QDomElement e_from = xml_operator->findTag("from", e);
    QDomElement e_to = xml_operator->findTag("to", e);
    e_construct = xml_operator->findTag("constructor", e);
    if(e_type.isNull() || e_num_id.isNull() || e_from.isNull() || e_to.isNull() || e_construct.isNull())
        return ret;
    type =  e_type.text().toInt(&ok);
    // if(!ok) type = Resonator::RESONATOR;
    num_id = e_num_id.text().toInt(&ok);
    from = e_from.text();
    to = e_to.text();
    ret = true;
    return ret;

}
