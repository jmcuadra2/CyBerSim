/***************************************************************************
                          netfactory.h  -  description
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

#ifndef NETFACTORY_H
#define NETFACTORY_H

#include <QDomDocument>

#include "icodevice.h"
#include "resonator.h"
#include "subnet.h"

/**
  *@author Jose M. Cuadra Troncoso
  */

class XMLOperator;  

class NetFactory {

public:
    ~NetFactory();
    static NetFactory* instance(void);

    Neuron* factoryNeuron(const int& type, const ulong& n_i,
                          const QDomElement& e);
    Neuron* factoryNeuron(const int& type, const ulong& n_i,
                          const int& lay);
    Synapse* factorySynapse(const int& type, const ulong& n_i,
                            const QDomElement& e);
    // nuevo constructor de sinapsis  para subredes
    SubNetSynapse* factorySubNetSynapse(const int& type, const ulong& n_i,const QString& from, const QString& to,const QDomElement& e);

    Synapse* factorySynapse(const int& type, const ulong& n_i);

    Resonator* factoryResonator(const int& type, const ulong& n_i,
                                const QDomElement& e);
    Resonator* factoryResonator(const int& type, const ulong& n_i);
    bool readSubObject(int& type, int& num_id,
                       QDomElement& e_construct, const QDomElement& e);
    bool readSubObjectSubNetSynapse(int& type, int& num_id, QString& from, QString& to,
                                    QDomElement& e_construct, const QDomElement& e);

    void buildResonators(QList<Resonator *> &sublist, const QDomElement& e_list);
    void buildAxon(QList<Synapse *> &sublist, const QDomElement& e_list);
    
    // nuevo constructor de axones para subredes
    void buildAxon(QList<SubNetSynapse *> &sublist, const QDomElement& e_list);
    SubNet* factorySubNet( const ulong& n_i, const QString& path,
                           const QString& filename);

protected:
    NetFactory();

protected:
    static NetFactory* factory;
    XMLOperator* xml_operator;

};

#endif
