/***************************************************************************
                          learnsynapse.cpp  -  description
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

#include "learnsynapse.h"

LearnSynapse::LearnSynapse(unsigned long n_i, bool recmon,
                            QObject *parent, const char *name)
							:Synapse(n_i, recmon, parent, name)
{

  defaultValues();

}

LearnSynapse::LearnSynapse(unsigned long n_i, uchar n_color, double w, ulong To_,
							    	double lear_r, bool recmon,
                    QObject *parent, const char *name)
           			:Synapse(n_i, n_color,  w,  To_, recmon, parent, name)

{

	learning_rate = lear_r;

}

void LearnSynapse::defaultValues(bool to_base)
{

  if(to_base) Synapse::defaultValues();
	learning_rate = 0.001;

}  

bool LearnSynapse::setParams(const QDomElement& e)
{

  bool ok = false;
  Synapse::setParams(e);
  if(!e.isNull()) 
    learning_rate = e.attribute("learning_rate").toDouble(&ok);       
  return ok;

}


LearnSynapse::~LearnSynapse()
{
}

const QString LearnSynapse::connName(bool isCap)
{

	return isCap ? tr("Learner synapse") : tr("learner synapse");
		
}
