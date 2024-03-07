/***************************************************************************
       robotsimulationfactory.cpp  -  description
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

#include <iostream>


#include "robotsimulationfactory.h"
#include "ariasimulation.h"
#include "clocksdispatcher.h"
#include "../robot/robot.h"
#include "../robot/sensorsset.h"
#include "../robot/robotbuilder.h"
#include "../robot/robotmonitorstechnician.h"
#include "../world/world.h"
#include "../world/worldview.h"
#include "../neuraldis/windowoperations.h"
#include "../neuraldis/settings.h"
#include "../control/basesimcontrol.h"
#include "../control/codecontrolfactory.h"
#include "../control/smginterface.h"

RobotSimulationFactory::RobotSimulationFactory()
{

  robot = 0;
  r_sim = 0;  
  
}

RobotSimulationFactory::~RobotSimulationFactory(){
}

BaseSimulation* RobotSimulationFactory::factorySim(int sim_type)
{
  
  if(sim_type == BaseSimulation::ROBOT_SIM) {
    r_sim = new RobotSimulation();
    setSimBase(r_sim);
  }
  else if(sim_type == BaseSimulation::ARIA_SIM) {
    r_sim = new AriaSimulation();
    setSimBase(r_sim);
  } 
  return (BaseSimulation*) r_sim;
   
}

BaseSimulation* RobotSimulationFactory::factorySim(int sim_type,
                      const QDomElement& e, const QString& sim_name)
{
  
  factorySim(sim_type);
  info_sim = e;  
  r_sim->setFileName(sim_name);

  return (BaseSimulation*) r_sim;
   
}

bool RobotSimulationFactory::construct(void)
{

  WindowOperations* w_oper = WindowOperations::instance();
  Settings *prog_settings = Settings::instance();
  bool ret = false;
  bool ok;
  QDomElement e_rob;
  QDomElement e_geom;
  QDomElement e_mon;
  QDomElement e_prob;
  
  
  AbstractWorld* world = prog_settings->getWorld();   
  if(!world) {
    need_world = true;
    return ret;
  }  
  r_sim->setWorld_Settings(world, prog_settings);     
  if(!info_sim.isNull()) {
    if(info_sim.hasChildNodes()) {
/*      e_geom = xml_operator->findTag("World_geometry", info_sim);
      AbstractWorld* world = prog_settings->getWorld();   
      if(!world) {
        need_world = true;
        return ret;
      }*/        
      /*r_sim->setWorld_Settings(world, prog_settings); */     
      e_rob = xml_operator->findTag("Robot", info_sim);
      e_mon = xml_operator->findTag("Robot_monitoring", info_sim);
      if(e_rob.isNull())
        return ret;
      e_prob = xml_operator->findTag("World_probabil", info_sim);
      if(!e_prob.isNull()) {
        double lambda = e_prob.attribute("lambda").toDouble(&ok);
        double w_short = e_prob.attribute("w_short").toDouble(&ok);
/*        double w_rand = e_prob.attribute("w_rand").toDouble(&ok);
        double a_hit = e_prob.attribute("a_hit").toDouble(&ok);
        if (!ok) a_hit = 90/NDMath::RAD2GRAD;
        double a_max = e_prob.attribute("a_max").toDouble(&ok);
        if (!ok) a_max = 90/NDMath::RAD2GRAD;
        double transparency = e_prob.attribute("transparency").toDouble(&ok);
        if (!ok) transparency = 0; */       
        
//         world->setProbabilisticParams(w_rand, lambda, w_short, a_hit, a_max, transparency);
        world->setProbabilisticParams(lambda, w_short);
      }
      else
//         world->setProbabilisticParams(0, 0, 0, 90/NDMath::RAD2GRAD, 90/NDMath::RAD2GRAD, 0);
        world->setProbabilisticParams(0, 0);
    }
    else {
      e_rob = info_sim.ownerDocument().createElement("Robot");
      info_sim.appendChild(e_rob);
    }
  }
  
  if(!BaseSimulationFactory::construct())
    return ret;
  r_sim->robot = buildRobot(e_rob);
 
  if(!r_sim->robot)
    return ret;
  else
    ret = true;
  if(!e_geom.isNull()) {
    QWidget* w = r_sim->world->view();
    w_oper->setPosSize(w, e_geom);
  }
  buildRobotMonitors(e_mon);   
  ConnectRobotToControl(r_sim->control);
  robot->readSensors();
  setClocks();  
  return ret;

}

