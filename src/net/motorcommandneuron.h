/***************************************************************************
                          motorcommandneuron.h  -  description
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

#ifndef MOTORCOMMANDNEURON_H
#define MOTORCOMMANDNEURON_H

#include "neuron.h"

class Synapse;


/**Esto es una pueba de
documentación de clases
  *@author Jose M. Cuadra Troncoso
  */
    
class MotorCommandNeuron : public Neuron  {
    Q_OBJECT

  public:
    MotorCommandNeuron(unsigned long n_id, int lay, bool recmon = false,
                    QObject *parent = 0, const char *name = 0);
	 
    MotorCommandNeuron(unsigned long n_id, bool recmon = false,
                    QObject *parent = 0, const char *name = 0);                    
	  ~MotorCommandNeuron();

    int rtti_dev(void) const;
    void input(double val = 0.0, Synapse *synapse = 0);
    bool setParams(const QDomElement& e);
    void write_constructor(QDomElement& e);    

    
  protected:
    void defaultValues(bool to_base = true);     
    QewExtensibleDialog* createDialog(bool mode, bool added); 
    
  
};

#endif
