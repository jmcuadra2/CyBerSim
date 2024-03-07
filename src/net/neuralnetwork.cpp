/***************************************************************************
                          neuralnetwork.cpp  -  description
                             -------------------
    begin                : Tue Oct 15 2002
    copyright            : (C) 2002 by Jose M. Cuadra Troncoso
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

#include <iostream>
#include <fstream>

#include <QList>

using namespace std;

#include <QMessageBox>
#include <QApplication>

#include "neuralnetwork.h"
#include "netfactory.h"

#include "../neuraldis/neuraldisdoc.h"
#include "netbuilder.h"
#include "../neuraldis/settings.h"
#include "../disview/neuraldisview.h"
#include "../neuraldis/xmloperationslog.h"

static double weightsM[32] =
{   1,  -1,   0, 0.9,  0.5,  5,  -0.2,    -0.3,
    1.2, -0.8, 0.55, 1.1, 0.45, 10,   -3,    -5,
    0.3,-0.5, 0.2,  0.4,  0.6, 0.7,  0.8,  -0.7,
    3,   5, -10, -0.4,  9.1,-9.1,    2,  -1.5};

NeuralNetwork::NeuralNetwork(void)
{

    factory = NetFactory::instance();
    NumberOfSynapses = 0;
    NumberOfNeurons = 0;
    NumberOfSubNets = 0;
    NumberOfSubNetSynapses = 0;
    num_InputN = 0;
    num_HiddenN = 0;
    num_OutputN = 0;
    fromSelNeuron = 0;
    toSelNeuron = 0;
    // nuevo para seleccionar subredes
    fromSelSubNet = 0;
    toSelSubNet = 0;
    selectedLayer = -1;
    undoLog = XMLOperationsLog::instance();
    ///////////////////////////////////////////////////////////////
    can_draw_pixmaps = true;
    neuronViewFactory = NeuronViewFactory::instance();
    if(!neuronViewFactory->readFileConf())
    {
        can_draw_pixmaps = false;
    }


    // nueva instancia para subredes
    subnetViewFactory = SubNetViewFactory::instance();
    if(!subnetViewFactory->readFileConf())
    {
        can_draw_pixmaps = false;
    }


    //D//   Synapse::weightsMask.duplicate(weightsM, 32);
    Synapse::weightsMask.clear();
    for(int i = 0; i < 32 ; i++){
        Synapse::weightsMask.append(weightsM[i]);
    }
    weightsMask = Synapse::weightsMask;
    // nueva variable
    NumberOfSubNets = 0;

}

NeuralNetwork::~NeuralNetwork()
{

    clearNet(false);
    emit netClosed();

}


void NeuralNetwork::setName(QString n)
{

    _name = n;
    return;

}

QString NeuralNetwork::name(void)
{

    return _name;
}

QList<Neuron*>& NeuralNetwork::neurons(void)
{

    return Neurons;

}
// nuevo método para devolver la lista de vistas de red

QList<SubNet *>& NeuralNetwork::subnets(void)
{

    return SubNets;

}


void NeuralNetwork::clearNet(bool save_)
{

    QListIterator<Neuron *> itn(Neurons);
    clearSelection();
    while(itn.hasNext()) {
        delete itn.next();
    }
    Input_Neuron.clear();
    HiddenNeuron.clear();
    OutputNeuron.clear();
    Neurons.clear();
    // borro la lista de vistas de red
    SubNets.clear();
    QListIterator<Synapse *> its(Axons);
    while(its.hasNext()) {
        delete its.next();
    }
    Axons.clear();

    num_InputN = 0;
    num_HiddenN = 0;
    num_OutputN = 0;
    NumberOfSynapses = 0;
    NumberOfNeurons = 0;
    NumberOfSubNets = 0;
    NumberOfSubNetSynapses = 0;
    undoLog->clear();
    if(save_)
        emit netChanged();

}

QString NeuralNetwork::saveNet(void)
{

    XMLOperator* xml_operator = XMLOperator::instance();
    QDomElement e = netDoc.firstChild().toElement();
    xml_operator->removeChildren(e);
    e.appendChild(xml_operator->createTextElement(netDoc,
                                                  "number_input", QString::number(num_InputN)));
    e.appendChild(xml_operator->createTextElement(netDoc,
                                                  "number_hidden", QString::number(num_HiddenN)));
    e.appendChild(xml_operator->createTextElement(netDoc,
                                                  "number_output", QString::number(num_OutputN)));
    e.appendChild(xml_operator->createTextElement(netDoc,
                                                  "number_synapses", QString::number(NumberOfSynapses)));
    // añado otra etiqueta para saber el número de subredes
    if(NumberOfSubNets>0) {
        e.appendChild(xml_operator->createTextElement(netDoc,
                                                      "number_subnets", QString::number(NumberOfSubNets)));
    }
    // añado otra etiqueta para saber las sinapsis de susbredes
    if(NumberOfSubNetSynapses>0) {
        e.appendChild(xml_operator->createTextElement(netDoc,
                                                      "number_subnetsynapses", QString::number(NumberOfSubNetSynapses)));
    }

    QDomElement tag_neuron_l = netDoc.createElement( "Neurons_list" );
    e.appendChild(tag_neuron_l);
    Neuron *neuron;

    QListIterator<Neuron *> iti(Neurons);
    while (iti.hasNext()) {
        neuron = iti.next();
        neuron->write(netDoc, tag_neuron_l);
    }

    // aquí creo la lista de subredes si existen
    if(NumberOfSubNets>0) {
        QDomElement tag_subnet_l = netDoc.createElement( "Subnets_list" );
        e.appendChild(tag_subnet_l);
        SubNet *subnet;
        QListIterator<SubNet *> its(SubNets);
        while (its.hasNext()) {
            subnet = its.next();
            subnet->write(netDoc, tag_subnet_l);
        }
    }

    uint n_weights = weightsMask.size();
    QDomElement tag_weight_l = netDoc.createElement( "Weights_list" );
    e.appendChild(tag_weight_l);
    for(uint  i = 0; i < n_weights; ++i) {
        tag_weight_l.appendChild(xml_operator->createTextElement(netDoc,
                                                                 "weight", QString::number(weightsMask[i])));
    }

    // aquí deberá guardar toda la información relativa a subredes
    // recorriendo su lista está pendiente
    QString content = netDoc.toString(4);
    return content;

}

void NeuralNetwork::addNeuron()
{

    Neuron *neuron;
    clearSelection();

    addNeuronDialog *dialog =
            new addNeuronDialog(0, 0, false);
    dialog->setMinMaxValues(num_InputN, num_HiddenN, num_OutputN);
    dialog->setLayer(LAYER_HIDDEN);
    dialog->setDevice(Neuron::THRESHOLD_DEVICE);
    if( dialog->exec() == addNeuronDialog::Accepted ) {
        neuron = addNeuron(dialog->getNum_IdNeuron(),
                           dialog->getLayer(), dialog->getDevice());
        if(neuron) {
            neuron->setSelected(true);
            if(fromSelNeuron)
                fromSelNeuron->setSelected(false);
            fromSelNeuron = neuron;

        }
    }
    delete dialog;
}

Neuron* NeuralNetwork::addNeuron(ulong n_i, int layer, int type, bool edit, bool write_undo)
{

    Neuron *neuron, *prev_neuron, *post_neuron;
    prev_neuron = 0;
    post_neuron = 0;

    // si se trata de una neurona de entrada le cambio el tipo antes de crearla para que salga bien representada como neurona de entrada
    if(layer==0)  type=2;

    neuron = factory->factoryNeuron(type, n_i, layer);
    
    if(edit){
        if(!neuron->edit(true, true)) {
            delete neuron;
            neuron = 0;
            return neuron;
        }
    }

    Neurons.insert(n_i - 1, neuron);

    if(n_i > 1) {
        prev_neuron = Neurons.at(n_i - 2);
    }

    if(n_i < NumberOfNeurons) {
        post_neuron =  Neurons.at(n_i);
    }

    ++NumberOfNeurons;
    
    switch(layer) {
    case LAYER_INPUT:
        Input_Neuron.insert(n_i - 1, neuron);
        num_InputN++;
        break;
    case LAYER_HIDDEN:
        HiddenNeuron.insert(n_i - num_InputN - 1,neuron);
        num_HiddenN++;
        break;
    case LAYER_OUTPUT:
        OutputNeuron.insert(n_i - num_InputN - num_HiddenN - 1, neuron);
        num_OutputN++;
        break;
    }

    // es necesario asignar el pixmap correspondiente a la nueva neurona
    // para evitar SEGFAULTs a la hora de representar

    neuronViewFactory->assignNeuronPixmap(neuron);

    if(prev_neuron && post_neuron)
        disconnect(prev_neuron, 0, post_neuron, 0);

    if(prev_neuron)
        connect(prev_neuron, SIGNAL(num_idChanged(ulong)),
                neuron, SLOT(num_idChange(ulong)));

    if(post_neuron) {
        connect(neuron, SIGNAL(num_idChanged(ulong)),
                post_neuron, SLOT(num_idChange(ulong)));
        post_neuron->num_idChange(n_i);
    }

    connect(neuron, SIGNAL(changed()), this, SIGNAL(netChanged()));
    emit neuronAddSim(neuron);
    emit netChanged();

    if(write_undo) {
        undoLog->insertComment ("DEBUG:: add neuron :: num_id="+QString::number(neuron->getNumId()));
        undoLog->insertLogOperation (neuron, "add_neuron");
    }

    return neuron;
}


void NeuralNetwork::delNeuron()
{

    if(NumberOfNeurons < 1) {

        return;
    }
    clearSelection();
    delNeuronDialog *dialog = new delNeuronDialog(0, 0, false);

    dialog->setMinMaxValues(num_InputN, num_HiddenN, num_OutputN);
    if(num_HiddenN>0)
        dialog->setLayer(LAYER_HIDDEN);
    else if(num_InputN>0)
        dialog->setLayer(LAYER_INPUT);
    else if(num_OutputN>0)
        dialog->setLayer(LAYER_OUTPUT);

    if( dialog->exec() == delNeuronDialog::Accepted )
        delNeuron(dialog->getNum_IdNeuron());

    delete dialog;

}

void NeuralNetwork::delNeuron(ulong n_i)
{

    Neuron *neuron, *prev_neuron, *post_neuron;           //, *gp_neuron;
    prev_neuron = 0;
    post_neuron = 0;
    Synapse *synapse;
    int layer;

    neuron = Neurons.at(n_i - 1);

    // se almacena la operacion para undo/redo
    // se abre un grupo ya que puede haber operaciones recursivas
    // se comprueba primero si ya hay un grupo activo
    // (abierto por una operacion undo/redo)
    bool open_a_group = false;
    if(!undoLog->activeGroup()){
        open_a_group = true;
        undoLog->openGroup();
    }

    layer = neuron->getLayer();

    QList<Synapse*> Axon = neuron->getAxon();
    for(int i = 0; i < Axon.size(); i++) {
        synapse = Axon.at(i);
        delSynapse(synapse->getNumId());
        // aqui si(yes) avanza al contador porque
        // delSynapse() hace remove() en una referencia a Axon que no es esta
    }

    for(int i = 0; i < Axons.size(); i++) {
        synapse = Axons.at(i);
        if(synapse->getPointerTo()->getNumId() == neuron->getNumId()) {
            delSynapse(synapse->getNumId());
            i--; //    aqui no avanza al contador
        }
    }

    undoLog->insertLogOperation (neuron, "del_neuron");

    // se cierra el grupo en el historico
    if(open_a_group){
        undoLog->closeGroup();
    }

    if(fromSelNeuron == neuron)
        fromSelNeuron = 0;
    delete neuron;

    Neurons.removeAt(n_i - 1);
    //DX//   Neurons.removeAt(n_i - 1);

    NumberOfNeurons--;
    if(n_i > 1)
        prev_neuron =  Neurons.at(n_i - 2);
    if(n_i <= NumberOfNeurons )
        post_neuron = Neurons.at(n_i - 1);

    switch(layer) {
    case LAYER_INPUT:
        Input_Neuron.removeAt(n_i - 1);
        num_InputN--;
        break;
    case LAYER_HIDDEN:
        HiddenNeuron.removeAt(n_i - num_InputN - 1);
        num_HiddenN--;
        break;
    case LAYER_OUTPUT:
        OutputNeuron.removeAt(n_i - num_InputN - num_HiddenN - 1);
        num_OutputN--;
        break;
    }
    if(prev_neuron && post_neuron)
        connect(prev_neuron, SIGNAL(num_idChanged(ulong)),
                post_neuron, SLOT(num_idChange(ulong)));
    if(post_neuron)
        post_neuron->num_idChange(n_i - 1);

    undoLog->insertComment ("DEBUG::fin");

    emit netChanged();

}


void NeuralNetwork::editNeuron()
{

    Neuron *neuron = 0;
    ulong n_i;
    int layer;

    if(NumberOfNeurons < 1) {
        QMessageBox::information(nullptr,tr("editNeuron"),
                                 tr("Empty net.\nUse Build neural network first."), QMessageBox::Ok);
        return;
    }
    clearSelection();
    editNeuronDialog *dialog =
            new editNeuronDialog(0, 0, false);

    dialog->setMinMaxValues(num_InputN, num_HiddenN, num_OutputN);
    if(num_HiddenN>0)
        dialog->setLayer(LAYER_HIDDEN);
    else if(num_InputN>0)
        dialog->setLayer(LAYER_INPUT);
    else if(num_OutputN>0)
        dialog->setLayer(LAYER_OUTPUT);
    if( dialog->exec() == editNeuronDialog::Accepted ) {
        n_i = dialog->getNum_IdNeuron();
        layer = dialog->getLayer();

        switch(layer) {
        case LAYER_INPUT:
            neuron = Input_Neuron.at(n_i - 1);
            break;

        case LAYER_HIDDEN:
            neuron = HiddenNeuron.at(n_i - num_InputN - 1);
            break;
        case LAYER_OUTPUT:
            neuron = OutputNeuron.at(n_i - num_InputN - num_HiddenN - 1);
            break;
        }

        if(neuron) {
            neuron->setSelected(true);
            if(fromSelNeuron)
                fromSelNeuron->setSelected(false);
            fromSelNeuron = neuron;
            undoLog->insertComment ("DEBUG:: edit neuron :: num_id="+QString::number(neuron->getNumId()));
            undoLog->insertLogOperation (neuron, "edit_neuron");
            if(!neuron->edit())
                undoLog->removeLastEditOperation();
        }
    }
    //undoLog->insertLogOperation (neuron, "edit_neuron");
    delete dialog;

}

//
// addSynapse()
//
void NeuralNetwork::addSynapse()
{

    Neuron *neuron_from, *neuron_to;
    // nuevo
    SubNet *subnet_from=0, *subnet_to=0;
    int sn_from, sn_to;

    if(NumberOfNeurons < 1) {
        QMessageBox::information(nullptr,tr("addSynapse"),
                                 tr("Empty net.\nUse Build neural network first."), QMessageBox::Ok);
        return;
    }

    clearSelection();
    //addSynapseDialog *dialog = new addSynapseDialog(0, 0, false, 0);
    // nuevo cuadro de dialogo para añadir sinapsis con vistas de red
    addSynapseNetDialog *dialog = new addSynapseNetDialog(0, 0, false);
    dialog->setCurrentMinMaxValues (num_InputN, num_HiddenN, num_OutputN);
    dialog->setSubNets(&SubNets);

    if( dialog->exec() == addSynapseNetDialog::Accepted ) {
        if(dialog->isCurrentNetworkFrom() && dialog->isCurrentNetworkTo()) {
            // synapse dentro de la red principal
            neuron_from = neuronPointer(dialog->getNum_IdNeuronFrom());
            neuron_to = neuronPointer(dialog->getNum_IdNeuronTo());

            if(neuron_from) {
                neuron_from->setSelected(true);
                if(fromSelNeuron)
                    fromSelNeuron->setSelected(false);
                fromSelNeuron = neuron_from;
            }
            if(neuron_to) {
                neuron_to->setSelected(true);
                if(toSelNeuron)
                    toSelNeuron->setSelected(false);
                toSelNeuron = neuron_to;
            }

            addSynapse( dialog->getNum_IdNeuronFrom(), dialog->getNum_IdNeuronTo(), dialog->getType());
        }
        else {
            // synapse hacia o desde una subnet

            // para subredes
            sn_from = dialog->getNum_IdNetFrom();
            if(dialog->isCurrentNetworkFrom())
                sn_from = 0;

            if(sn_from >0)
                subnet_from = subNetPointer(sn_from);

            sn_to = dialog->getNum_IdNetTo();
            if(dialog->isCurrentNetworkTo())
                sn_to = 0;
            
            if(sn_to >0)
                subnet_to = subNetPointer(sn_to);

            if(subnet_from) {
                subnet_from->setSelected(true);

                if(fromSelSubNet)
                    fromSelSubNet->setSelected(false);
                fromSelSubNet = subnet_from;
            }

            if(subnet_to) {
                subnet_to->setSelected(true);

                if(toSelSubNet)
                    toSelSubNet->setSelected(false);
                toSelSubNet = subnet_to;
            }

            neuron_from = neuronPointer(dialog->getNum_IdNeuronFrom());
            neuron_to = neuronPointer(dialog->getNum_IdNeuronTo());

            if(neuron_from && dialog->isCurrentNetworkFrom()) {
                neuron_from->setSelected(true);
                if(fromSelNeuron)
                    fromSelNeuron->setSelected(false);
                fromSelNeuron = neuron_from;
            }

            if(neuron_to && dialog->isCurrentNetworkTo()) {
                neuron_to->setSelected(true);
                if(toSelNeuron)
                    toSelNeuron->setSelected(false);
                toSelNeuron = neuron_to;
            }

            addSynapse(sn_from,sn_to,dialog->getNum_IdNeuronFrom(),dialog->getNum_IdNeuronTo(), false);
        }
    }

    delete dialog;
}


//
// addSynapse()
//
void NeuralNetwork::addSynapse(ulong n_from, ulong n_to, int type, bool edit,
                               bool write_undo)
{
    Neuron *neuron_from = 0, *neuron_to = 0, *neuron;
    Synapse *synapse = 0;
    currentSynapse = 0;
    bool  exist_synapse = false;
    ulong insert_pos = 0;
    int i;
    int last;
    neuron_from = Neurons.at(n_from - 1);

    QListIterator<Neuron *> itn(Neurons);
    QList<Synapse *> & Axon = neuron_from->getAxon();

    for(uint i = 0; i < n_from; i++)
        itn.next();

    while(itn.hasPrevious()) {
        neuron = itn.previous();
        QList<Synapse *> & n_axon = neuron->getAxon();
        if((last = n_axon.size())) {
            insert_pos = n_axon.at(last - 1)->getNumId();
            break;
        }
    }

    itn.toFront();
    for(uint i = 0; i < n_from; i++){
        itn.next();
    }
    while(itn.hasPrevious()) {
        neuron = itn.previous();
        neuron_to = Neurons.at(n_to - 1);
        QListIterator<Synapse *> its(Axon);
        while((its.hasNext()) && !exist_synapse) {
            synapse = its.next();
            if(synapse->getTo() == neuron_to->getNumId()) {
                exist_synapse = true;
                QMessageBox::information(nullptr,tr("addSynapse"),
                                         QString(tr("Synapse from         neuron: %1 to neuron: %2\n\t already exists.")).
                                         arg(QString::number(neuron_from->getNumId())).
                                         arg(QString::number(neuron_to->getNumId())),
                                          QMessageBox::Ok);
            }
        }
    }

    if(!exist_synapse) {
        synapse = factory->factorySynapse(type, insert_pos + 1);
        if(!synapse->setPointerFrom(neuron_from)) {
            QMessageBox::warning(nullptr,tr("addSynapse"),
                                 tr("Output neuron invalid."), QMessageBox::Ok);
            delete synapse;
            return;
        }
        if(!synapse->setPointerTo(neuron_to)) {
            QMessageBox::warning(nullptr,tr("addSynapse"),
                                 tr("Input neuron invalid."), QMessageBox::Ok);
            delete synapse;
            return;
        }
        Axons.insert(insert_pos, synapse);
        NumberOfSynapses++;

        Axon.append(synapse);

        connect(neuron_to, SIGNAL(num_idChanged(ulong)),
                synapse, SLOT(setTo(ulong)));
        connect(synapse, SIGNAL(changed()),
                this, SIGNAL(netChanged()));
        connect(synapse, SIGNAL(weightsDefChange(int, double )),
                this, SIGNAL(weightsDefChange(int, double)));

        QListIterator<Synapse *> it_n(Axons);
        synapse->nonEditableWeights(it_n);
        for(i=insert_pos + 1; i < int(NumberOfSynapses); i++)
            Axons.at(i)->setNumId(Axons.at(i)->getNumId() + 1);

        // vemos si hay que mostrar el dialogo de edicion
        if(edit) {
            if(!synapse->edit(true, true)) {
                delSynapse(synapse->getNumId());
                return;
            }
        }

        // se ha creado la nueva synapse
        currentSynapse = synapse;

        // se almacena la operacion para undo/redo
        if(write_undo) {
            undoLog->insertComment ("DEBUG:: add synapse :: num_id="+QString::number(currentSynapse->getNumId()));
            undoLog->insertLogOperation (synapse, "add_synapse");
        }

        emit netChanged();

    }

}

//
// nuevo addSynapse para conectar subredes y neuronas
//
void NeuralNetwork::addSynapse(ulong id_fromsubnet,ulong id_tosubnet, ulong id_from, ulong id_to, int /*type*/)
{

    Neuron *neuron_from = 0, *neuron_to = 0/*, *neuron*/;
    SubNet *subnet_from = 0, *subnet_to = 0/*, *subnet*/;
    SubNetSynapse *synapse = 0;
    // esto es nuevo para construir los axones
    subnet_from = SubNets.at(id_fromsubnet - 1);

    // 1.- From
    if(id_fromsubnet > 0) {
        // desde una subnet
        subnet_from = SubNets.at(id_fromsubnet - 1);

        // 2.- To
        if(id_tosubnet>0) {
            // hacia una subnet
            subnet_to = SubNets.at(id_tosubnet - 1);
            if(subnet_from && subnet_to)
                synapse = new SubNetSynapse (1, subnet_from, id_from, subnet_to, id_to);
        }
        else {
            // hacia una neurona de la red principal
            neuron_to = Neurons.at(id_to - 1);
            if(subnet_from && neuron_to)
                synapse = new SubNetSynapse (1, subnet_from, id_from, neuron_to);
        }

        if(synapse)
            subnet_from->addSynapse(synapse);

    }
    else {
        //desde una neurona
        neuron_from = Neurons.at(id_from - 1);
        // hacia una subnet
        subnet_to = SubNets.at(id_tosubnet - 1);

        if(subnet_to && neuron_from)
            synapse = new SubNetSynapse (1, neuron_from, subnet_to, id_to);

        if(synapse)
            subnet_to->addSynapse(synapse);

    }

    // nuevo para intentar crear la sinapsis
    // incremento número de sinapsis de subred
    NumberOfSubNetSynapses++;

    if(id_tosubnet==0){
        connect(neuron_to, SIGNAL(num_idChanged(ulong)),
                synapse, SLOT(setTo(ulong)));
        connect(synapse, SIGNAL(changed()),
                this, SIGNAL(netChanged()));
    }
    else {
        connect(subnet_to, SIGNAL(num_idChanged(ulong)),
                synapse, SLOT(setTo(ulong)));
        connect(synapse, SIGNAL(changed()),
                this, SIGNAL(netChanged()));
    }

    // se almacena la operacion para undo/redo
    // undoLog->insertComment ("DEBUG:: add synapse 111111111111");
    //  undoLog->insertLogOperation (synapse, "add_synapse",n_from);
    // en el log tengo que poner bien los valores si se trata se sinapsis entre subredes y neuronas
    emit netChanged();

} 

