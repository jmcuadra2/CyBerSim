/***************************************************************************
                          learnsynapse.h  -  description
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

#ifndef LEARNSYNAPSE_H
#define LEARNSYNAPSE_H

#include "synapse.h"

/**
  *@author Jose M. Cuadra Troncoso
  */

class LearnSynapse : public Synapse  {
	
	Q_OBJECT
	
  public:
  	LearnSynapse(unsigned long n_i, bool recmon = false,
                    QObject *parent = 0, const char *name = 0);
	  LearnSynapse(unsigned long n_i, uchar n_color, double w, ulong To_,
           					  double lear_r, bool recmon = false,
                      QObject *parent = 0, const char *name = 0);         
  	~LearnSynapse();

    bool setParams(const QDomElement& e) ;
  	virtual void setLearnRate(double lr)
  				{learning_rate = (lr < 0 || lr > 1) ? learning_rate : lr ;}
  	virtual double getLearnRate(void) const {return learning_rate ;}
  	const QString connName(bool isCap);
		virtual void learn(void) = 0;
  	
	protected:
		double learning_rate;
    void defaultValues(bool to_base = true);  	
		
};

#endif
