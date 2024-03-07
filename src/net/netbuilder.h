/***************************************************************************
                          netbuilder.h  -  description
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

#ifndef NETBUILDER_H
#define NETBUILDER_H

#include <QDomDocument>
#include <QList>

class NeuralNetwork;
class Neuron;
class Synapse;
class NetFactory;
class NeuralDisDoc;
class NeuralDisView;
class XMLOperator;
// nuevo class
class SubNet;

/**
  *@author Jose M. Cuadra Troncoso
  */

class NetBuilder {

public:
    NetBuilder(void);
    ~NetBuilder();

    NeuralNetwork* construct(void);
    // nuevo método para construir las subredes
    NeuralNetwork* newNet(void);
    void setView(NeuralDisView *(&view));
    
protected:
    void buildNeurons(const QDomElement& e_list);
    void buildWeights(const QDomElement& e_list);
    // creo un nuevo elemento para construir las vistas de red
    void buildSubNets(const QDomElement& e_list);
    void connectAxons(void);
    NeuralNetwork* buildNewNet(int n_i, int n_h, int n_o);
    void connetcNet2Doc(void);

protected:
    QDomDocument netDoc;
    QDomElement netInfo;
    NeuralNetwork* net;
    NetFactory* factory;
    XMLOperator* xml_operator;

};

#endif
