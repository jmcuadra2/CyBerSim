/***************************************************************************
                          netsimcontrol.cpp  -  description
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

#include "netsimcontrol.h"
#include "netinputfusionnode.h"
#include "smginterface.h"
#include "../recmon/netrecmontechnician.h"
#include "../net/neuralnetwork.h"
#include "../net/inputneuron.h"
#include "../net/motorcommandneuron.h"
#include "../neuraldis/settings.h"
#include "../neuraldis/xmloperator.h"

NetSimControl::NetSimControl(){

    net = 0;
    netRecMonTech = 0;
    weightStabMode = 0;
    isCommonLearnRate = false;
    CommonLearnRate = 0.0;
    weightStabVal = 100.0;
    positiveWeights = true;
    instantPropagate = true;
    initWeightsMode = false;
    initWeightsVal = 0.0;
    learningDecay = 0.0;
    hasView = true;
    controlName = "NetControl";

    //   defaultCommand.setPriority(1);
    //   defaultCommand.setCommandId(MotorCommand::WheelsVel);
    //   NDPose* commandValue = new NDPose2D();
    //   defaultCommand.setCommandValue(commandValue, false);
    //   motorPlan->addCommand(defaultCommand);

}

NetSimControl::~NetSimControl()
{

    Settings* prog_settings = Settings::instance();
    prog_settings->deleteNet();
    prog_settings->deleteNetView();
    if(netRecMonTech)
        delete netRecMonTech;

}

void NetSimControl::initElements(void)
{

    Neuron *neuron;
    QList<Neuron*> input_lay = net->getInputLayer();
    QList<Neuron*> hidden_lay = net->getHiddenLayer();
    QList<Neuron*> output_lay = net->getOutputLayer();

    QListIterator<Neuron*> iti(input_lay);
    QListIterator<Neuron*> ith(hidden_lay);
    QListIterator<Neuron*> ito(output_lay);
    while(iti.hasNext()) {
        neuron = iti.next();
        neuron->initOldOut();
    }
    while(ith.hasNext()) {
        neuron = ith.next();
        neuron->initOldOut();
    }
    SMGInterface* smgInterface = getSMGInterface();
    if(!smgInterface) {
        cerr << "Warning: NetSimControl::initElements() null SMGInterface" << endl;
        return;
    }

    if(!smgInterface->isInitialized()) {
        cerr << "Warning: NetSimControl::initElements() SMGInterface not initialized" << endl;
        return;
    }
    double vel1, vel2;
    vector<double> initOut(4 ,0.0);
    smgInterface->getOutputMotorPlan()->getInitialCommand(vel1, vel2);  // provisional
    DriveControl* drive = getSMGInterface()->getDrive();

    NDPose* commandDefault = new NDPose2D();
    commandDefault->setCoordinates(vel1, vel2, 0);
    defaultCommand.setCommandValue(commandDefault);
    if(drive) {
        if(drive->isActive())
            drive->setDefaultCommand(commandDefault);
    }

    // cuidado matar puntero ?

    if(vel1 >= 0.0) {
        initOut[0] = vel1;
        initOut[1] = 0.0;
    } else {
        initOut[1] = -vel1;
        initOut[0] = 0.0;
    }
    if(vel2 >= 0.0) {
        initOut[3] = vel2;
        initOut[2] = 0.0;
    } else {
        initOut[2] = -vel2;
        initOut[3] = 0.0;
    }

    QListIterator<MotorCommandNeuron* > it(commandNeurons);
    MotorCommandNeuron* neuron_com;
    int i = 0;
    while (it.hasNext()) {
        neuron_com = it.next();
        neuron_com->setInitExtern(initOut.at(i));
        QList<Synapse*> axon = neuron_com->getAxon();
        QListIterator<Synapse*> its(axon);
        Synapse* synapse;
        while(its.hasNext()) {
            synapse = its.next();
            Neuron* neuron_to = synapse->getPointerTo();
            Neuron* neuron_out;
            ito.toFront();
            while(ito.hasNext()) {
                neuron_out = ito.next();
                if(neuron_out == neuron_to)
                    neuron_out->input(neuron_com->getInitExtern(), synapse);
            }
        }
        i++;
    }

    ito.toFront();
    while(ito.hasNext()) {
        neuron = ito.next();
        neuron->initOldOut();
        neuron->calculateOutput();
        neuron->input(neuron->output());
    }
    
}


void NetSimControl::propagate(void)
{

    double out;
    int i = 0;
    Neuron *neuron_from, *neuron_to;
    Synapse *synapse;
    QList<Neuron*> input_lay = net->getInputLayer();
    QList<Neuron*> hidden_lay = net->getHiddenLayer();
    QList<Neuron*> output_lay = net->getOutputLayer();

    QListIterator<Neuron*> iti(input_lay);
    QListIterator<Neuron*> ith(hidden_lay);
    QListIterator<Neuron*> ito(output_lay);

    double vel1, vel2;
    vector<double> initOut(4, 0.0);

    DriveControl* drive = getSMGInterface()->getDrive();

    if(drive) {
        if(drive->isActive()) {
            QList<MotorCommand> driveCommands = drive->getMotorPlan()->getCommands();
            NDPose* commandValue = driveCommands[0].getCommandValue();

            vel1 = commandValue->getPosition().at(0);
            vel2 = commandValue->getPosition().at(1);
        }
        else {
            NDPose* commandValue = defaultCommand.getCommandValue();
            vel1 = commandValue->getPosition().at(0);
            vel2 = commandValue->getPosition().at(1);
        }
    }
    else {
        NDPose* commandValue = defaultCommand.getCommandValue();
        vel1 = commandValue->getPosition().at(0);
        vel2 = commandValue->getPosition().at(1);
    }

    if(vel1 >= 0.0) {
        initOut[0] = vel1;
        initOut[1] = 0.0;
    } else {
        initOut[1] = -vel1;
        initOut[0] = 0.0;
    }
    if(vel2 >= 0.0) {
        initOut[3] = vel2;
        initOut[2] = 0.0;
    } else {
        initOut[2] = -vel2;
        initOut[3] = 0.0;
    }

    QListIterator<MotorCommandNeuron* > it(commandNeurons);
    MotorCommandNeuron* neuron_com;
    while (it.hasNext()) {
        neuron_com = it.next();
        neuron_com->setInitExtern(initOut.at(i));
        i++;
    }


    // transmision secuencial ( 3(capas) pasos de simulacion) de sensores a motores
    if(!instantPropagate) {
        while(ito.hasNext()) {
            neuron_from = ito.next();
            out = neuron_from->calculateOutput();
            QList<Synapse*> Axon = neuron_from->getAxon();
            QListIterator<Synapse*> its(Axon);
            while(its.hasNext()) {
                synapse = its.next();
                neuron_to = synapse->getPointerTo();
                if(neuron_to)
                    neuron_to->input(out, synapse);
            }
            ++i;
        }
        while(ith.hasNext()) {
            neuron_from = ith.next();
            out = neuron_from->calculateOutput();
            QList<Synapse*> Axon = neuron_from->getAxon();
            QListIterator<Synapse*> its(Axon);
            while(its.hasNext()) {
                synapse = its.next();
                neuron_to = synapse->getPointerTo();
                if(neuron_to)
                    neuron_to->input(out, synapse);
            }
        }

        while(iti.hasNext()) {
            neuron_from = iti.next();
            out = neuron_from->calculateOutput();
            QList<Synapse*> Axon = neuron_from->getAxon();
            QListIterator<Synapse*> its(Axon);
            while(its.hasNext()) {
                synapse = its.next();
                neuron_to = synapse->getPointerTo();
                if(neuron_to)
                    neuron_to->input(out, synapse);
            }
        }
    }
    else {

        // transmision instantanea ( 1 paso de simulacion) de sensores a motores )

        while(iti.hasNext()) {
            neuron_from = iti.next();
            out = neuron_from->calculateOutput();
            QList<Synapse*> Axon = neuron_from->getAxon();
            QListIterator<Synapse*> its(Axon);
            while(its.hasNext()) {
                synapse = its.next();
                neuron_to = synapse->getPointerTo();
                if(neuron_to)
                    neuron_to->input(out, synapse);
            }
        }

        while(ith.hasNext()) {
            neuron_from = ith.next();
            out = neuron_from->calculateOutput();
            QList<Synapse*> Axon = neuron_from->getAxon();
            QListIterator<Synapse*> its(Axon);
            while(its.hasNext()) {
                synapse = its.next();
                neuron_to = synapse->getPointerTo();
                if(neuron_to)
                    neuron_to->input(out, synapse);
            }
        }

        while(ito.hasNext()) {
            neuron_from = ito.next();
            out = neuron_from->calculateOutput();
            QList<Synapse*> Axon = neuron_from->getAxon();
            QListIterator<Synapse*> its(Axon);
            while(its.hasNext()) {
                synapse = its.next();
                neuron_to = synapse->getPointerTo();
                if(neuron_to)
                    neuron_to->input(out, synapse);
            }
            ++i;
        }
    }

}

void NetSimControl::readInputs(void)
{
    sampleInputs();
    
    QList<Neuron*> input_lay = net->getInputLayer();
    QListIterator<Neuron*> it(input_lay);
    Neuron* neuron;
    while(it.hasNext()) {
        neuron= it.next();
        neuron->input();
    }

}

void NetSimControl::backPropagate(void)
{

}

void NetSimControl::sendOutputs(void)
{

    QList<Neuron*> output_lay = net->getOutputLayer();
    double lvel = output_lay.at(0)->output() - output_lay.at(1)->output();
    double rvel = output_lay.at(3)->output() - output_lay.at(2)->output();

    OutputMotorPlan* outputMotorPlan = getSMGInterface()->getOutputMotorPlan();
    if(outputMotorPlan) {
        MotorCommand command;
        command.setPriority(1);
        command.setCommandId(MotorCommand::WheelsVel);
        NDPose* commandValue = new NDPose2D();
        commandValue->setCoordinates(lvel, rvel, 0);
        command.setCommandValue(commandValue, false);
        outputMotorPlan->addCommand(command);
        outputMotorPlan->sendCommand();
    }
    else
        cerr << "Warning: NetSimControl::sendOutputs(), no motor plan" << endl;
}

int NetSimControl::getType(void)
{
    return BASE_NET_CONTROL;
}

int NetSimControl::getSuperType(void)
{
    return NET_CONTROL;
}

void NetSimControl::setNet(NeuralNetwork* net_)
{
    net = net_;

    QList<Neuron*> input_lay = net->getInputLayer();
    QList<Neuron*> hidden_lay = net->getHiddenLayer();
    QListIterator<Neuron*> iti(input_lay);
    QListIterator<Neuron*> ith(hidden_lay);

    InputNeuron* neuron_in;
    Neuron* neuron;
    while(iti.hasNext()) {
        neuron = iti.next();
        if((neuron_in = dynamic_cast<InputNeuron*> (neuron))) {
            sensorNeurons.append(neuron_in);
        }
    }

    MotorCommandNeuron* neuron_com;
    while(ith.hasNext()) {
        if((neuron_com = dynamic_cast<MotorCommandNeuron*> (ith.next()))) {
            commandNeurons.append(neuron_com);
        }
    }

}

void NetSimControl::setRecording(bool on, bool crono_reset)
{

    if(!netRecMonTech)
        return;
    netRecMonTech->startMonitor(on);
    netRecMonTech->startRecorder(on, crono_reset);
    netRecMonTech->connectToTime(on);

}  

void NetSimControl::connectIO(void)
{

    if(!net)
        return;
    QListIterator<InputNeuron* > it(sensorNeurons);
    InputNeuron* neuron_in;

    while (it.hasNext()) {
        neuron_in = it.next();
        SensorsSet* sensorsSet = getSMGInterface()->getSensorsSet();
        SensorGroup* group = sensorsSet->getSensorGroup(neuron_in->getSensorGroupID());
        if(group) {
            AbstractSensor* sensor = group->getSensors(neuron_in->getInputNodeId());
            if(sensor) {
                NetInputFusionNode* inputNode = new NetInputFusionNode(sensor);
                inputNode->initialize();
                neuron_in->setInputNode(inputNode);
                QVector< AbstractFusionNode* > inputNodeList;
                if(!inputNodeMap.contains(group->getGroupName()))
                    inputNodeMap.insert(group->getGroupName(), inputNodeList);
                inputNodeMap[group->getGroupName()].append(inputNode);
            }
        }
    }

}

void NetSimControl::doMonitor(void)
{
    if(!netRecMonTech) return;
    netRecMonTech->doMonitor();
}

QList<QewExtensibleDialog*> NetSimControl::tabsForEdit(QewExtensibleDialog* parent)
{

    QList<QewExtensibleDialog*> add_dialogs;
    if(netRecMonTech)
        add_dialogs = netRecMonTech->tabsForEdit(parent);

    return add_dialogs;

}

void NetSimControl::setWeightStabMode(int w_stabmode)
{
    weightStabMode = w_stabmode;
}

void NetSimControl::setWeightStabVal(double w_stabval)
{
    weightStabVal = w_stabval;
}

int NetSimControl::getWeightStabMode(void)
{
    return weightStabMode;
}

double NetSimControl::getWeightStabVal(void)
{
    return weightStabVal;
}

void NetSimControl::setCommonLearnRate(double l_rate)
{
    CommonLearnRate = l_rate;
}

double NetSimControl::getCommonLearnRate(void)
{
    return CommonLearnRate;
}

bool NetSimControl::getCommonLearnMode(void)
{
    return isCommonLearnRate;
}

void NetSimControl::setCommonLearnMode(bool l_mode)
{
    isCommonLearnRate = l_mode;
}

void NetSimControl::setInitWeightsVal(double w_initval)
{
    initWeightsVal = w_initval;
}

double NetSimControl::getInitWeightsVal(void)
{
    return initWeightsVal;
}

bool NetSimControl::getInitWeightsMode(void)
{
    return initWeightsMode;
}

void NetSimControl::setInitWeightsMode(bool w_initmode)
{
    initWeightsMode = w_initmode;
}

void NetSimControl::setPositiveWeights(bool yes)
{
    positiveWeights = yes;
}

bool NetSimControl::getPositiveWeights(void)
{
    return positiveWeights;
}

void NetSimControl::setLearningDecay(double decay)
{
    learningDecay = decay;
}

double NetSimControl::getLearningDecay(void)
{
    return learningDecay;
}

void NetSimControl::save_special(QDomDocument& doc_control, QDomElement& e)
{

    XMLOperator* xml_operator = XMLOperator::instance();

    QString relativeName = net_name;
    e.appendChild(xml_operator->createTextElement(doc_control,
                 "net_file", relativeName.remove(Settings::instance()->getNetDirectory() + QDir::separator())));

    e.appendChild(xml_operator->createTextElement(doc_control,
                                                  "hasView", QString::number(hasView)));

    QDomElement tag_params = doc_control.createElement("parameters");

    tag_params.appendChild(xml_operator->createTextElement(doc_control,
                       "weightStabMode", QString::number(weightStabMode)));
    tag_params.appendChild(xml_operator->createTextElement(doc_control,
                       "weightStabVal", QString::number(weightStabVal)));
    tag_params.appendChild(xml_operator->createTextElement(doc_control,
                       "ISOCommonLearnMode", QString::number(isCommonLearnRate)));
    tag_params.appendChild(xml_operator->createTextElement(doc_control,
                       "ISOCommonLearnRate", QString::number(CommonLearnRate)));
    tag_params.appendChild(xml_operator->createTextElement(doc_control,
                       "positiveWeights", QString::number(positiveWeights)));

    tag_params.appendChild(xml_operator->createTextElement(doc_control,
                       "instantPropagate", QString::number(instantPropagate)));

    tag_params.appendChild(xml_operator->createTextElement(doc_control,
                       "learningDecay", QString::number(learningDecay)));

    e.appendChild(tag_params);
    if(netRecMonTech)
        netRecMonTech->write(doc_control, e);

}

void NetSimControl::setParams(const QDomElement& e)
{
    bool ok;
    XMLOperator* xml_operator = XMLOperator::instance();
    QDomElement e_instp = xml_operator->findTag("instantPropagate", e);
    setInstantPropagation(bool(e_instp.text().toInt(&ok)));
}

void NetSimControl::setInstantPropagation(bool is_instant)
{
    instantPropagate = is_instant;
}

bool NetSimControl::getInstantPropagation(void)
{
    return instantPropagate;
}

bool NetSimControl::createSMGInterface(void)
{
    bool ret = false;
    if(!getSMGInterface()) {
        setSMGInterface(new SMGInterface());
        ret = true;
    }
    else
        cerr << "Warning: NetSimControl::creategetSMGInterface()() getSMGInterface() already assigned" << endl;
    return ret;

}