void NeuralNetwork::delSynapse()
{

    if(NumberOfNeurons < 1) {
        QMessageBox::information(nullptr,tr("delSynapse"),
                                 tr("Empty net.\nUse Build neural network first."), QMessageBox::Ok);
        return;
    }
    if(NumberOfSynapses < 1 && NumberOfSubNetSynapses < 1) {
        QMessageBox::information(nullptr,tr("del Synapse"),
                                 tr("No synapses in net.\nUse Add a New Synapse first."), QMessageBox::Ok);
        return;
    }

    clearSelection();
    /* nuevo cuadro de dialogo */

    selectSynapseNetDialog *dialog = new selectSynapseNetDialog(0, 0, false);
    dialog->setMinMaxValues(num_InputN, num_HiddenN, num_OutputN,
                            Input_Neuron, HiddenNeuron, OutputNeuron);
    dialog->setCurrentMinMaxValues (num_InputN, num_HiddenN, num_OutputN);
    dialog->setSubNets(&SubNets);

    if( dialog->exec() == selectSynapseNetDialog::Accepted )
        delSynapse(dialog->getNum_IdSynapse());

    delete dialog;
}

void NeuralNetwork::delSynapse(ulong n_s)
{
    Synapse *synapse;
    Neuron *neuron_from  = 0;

    synapse = Axons.at(n_s - 1);
    if(!synapse)
        return;
    
    neuron_from = synapse->getPointerFrom();
    QList<Synapse *> & Axon = neuron_from->getAxon();

    // se almacena la operacion para undo/redo
    undoLog->insertLogOperation (synapse, "del_synapse");

    // solo debug
    Synapse *syntemp;
    QString cadtemp("DEBUG: axones : ");
    QListIterator<Synapse *> its(Axon);
    while(its.hasNext()) {
        syntemp = its.next();
        cadtemp+="["+QString::number(Axon.indexOf(syntemp))+"]" + QString::number(syntemp->getTo());
    }

    Axon.removeOne(synapse);

    // descomentar cuando proceda
    //synapse->clean();
    delete synapse;
    Axons.removeAt(n_s - 1);
    //DX//   Axons.removeAt(n_s - 1);
    NumberOfSynapses--;
    for(ulong i = n_s - 1; i < NumberOfSynapses; i++)
        Axons.at(i)->setNumId(Axons.at(i)->getNumId() - 1);
    emit netChanged();

}

