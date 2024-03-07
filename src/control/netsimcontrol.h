/***************************************************************************
                          netsimcontrol.h  -  description
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

#ifndef NETSIMCONTROL_H
#define NETSIMCONTROL_H

#include "basesimcontrol.h"
#include "smginterface.h"
#include <QList>

class NeuralNetwork;
class NetRecMonTechnician;
class InputNeuron;
class MotorCommandNeuron;

/**
  *@author Jose M. Cuadra Troncoso
  */

class NetSimControl : public BaseSimControl
{

    friend class NetRecMonTechnician;
    friend class NetSimControlFactory;

public:
    NetSimControl();
    ~NetSimControl();

    void initElements(void);
    void sendOutputs(void);
    int getType(void);
    int getSuperType(void);
    void backPropagate(void);
    void setRecording(bool on, bool crono_reset);
    void connectIO(void);
    void doMonitor(void);
    QList<QewExtensibleDialog*> tabsForEdit(QewExtensibleDialog* parent);
    void save_special(QDomDocument& doc_control, QDomElement& e);
    void setParams(const QDomElement& e);

    virtual void setNet(NeuralNetwork* net_);
    void setNetName(const QString& netname) {net_name = netname ;}
    virtual int getWeightStabMode(void);
    virtual double getWeightStabVal(void);
    virtual void setWeightStabMode(int w_stabmode);
    virtual void setWeightStabVal(double w_stabval);
    virtual bool getInitWeightsMode(void);
    virtual double getInitWeightsVal(void);
    virtual void setInitWeightsMode(bool w_initmode);
    virtual void setInitWeightsVal(double w_initval);
    virtual void setCommonLearnRate(double l_rate);
    virtual double getCommonLearnRate(void);
    virtual void setCommonLearnMode(bool l_mode);
    virtual bool getCommonLearnMode(void);
    virtual void setPositiveWeights(bool yes);
    virtual bool getPositiveWeights(void);
    virtual void setInstantPropagation(bool is_instant);
    virtual bool getInstantPropagation(void);
    virtual void setLearningDecay(double decay);
    virtual double getLearningDecay(void);

    bool createSMGInterface(void);

protected:
    void propagate(void);
    void readInputs(void);

protected:
    NeuralNetwork* net;
    QString net_name;
    NetRecMonTechnician* netRecMonTech;
    int weightStabMode;
    bool isCommonLearnRate, initWeightsMode;
    double CommonLearnRate, weightStabVal, initWeightsVal;
    bool positiveWeights;
    bool instantPropagate;
    double learningDecay;
    QList<InputNeuron* > sensorNeurons;
    QList<MotorCommandNeuron* > commandNeurons;

};

#endif
