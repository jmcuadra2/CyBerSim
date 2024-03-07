/***************************************************************************
                          inputneuron.h  -  description
                             -------------------
    begin                : Tue Feb 8 2005
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

#ifndef INPUTNEURON_H
#define INPUTNEURON_H

#include "neuron.h"

class Synapse;
// class IOControlMessage;
// class IOControlMessageGroup;
class AbstractFusionNode;

/**Esto es una pueba de
documentación de clases
  *@author Jose M. Cuadra Troncoso
  */
    
class InputNeuron : public Neuron  {
    Q_OBJECT

  public:
    InputNeuron(unsigned long n_id, int lay, bool recmon = false,
                    QObject *parent = 0, const char *name = 0);
    InputNeuron(ulong n_i, int lay, int sub_lay, const QString& sensorGroupID, int sen_id, double th,
                      int actF, double initext, bool recmon = false,
                    QObject *parent = 0, const char *name = 0);
    InputNeuron(unsigned long n_id, bool recmon = false,
                    QObject *parent = 0, const char *name = 0);                    
    ~InputNeuron();

    int rtti_dev(void) const;
//     void setInputGenerator(IOControlMessage* i_gen) {input_gen = i_gen ; } ;
//     void setIOControlMessage(int inputMessagePos) {this->inputMessagePos = inputMessagePos ; } ;
    void input(double val = 0.0, Synapse *synapse = 0);
    bool setParams(const QDomElement& e);
    void write_constructor(QDomElement& e);
//     int getInputMessageId(void);
//     virtual void setInputMessageId(int messageId);
    int getInputNodeId(void) const;
    virtual void setInputNode(AbstractFusionNode* inputNode);
    void setInputNodeId(int const& inputNode);
    const QString devName(bool isCap);
//     void setIOControlMessageGroup(IOControlMessageGroup* inputMessages) { this->inputMessages = inputMessages ; } 
//     bool edit(bool mode = true, bool adding = false);
    void setSensorGroupID(const QString& sensorGroupID);
    QString getSensorGroupID() const { return sensorGroupID ; }
//     int getInputMessagePos() const { return inputMessagePos ; }
  
  
    
  protected:
    void defaultValues(bool to_base = true);     
    QewExtensibleDialog* createDialog(bool mode, bool added); 
    
  protected:
//     IOControlMessage* input_gen;
//     IOControlMessageGroup* inputMessages;
    AbstractFusionNode* inputNode;
//     int inputMessagePos;
//     int inputMessageId;
    int inputNodeId;
    QString sensorGroupID;
};

#endif