Robot* RobotSimulationFactory::buildRobot(QDomElement& e)
{

  RobotBuilder* robotBuilder = new RobotBuilder(r_sim->world);
  robot = robotBuilder->construct(e);
  delete robotBuilder;
  if(!robot) return robot;
  QObject::connect(robot, SIGNAL(pause(void)), r_sim, SLOT(pause(void)));
  QObject::connect(robot, SIGNAL(outWorld()), r_sim, SLOT(pauseOutWorld()));

  return robot;

}


// void RobotSimulationFactory::setWorldPhysics(const QDomElement& e)
// {
// 
// //   bool ok;
// //   QDomElement e_inrt = xml_operator->findTag("inertia", e);
// //   sim->type_iner = e_inrt.text().toInt(&ok);
//   
// }

void RobotSimulationFactory::setClocks(void)
{

  r_sim->real_advance_period = r_sim->robot->getAdvancePeriod();
  r_sim->init_lvel = r_sim->robot->getInitLeftVel();
  r_sim->init_rvel = r_sim->robot->getInitRightVel();
  BaseSimulationFactory::setClocks();

}

void RobotSimulationFactory::ConnectRobotToControl(BaseSimControl* control)
{

//  if(control->needSensors())  por implementar leyendo xml
//     control->appendSubcontrols(robot->getSensorsSet()->getSensorGroupControls());

  SMGInterface* smgInterface;
  if(!control->createSMGInterface()) {
    cerr << "RobotSimulationFactory::ConnectRobotToControl() can't assign SMGInterface" << endl;
    return;
  }

  smgInterface = control->getSMGInterface();
  smgInterface->setRobot(robot);
//   smgInterface->setSensorsSet(robot->getSensorsSet());
  
/*  // por implementar leyendo xml
  DifferentialMotor* motor = new DifferentialMotor("MainMotor", robot);
  motor->setWheelsSeparation(robot->getWheelsSep());
  robot->addMotor("MainMotor", motor);*/  
//   robot->getMainControl()->appendSubcontrol(control);

//   smgInterface->setMotor(robot->getMainMotor());
  
//   DriveControl* drive = dynamic_cast<DriveControl*>(control->getSubcontrol(0));
//   QObject::connect(robot, SIGNAL(sendKeyEvent(QKeyEvent*, bool )), smgInterface->getDrive(), SLOT(getKeyEvent(QKeyEvent* , bool )));
//   QObject::connect(robot, SIGNAL(sendDrived( bool )), smgInterface->getDrive(), SLOT(activate( bool )));
  control->connectIO();
  
}                          

void RobotSimulationFactory::buildRobotMonitors(QDomElement& e)
{

  QString f_name = "";
  bool initLeds = true;
  QDomElement e_file, e_leds;
  if(!e.isNull()) {
    e_file = xml_operator->findTag("robot_monrec_file", e);
    if(!e_file.isNull())
      f_name = e_file.text();
    e_leds = xml_operator->findTag("initialLED", e);
    if(!e_leds.isNull())
      initLeds = (bool)e_leds.text().toInt();    
  }   

  RobotMonitorsTechnician* monitorsTech = new RobotMonitorsTechnician(robot, r_sim->world->getWorkspace());
  if(initLeds)
    monitorsTech->constructLeds();
  QObject::connect(r_sim->world, SIGNAL(drawingsIdent(QList<int> )), monitorsTech, SLOT(constructLeds(QList< int >)));
  if(monitorsTech->setFile(e_file.text())) {
    monitorsTech->constructGraphMons();
    monitorsTech->constructRecorder();
  }
  r_sim->monitorsTech = monitorsTech;  
          
}