void NeuralNetwork::editSynapse(void)
{

    Neuron *neuron_from, *neuron_to;
    // utilizo prev_synapse para guardar lo que ya existía antes
    Synapse *synapse;
    QDomElement e_construct;

    if(NumberOfNeurons < 1) {
        QMessageBox::information(nullptr,tr("editSynapse"),
                                 tr("Empty net.\nUse Build neural network first."), QMessageBox::Ok);
        return;
    }
    if(NumberOfSynapses < 1 && NumberOfSubNetSynapses < 1 ) {
        QMessageBox::information(nullptr,tr("editSynapse"),
                                 tr("No synapses in net.\nUse Add a New Synapse first."), QMessageBox::Ok);
        return;
    }

    clearSelection();
    
    selectSynapseNetDialog *dialog = new selectSynapseNetDialog(0, 0, false);
    dialog->setMinMaxValues(num_InputN, num_HiddenN, num_OutputN,
                            Input_Neuron, HiddenNeuron, OutputNeuron);
    dialog->setCurrentMinMaxValues (num_InputN, num_HiddenN, num_OutputN);
    dialog->setSubNets(&SubNets);
    dialog->setWindowTitle(tr("Edit Synapse"));

    if( dialog->exec() == selectSynapseNetDialog::Accepted ) {
        synapse = Axons.at(dialog->getNum_IdSynapse() - 1);

        neuron_from = synapse->getPointerFrom();
        neuron_to = synapse->getPointerTo();
        if(neuron_from) {
            neuron_from->setSelected(true);
            if(fromSelNeuron)
                fromSelNeuron->setSelected(false);
            fromSelNeuron = neuron_from;
        }
        if(neuron_to) {
            neuron_to->setSelected(true);
            if(toSelNeuron)
                toSelNeuron->setSelected(false);
            toSelNeuron = neuron_to;
        }
        undoLog->insertComment("DEBUG:: edit synapse :: num_id="+QString::number(synapse->getNumId()));
        undoLog->insertLogOperation(synapse, "edit_synapse");

        if(synapse->edit()) {
            nonEditableWeights();
            // undoLog->insertLogOperation (synapse_tmp, "edit_synapse",neuron_from->getNumId());
            emit netChanged();
        }
        else
            undoLog->removeLastEditOperation();
    }

    delete dialog;
}



