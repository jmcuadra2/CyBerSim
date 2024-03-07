/***************************************************************************
                          learninglink.cpp  -  description
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

#include "learninglink.h"
#include "neuron.h"

#include <QMessageBox>

LearningLink::LearningLink(unsigned long n_i, bool recmon,
                            QObject *parent, const char *name)
              : Synapse(n_i, recmon, parent, name)
{

  defaultValues();

}

LearningLink::LearningLink(unsigned long n_i, uchar n_color,
                            double w, ulong To_, bool recmon, 
                            QObject *parent, const char *name)
        : Synapse(n_i, n_color, w, To_, recmon, parent, name)
{
}

LearningLink::~LearningLink(){
}


const QString LearningLink::connName(bool isCap)
{

  return isCap ? tr("Learning link") : tr("learning link");

}

int LearningLink::type(void) const
{

  return Synapse::LEARNING_LINK;

} 

bool LearningLink::setPointerTo(Neuron* neuron_to)
{

  Synapse::setPointerTo(neuron_to);
  neuron_to->setLearningLink(this);
  return true;

}
