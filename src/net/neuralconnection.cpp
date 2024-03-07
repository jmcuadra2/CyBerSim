/***************************************************************************
                          neuralconnection.cpp  -  description
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

#include "neuralconnection.h"

NeuralConnection::NeuralConnection(unsigned long n_id, bool recmon,
                        QObject *parent, const char *name)
  									:RecMonConnection(n_id, recmon, parent, name)
{

  defaultValues();
	
}

NeuralConnection::~NeuralConnection(){
}

void NeuralConnection::defaultValues(bool)
{

  weight = 0.0;
  posWeight = false;
	stabWeight = WS_NONE;
	stabWeightVal = 100.0;
  
}

    