Synapse* NeuralNetwork::findSynapse(ulong n_from, ulong n_to)
{

    Neuron *neuron;
    Synapse *synapse = 0;

    neuron = Neurons.at(n_from - 1);
    if(neuron) {
        QList<Synapse *> Axon = neuron->getAxon();
        QListIterator<Synapse *> its(Axon);
        while(its.hasNext()){
            synapse = its.next();
            if(synapse->getTo() == n_to)
                its.toBack();
            else
                synapse = 0;
        }
    }
    return synapse;

}

void NeuralNetwork::nonEditableWeights(void)
{
    Synapse *synapse;
    QListIterator<Synapse *> it(Axons);
    QListIterator<Synapse *> it_n(Axons);
    while(it.hasNext()) {
        synapse = it.next();
        it_n.toFront();
        synapse->nonEditableWeights(it_n);
    }
    emit netChanged();
}

Neuron* NeuralNetwork::neuronPointer(ulong n_neuron)
{

    Neuron *ret = 0;
    if(n_neuron >= 1 && n_neuron <= NumberOfNeurons)
        ret = Neurons.at(n_neuron - 1);
    return ret;
}
// nuevo

SubNet* NeuralNetwork::subNetPointer(ulong n_subnet)
{
    SubNet *subnet = 0;
    if(n_subnet >= 1 && n_subnet <= NumberOfSubNets)
        subnet = SubNets.at(n_subnet-1);
    return subnet;
}
// fin nuevo

