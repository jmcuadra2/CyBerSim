/***************************************************************************
                          nullsimcontrol.h  -  description
                             -------------------
    begin                : Sat Apr 30 2005
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

#ifndef NULLSIMCONTROL_H
#define NULLSIMCONTROL_H

#include "basesimcontrol.h"

class QewExtensibleDialog;

/**
  *@author Jose M. Cuadra Troncoso
  */

class NullSimControl : public BaseSimControl  {

    //   Q_OBJECT
public:
    NullSimControl();
    ~NullSimControl();

    void sendOutputs(void);
    int getType(void);
    int getSuperType(void);
    void backPropagate(void);
    void doMonitor(void);
    void connectIO(void);
    QList<QewExtensibleDialog*> tabsForEdit(QewExtensibleDialog* parent);
    void initElements(void);
    void setParams(const QDomElement& e);
    bool createSMGInterface(void);

protected:
    void propagate(void);
    void readInputs(void);

};

#endif
