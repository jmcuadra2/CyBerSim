/***************************************************************************
                          netbuilder.cpp  -  description
                             -------------------
    begin                : Wed Mar 16 2005
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

#include <QApplication>
#include <QDir>
#include <QObject>
#include <QWhatsThis>
#include <QList>
#include <QMdiSubWindow>

#include "netbuilder.h"
#include "netfactory.h"
#include "neuralnetwork.h"
#include "neuron.h"
#include "synapse.h"
#include "../neuraldis/neuraldisdoc.h"
#include "../disview/neuraldisview.h"
#include "../disview/gui/buildnetdialog.h"
#include "../neuraldis/settings.h"
#include "../neuraldis/xmloperator.h"

NetBuilder::NetBuilder(void)
{

    factory = NetFactory::instance();
    xml_operator = XMLOperator::instance();

}

NetBuilder::~NetBuilder()
{

}

NeuralNetwork* NetBuilder::construct(void)
{
    // este método se ha cambiado para que al construir la red nos ponga también las subredes
    bool ok;

    Settings* prog_settings = Settings::instance();
    NeuralDisDoc* doc = NeuralDisDoc::instance();
    netDoc = doc->getXMLNet();

    QDomNode n_info = netDoc.firstChild();
    netInfo = n_info.toElement();
    net = new NeuralNetwork();
    net->setNetDoc(netDoc);
    prog_settings->setNet(net);
    if(netInfo.isNull()) {
        newNet();
        return net;
    }

    net->NumberOfNeurons = 0;
    QDomElement e_ninput = xml_operator->findTag("number_input", netInfo);
    QDomElement e_nhidden = xml_operator->findTag("number_hidden", netInfo);
    QDomElement e_noutput = xml_operator->findTag("number_output", netInfo);
    QDomElement e_nsyn = xml_operator->findTag("number_synapses", netInfo);
    // nuevo elemento para contar el numero de subredes
    QDomElement e_nsubnets = xml_operator->findTag("number_subnets", netInfo);
    QDomElement e_nsubnetsynapses = xml_operator->findTag("number_subnetsynapses", netInfo);
    QDomElement e_neurons = xml_operator->findTag("Neurons_list", netInfo);
    // nuevo elemento para leer la lista de subredes
    QDomElement e_subnets = xml_operator->findTag("Subnets_list", netInfo);
    QDomElement e_weights = xml_operator->findTag("Weights_list", netInfo);
    if(e_ninput.isNull() || e_nhidden.isNull() || e_noutput.isNull() ||
            e_nsyn.isNull() || e_neurons.isNull() || e_weights.isNull())
    {
        newNet();
        return net;
    }
    net->num_InputN = e_ninput.text().toUInt(&ok);
    net->NumberOfNeurons += net->num_InputN;
    net->num_HiddenN = e_nhidden.text().toUInt(&ok);
    net->NumberOfNeurons += net->num_HiddenN;
    net->num_OutputN = e_noutput.text().toUInt(&ok);
    net->NumberOfNeurons += net->num_OutputN;
    net->NumberOfSynapses = e_nsyn.text().toUInt(&ok);
    buildNeurons(e_neurons);
    buildWeights(e_weights);
    // tengo subredes y por tanto puedo llamar a su constructor
    if(!e_nsubnets.isNull())
    {
        net->NumberOfSubNets = e_nsubnets.text().toUInt(&ok);
        net->NumberOfSubNetSynapses = e_nsubnetsynapses.text().toUInt(&ok);
        buildSubNets(e_subnets);
    }

    connectAxons();

    connetcNet2Doc();

    net->nonEditableWeights();  // emite netChanged()
    net->linkNumId();
    doc->setModified(false);
    return net;

}

void NetBuilder::buildNeurons(const QDomElement& e_list)
{

    bool ok;
    int type;
    uint num_id;
    QDomNode n = e_list.firstChild();
    while(!n.isNull()) {
        QDomElement e = n.toElement();
        if(e.isNull()) {
            n = n.nextSibling();
            continue;
        }
        QDomElement e_type = xml_operator->findTag("type", e);
        QDomElement e_num_id = xml_operator->findTag("num_id", e);
        QDomElement e_construct = xml_operator->findTag("constructor", e);
        QDomElement e_subobj = xml_operator->findTag("SubObjects", e);
        if(e_type.isNull() || e_num_id.isNull() || e_construct.isNull()) {
            n = n.nextSibling();
            continue;
        }
        num_id = e_num_id.text().toUInt(&ok);
        type =  e_type.text().toInt(&ok);
        Neuron* neuron = factory->factoryNeuron(type, num_id, e_construct);
        if(neuron) {
            net->neurons().append(neuron);
            QObject::connect(neuron, SIGNAL(changed()),
                             net, SIGNAL(netChanged()));
            switch(neuron->getLayer()) {
            case NeuralNetwork::LAYER_INPUT:
                net->Input_Neuron.append(neuron);
                break;
            case NeuralNetwork::LAYER_HIDDEN:
                net->HiddenNeuron.append(neuron);
                break;
            case NeuralNetwork::LAYER_OUTPUT:
                net->OutputNeuron.append(neuron);
                break;
            }
            if(!e_subobj.isNull())
                neuron->initSubObjects(e_subobj);
        }
        n = n.nextSibling();
    }

}

void NetBuilder::buildSubNets(const QDomElement& e_list)
{

    bool ok;
    QString path;
    QString filename;
    uint num_id;
    QDomNode n = e_list.firstChild();
    SubNet* subnet = 0;

    while(!n.isNull()) {
        QDomElement e = n.toElement();
        if(e.isNull()) {
            n = n.nextSibling();
            continue;
        }
        QDomElement e_num_id = xml_operator->findTag("num_id", e);
        QDomElement e_path = xml_operator->findTag("path", e);
        QDomElement e_filename = xml_operator->findTag("filename", e);
        QDomElement e_subobj = xml_operator->findTag("SubObjects", e);

        if(e_path.isNull() || e_num_id.isNull() || e_filename.isNull()) {
            n = n.nextSibling();
            continue;
        }
        num_id = e_num_id.text().toUInt(&ok);
        path =  e_path.text();
        filename =  e_filename.text();


        subnet = factory->factorySubNet(num_id, path, filename);
        // construyo subobjetos

        if(subnet) {
            net->SubNets.append(subnet);
            QObject::connect(subnet, SIGNAL(changed()),
                             net, SIGNAL(netChanged()));
        }
        if(!e_subobj.isNull()) {
            subnet->initSubObjects(e_subobj);
        }

        n = n.nextSibling();
    }

}

void NetBuilder::buildWeights(const QDomElement& e_list)
{

    bool ok;
    uint i = 0;
    QDomNode n_weight = e_list.firstChild();
    uint n_weights = net->weightsMask.size();
    while( !n_weight.isNull() )
    {
        QDomElement e_weight = n_weight.toElement();
        if(e_weight.isNull()) {
            n_weight = n_weight.nextSibling();
            continue;
        }
        if(i < n_weights)
            net->weightsMask[i] =  e_weight.text().toDouble(&ok);
        else {
            net->weightsMask.resize(n_weights + 1);
            net->weightsMask[i] = e_weight.text().toDouble(&ok);
        }
        ++i;
        n_weight = n_weight.nextSibling();
    }

}

NeuralNetwork* NetBuilder::newNet(void)
{

    NeuralNetwork* net = 0;

    buildNetDialog *dialog = new buildNetDialog(0, 0, false);
    dialog->setNumInput(16);
    dialog->setNumHidden(4);
    dialog->setNumOutput(4);

    if ( dialog->exec() == buildNetDialog::Accepted ) {
        net = buildNewNet(dialog->getNumInput(), dialog->getNumHidden(),
                          dialog->getNumOutput());
        connetcNet2Doc();
        net->isChanged();
    }
    delete dialog;
    return net;
}

NeuralNetwork* NetBuilder::buildNewNet(int n_i, int n_h, int n_o)
{

    unsigned long i;
    Neuron* neuron = 0;
    net = new NeuralNetwork();
    ulong n_input = ulong(n_i);
    ulong n_hiddden = ulong(n_h);
    ulong n_output = ulong(n_o);
    ulong n_neurons = n_input + n_hiddden + n_output;
    if(n_neurons > 0) {
        net->num_InputN = n_input;
        net->num_HiddenN = n_hiddden;
        net->num_OutputN = n_output;
        net->NumberOfNeurons = n_neurons;
        net->NumberOfSynapses = 0;
        net->NumberOfSubNets = 0;
        net->NumberOfSubNetSynapses = 0;

        for(i = 0; i < n_neurons; i++) {
            if(i < n_input) {
                neuron = factory->factoryNeuron(Neuron::INPUT_NEURON,
                                                i + 1, NeuralNetwork::LAYER_INPUT);
                net->Input_Neuron.append(neuron);
            }
            else if(i >= n_input && i < n_input + n_hiddden) {
                neuron = factory->factoryNeuron(Neuron::THRESHOLD_DEVICE,
                                                i + 1, NeuralNetwork::LAYER_HIDDEN);
                net->HiddenNeuron.append(neuron);
            }
            else if(i >= n_input + n_hiddden && i < n_neurons) {
                neuron = factory->factoryNeuron(Neuron::THRESHOLD_DEVICE,
                                                i + 1, NeuralNetwork::LAYER_OUTPUT);
                net->OutputNeuron.append(neuron);
            }
            net->neurons().append(neuron);
            QObject::connect(neuron, SIGNAL(changed()), net, SIGNAL(netChanged()));
        }

        net->linkNumId();
        QDomDocument doc("neuralnet");
        QDomElement e_net = doc.createElement("net");
        doc.appendChild(e_net);
        netDoc = doc;
        net->setNetDoc(netDoc);

    }
    return net;

}

void NetBuilder::connectAxons(void)
{

    Synapse *synapse;
    Neuron *neuron_from, *neuron_to;
    QListIterator<Neuron *> itn(net->neurons());
    while(itn.hasNext()){
        neuron_from = itn.next();
        QList<Synapse *> Axon = neuron_from->getAxon();
        QListIterator<Synapse *> its(Axon);
        while(its.hasNext()){
            synapse = its.next();
            synapse->setPointerFrom(neuron_from);
            net->Axons.append(synapse);
            QObject::connect(synapse, SIGNAL(changed()),
                             net, SIGNAL(netChanged()));
            QObject::connect(synapse, SIGNAL(weightsDefChange(int, double )),
                             net, SIGNAL(weightsDefChange(int, double)));
            if(synapse->getTo() > 0){
                neuron_to = net->neurons().at(synapse->getTo() - 1);
                synapse->setPointerTo(neuron_to);
                QObject::connect(neuron_to, SIGNAL(num_idChanged(ulong)),
                                 synapse, SLOT(setTo(ulong)));
            }
        }
    }

    SubNetSynapse *sn_synapse;
    SubNet *subnet, *subnet_from, *subnet_to;

    QListIterator<SubNet *> itsn(net->subnets());
    while(itsn.hasNext()){
        subnet = itsn.next();
        QList<SubNetSynapse *> snAxon = subnet->getAxon();
        QListIterator<SubNetSynapse *> its(snAxon);
        while(its.hasNext()){
            sn_synapse = its.next();
            if (sn_synapse->getIdSubNetFrom()>0){
                // el origen es una subred
                subnet_from = net->subnets().at(sn_synapse->getIdSubNetFrom()-1);
                sn_synapse->setPointerFromSubNet(subnet_from);
                sn_synapse->set_neuronFromId (sn_synapse->getIdNeuronFrom( ));
            }
            else {
                // el origen es una neurona
                neuron_from = net->neurons().at(sn_synapse->getIdNeuronFrom()-1);
                sn_synapse->setPointerFrom(neuron_from);
            }

            if (sn_synapse->getIdSubNetTo()>0){
                // el destino es una subred
                subnet_to = net->subnets().at(sn_synapse->getIdSubNetTo()-1);
                sn_synapse->setPointerToSubNet(subnet_to);

                sn_synapse->set_neuronToId (sn_synapse->getIdNeuronTo());
            }
            else {
                // el destino es una neurona
                neuron_to = net->neurons().at(sn_synapse->getIdNeuronTo()-1);
                sn_synapse->setPointerTo(neuron_to);
            }
            QObject::connect(sn_synapse, SIGNAL(changed()),
                             net, SIGNAL(netChanged()));
            QObject::connect(sn_synapse, SIGNAL(weightsDefChange(int, double )),
                             net, SIGNAL(weightsDefChange(int, double)));
        }
    }
}


void NetBuilder::setView(NeuralDisView *(&view))
{

    Settings* prog_settings = Settings::instance();
    NeuralDisDoc* doc = NeuralDisDoc::instance();

    if(!view) {

        QGraphicsScene* mscene = new QGraphicsScene();
        prog_settings->getCanvasList()->append(mscene);

        mscene->setSceneRect(0,0,1.25*PAINTERWIDTH,int(1.25*PAINTERHEIGHT));
        view = new NeuralDisView(mscene, 0, net);
        view->resize(1000,1000);
        view->show();

        QMdiSubWindow* subw =  prog_settings->getWorkspace()->addSubWindow(view);
        view->setMdiSubWindow(subw);

        QObject::connect(net, SIGNAL(neuronAddSim(Neuron *)),
                         view, SLOT(neuronConnectSim(Neuron *)));
        view->deactivateAllNeurons();
        QObject::connect(doc, SIGNAL(documentChanged(const QString&)),
                         view, SLOT(slotDocumentChanged(const QString&)));
        view->slotDocumentChanged(doc->getXMLNet().toString());

    }
    else {
        view->initNet();
    }

    prog_settings->setNetView(view);
    view->setWindowTitle(view->windowTitle().section(":", 0, 0) + ": " + doc->getFileName().section(QDir::toNativeSeparators("/"), -1));

    view->getMdiSubWindow()->show();

}

void NetBuilder::connetcNet2Doc(void)
{
    NeuralDisDoc* doc = NeuralDisDoc::instance();
    QObject::connect(net, SIGNAL(netChanged(QString)),
                     doc, SLOT(textoChange(QString)));
    QObject::connect(net, SIGNAL(netClosed(void)),
                     doc, SLOT(netClosed()));
    QObject::connect(doc, SIGNAL(documentName(QString)),
                     net, SLOT(setName(QString)));
    QObject::connect(net, SIGNAL(netChanged(void)),
                     net, SLOT(isChanged(void)));
}
