/***************************************************************************
                          isosimcontrol.cpp  -  description
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
#include <QObject>

#include "isosimcontrol.h"
#include "gui/ISOcontroltab.h"
#include "../net/neuralnetwork.h"
#include "../net/isodevice.h"
#include "../recmon/netrecmontechnician.h"
#include "../neuraldis/xmloperator.h"

ISOSimControl::ISOSimControl()
{

    weightStabMode = 0;
    isCommonLearnRate = false;
    CommonLearnRate = 0.0;
    weightStabVal = 100.0;
    positiveWeights = true;
    learningDecay = 0.0;

}

ISOSimControl::~ISOSimControl(){
}

void ISOSimControl::setNet(NeuralNetwork* net_)
{

    NetSimControl::setNet(net_);
    setIsos();


}

//void ISOSimControl::save_special(QDomDocument &doc_control, QDomElement &e)
//{
//    NetSimControl::  save_special(doc_control, e);
//}

int ISOSimControl::getType(void)
{

    return ISO_NET_CONTROL;

}

int ISOSimControl::getSuperType(void)
{

    return NET_CONTROL;

}

void ISOSimControl::setIsos(void)
{

    QListIterator<Neuron*> it(net->neurons());
    ISODevice *iso;
    Neuron *neuron;

    while(it.hasNext()) {
        neuron = it.next();
        if(neuron->rtti_dev() == Neuron::ISO_DEVICE || neuron->rtti_dev() == Neuron::ICO_DEVICE) {
            iso = dynamic_cast<ISODevice*> (neuron);
            if(!iso) {
                continue;
            }
            isos.append(iso);
            iso->setMaxSum();
        }
    }

}

QList<QewExtensibleDialog*> ISOSimControl::tabsForEdit(QewExtensibleDialog* parent)
{

    QList<QewExtensibleDialog*> add_dialogs = NetSimControl::tabsForEdit(parent);
    ISOControlTab* control_tab = new ISOControlTab(parent);

    if(control_tab->setUpDialog()) {
        add_dialogs.append(control_tab);
        control_tab->init(this);
    }
    else
        delete control_tab;

    return add_dialogs;

}

void ISOSimControl::setWeightStabMode(int w_stabmode)
{
    QListIterator<ISODevice*> it(isos);
    ISODevice *iso;
    weightStabMode = w_stabmode;
    while(it.hasNext()) {
        iso = it.next();
        iso->setWeightStabMode(weightStabMode);
    }
}

void ISOSimControl::setWeightStabVal(double w_stabval)
{
    QListIterator<ISODevice*> it(isos);
    ISODevice *iso;
    weightStabVal = w_stabval;
    while(it.hasNext()) {
        iso = it.next();
        iso->setWeightStabVal(weightStabVal);
    }
}

void ISOSimControl::setCommonLearnRate(double l_rate)
{

    if(isCommonLearnRate) {
        QListIterator<ISODevice*> it(isos);
        ISODevice *iso;
        CommonLearnRate = l_rate;
        while(it.hasNext()) {
            iso = it.next();
            iso->setLearningRate(CommonLearnRate);
        }
    }
}

void ISOSimControl::setInitWeightsVal(double w_initval)
{

    if(initWeightsMode) {
        QListIterator<ISODevice*> it(isos);
        ISODevice *iso;
        initWeightsVal = w_initval;
        while(it.hasNext()) {
            iso = it.next();
            iso->setInitWeights(initWeightsVal);
        }
    }

}

void ISOSimControl::setPositiveWeights(bool yes)
{

    positiveWeights = yes;
    QListIterator<ISODevice*> it(isos);
    ISODevice *iso;
    while(it.hasNext()) {
        iso = it.next();
        iso->setPositiveWeights(yes);
    }

}

void ISOSimControl:: setLearningDecay(double decay)
{

    QListIterator<ISODevice*> it(isos);
    ISODevice *iso;
    learningDecay = decay;
    while(it.hasNext()) {
        iso = it.next();
        iso->setLearningDecay(learningDecay);
    }
}

void ISOSimControl::setParams(const QDomElement& e)
{

    bool ok;
    NetSimControl::setParams(e);

    XMLOperator* xml_operator = XMLOperator::instance();
    QDomElement e_s_mode = xml_operator->findTag("weightStabMode", e);
    QDomElement e_s_val = xml_operator->findTag("weightStabVal", e);
    QDomElement e_l_mode = xml_operator->findTag("ISOCommonLearnMode", e);
    QDomElement e_l_rate = xml_operator->findTag("ISOCommonLearnRate", e);
    QDomElement e_posw = xml_operator->findTag("positiveWeights", e);
//    QDomElement e_instp = xml_operator->findTag("instantPropagate", e);
    QDomElement e_ldecay = xml_operator->findTag("learningDecay", e);
//    setInstantPropagation(bool(e_instp.text().toInt(&ok)));
    setWeightStabMode(e_s_mode.text().toInt(&ok));
    setWeightStabVal(e_s_val.text().toDouble(&ok));
    setCommonLearnMode(bool(e_l_mode.text().toInt(&ok)));
    setCommonLearnRate(e_l_rate.text().toDouble(&ok));
    setPositiveWeights(bool(e_posw.text().toInt(&ok)));
    setLearningDecay(e_ldecay.text().toDouble(&ok));
    setSamplingPeriod(sampling_period);

}

void ISOSimControl::setSamplingPeriod(double s_period) 
{
    if(s_period > 0) {
        sampling_period = s_period;
        QListIterator<ISODevice*> it(isos);
        ISODevice *iso;
        while(it.hasNext()) {
            iso = it.next();
            iso->setSamplingPeriod(s_period);
        }
    }
}
