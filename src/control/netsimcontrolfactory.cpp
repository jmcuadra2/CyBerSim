/***************************************************************************
                          netsimcontrolfactory.cpp  -  description
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

#include "netsimcontrolfactory.h"
#include "netsimcontrol.h"
#include "isosimcontrol.h"
#include "../net/netbuilder.h"
#include "../neuraldis/neuraldisdoc.h"
#include "../disview/neuraldisview.h"
#include "../recmon/netrecmontechnician.h"
#include "../neuraldis/xmloperator.h"

#include <QObject>

NetSimControlFactory::NetSimControlFactory(){

    net = 0;
    net_control = 0;

}

NetSimControlFactory::~NetSimControlFactory(){
}


BaseSimControl* NetSimControlFactory::construct(int type, const QDomElement& e, const QString& control_filename)
{

    XMLOperator* xml_operator = XMLOperator::instance();
    NeuralDisDoc* doc = NeuralDisDoc::instance();
    NeuralDisView* view = 0;
    BaseSimControl* base_control = 0;

    if(type == BaseSimControl::BASE_NET_CONTROL) {
        NetSimControl* control = new NetSimControl();
        net_control = control;
    }
    else if(type == BaseSimControl::ISO_NET_CONTROL) {
        ISOSimControl* control = new ISOSimControl();
        net_control = control;
    }

    if(!net_control) return base_control;

    QDomElement e_net = xml_operator->findTag("net_file", e);
    QDomElement e_hasview = xml_operator->findTag("hasView", e);
    bool has_view;
    bool ok;
    if(e_hasview.isNull())
        has_view = true;
    else
        has_view = bool(e_hasview.text().toInt(&ok));
    if(!ok)
        has_view = true;
    QDomElement e_par = xml_operator->findTag("parameters", e);
    QDomElement e_mon = xml_operator->findTag("monitors_file", e);
    if(e_net.isNull() || e_par.isNull()) {
        delete net_control;
        net_control = 0;
        return base_control;
    }
    QString file_name = e_net.text();
    if(doc->load(file_name)) {  //file_name pasa por referencia
        NetBuilder* netBuilder = new NetBuilder();
        net = netBuilder->construct();
        if(!net) {
            delete net_control;
            return base_control;
        }
        if(file_name != e_net.text())
            net_control->setFileChanged( true);
        net->setName(file_name);
        net_control->setNet(net);
        if(has_view)
            netBuilder->setView(view);
        delete netBuilder;
    }
    else {
        delete net_control;
        return base_control;
    }
    net_control->setControlFileName(control_filename);
    net_control->setNetName(net->name());
    net_control->setParams(e_par);
    net_control->netRecMonTech =
            factoryNetMonTech(net_control->getControlFileName(), e_mon);
    net_control->setFileChanged(net_control->fileChanged() || net_control->netRecMonTech->fileChanged());

    base_control = net_control;
    return base_control;

}

BaseSimControl* NetSimControlFactory::construct(void)
{

    BaseSimControl* base_control = 0;
    NeuralDisDoc* doc = NeuralDisDoc::instance();
    QDomElement control_classes;
    DictionaryManager* dict_manager = DictionaryManager::instance();
    NameClass_Map names_list;

    bool ret = dict_manager->readFileConf("newControl", "BaseSimControl", control_classes, BaseSimControl::NET_CONTROL);
    if(ret) {
        dict_manager->getNamesMap(control_classes, names_list);
        int control_type = dict_manager->chooseClass(names_list, QObject::tr("Choose a type of net based control"));
        if(control_type == BaseSimControl::BASE_NET_CONTROL) {
            NetSimControl* control = new NetSimControl();
            //     control->setControlFileName(control_filename);
            net_control = control;
        }
        else if(control_type == BaseSimControl::ISO_NET_CONTROL) {
            ISOSimControl* control = new ISOSimControl();
            //     control->setControlFileName(control_filename);
            net_control = control;
        }
        else
            return base_control;
        QString net_name;
        if(!doc->load(net_name)) {  //file_name pasa por referencia
            delete net_control;
        }
        else {
            net_control->setFileChanged(net_control->fileChanged());
            net_control->setNetName(net_name);
            base_control = net_control;
        }
    }
    return base_control;

}  


NetRecMonTechnician* NetSimControlFactory::
factoryNetMonTech(const QString& control_filename, const QDomElement& e_mon)
{

    NetRecMonTechnician* netRMTech = new NetRecMonTechnician();
    QList<RecMonDevice*> devs;
    QListIterator<Neuron*> it_n(net->neurons());
    Neuron* neuron;
    while(it_n.hasNext()) {
        neuron = it_n.next();
        if(neuron->isRecMonable())
            devs.append(neuron);
    }
    netRMTech->setControlFileName(control_filename);
    netRMTech->setNetName(net->name());
    netRMTech->setDevices(devs);
    netRMTech->verifyNet();
    if(!netRMTech->isNetOK()) {
        delete netRMTech;
        netRMTech = 0;
        return netRMTech;
    }
    if(e_mon.isNull())
        netRMTech->setFile("");
    else {
        if(!e_mon.text().isEmpty()) {
            if(netRMTech->setFile(e_mon.text())) {
                netRMTech->construct();
                if((netRMTech->isLoadOK()))
                    netRMTech->setNetConnection();
            }
            else
                netRMTech->setFile("");
        }
        else
            netRMTech->setFile("");
    }
    return netRMTech;

}