int NeuralNetwork::getLayer(void)
{
    return selectedLayer;
}
void NeuralNetwork::setLayer(int l)
{
    selectedLayer = l;
}

int NeuralNetwork::getLayerTo(void)
{
    return selectedLayerTo;
}
void NeuralNetwork::setLayerTo(int l)
{
    selectedLayerTo = l;
}

void NeuralNetwork::clearSelection(void)
{

    if(fromSelNeuron) {
        fromSelNeuron->setSelected(false);
        fromSelNeuron = 0;
    }
    if(toSelNeuron) {
        toSelNeuron->setSelected(false);
        toSelNeuron = 0;
    }

    // nuevo para limpiar de subredes
    if(fromSelSubNet) {
        fromSelSubNet->setSelected(false);
        fromSelSubNet = 0;
    }
    if(toSelSubNet) {
        toSelSubNet->setSelected(false);
        toSelSubNet = 0;
    }
    // para deseleccionar todas las neuronas
    // (no solo la de origen y destino)
    Neuron *neuron;
    QListIterator<Neuron *> itn(Neurons);
    while(itn.hasNext()) {
        neuron = itn.next();
        neuron->setSelected(false);
    }

    // para deseleccionar todas las subredes
    // (no solo la de origen y destino)
    SubNet *subnet;
    QListIterator<SubNet *> its(SubNets);
    while(its.hasNext()) {
        subnet = its.next();
        subnet->setSelected(false);
    }

    setLayer(-1);
}

void NeuralNetwork::setSynapseColors(QStringList syn_col)
{
    Synapse::synapse_colors = syn_col;
}

void NeuralNetwork::linkNumId(void)
{

    Neuron *neuron, *prev_neuron = 0;
    QListIterator<Neuron *> it(Neurons);
    while (it.hasNext()) {
        neuron = it.next();
        if(prev_neuron)
            connect(prev_neuron, SIGNAL(num_idChanged(ulong)),
                    neuron, SLOT(num_idChange(ulong)));
        prev_neuron = neuron;
    }

}

void NeuralNetwork::isChanged(void)
{
    emit netChanged(saveNet());
}


void NeuralNetwork::addNetwork(const QString& subnet_name)
{
    addNetworkDialog  *dialog =
            new addNetworkDialog(0, 0, false);
    if( dialog->exec() == addNetworkDialog::Accepted )
        addNetwork(dialog->getNum_IdNetwork(),subnet_name);

    delete dialog;
}

