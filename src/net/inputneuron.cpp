/***************************************************************************
                          inputneuron.cpp  -  description
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

#include "inputneuron.h"
#include "../disview/gui/editinputneurontab.h"
#include "../control/abstractfusionnode.h"
#include "../disview/gui/neuronvaluesdialog.h"

#include <QMessageBox>

InputNeuron::InputNeuron(unsigned long n_id, int lay, bool recmon,
                            QObject *parent, const char *name)
                  : Neuron(n_id, lay, recmon, parent, name)
{

//   inputMessagePos = -1;
//   inputMessages = 0;
//   inputMessageId = 0;
  inputNodeId = -1;
  inputNode = 0;
  infoWhatsThis = tr("Input from input generator") + " " + QString::number(/*inputMessageId*/inputNodeId)  +" " + tr("of unknown type");
    
}

InputNeuron::InputNeuron(ulong n_id, int lay, int sub_lay, const QString& sensorGroupID,  int /*messageId*/sen_id, double th, int actF,
                        double initext, bool recmon,
                        QObject *parent, const char *name)
             :Neuron(n_id, lay, sub_lay, th, actF, initext,
                              recmon, parent, name)
{
  static int n_bump;
//   inputMessagePos = -1;
//   inputMessages = 0;

  inputNode = 0;
  this->sensorGroupID = sensorGroupID;
 
//   this->inputMessageId = messageId;
  this->inputNodeId = sen_id;
  
  //old version
//   if(!inputMessageId) {
//     if(sub_layer == 2) {
//       ++n_bump;
//       if(n_id == 6) inputMessageId =19;
//       else if(n_id == 7) inputMessageId = 18;
//       else if(n_id ==5) inputMessageId = 17;
//     } else {
//       if(n_id < 5) inputMessageId = n_id;
//       else inputMessageId = n_id - n_bump;
//     }
//   }

  if(!inputNodeId) {
    if(sub_layer == 2) {
      ++n_bump;
      if(n_id == 6) inputNodeId =19;
      else if(n_id == 7) inputNodeId = 18;
      else if(n_id ==5) inputNodeId = 17;
    } else {
      if(n_id < 5) inputNodeId = n_id;
      else inputNodeId = n_id - n_bump;
    }
  }

  //
  infoWhatsThis = tr("Input from input generator") + " " + QString::number(inputNodeId)  + " " + tr("of type") + " " + sensorGroupID;

}

InputNeuron::InputNeuron(ulong n_id, bool recmon,
                            QObject *parent, const char *name)
                    :Neuron(n_id, recmon, parent, name)
{

  defaultValues();

} 

InputNeuron::~InputNeuron(){
}

void InputNeuron::defaultValues(bool to_base)
{

  if(to_base) Neuron::defaultValues();
//   input_gen = 0;
//   inputMessagePos = -1;
//   inputMessages = 0;
//    
//   inputMessageId = 0;
  inputNode = 0;
  inputNodeId = -1;
  infoWhatsThis = tr("Input from input generator") + " " + QString::number(/*inputMessageId*/inputNodeId)  + " " + tr("of unknown type");
                              
}

