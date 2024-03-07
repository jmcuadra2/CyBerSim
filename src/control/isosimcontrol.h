/***************************************************************************
                          isosimcontrol.h  -  description
                             -------------------
    begin                : Sun Jun 5 2005
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

#ifndef ISOSIMCONTROL_H
#define ISOSIMCONTROL_H

#include "netsimcontrol.h"

class ISODevice;

/**
  *@author Jose M. Cuadra Troncoso
  */

class ISOSimControl : public NetSimControl  {

    //   Q_OBJECT
    friend class NetRecMonTechnician;
    friend class NetSimControlFactory;

public:
    ISOSimControl();
    ~ISOSimControl();

    int getType(void);
    int getSuperType(void);
    void setNet(NeuralNetwork* net_);
//    void save_special(QDomDocument& doc_control, QDomElement& e);
    QList<QewExtensibleDialog*> tabsForEdit(QewExtensibleDialog* parent);
    void setWeightStabMode(int w_stabmode);
    void setWeightStabVal(double w_stabval);
    void setCommonLearnRate(double l_rate);
    void setInitWeightsVal(double w_initval);
    void setPositiveWeights(bool yes);
    void setParams(const QDomElement& e);
    void setLearningDecay(double decay);
    void setSamplingPeriod(double s_period);

protected:
    virtual void setIsos(void);

protected:
    QList<ISODevice*> isos;

};

#endif