SubNet* NeuralNetwork::addNetwork(ulong id_network, const QString& subnet_name, bool /*edit*/ )
{
    SubNet *subnet, *prev_subnet, *post_subnet;
    prev_subnet = 0;
    post_subnet = 0;;
    int i;

    i = subnet_name.lastIndexOf( '/' );

    //es windows el SO
    if(i == -1){
        i = subnet_name.lastIndexOf( '\\' );
    }

    QString path = subnet_name.left(i+1);
    QString filename =subnet_name.right(subnet_name.length()-(i+1));

    subnet = factory->factorySubNet(id_network,path, filename);
    // se añade a la lista de subredes de la red actual
    SubNets.insert(id_network - 1, subnet);

    if(id_network > 1)
        prev_subnet = SubNets.at(id_network - 2);

    if(id_network <= NumberOfSubNets)
        post_subnet =  SubNets.at(id_network);

    ++NumberOfSubNets;

    subnetViewFactory->assignSubNetPixmap(subnet);

    if(prev_subnet && post_subnet)
        disconnect(prev_subnet, 0, post_subnet, 0);

    if(prev_subnet)
        connect(prev_subnet, SIGNAL(num_idChanged(ulong)), subnet, SLOT(num_idChange(ulong)));

    // en subnet falla el post_subnet
    if(post_subnet) {
        connect(subnet, SIGNAL(num_idChanged(ulong)),
                post_subnet, SLOT(num_idChange(ulong)));
        post_subnet->num_idChange(id_network);

    }

    connect(subnet, SIGNAL(changed()), this, SIGNAL(netChanged()));
    emit netChanged();

    // historico de operaciones para undo/redo
    undoLog->insertComment ("DEBUG:: add subnet :: num_id="+QString::number(subnet->getNumId()));
    undoLog->insertLogOperation (subnet, "add_subnet");

    return subnet;
}

QString NeuralNetwork::editNetwork(void)
{

    ulong n_i;
    SubNet *subnet = 0;
    QString filename;
    QString path;

    if(NumberOfSubNets < 1) {
        QMessageBox::information(nullptr,tr("editSubNet"),
                                 tr("No subnets in network.\nUse Add Subnet before editing Subnet."),
                                  QMessageBox::Ok);
        return("");
    }

    clearSelection();
    editNetworkDialog *dialog =
            new editNetworkDialog(0, 0, false);
    if( dialog->exec() == editNetworkDialog::Accepted )
        n_i=dialog->getNum_IdNetwork();

    delete dialog;

    subnet = SubNets.at(n_i-1);
    path=subnet->getPath();
    filename=subnet->getFileName();
    undoLog->insertLogOperation (subnet, "edit_subnet");
    return (path+filename);

}

void NeuralNetwork::delNetwork(void)
{
    if(NumberOfSubNets < 1)
        return;

    clearSelection();
    delNetworkDialog  *dialog =
            new delNetworkDialog(0, 0, false);

    if( dialog->exec() == delNetworkDialog::Accepted )
        delNetwork(dialog->getNum_IdNetwork());

    delete dialog;

}

void NeuralNetwork::delNetwork(ulong id_network)
{
    SubNet *subnet, *prev_subnet, *post_subnet;

    prev_subnet= 0;
    post_subnet= 0;

    subnet = SubNets.at(id_network-1);

    if(!subnet)
        return;

    undoLog->insertComment ("DEBUG:: del subnet :: num_id="+QString::number(id_network));
    undoLog->insertLogOperation (subnet, "del_subnet");

    delete subnet;

    SubNets.removeAt(id_network-1);
    NumberOfSubNets--;

    if(id_network > 1)
        prev_subnet =  SubNets.at(id_network - 2);
    if(id_network <= NumberOfSubNets )
        post_subnet = SubNets.at(id_network - 1);

    if(prev_subnet && post_subnet)
        connect(prev_subnet, SIGNAL(num_idChanged(ulong)),
                post_subnet, SLOT(num_idChange(ulong)));

    if(post_subnet)
        post_subnet->num_idChange(id_network - 1);

    emit netChanged();

}

// nuevo código para

bool NeuralNetwork::cut()
{
    // comprobar si hay alguna neurona seleccionada
    QListIterator<Neuron *> itn(Neurons);
    Neuron *neuron, *copyneuron;
    int contador=0;
    // nuevo para seleccionar subredes
    QListIterator<SubNet *> its(SubNets);
    SubNet *subnet, *copysubnet;
    int contadorsubnet = 0;

    while(itn.hasNext()) {
        neuron = itn.next();
        if(neuron->getSelected()){
            copyneuron = neuron;
            contador++;
        }
    }

    // nuevo para recorrer subredes
    while(its.hasNext()) {
        subnet = its.next();
        if(subnet->getSelected()){
            copysubnet=subnet;
            contadorsubnet++;
        }
    }

    if(contador < 1 && contadorsubnet < 1){
        // hay más de una neurona seleccionada
        QMessageBox::information(nullptr, tr("cut"), tr("Select a neuron or subnet"), QMessageBox::Ok);
        return false;
    }
    if(contador > 1){
        // hay más de una neurona seleccionada
        QMessageBox::information(nullptr, tr("cut"), tr("Select only one neuron"), QMessageBox::Ok);
        return false;
    }
    if(contadorsubnet > 1){
        // hay más de una subred seleccionada
        QMessageBox::information(nullptr, tr("cut"), tr("Select only one  subnet"), QMessageBox::Ok);
        return false;
    }

    QDomDocument clip ( "clip" );
    QDomElement root = clip.createElement( "content" );
    clip.appendChild( root );
    // solo dejo que copie según el contador
    if(contador == 1)
        copyneuron->write (clip, root);
    if(contadorsubnet == 1)
        copysubnet->write(clip, root);

    QApplication::clipboard()->setText(clip.toString());

    QString text_tmp = QApplication::clipboard()->text(QClipboard::Clipboard);
    QDomDocument clipcut;
    if(!clipcut.setContent(text_tmp, false))
        return false;

    XMLOperator* xml_operator = XMLOperator::instance();
    if(contador == 1){
        QDomElement xml_neuron = xml_operator->findTag("Neuron",
                                                       clipcut.documentElement(), false);
        deleteNeuron(xml_neuron);
    }

    // si he seleccionado subred borro la subred

    if(contadorsubnet == 1){
        QDomElement xml_subnet = xml_operator->findTag("SubNet",clipcut.documentElement(), false);
        deleteSubNet(xml_subnet);
    }
    clearSelection();
    emit netChanged();

    return true;

}



bool NeuralNetwork::copy()
{
    // comprobar si hay alguna neurona seleccionada

    QListIterator<SubNet *> its(SubNets);
    SubNet *subnet, *copysubnet;
    int contadorsubnet=0;

    while(its.hasNext()) {
        subnet = its.next();
        if(subnet->getSelected()){
            copysubnet=subnet;
            contadorsubnet++;
        }
    }

    QDomDocument clip ( "clip" );
    QDomElement root = clip.createElement( "content" );
    clip.appendChild( root );

    if(fromSelNeuron && toSelNeuron) {
        Synapse* copysynapse = findSynapse(fromSelNeuron->getNumId(), toSelNeuron->getNumId());
        if(copysynapse)
            copysynapse->write (clip, root);
    }
    else if(fromSelNeuron) {
        fromSelNeuron->write (clip, root);
        XMLOperator* xml_operator = XMLOperator::instance();
        xml_operator->deleteElementsByTagName("Axon", root);
    }
    else {
        // no hay neurona seleccionada
        QMessageBox::information(nullptr, tr("copy"), tr("Select a neuron or synapse"), QMessageBox::Ok);
        return false;
    }

    if(contadorsubnet == 1)
        copysubnet->write(clip, root);

    QApplication::clipboard()->setText(clip.toString());
    clearSelection();
    return true;
}



