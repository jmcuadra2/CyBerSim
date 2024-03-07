/***************************************************************************
                          basesimulationfactory.cpp  -  description
                             -------------------
    begin                : Thu Apr 21 2005
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

#include "basesimulationfactory.h"
#include "basesimulation.h"
#include "clocksdispatcher.h"
#include "../neuraldis/settings.h"
#include "../neuraldis/xmloperator.h"
#include "../disview/neuraldisview.h"
#include "../control/basesimcontrol.h"
#include "../control/basesimcontrol.h"
#include "../control/codecontrolfactory.h"
#include "../control/netsimcontrolfactory.h"

BaseSimulationFactory::BaseSimulationFactory(){

  xml_operator = XMLOperator::instance();
  sim = 0;
  need_world = false;
  
}
BaseSimulationFactory::~BaseSimulationFactory(){
}


bool BaseSimulationFactory::construct(void)
{

//   bool ok;
  bool ret = false;
  if(!sim) return ret;
      
//   QDomElement e_noise = xml_operator->findTag("noise", info_sim);
  if(!info_sim.isNull()) {
    QDomElement e_time = xml_operator->findTag("Time_control", info_sim);
    QDomElement e_scr = xml_operator->findTag("Screen_update", info_sim);
    QDomElement e_ctrl = xml_operator->
                                  findTag("Simulation_control", info_sim);
    if(!e_ctrl.isNull()) {
//       return ret;
  
  //   sim->noise = e_noise.text().toInt(&ok);
      setTimeControl(e_time);
      setScreenUpdate(e_scr);
      sim->control = constructControl(e_ctrl);
    }
    else
      sim->control = constructControl();
  }

  if(!sim->control)
    return ret;
  sim->control->setSamplingPeriod(sim->real_advance_period/1000.0);
//  setClocks();
  ret = true;
  return ret;
  
}     

//void BaseSimulationFactory::construct(BaseSimulation* b_sim,
//                                      QDomElement& e)
//{
//
//  sim = b_sim;
//  bool ok;
//  QDomElement e_noise = xml_operator->findTag("noise", e);
//  QDomElement e_time = xml_operator->findTag("Time_control", e);
//  QDomElement e_scr = xml_operator->findTag("Screen_update", e);
//  QDomElement e_ctrl = xml_operator->findTag("Simulation_control", e);
//  if(e_ctrl.isNull()) {
//    delete sim;
//    sim = 0;
//    return;
//  }
//  sim->noise = e_noise.text().toInt(&ok);
//  setTimeControl(e_time);
//  setScreenUpdate(e_scr);
//  sim->control = constructControl(e_ctrl);
//  if(!sim->control) {
//    delete sim;
//    sim = 0;
//    return;
//  }
////  setClocks();
//
//}

void BaseSimulationFactory::setTimeControl(const QDomElement& e)
{

  bool ok;
  QDomElement e_r_adv = xml_operator->findTag("real_advance_period", e);
  QDomElement e_s_adv = xml_operator->findTag("sim_advance_period", e);
  QDomElement e_t_adv = xml_operator->findTag("type_sim_advper", e);
  QDomElement e_cr = xml_operator->findTag("crono_reset", e);
  QDomElement e_dur = xml_operator->findTag("duration", e);

  sim->real_advance_period = e_r_adv.text().toInt(&ok);
  sim->sim_advance_period = e_s_adv.text().toInt(&ok);
  sim->type_sim_advper = e_t_adv.text().toInt(&ok);
  sim->crono_reset = bool(e_cr.text().toInt(&ok));
  QTime dur(0, 0);
  QTime duration = QTime::fromString(e_dur.text());
  if(duration.isValid())
    sim->duration = dur.msecsTo(duration);
  
}

void BaseSimulationFactory::setClocks(void)
{

  sim->advance_timer = ClocksDispatcher::instance();
  sim->advance_timer->setRealAdvPeriod(sim->real_advance_period);
  sim->advance_timer->setFileName(sim->file_name);
  
  QObject::connect(sim->advance_timer,SIGNAL(timeout()),
                sim, SLOT(slotStep()));
  QObject::connect(sim, SIGNAL(Crono(int)),
                  sim->advance_timer, SIGNAL(Crono(int)));
  QObject::connect(sim, SIGNAL(initCrono(int)),
                  sim->advance_timer, SIGNAL(initCrono(int)));
  QObject::connect(sim, SIGNAL(CronoReset(bool)),
                  sim->advance_timer, SIGNAL(CronoReset(bool)));
  sim->setClocks();

}

BaseSimControl* BaseSimulationFactory::
          constructControl(QDomElement& e)
{

  BaseSimControl* base_control = 0;
  BaseSimControlFactory* factory = 0;
  Settings* prog_settings = Settings::instance();  
  bool ok;
  bool control_file_changed = false;
  
  QDomElement e_file = xml_operator->findTag("control_file", e);
//   if(e_file.isNull()) return base_control;
  QString file_name = prog_settings->fillPath(e_file.text(), prog_settings->getControlDirectory());     
  if(!xml_operator->insertDocContent(e, file_name, "neuraldis_control", Settings::tr("Control file loading")))
    return base_control;  
  if(file_name != e_file.text()) {
    control_file_changed = true;
    xml_operator->changeTextInElement(e_file, file_name);
  }

  QDomElement e_super = xml_operator->findTag("super_type", e);
  QDomElement e_type = xml_operator->findTag("control_type", e);
  if(e_super.isNull() || e_type.isNull())
    return base_control;
  int super_type = e_super.text().toInt(&ok);
  int control_type = e_type.text().toInt(&ok);
    
  if(super_type == BaseSimControl::CODE_CONTROL) {
    factory = new CodeControlFactory();
    base_control = factory->construct(control_type, e, file_name);
  }
  else if(super_type == BaseSimControl::NET_CONTROL) {
    factory = new NetSimControlFactory();
    base_control = factory->construct(control_type, e, file_name);

    if(prog_settings->getNetView()) {
      QObject::connect(sim, SIGNAL(scrUpdate(bool)),
              prog_settings->getNetView(), SLOT(simConnection(bool)));
      QObject::connect(sim, SIGNAL(initCrono(int)),
              prog_settings->getNetView(), SLOT(update(void)));
      QObject::connect(sim, SIGNAL(statusGo(bool)),
              prog_settings->getNetView(), SLOT(setSimView(bool)));  
    }         
  }
  if(base_control) {
    if(control_file_changed) 
      base_control->setFileChanged(true);
    base_control->setControlFileName(file_name);
    base_control->setControlDoc(e);
    
  }
  delete factory;
  return base_control;

}

BaseSimControl* BaseSimulationFactory::
          constructControl(void)
{

  BaseSimControl* base_control = 0;
  BaseSimControlFactory* factory = 0;
  QDomElement control_classes;
  DictionaryManager* dict_manager = DictionaryManager::instance();    
  NameClass_Map names_list;
  
  bool ret = dict_manager->readFileConf("newControl", "BaseSimControl", control_classes);
  if(ret) {
    dict_manager->getNamesMap(control_classes, names_list, true);       
    int super_type = dict_manager->chooseClass(names_list, QObject::tr("Choose a supertype of control")); 
    
    if(super_type == BaseSimControl::CODE_CONTROL) {
      factory = new CodeControlFactory();
      base_control = factory->construct();
    }
    else if(super_type == BaseSimControl::NET_CONTROL) {
      factory = new NetSimControlFactory();
      base_control = factory->construct();    
    } 
  }
  delete factory;
  return base_control;  
  
}  

void BaseSimulationFactory::setScreenUpdate(const QDomElement& e)
{

  bool ok;
  QDomElement e_upd = xml_operator->findTag("scr_update", e);
  QDomElement e_t_upd = xml_operator->findTag("type_scr_update", e);  
  sim->scr_update = e_upd.text().toInt(&ok);
  sim->type_scr_update = e_t_upd.text().toInt(&ok);

}    

