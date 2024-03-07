/***************************************************************************
                          hebbiansynapse.cpp  -  description
                             -------------------
    begin                : Thu Dec 2 2004
    copyright            : (C) 2004 by Jose M. Cuadra Troncoso
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

#include <QMessageBox> 

#include "hebbiansynapse.h"
#include "neuron.h"
#include "../disview/gui/edithebbiansyntab.h"

HebbianSynapse::HebbianSynapse(unsigned long n_i , bool recmon, QObject *parent,
									const char *name)
							:LearnSynapse(n_i, recmon, parent, name)
{

  defaultValues();	

}

HebbianSynapse::HebbianSynapse(unsigned long n_i, uchar n_color, double w,
                                  ulong To_, double lear_r , bool recmon,
                                  QObject *parent , const char *name )
							:LearnSynapse(n_i, n_color,  w, To_, lear_r,
                                recmon, parent, name)

{
}

HebbianSynapse::~HebbianSynapse()
{
}

void HebbianSynapse::defaultValues(bool to_base)
{

  if(to_base) Synapse::defaultValues();
  weight = 0.0;

}

bool HebbianSynapse::setParams(const QDomElement& e)
{

  bool ok = LearnSynapse::setParams(e);
  if(!ok) {
      QMessageBox::warning(nullptr,tr("Hebbian synapse") + " " + QString::number(num_id),
                  tr("Error in derived parameters, using defaults"), QMessageBox::Ok);
    defaultValues(false);
  }  
  return ok;
  
}

const QString HebbianSynapse::connName(bool isCap)
{

	return isCap ? tr("Hebbian synapse") : tr("Hebbian synapse");
		
}

void HebbianSynapse::learn(void) // Aquí se coloca el algoritmo de aprendizaje local
{

	weight += learning_rate*Input*ptrTo->output();
    // Sustituir esta línea por el algoritmo que sea
	
}

void HebbianSynapse::write_constructor(QDomElement& e)
{
  Synapse::write_constructor(e);
	e.setAttribute("learning_rate", QString::number(learning_rate));
}

int HebbianSynapse::type(void) const
{

  return Synapse::HEBBIAN;

}
/*
bool HebbianSynapse::edit(bool mode, bool adding)
{

  bool ret = false;
//   EditHebbianSynDialog *dialog = new EditHebbianSynDialog();
//   dialog->init(this, mode, adding);
//   if ( dialog->exec() == EditHebbianSynDialog::Accepted ) {
//     emit changed();
//     ret = true;
//   }
// 	delete dialog;
	
	synapseValuesDialog* dialog = new synapseValuesDialog();
	ret = dialog->setUpDialog();
	if(ret) {	
	  EditHebbianSynTab* syn_tab = new EditHebbianSynTab(dialog);
	  syn_tab->setUpDialog();
    dialog->addExtensibleChild(syn_tab, "");
    dialog->init((Synapse*) this, mode, adding);		 
    syn_tab->init(this, mode); 
	  		 
    if(dialog->exec() == synapseValuesDialog::Accepted) {
      acceptEdit();
      ret = true;	
    } 
    else ret = false; 	  		 
	}
	delete dialog;	
	
  return ret;
}*/

QewExtensibleDialog* HebbianSynapse::createDialog(bool mode, bool added)
{

  QewExtensibleDialog* dialog = Synapse::createDialog(mode, added);
  if(dialog) {
	  EditHebbianSynTab* syn_tab = new EditHebbianSynTab(dialog);
	  syn_tab->setUpDialog();
    dialog->addExtensibleChild(syn_tab, "");	 
    syn_tab->init(this);  
  }
  return dialog;
}