bool NeuralNetwork::paste()
{
    // recoger informacion del clipboard
    QString text_tmp = QApplication::clipboard()->text(QClipboard::Clipboard);

    QDomDocument clip;
    if(!clip.setContent(text_tmp, false))
        return false;

    XMLOperator* xml_operator = XMLOperator::instance();
    QDomElement xml_neuron = xml_operator->findTag("Neuron", clip.documentElement(), false);
    QDomElement xml_subnet = xml_operator->findTag("SubNet", clip.documentElement(), false);
    QDomElement xml_synapse = xml_operator->findTag("Synapse", clip.documentElement(), false);
    if(!xml_neuron.isNull())
        loadNeuron(xml_neuron);

    if(!xml_subnet.isNull())
        loadSubNet(xml_subnet);

    if(!xml_synapse.isNull())
        loadSynapse(xml_synapse);

    clearSelection();
    emit netChanged();

    return true;
}

// undo
bool NeuralNetwork::undo()
{
    // llamada al log de operaciones
    // se utiliza un bucle para las operaciones recursivas
    // (p.e. para recuperar una neurona con sus synapses)
    undoLog->undo();
    while (undoLog->next_undo()) {
        QString operacion = undoLog->getCurrentOperationName();
        QDomElement elem = undoLog->getCurrentElement();

        // abrimos una operacion undo
        // las operaciones que se ejecutan mientras esta abierta
        // pasan directamente a la cola de redo
        undoLog->openUndo();

        if(operacion=="add_synapse"){
            loadSynapse(elem);
            // QMessageBox::information(nullptr, tr("DEBUG"),tr("undo::add_synapse"),tr("&Ok"));
        }
        else if(operacion=="del_synapse"){
            deleteSynapse(elem);
            // QMessageBox::information(nullptr, tr("DEBUG"),tr("undo::del_synapse"),tr("&Ok"));
        }
        else if(operacion=="edit_synapse"){
            editSynapse(elem);
            // QMessageBox::information(nullptr, tr("DEBUG"),tr("undo::edit_synapse"),tr("&Ok"));
        }
        else if(operacion=="add_neuron"){
            loadNeuron(elem);
            // QMessageBox::information(nullptr, tr("DEBUG"),tr("undo::add_neuron"),tr("&Ok"));
        }
        else if(operacion=="del_neuron"){
            // QMessageBox::information(nullptr, tr("DEBUG"),tr("undo ::del_neuron"),tr("&Ok"));
            deleteNeuron(elem);
        }
        else if(operacion=="edit_neuron"){
            // QMessageBox::information(nullptr, tr("DEBUG"),tr("undo ::edit_neuron"),tr("&Ok"));
            editNeuron(elem);
        }
        else if(operacion=="add_subnet"){
            loadSubNet(elem);
            // QMessageBox::information(nullptr, tr("DEBUG"),tr("undo::add_subnet"), tr("&Ok"));
        }
        else if(operacion=="del_subnet"){
            deleteSubNet(elem);
            // QMessageBox::information(nullptr, tr("DEBUG"),tr("undo::del_subnet"),tr("&Ok"));
        }
        else if(operacion=="edit_subnet"){
            editSubNet(elem);
            // QMessageBox::information(nullptr, tr("DEBUG"),tr("undo::edit_subnet"),tr("&Ok"));
        }

        undoLog->closeUndo();

    } // termina while()
    undoLog->closeGroup();
    return true;
}

bool NeuralNetwork::redo()
{

    // llamada al log de operaciones
    // se utiliza un bucle para las operaciones recursivas
    // (p.e. para recuperar una neurona con sus synapses)
    undoLog->redo();
    while (undoLog->next_redo())
    {
        QString operacion = undoLog->getCurrentOperationName();
        QDomElement elem = undoLog->getCurrentElement();

        if(operacion=="add_synapse"){
            loadSynapse(elem);
            // QMessageBox::information(nullptr, tr("DEBUG"),tr("redo::add_synapse"),tr("&Ok"));
        }else if(operacion=="del_synapse"){
            deleteSynapse(elem);
            // QMessageBox::information(nullptr, tr("DEBUG"),tr("redo::del_synapse"),tr("&Ok"));
        }else if(operacion=="edit_synapse"){
            editSynapse(elem);
            // QMessageBox::information(nullptr, tr("DEBUG"),tr("redo::edit_synapse"),tr("&Ok"));
        }
        else if(operacion=="add_neuron"){
            loadNeuron(elem);
            // QMessageBox::information(nullptr, tr("DEBUG"), tr("redo::add_neuron"),tr("&Ok"));
        }
        else if(operacion=="del_neuron"){
            // QMessageBox::information(nullptr, tr("DEBUG"),tr("redo ::del_neuron"),tr("&Ok"));
            deleteNeuron(elem);
        }
        else if(operacion=="edit_neuron"){
            // QMessageBox::information(nullptr, tr("DEBUG"),tr("redo ::edit_neuron"),tr("&Ok"));
            editNeuron(elem);
        }
        else if(operacion=="add_subnet"){
            loadSubNet(elem);
            // QMessageBox::information(nullptr, tr("DEBUG"),tr("redo::add_subnet"),tr("&Ok"));
        }
        else if(operacion=="del_subnet"){
            deleteSubNet(elem);
            // QMessageBox::information(nullptr, tr("DEBUG"), tr("redo::del_subnet"),tr("&Ok"));
        }
        else if(operacion=="edit_subnet"){
            editSubNet(elem);
            // QMessageBox::information(nullptr, tr("DEBUG"),tr("redo::edit_subnet"),tr("&Ok"));
        }

    } // termina while()
    undoLog->closeGroup();
    return true;
}



bool NeuralNetwork::loadSynapse(const QDomElement& e)
{
    XMLOperator* xml_operator = XMLOperator::instance();
    bool ret = false;

    //leo los valores que restauraré en la synapsis que devuelvo
    QDomElement e_type = xml_operator->findTag("type", e);
    //  QDomElement e_num_id = xml_operator->findTag("num_id", e);
    QDomElement e_construct = xml_operator->findTag("constructor", e);
    //  int num_id = e_num_id.text().toUInt();
    int type = e_type.text().toInt();

    // from
    QString qsFrom = e_construct.attribute("From");
    ulong from = qsFrom.toULong();

    // to
    QString qsTo = e_construct.attribute("To");
    ulong to = qsTo.toULong();

    // crear la synapse con los valores basicos

    ////////////////////
    // No se debe usar esto sino factorySynapse(type, num_id, e_construct) y demás, ver NetBuilder y Netfactory::buildAxon(...), <-- problemas con num_id al insertar la sinapsis
    addSynapse(from, to, type, false, false);

    // ajustar los valores que faltan
    if(currentSynapse) { // currentSynapse viene de addSynapse(...)
        currentSynapse->setParams(e_construct);
        undoLog->insertComment ("DEBUG:: add synapse :: num_id="+QString::number(currentSynapse->getNumId()));
        undoLog->insertLogOperation (currentSynapse, "add_synapse");
        ret = true;
    }
    emit netChanged();
    return ret;

}

