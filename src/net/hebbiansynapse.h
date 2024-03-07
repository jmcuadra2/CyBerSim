/***************************************************************************
                          hebbiansynapse.h  -  description
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

#ifndef HEBBIANSYNAPSE_H
#define HEBBIANSYNAPSE_H

#include "learnsynapse.h"

class Neuron;

/**
  *@author Jose M. Cuadra Troncoso
  */

class HebbianSynapse : public LearnSynapse  {
	
	Q_OBJECT
	
  public:
  	HebbianSynapse(unsigned long n_i,  bool recmon = false,
                        QObject *parent = 0, const char *name = 0);
	  HebbianSynapse(unsigned long n_i, uchar n_color, double w,
                      ulong To_, double lear_r, bool recmon = false,
                      QObject *parent = 0, const char *name = 0);                                 
  	~HebbianSynapse();

    bool setParams(const QDomElement& e);  	
    const QString connName(bool isCap);
    void write_constructor(QDomElement& e);
    void learn(void);
    int type(void) const;
/*    bool edit(bool mode, bool adding);  */  

  protected:
    void defaultValues(bool to_base = true);
    QewExtensibleDialog* createDialog(bool mode, bool added);   
    
};

#endif
