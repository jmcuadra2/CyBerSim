/***************************************************************************
                          isoinputsynapse.cpp  -  description
                             -------------------
    begin                : Sun Jun 19 2005
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

#include "isoinputsynapse.h"
#include "isodevice.h"
#include "../disview/gui/synapsevaluesdialog.h"
#include "../disview/gui/inputisosyntab.h"

#include <QMessageBox>

ISOInputSynapse::ISOInputSynapse(unsigned long n_i, bool recmon,
                            QObject *parent, const char *name)
              : Synapse(n_i, recmon, parent, name)
{

  defaultValues();
  
}

ISOInputSynapse::ISOInputSynapse(unsigned long n_i, bool isCondStim,
                            uchar n_color, double w, ulong To_,
                            bool recmon, QObject *parent,
                            const char *name)
        : Synapse(n_i, n_color, w, To_, recmon, parent, name)
{

  defaultValues(false);
  isCS = isCondStim;
  firstSetPointerTo = false;
}                            

ISOInputSynapse::~ISOInputSynapse()
{
  if(iso_to) {
    if(isCS)
      iso_to->setCSInput(0); 
    else
      iso_to->setUSInput(0);
  }
}

void ISOInputSynapse::defaultValues(bool to_base)
{

  if(to_base) Synapse::defaultValues(to_base);
  isCS = true;
  iso_to = 0;
  firstSetPointerTo = true;

}

bool ISOInputSynapse::setParams(const QDomElement& e)
{

  bool ok = Synapse::setParams(e);
  if(!ok) {
      QMessageBox::warning(nullptr,tr("ISO input synapse") + " " + QString::number(num_id),
                  tr("Error in derived parameters, using defaults"), QMessageBox::Ok);
    defaultValues(false);
  }
  else {
    isCS = bool(e.attribute("is_cond_stim").toInt(&ok));
    if(!ok) 
      isCS = true;
    else 
      firstSetPointerTo = false;
  }
  return ok;

}    

bool ISOInputSynapse::setPointerTo(Neuron* neuron_to)
{

  bool ret = false;
//   ptrTo = neuron_to;
  iso_to = dynamic_cast<ISODevice*> (neuron_to);
  if(iso_to) {
    Synapse::setPointerTo(neuron_to); // cosas nuevas "this =="
    if(firstSetPointerTo) {
      ret = true; 
      firstSetPointerTo = false;
    }
    else {
      if(isCS) {
        if(!iso_to->getCSInput() || this == iso_to->getCSInput()) {
          iso_to->setCSInput(this);
          ret = true;
        }
      }
      else {
        if(!iso_to->getUSInput() || this == iso_to->getUSInput()) {
          iso_to->setUSInput(this);
          ret = true;
        }        
      }
    }
  }
  return ret;
      
}

//bool ISOInputSynapse::getIsCS(void)
//{
//
//  return isCS;
//}

void ISOInputSynapse::setIsCS(bool isCondStim)
{

  if(validStim(isCondStim)) {
    isCS = isCondStim;
    if(isCS)
      iso_to->setCSInput(this); 
    else
      iso_to->setUSInput(this);
  }
  
}

bool ISOInputSynapse::validStim(bool isCondStim)
{
  bool ret = false;
  if(!iso_to) return true;
  if(isCondStim) {
    if(!iso_to->getCSInput() || this == iso_to->getCSInput())
      ret = true;
  }
  else {
    if(!iso_to->getUSInput() || this == iso_to->getUSInput())
      ret = true;
  }
  if(!ret) {
    QString stim = isCS ? tr("conditioned stimulus") :
                            tr("unconditioned stimulus");
      QMessageBox::warning(nullptr,tr("ISO input synapse") + " " + QString::number(num_id),
                  tr("ISO device only allows one") + " " + stim, QMessageBox::Ok);
  }
  return ret;
    
}  

int ISOInputSynapse::type(void) const
{

  return Synapse::ISO_INPUT;

}

const QString ISOInputSynapse::connName(bool isCap)
{

  QString is_cs = isCS ? tr("conditioned") : tr("unconditioned");
  return isCap ? tr("ISO input") + " " + is_cs :
                      tr("ISO input") + " " + is_cs;

}

void ISOInputSynapse::write_constructor(QDomElement& e)
{

  Synapse::write_constructor(e);
	e.setAttribute("is_cond_stim", QString::number(isCS));
  
}

// bool ISOInputSynapse::edit(bool mode, bool adding)
// {
// 
//   bool ret = false;
// //   InputISOSynDialog *dialog = new InputISOSynDialog();
// //   dialog->init(this, mode, adding);
// //   if ( dialog->exec() == InputISOSynDialog::Accepted ) {
// //     emit changed();
// //     ret = true;
// //   }
// // 	delete dialog;
// 
// 	synapseValuesDialog* dialog = new synapseValuesDialog();
// 	InputISOSynTab* syn_tab = new InputISOSynTab(dialog);
// 
// 	ret = dialog->setUpDialog();
// 	if(ret) {
// 	  syn_tab->setUpDialog();
//     dialog->addExtensibleChild(syn_tab, "");
//     dialog->init((Synapse*) this, mode, adding);		 
//     syn_tab->init(this, mode);  		 
//     if(dialog->exec() == synapseValuesDialog::Accepted) {
//       acceptEdit();
//       ret = true;	
//     }
//     else ret = false;
// 	}
// 	delete dialog;
// 
//   return ret;
// }

QewExtensibleDialog* ISOInputSynapse::createDialog(bool mode, bool added)
{

  QewExtensibleDialog* dialog = Synapse::createDialog(mode, added);
  if(dialog) {
    InputISOSynTab* syn_tab = new InputISOSynTab(dialog);
    syn_tab->setUpDialog();
    dialog->addExtensibleChild(syn_tab, "");	 
    syn_tab->init(this);  
  }
  return dialog;
}