bool NeuralNetwork::deleteSynapse(const QDomElement& e)
{
    XMLOperator* xml_operator = XMLOperator::instance();

    //leo los valores de la synapse que hay que borrar
    QDomElement e_construct = xml_operator->findTag("constructor", e);

    // from
    QString qsFrom = e_construct.attribute("From");
    ulong from = qsFrom.toULong();

    // to
    QString qsTo = e_construct.attribute("To");
    ulong to = qsTo.toULong();

    currentSynapse = findSynapse(from, to);
    if(currentSynapse)
        delSynapse(currentSynapse->getNumId());

    return true;
}

bool NeuralNetwork::editSynapse(const QDomElement& e)
{
    XMLOperator* xml_operator = XMLOperator::instance();
    bool ok = false;

    //leo los valores que restaurar�é en la synapsis que devuelvo
    QDomElement e_num_id = xml_operator->findTag("num_id", e);
    QDomElement e_construct = xml_operator->findTag("constructor", e);
    int num_id = e_num_id.text().toUInt(&ok);
    if(!ok)
        return false;

    currentSynapse = Axons.at(num_id - 1);

    if(!currentSynapse)
        return false;

    // se almacena la operacion en el log
    // para poder hacer redo
    undoLog->insertComment ("DEBUG:: edit synapse :: num_id="+QString::number(currentSynapse->getNumId()));
    undoLog->insertLogOperation (currentSynapse, "edit_synapse");

    // ajustar los valores que faltan
    currentSynapse->setParams(e_construct);
    emit netChanged();
    return true;

}

bool NeuralNetwork::loadNeuron(const QDomElement& e)
{

    XMLOperator* xml_operator = XMLOperator::instance();

    //leo los valores que restauraré en la neurona que devuelvo
    QDomElement e_type = xml_operator->findTag("type", e);
    QDomElement e_num_id = xml_operator->findTag("num_id", e);
    QDomElement e_construct = xml_operator->findTag("constructor", e);
    QDomElement e_subobj = xml_operator->findTag("SubObjects", e);


    int num_id = e_num_id.text().toUInt();
    int type = e_type.text().toInt();

    // leo la capa en la que se crea
    QString qsLayer = e_construct.attribute("layer");
    int layer = qsLayer.toInt();

    // se llama al metodo normal de añadir neurona
    // (con la opcion edit=false ya que los parametros se cargan desde aqui)
    currentNeuron = addNeuron(num_id,layer,type, false, false);

    if(!currentNeuron)
        return false;

    currentNeuron->setParams(e_construct);

    // si tiene subobjetos (synapses), cargarlos en la red
    if(!e_subobj.isNull()){
        currentNeuron->initSubObjects(e_subobj);
    }
    undoLog->insertComment ("DEBUG:: add neuron :: num_id="+QString::number(currentNeuron->getNumId()));
    undoLog->insertLogOperation (currentNeuron, "add_neuron");
    emit netChanged();
    
    return true;
}

bool NeuralNetwork::deleteNeuron(const QDomElement& e)
{

    XMLOperator* xml_operator = XMLOperator::instance();
    bool ok = false;
    QDomElement e_num_id = xml_operator->findTag("num_id", e);
    int num_id = e_num_id.text().toUInt(&ok);
    if(!ok)
        return false;

    delNeuron(num_id);

    return true;
}


bool NeuralNetwork::editNeuron(const QDomElement& e)
{
    XMLOperator* xml_operator = XMLOperator::instance();
    bool ok = false;

    //leo los valores que restauraré en la neurona que devuelvo
    QDomElement e_num_id = xml_operator->findTag("num_id", e);
    QDomElement e_construct = xml_operator->findTag("constructor", e);
    QDomElement e_subobj = xml_operator->findTag("SubObjects", e);

    int num_id = e_num_id.text().toUInt(&ok);
    if(!ok)
        return false;

    currentNeuron = Neurons.at(num_id - 1);
    if(currentNeuron) {
        undoLog->insertComment ("DEBUG:: edit neuron :: num_id="+QString::number(currentNeuron->getNumId()));
        undoLog->insertLogOperation (currentNeuron, "edit_neuron");
        currentNeuron->setParams(e_construct);
        if(!e_subobj.isNull()){
            xml_operator->deleteElementsByTagName("Axon", e_subobj);
            currentNeuron->initSubObjects(e_subobj);
        }
        emit netChanged();
    }

    return true;
}

bool NeuralNetwork::loadSubNet(const QDomElement& e) 
{
    XMLOperator* xml_operator = XMLOperator::instance();

    //leo los valores que restauraré en la subred que devuelvo
    QDomElement e_num_id = xml_operator->findTag("num_id", e);
    QDomElement e_path = xml_operator->findTag("path", e);
    QDomElement e_filename = xml_operator->findTag("filename", e);
    QDomElement e_subobj = xml_operator->findTag("SubObjects", e);

    int num_id = e_num_id.text().toUInt();
    QString path = e_path.text();
    QString filename = e_filename.text();

    currentSubNet = addNetwork(num_id,path+filename, false);

    if(!currentSubNet)
        return false;

    if(!e_subobj.isNull())
        currentSubNet->initSubObjects(e_subobj);

    emit netChanged();
    return true;

}

bool NeuralNetwork::deleteSubNet(const QDomElement& e)

{
    XMLOperator* xml_operator = XMLOperator::instance();

    //leo el id de la subred que voy a borrar
    QDomElement e_num_id = xml_operator->findTag("num_id", e);

    int num_id = e_num_id.text().toUInt();
    delNetwork(num_id);

    return true;

}

bool NeuralNetwork::editSubNet(const QDomElement& e)
{
    //    XMLOperator* xml_operator = XMLOperator::instance();

    //    //leo los valores que restauraré en la subred que devuelvo
    //    QDomElement e_type = xml_operator->findTag("type", e);
    //    QDomElement e_num_id = xml_operator->findTag("num_id", e);
    //    QDomElement e_path = xml_operator->findTag("path", e);
    //    QDomElement e_filename = xml_operator->findTag("filename", e);

    //    QString path = e_path.text();
    //    QString filename = e_filename.text();
    //    // queda pendiente si ajusto los valores o no

    return true;
}
