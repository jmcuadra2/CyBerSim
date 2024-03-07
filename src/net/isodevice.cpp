/***************************************************************************
                          isodevice.cpp  -  description
                             -------------------
    begin                : dom feb 22 2004
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

#include <QMessageBox>
#include <QPainter>

#include <QList>
#include <QVector>

#include "isodevice.h"
#include "netfactory.h"
#include "learninglink.h"
#include "../disview/gui/editisodevicetab.h"
#include "../disview/gui/neuronvaluesdialog.h"

ISODevice::ISODevice(ulong n_i, int sub_lay, double th, int actF,
                     double initext, double l_r, bool recmon,
                     QObject *parent , const char *name)
    :	Neuron(n_i, 1, sub_lay, th, actF,      // 1 = LAYER_HIDDEN
               initext, recmon, parent, name), signalOutDiff(-3)
{
    defaultValues();
    learning_rate = l_r;
}

ISODevice::ISODevice(ulong n_i, int lay, bool recmon,
                     QObject *parent , const char *name )
    :	Neuron(n_i, lay, recmon, parent, name ),
      signalOutDiff(-3)
{
    defaultValues();
    layer = lay;
}

ISODevice::ISODevice(ulong n_i, bool recmon,
                     QObject *parent , const char *name )
    :	Neuron(n_i, recmon, parent , name), signalOutDiff(-3)
{
    defaultValues();
}

ISODevice::~ISODevice()
{
    clearResonators();
}

void ISODevice::defaultValues(bool to_base)
{

    if(to_base)
        Neuron::defaultValues();
    layer = 1; // HIDDEN_LAYER
    learning_rate = 0.01;
    USVal = 0.0;
    CSVal = 0.0;
    refrac_period = 0;
    init_refrac_period = 0;
    maxSum = 0.0;
    infoWhatsThis = "Hola ISO " + QString::number(num_id);
    signal_map[tr("Output_difference")] = signalOutDiff;
    USInput = 0;
    CSInput = 0;
    old_diff = 0.0;

    derivative = 0.0;

}

bool ISODevice::setParams(const QDomElement& e)
{

    bool ok = false;

    Neuron::setParams(e);

    if(!e.isNull())
        learning_rate = e.attribute("learning_rate").toDouble(&ok);

    if(!ok) {
//        QMessageBox::warning(nullptr,tr("ISODevice") + " " + QString::number(num_id),
//                             tr("Error in derived parameters, using defaults"),tr("&Ok"));
        QMessageBox::warning(nullptr,tr("ISODevice") + " " + QString::number(num_id),
                             tr("Error in derived parameters, using defaults"), QMessageBox::Ok);
        defaultValues(false);
    }
    return ok;

}

QewExtensibleDialog* ISODevice::createDialog(bool mode, bool added)
{

    QewDialogFactory *factory = new QewDialogFactory();
    QewExtensibleDialog* dialog = factory->createDialog(QewExtensibleDialog::QewTabWidget);
    if(dialog->setUpDialog()) {
        neuronValuesDialog *base_dialog = new neuronValuesDialog(dialog);
        base_dialog->setUpDialog();
        base_dialog->init((Neuron*) this, mode, added);
        dialog->addExtensibleChild(base_dialog, "Common parameters");

        EditISODeviceTab* iso_tab = new EditISODeviceTab(dialog);
        iso_tab->setUpDialog();
        iso_tab->init(this, mode);
        dialog->addExtensibleChild(iso_tab, "");
    }
    else {
        delete dialog;
        dialog = 0;
    }
    return dialog;
}

QList<Resonator*> ISODevice::getResonators(void)
{
    return resonators;
}

void ISODevice::clearResonators(void)
{
    Resonator *res;
    QListIterator<Resonator *> it(resonators);
    while(it.hasNext()) {
        res = it.next();
        delete res;
    }
    resonators.clear();

}

void ISODevice::setResonators(QList<Resonator*> resons)
{

    Resonator *res;
    QListIterator<Resonator *> it(resons);
    while(it.hasNext()) {
        res = it.next();
        resonators.append(res);
        connect(res, SIGNAL(changed()), this, SIGNAL(changed()));
    }

}

void ISODevice::connectResonators(void)
{

    Resonator *res;
    QListIterator<Resonator *> it(resonators);
    while(it.hasNext()) {
        res = it.next();
        connect(res, SIGNAL(changed()), this, SIGNAL(changed()));
    }

}

int ISODevice::rtti_dev(void) const
{
    return (Neuron::ISO_DEVICE);
}

Resonator * ISODevice::addReson(int n_id, double a, int n, bool isCS,
                                double w, bool wf, bool noneg)
{

    Resonator *res;
    res = new Resonator(n_id, a, n, isCS, w, wf, noneg);
    connect(res, SIGNAL(changed()), this, SIGNAL(changed()));
    if(isCS) {
        resonators.append(res);
        if(init_refrac_period < n)
            init_refrac_period = n;
    } else  {
        resonators.prepend(res);
        if(init_refrac_period < n)
            init_refrac_period = n;
    }

    return res;

}

double ISODevice::calculateOutput(void)
{

    QListIterator<Resonator *> it(resonators);
    QVector<double> outs(resonators.count());
    QVector<double> weights(resonators.count());
    outs.fill(0.0);
    weights.fill(0.0);

    Resonator *res;
    double diff;
    double weights_sum = 0.0;
    double prevOut = Output;
    int i = 0;

    Output = 0.0; //Sum + InitExtern

    while(it.hasNext()) {
        res = it.next();
        if(res->isCStim())
            outs[i] = res->flts(CSVal);
        else
            outs[i] = res->flts(USVal);
        weights[i] = res->getWeight();
        if(res->isCStim())
            weights_sum += fabs(weights[i]);
        Output += outs[i]*weights[i];
        ++i;
    }

    i = 0;
    diff = calculateDerivative(prevOut)*learning_rate;

    it.toFront();
    while(it.hasNext()) {
        res = it.next();
        if(!res->getWeightFix()) {
            weights[i] += outs[i]*diff;
            res->stabilizeWeight(weights[i], weights_sum);
        }
        ++i;
    }
    //   if(Output > -0.6 && Output < 3.5)
    hasActvView();
    old_out = Output;
    input_sum = Sum;
    Sum = 0.0;
    old_diff = diff;
    return Output;

}

double ISODevice::calculateDerivative(double const& old_value) {

    derivative = Output - old_value;
    return derivative;
}

void ISODevice::input(double val, Synapse *synapse)
{

    if(synapse == (Synapse*)USInput) {
        USVal = val*synapse->getWeight();
        Sum  += USVal;
    } else if(synapse == (Synapse*)CSInput) {
        CSVal = val*synapse->getWeight();
        Sum  += CSVal;
    }

}

void ISODevice::initOldOut(void)
{
    old_out = 0.0;
}

void ISODevice::setMaxSum(void)
{
    if(USInput && CSInput)
        maxSum = USInput->getWeight() + CSInput->getWeight();
    else
        maxSum = 1.0;
}

void ISODevice::setWeightStabMode(int w_stabmode)
{

    QListIterator<Resonator *> it(resonators);
    Resonator *res;
    while(it.hasNext()) {
        res = it.next();
        res->setStabWeightMode(w_stabmode);
    }

}

void ISODevice::setWeightStabVal(double w_stabval)
{

    QListIterator<Resonator *> it(resonators);
    Resonator *res;
    while(it.hasNext()) {
        res = it.next();
        res->setStabWeightVal(w_stabval);
    }

}

void ISODevice::setLearningDecay(double decay)
{

    QListIterator<Resonator *> it(resonators);
    Resonator *res;
    while(it.hasNext()) {
        res = it.next();
        res->setLearningDecay(decay);
    }

}

void ISODevice::hasActvView(void)
{
    if(int(old_out*8.999999*15) != int(Output*8.999999*15))
        emit activateView(this, Output*15);
}

void ISODevice::emitToMonitor(void)
{

    emit emitMonVal(input_sum, signalInput);
    emit emitMonVal(Output, signalOutput);
    emit emitMonVal(derivative, signalOutDiff);

}

void ISODevice::emitToRecorder(void)
{
    emit emitRecVal(input_sum, signalInput);
    emit emitRecVal(Output, signalOutput);
    emit emitRecVal(derivative, signalOutDiff);
}

QList<RecMonConnection*> ISODevice::getRecMonConnections(void)
{
    Resonator *res;
    QListIterator<Resonator *> it(resonators);
    QList<RecMonConnection *> conn;
    while(it.hasNext()) {
        res = it.next();
        conn.append(res);
    }
    return(conn);

}

void ISODevice::setPositiveWeights(bool yes)
{

    Resonator *res;
    QListIterator<Resonator *> it(resonators);
    while(it.hasNext()) {
        res = it.next();
        res->setPosWeight(yes);
    }

}

void ISODevice::setInitWeights(double w_initval)
{

    Resonator *res;
    QListIterator<Resonator *> it(resonators);
    while(it.hasNext()) {
        res = it.next();
        if(!res->getWeightFix())
            res->setWeight(w_initval);
    }

}

const QString ISODevice::devName(bool isCap)
{
    return isCap ? tr("ISO Device") : tr("ISO device");
}

void ISODevice::write_constructor(QDomElement& e)
{
    Neuron::write_constructor(e);
    e.setAttribute("learning_rate", QString::number(learning_rate));
}

void ISODevice::write_subobjects(QDomDocument& doc, QDomElement& e)
{

    Neuron::write_subobjects(doc, e);
    Resonator *reson;
    QList<Resonator *> ress = getResonators();
    QListIterator<Resonator *> itr(ress);
    QDomElement tag_reson = doc.createElement("Resonators");
    e.appendChild(tag_reson);
    while (itr.hasNext()) {
        reson = itr.next();
        reson->write(doc, tag_reson);
    }

}

void ISODevice::initSubObjects(const QDomElement& e_list)
{

    NetFactory* factory = NetFactory::instance();
    Neuron::initSubObjects(e_list);
    QDomNode n = e_list.firstChild();
    while( !n.isNull() ) {
        QDomElement e_sub = n.toElement();
        if(e_sub.tagName() == "Resonators") {
            clearResonators();
            factory->buildResonators(resonators, e_sub);
            connectResonators();
        }
        n = n.nextSibling();
    }

}

void ISODevice::addSubObject(Resonator* res)
{
    resonators.append(res);
}

QAbstractGraphicsShapeItem* ISODevice::createCanvasItem(QGraphicsScene* scene)
{
//    QAbstractGraphicsShapeItem* item = new QGraphicsRectItem(-width()/2, -height()/2, width(), height());
    QAbstractGraphicsShapeItem* item = new QGraphicsRectItem(0, 0, width(), height());

    scene->addItem(item);
    return item;
}

void ISODevice::setLearningLink(LearningLink* syn_link)
{

    ISODevice *iso_from;
    QList<Resonator *> resons_from, resons_to;
    Resonator *res_f, *res_t;
    iso_from = dynamic_cast<ISODevice*> (syn_link->getPointerFrom());
    if(iso_from) {
        resons_from = iso_from->getResonators();
        resons_to = getResonators();
        QListIterator<Resonator *> itr_from(resons_from);
        QListIterator<Resonator *> itr_to(resons_to);
        if(resons_to.count() != resons_from.count()) {
            return;
        }
        while(itr_from.hasNext()) {
            res_f = itr_from.next();
            res_t = itr_to.next();
            if(res_f && res_t) {
                res_t->setLinkWeight(syn_link->getWeight());
                connect(res_f, SIGNAL(weightDif(double)), res_t, SLOT(addWeight(double)));
            }
        }
    }

}

void ISODevice::setSamplingPeriod(double s_period)
{

    Resonator *res;
    QListIterator<Resonator *> it(resonators);
    while(it.hasNext()) {
        res = it.next();
        res->setSamplingPeriod(s_period);
    }

}

void ISODevice::num_idChange(ulong prev_n_i)
{
    Neuron::num_idChange(prev_n_i);
    Resonator *res;
    QListIterator<Resonator *> it(resonators);
    while(it.hasNext()) {
        res = it.next();
        res->setNumId(getNumId() * 10000 + res->getNumId()%10000);
    }
}
