
/***************************************************************************
                          netsimcontrolfactory.h  -  description
                             -------------------
    begin                : Tue May 3 2005
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

#ifndef NETSIMCONTROLFACTORY_H
#define NETSIMCONTROLFACTORY_H

#include "basesimcontrolfactory.h"
#include <QDomDocument>

class NetSimControl;
class NetRecMonTechnician;
class NeuralNetwork;
class QMdiArea;

/**
  *@author Jose M. Cuadra Troncoso
  */

class NetSimControlFactory : public BaseSimControlFactory  {

public:
    NetSimControlFactory();
    ~NetSimControlFactory();

    BaseSimControl* construct(int type, const QDomElement& e, const QString& control_filename);
    BaseSimControl* construct(void);

protected:
    NetRecMonTechnician* factoryNetMonTech(const QString& control_filename, const QDomElement& e_mon);

protected:
    NeuralNetwork* net;
    NetSimControl* net_control;
    
};

#endif
