/***************************************************************************
                          netsimulationfactory.h  -  description
                             -------------------
    begin                : Thu Apr 21 2005
    copyright            : (C) 2005 by Jose M. Cuadra
    email                : jose@portatil-host
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef NETSIMULATIONFACTORY_H
#define NETSIMULATIONFACTORY_H

#include "basesimulationfactory.h"


/**
  *@author Jose M. Cuadra
  */

class NetSimulationFactory : public BaseSimulationFactory{
  
  public:
  	NetSimulationFactory();
  	~NetSimulationFactory();

//    void construct(const QDomElement& e) {};
//    void setWorldPhysics(const QDomElement& e) {};
//    void setTimeControl(const QDomElement& e) {};
//    void setClocks(void);

//  private:
//    BaseSimulation* sim;   
};

#endif
