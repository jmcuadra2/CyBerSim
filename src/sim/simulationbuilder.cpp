/***************************************************************************
                          simulationbuilder.cpp  -  description
                             -------------------
    begin                : Wed Apr 6 2005
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

#include <QDir>

#include "simulationbuilder.h"
#include "robotsimulation.h"
#include "robotsimulationfactory.h"
#include "clocksdispatcher.h"
#include "../world/world.h"
#include "../neuraldis/settings.h"
#include "../neuraldis/xmloperator.h"

SimulationBuilder::SimulationBuilder()
{

  sim = 0;
  need_world = false;

}
SimulationBuilder::~SimulationBuilder(){
}

BaseSimulation* SimulationBuilder::construct(QDomDocument& doc,
      const QString& f_name)
{

  bool ok;
  XMLOperator* xml_operator = XMLOperator::instance();
  file_name = f_name;
  QDomNode n = doc.firstChild();
  if(n.isNull()) return sim;
  QDomElement e_root = n.toElement();
  if(e_root.isNull()) return sim;
  if(e_root.tagName() != "Simulation") return sim;
  QDomElement e_type = xml_operator->findTag("simulation_type", e_root);
  QDomElement e_elems =
                xml_operator->findTag("Simulation_elements", e_root);
  if(e_type.isNull() || e_elems.isNull())
    return sim;
  int sim_type = e_type.text().toInt(&ok);  
  sim = factorySimulation(sim_type, e_elems);
//  cout << doc.toString(4);  
  return sim;
  
}

BaseSimulation* SimulationBuilder::construct(QDomDocument& doc)
{

  XMLOperator* xml_operator = XMLOperator::instance();
  QDomElement simulation_classes;
  DictionaryManager* dict_manager = DictionaryManager::instance();    
  NameClass_Map names_list;
  bool ret = dict_manager->readFileConf("newSimulation", "BaseSimulation", simulation_classes);
  if(ret) { 
    dict_manager->getNamesMap(simulation_classes, names_list);       
    int sim_type = dict_manager->chooseClass(names_list, QObject::tr("Choose a type of simulation"));
    
    QDomElement e_root = doc.createElement("Simulation");
    doc.appendChild(e_root); 
    e_root.appendChild(xml_operator->createTextElement(doc, "simulation_type", QString::number(sim_type)));
               
    QDomElement e_elems = doc.createElement("Simulation_elements");
    e_root.appendChild(e_elems);     
 
    sim = factorySimulation(sim_type, e_elems);
  } 
  return sim;
  
}

BaseSimulation* SimulationBuilder::
              factorySimulation(int sim_type, QDomElement& e)
{

  if(sim_type == BaseSimulation::ROBOT_SIM || sim_type == BaseSimulation::ARIA_SIM) {
    RobotSimulationFactory* factory = new RobotSimulationFactory();
    sim = factory->factorySim(sim_type, e, file_name);
    if(!factory->construct()) {
      need_world = factory->needWorld();
      delete sim;
      sim = 0;
    }
    delete factory;
  }                       
  return sim;
  
}
