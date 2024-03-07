/***************************************************************************
                          motorcommandneuron.cpp  -  description
                             -------------------
    begin                : Tue Feb 8 2007
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

#include "motorcommandneuron.h"
#include "../disview/gui/neuronvaluesdialog.h"

#include <QMessageBox>

MotorCommandNeuron::MotorCommandNeuron(unsigned long n_id, int lay, bool recmon,
                            QObject *parent, const char *name)
									: Neuron(n_id, lay, recmon, parent, name)
{
    
  infoWhatsThis = "Hola neurona de comando motor" ;
    
}


MotorCommandNeuron::MotorCommandNeuron(ulong n_id, bool recmon,
                            QObject *parent, const char *name)
                    :Neuron(n_id, recmon, parent, name)
{

  defaultValues();

} 

MotorCommandNeuron::~MotorCommandNeuron(){
}

void MotorCommandNeuron::defaultValues(bool to_base)
{

  if(to_base) Neuron::defaultValues();
  infoWhatsThis = "Hola neurona de comando motor" ;
                              
                              
}

bool MotorCommandNeuron::setParams(const QDomElement& e)
{

  bool ok = false;
  Neuron::setParams(e);
  return ok;
  
}  

void MotorCommandNeuron::write_constructor(QDomElement& e)
{

  Neuron::write_constructor(e);
  
                 
}


void MotorCommandNeuron::input(double val, Synapse *synapse)
{

//   if(synapse) Sum +=  activationObject->value(Sum,synapse->input(val));
//   else Sum += 0;

  if(synapse) Sum += synapse->input(val);
  else Sum += val;  

}

int MotorCommandNeuron::rtti_dev(void) const
{

  return (Neuron::MOTOR_COMMAND_NEURON);

}

QewExtensibleDialog* MotorCommandNeuron::createDialog(bool mode, bool added)
{
  QewDialogFactory *factory = new QewDialogFactory();
  QewExtensibleDialog* main_dialog = factory->createDialog(QewExtensibleDialog::QewSimpleWidget);    
  if(main_dialog->setUpDialog()) {
	neuronValuesDialog *dialog = new neuronValuesDialog(main_dialog);
	dialog->setUpDialog();
	dialog->init((Neuron*) this, mode, added);	
	main_dialog->addExtensibleChild(dialog, main_dialog->windowTitle());	 		  
  }
  else {
	delete main_dialog;
	main_dialog = 0;
  } 
  return main_dialog;
}
