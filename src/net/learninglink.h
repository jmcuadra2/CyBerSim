/***************************************************************************
                          learninglink.h  -  description
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

#ifndef LEARNINGLINK_H
#define LEARNINGLINK_H

#include "synapse.h"

/**
  *@author Jose M. Cuadra Troncoso
  */

class LearningLink : public Synapse  {

  Q_OBJECT
  public:
  	LearningLink(unsigned long n_i, bool recmon = false,
                      QObject *parent = 0, const char *name = 0);
	  LearningLink(unsigned long n_i, uchar n_color, double w, ulong To_, 
                       bool recmon = false, QObject *parent = 0,
                       const char *name = 0);   
  	~LearningLink();

    int type(void) const;
    const QString connName(bool isCap);
    bool setPointerTo(Neuron* neuron_to);

  signals:
    void weightToLink(double);
        
};

#endif
