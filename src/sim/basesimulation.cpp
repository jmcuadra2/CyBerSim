/***************************************************************************
                          basesimulation.cpp  -  description
                             -------------------
    begin                : Wed Nov 10 2004
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

#include <QDir>
#include <QMessageBox>
#include <QSize>
#include <QApplication>
#include <QString>
#include <QColor>
#include <QTextStream>

#include "basesimulation.h"
#include "clocksdispatcher.h"
#include "../neuraldis/ndmath.h"
#include "../neuraldis/xmloperator.h"
#include "../world/rtti.h"
#include "gui/basesimulationdialog.h"
#include "../control/basesimcontrol.h"

BaseSimulation::BaseSimulation(QObject *parent, const char *name)
    : QObject(parent){

    file_name = "";
    real_advance_period = REAL_TIME_ADV_PERIOD;
    sim_advance_period = 1 ;
    type_sim_advper = 2; // maxima del sistema
    advance_timer = 0;
    crono = 0;
    crono_reset = true;
    sim_name_timer = tr("Simulation");
    type_scr_update = 0; // wcanvas->update continuo
    scr_update = 10;
    recording = false;
    duration = 3600000; // 1 hora
    control = 0;
    is_running = 0;
    view_sensors = false;
    has_changed = false;
    noCommandsMode = false;
}

BaseSimulation::~BaseSimulation(){

    if(control)
        delete control;
    if(advance_timer)
        advance_timer->deleteClocks();

}

bool BaseSimulation::edit(int )
{

    if(isRunning())
        return false;

    bool ret = false;
    QewExtensibleDialog* dialog = createDialog();
    if(dialog) {
        addControlTabs(dialog);
        if(dialog->exec() == QewExtensibleDialog::Accepted) {
            ret = true;
            has_changed = true;
        }
        delete dialog;
    }
    return ret;
}

QewExtensibleDialog* BaseSimulation::createDialog(void)
{

    baseSimulationDialog* main_dialog = new baseSimulationDialog();
    if(main_dialog->setUpDialog())
        main_dialog->init((BaseSimulation*) this);
    else {
        delete main_dialog;
        main_dialog = 0;
    }
    return (QewExtensibleDialog*)main_dialog;
}

void BaseSimulation::addControlTabs(QewExtensibleDialog *dialog)
{
    QList<QewExtensibleDialog*> control_tabs = control->tabsForEdit(dialog);
    dialog->addExtensibleChildrenList(control_tabs, "Control");
}

void BaseSimulation::init()
{

    control->initElements();
    emit CronoReset(crono_reset);
    switch(type_sim_advper) {
    case 0:     // real time
        sim_advance_period = 1;
        break;
    case 1:
        //sim_advance_period = dialog->getSimSpeed();
        break;
    case 2:
        sim_advance_period = 0; // mínima del sistema
        break;
    default:
        sim_advance_period = 1;
        break;
    }
    if(type_scr_update && !scr_update)
        emit scrUpdate(false);
    else
        emit scrUpdate(true);

}

void BaseSimulation::save(QTextStream &ts)
{    
    ts << save();
}

QString BaseSimulation::save(void)
{

    XMLOperator* xml_operator = XMLOperator::instance();
    QDomDocument doc("neuraldis_simulation");

    QDomElement root = doc.createElement("Simulation");
    doc.appendChild(root);
    root.appendChild(xml_operator->createTextElement(doc,
                               "simulation_type", QString::number(simulationType())));

    QDomElement tag_sim_elem = doc.createElement("Simulation_elements");
    root.appendChild(tag_sim_elem);

    QDomElement tag_time = doc.createElement("Time_control");
    tag_sim_elem.appendChild(tag_time);

    tag_time.appendChild(xml_operator->createTextElement(doc,
                            "real_advance_period", QString::number(real_advance_period)));
    tag_time.appendChild(xml_operator->createTextElement(doc,
                            "sim_advance_period",  QString::number(sim_advance_period)));
    tag_time.appendChild(xml_operator->createTextElement(doc,
                            "type_sim_advper", QString::number(type_sim_advper)));
    tag_time.appendChild(xml_operator->createTextElement(doc,
                            "crono_reset", QString::number(crono_reset)));
    QTime dur(0, 0);
    dur = dur.addMSecs(duration);
    tag_time.appendChild(xml_operator->
                         createTextElement(doc, "duration", dur.toString()));

    QDomElement tag_srcupd = doc.createElement("Screen_update");
    tag_sim_elem.appendChild(tag_srcupd);

    tag_srcupd.appendChild(xml_operator-> createTextElement(doc,
                                  "scr_update", QString::number(scr_update)));
    tag_srcupd.appendChild(xml_operator->createTextElement(doc,
                                  "type_scr_update", QString::number(type_scr_update)));

    QDomElement tag_control = doc.createElement("Simulation_control");
    tag_sim_elem.appendChild(tag_control);

    if(control->getControlFileName() == tr("Untitled.ctrl"))
        control->save();
    control->write(doc, tag_control);

    save_special(doc, tag_sim_elem);
    has_changed = false;
    return doc.toString(4);

}  

void BaseSimulation::go(void)
{

    is_running = true;
    NDMath::randRenew();
    NDMath::randGaussian(true);

    if(crono_reset) {
        crono = 0;
        advance_timer->reset();
    }
    init();
    emit CronoReset(crono_reset);
    emit initCrono(crono);
    emit Crono(crono);
    emit statusGo(is_running);
    initRecordersMonitors();
    crono += real_advance_period;
    emit Crono(crono);
    setAdvancePeriod(real_advance_period*sim_advance_period);

}

void BaseSimulation::initRecordersMonitors(void)
{    
    control->doMonitor();
}  

void BaseSimulation::step(void)
{

    is_running = true;
    emit statusGo(is_running);
    slotStep();

}

void BaseSimulation::slotStep(void)
{

    advance_timer->startClock("", sim_name_timer);
    crono += real_advance_period;

    advance_timer->startClock("Control", tr("Input"));
    control->sampleInputs();
    advance_timer->pauseClock("Control", tr("Input"));

    advance_timer->startClock("Control", tr("Propagate"));
    control->think();
    advance_timer->pauseClock("Control", tr("Propagate"));

    advance_timer->startClock("Control", tr("Backpropagate"));
    control->backPropagate();
    advance_timer->pauseClock("Control", tr("Backpropagate"));

    advance_timer->startClock("Control", tr("Output"));
    control->sendOutputs();
    advance_timer->pauseClock("Control", tr("Output"));

    advance_timer->startClock("Control", tr("Monitor"));
    control->doMonitor();
    advance_timer->pauseClock("Control", tr("Monitor"));

    advance_timer->pauseClock("Control", sim_name_timer);

    if(crono > duration)
        stop();
    else
        emit Crono(crono);

}

void BaseSimulation::pause(void)
{   
    setAdvancePeriod(-1);
}

void BaseSimulation::stop(void)
{

    BaseSimulation::pause();
    stopReset();
    is_running = false;
    emit statusGo(is_running);

}

void BaseSimulation::stopReset(void)
{  
    advance_timer->write();
}

void BaseSimulation::setAdvancePeriod(int ms)
{

    if ( ms<0 )
        advance_timer->stop();
    else
        advance_timer->start(ms);

}

bool BaseSimulation::isRecording(void)
{   
    return recording;
}

void BaseSimulation::setRecording(bool on)
{

    if(recording == on) return;
    recording = on;
    control->setRecording(on, crono_reset);

}                                        

int BaseSimulation::simulationType(void) const
{   
    return BaseSimulation::BASE_SIM;
}

void BaseSimulation::setClocks(void)
{

    advance_timer->addClock("Control", tr("Propagate"));
    advance_timer->addClock("Control", tr("Backpropagate"));
    advance_timer->addClock("Control", tr("Output"));
    advance_timer->addClock("Control", tr("Control monitor"));

}

void BaseSimulation::setDuration(int ms)
{   
    duration = ms > max_duration ? max_duration : ms;
}

int BaseSimulation::getDuration(void)
{    
    return duration;
}

bool BaseSimulation::isRunning(void)
{    
    return is_running;
}  

bool BaseSimulation::getViewSensors(void) 
{ 
    return view_sensors;
}

QList<QString> BaseSimulation::getSensorGroupsNames()
{
    QList<QString> list;
    return list;
}

void BaseSimulation::recordSensorsDialog(bool /*on*/)
{
}
