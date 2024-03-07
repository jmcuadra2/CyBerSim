/***************************************************************************
                          basesimcontrol.cpp  -  description
                             -------------------
    begin                : Fri Apr 29 2005
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

#include <QMessageBox>

#include "basesimcontrol.h"
#include "smginterface.h"
#include "../neuraldis/xmloperator.h"
#include "../neuraldis/neuraldisdoc.h"
#include "../neuraldis/settings.h"

#include <iostream>

using namespace std;

BaseSimControl::BaseSimControl() : QObject()
{

    motorPlan = new AbstractMotorPlan();
    smgInterface = 0;

    file_changed = false;
    hasView = false;
    sampling_period = 0.02;
    file_changed = false;
    control_filename = "Untitled.ctrl";
    controlName = "BaseControl";
    active = true;

}

BaseSimControl::~BaseSimControl()
{
    QHashIterator<QString, QVector< AbstractFusionNode* > > it(inputNodeMap);
    while (it.hasNext()) {
        it.next();
        QVector< AbstractFusionNode* > nodes = it.value();
        int num_nodes = nodes.size();
        for(int i = 0; i < num_nodes; i++)
            delete nodes[i];
    }
    if(motorPlan)
        delete motorPlan;
    if(smgInterface)
        delete smgInterface;

}

void BaseSimControl::setControlDoc(QDomElement& )
{

}

void BaseSimControl::sampleInputs(void)
{
    QHashIterator<QString, QVector< AbstractFusionNode* > > it(inputNodeMap);
    while (it.hasNext()) {
        it.next();
        QVector< AbstractFusionNode* > nodes = it.value();
        int num_nodes = nodes.size();
        for(int i = 0; i < num_nodes; i++) {
            AbstractFusionNode* node = nodes[i];
            node->doFusion();
        }
    }

}

void BaseSimControl::think(void) {

    readInputs();
    propagate();

}

void BaseSimControl::appendSubcontrol(BaseSimControl* subcontrol) 
{ 
    subcontrols.append(subcontrol);
}

void BaseSimControl::appendSubcontrols(QList<BaseSimControl*> subcontrols) 
{ 
    this->subcontrols += subcontrols;
} 

void BaseSimControl::write(QDomDocument& doc, QDomElement& e)
{

    if(file_changed) {
//        if(QMessageBox::information(nullptr, QObject::tr("Control save"),
//                                    QObject::tr("Control definitions have changed.") + "\n" + QObject::tr("Save ?"),
//                                    QObject::tr("&Ok"), QObject::tr("&Cancel")) == 0)
        if(QMessageBox::information(nullptr, QObject::tr("Control save"),
                                     QObject::tr("Control definitions have changed.") + "\n" + QObject::tr("Save ?"), QMessageBox::Ok |QMessageBox::Cancel) == QMessageBox::Ok)
            save();
        file_changed = false;
    }

    XMLOperator* xml_operator = XMLOperator::instance();
    QString relativeName = control_filename;
    e.appendChild(xml_operator->createTextElement(doc, "control_file",
                  relativeName.remove(Settings::instance()->getControlDirectory() + QDir::separator())));

}

void BaseSimControl::save(void)
{

    XMLOperator* xml_operator = XMLOperator::instance();
    NeuralDisDoc* doc_manager = NeuralDisDoc::instance();

    QDomDocument doc_control("neuraldis_control");
    QDomElement info_control = doc_control.createElement("Simulation_control");
    doc_control.appendChild(info_control);

    info_control.appendChild(xml_operator->createTextElement(doc_control, "super_type",
                                                             QString::number(getSuperType())));
    info_control.appendChild(xml_operator->createTextElement(doc_control, "control_type",
                                                             QString::number(getType())));

    save_special(doc_control, info_control);

    QString full_name =  Settings::instance()->fillPath(control_filename, Settings::instance()->getControlDirectory());
    QString extens = Settings::instance()->getControlExtension();
    extens = extens.contains("*.") ? extens.right(extens.length() - 2) : extens;
    QString ret_name = doc_manager->saveDocument(full_name, doc_control.toString(4), "neuraldis_control", extens, QObject::tr("Saving control"), true);
    if(!ret_name.isEmpty())
        control_filename = ret_name;

} 

void BaseSimControl::save_special(QDomDocument& , QDomElement& ) 
{

}

void BaseSimControl::setSamplingPeriod(double s_period)
{
    if(s_period > 0)
        sampling_period = s_period;
}

void BaseSimControl::setDefaultCommand(NDPose* const  commandValues)
{
    defaultCommand.setCommandValue(commandValues);
}

bool BaseSimControl::setSMGInterface(SMGInterface * smgInterface)
{
    bool ret = false;
    if(!this->smgInterface) {
        this->smgInterface = smgInterface;
        ret = true;
    }
    else
        cerr << "Warning: NetSimControl::setSMGInterface()() getSMGInterface() already assigned" << endl;
    return ret;
}

void BaseSimControl::activate(bool on)
{
    if(active != on) {
        active = on;
        emit activation(on);
    }
}