bool InputNeuron::setParams(const QDomElement& e)
{

  bool ok = false;
  Neuron::setParams(e);
  if(!e.isNull()) 
    inputNodeId = e.attribute("inputNodeId").toInt(&ok);
  if(!ok) {
    if(!e.isNull()) 
      inputNodeId = e.attribute("inputMessageId").toInt(&ok);
    if(!ok) // old version
      inputNodeId = e.attribute("input_gen_id").toInt(&ok);
  }
     
  if(!ok) {
    QMessageBox::warning(nullptr,tr("InputNeuron") + " " + QString::number(num_id),
                  tr("Error in derived parameters, using defaults"), QMessageBox::Ok);
    defaultValues(false);
  }
  sensorGroupID = e.attribute("sensorGroupID");
      
  //old version
  static int n_bump;
//   if(!inputMessageId) {
//     if(sub_layer == 2) {
//       ++n_bump;
//       if(num_id == 6) inputMessageId =19;
//       else if(num_id == 7) inputMessageId = 18;
//       else if(num_id ==5) inputMessageId = 17;
//     } else {
//       if(num_id < 5) inputMessageId = num_id;
//       else inputMessageId = num_id - n_bump;
//     }
//   }
  if(!inputNodeId) {
    if(sub_layer == 2) {
      ++n_bump;
      if(num_id == 6) inputNodeId =19;
      else if(num_id == 7) inputNodeId = 18;
      else if(num_id ==5) inputNodeId = 17;
    } else {
      if(num_id < 5) inputNodeId = num_id;
      else inputNodeId = num_id - n_bump;
    }
  }
  n_bump %= 3;
  //
  
  if(ok)
    infoWhatsThis = tr("Input from input generator") + " " + QString::number(inputNodeId)  + " " + tr("of type") + " " + sensorGroupID;
  else 
    infoWhatsThis = tr("Input from input generator") + " " + QString::number(inputNodeId)  + " " + tr("of unknown type");
   
  return ok;
  
}  

void InputNeuron::write_constructor(QDomElement& e)
{

  Neuron::write_constructor(e);
  e.setAttribute("inputNodeId", QString::number(inputNodeId));
  e.setAttribute("sensorGroupID", sensorGroupID);
                 
}


void InputNeuron::input(double val, Synapse *synapse)
{

  if(synapse) Sum += synapse->input(val);
//   else Sum += input_gen ? input_gen->getNormalizedValue() : 0.0;
  else {
  if(inputNode) {
//     IOControlMessage mess = inputMessages->getMessage(inputMessagePos);
//     Sum += mess.getNormalizedValue();
    Sum += inputNode->result().getY().at(0);
  }
//   Sum += inputMessagePos >= 0 ? inputMessages->getMessage(inputMessagePos).getNormalizedValue() : 0.0;
  }
}

int InputNeuron::rtti_dev(void) const
{

  return (Neuron::INPUT_NEURON);

}

QewExtensibleDialog* InputNeuron::createDialog(bool mode, bool added)
{

  QewExtensibleDialog* dialog = Neuron::createDialog(mode, added);
  if(dialog) {
    EditInputNeuronTab* inputneuron_tab = new EditInputNeuronTab(dialog);
    inputneuron_tab->setUpDialog();
    inputneuron_tab->init(this);    
    dialog->addExtensibleChild(inputneuron_tab, "");     
  }
  return dialog;
}

const QString InputNeuron::devName(bool isCap)
{

  return isCap ? tr("Input neuron") : tr("input neuron");

}

void InputNeuron::setInputNode(AbstractFusionNode* inputNode)
{
  if(inputNode) {
    inputNodeId = atoi(inputNode->result().getEntity());
    infoWhatsThis = tr("Input from input generator") + " " + QString::number(inputNodeId)  + " " + tr("of type") + " " + sensorGroupID;
    this->inputNode = inputNode;
  }
  else 
    cerr << "InputNeuron::setInputNode() null pointer" << endl;
}

// void InputNeuron::setInputMessageId(int message_id) 
// { 
//   inputMessageId = message_id;
//   infoWhatsThis = tr("Input from input generator") + " " + QString::number(inputMessageId)  + " " + tr("of type") + " " + sensorGroupID;   
// }
// 
// int InputNeuron::getInputMessageId(void) 
// { 
//   return inputMessageId; 
// }

int InputNeuron::getInputNodeId(void) const
{ 
  return inputNodeId; 
}

void InputNeuron::setInputNodeId(int const& inputNodeId)
{
  this->inputNodeId = inputNodeId;
}

void InputNeuron::setSensorGroupID(const QString& sensorGroupID) 
{ 
  this->sensorGroupID = sensorGroupID;
  infoWhatsThis = tr("Input from input generator") + " " + QString::number(inputNodeId)  + " " + tr("of type") + " " + sensorGroupID;
}
