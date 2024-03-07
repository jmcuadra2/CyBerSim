/***************************************************************************
                          robotbuilder.cpp  -  description
                             -------------------
    begin                : Tue Mar 29 2005
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

#include <QObject>

#include "robotbuilder.h"
#include "robotdrawer.h"
#include "kheperadrawer.h"
#include "robot.h"
#include "sensorsset.h"
#include "sensorsbuilder.h"
#include "pointssampledsensorgroup.h"
#include "totsimulrobotfactory.h"
#include "ndariarobotfactory.h"
#include "nomadrobotfactory.h"
#include "nomadsimrobotfactory.h"
#include "../world/abstractworld.h"
#include "../neuraldis/ndmath.h"
#include "../neuraldis/settings.h"
#include "../neuraldis/xmloperator.h"

RobotBuilder::RobotBuilder(AbstractWorld* wor){

  world = wor;
  xml_operator = XMLOperator::instance();
  factory = 0;   
  
}

RobotBuilder::~RobotBuilder(){
}

Robot* RobotBuilder::construct(QDomElement& e)
{

//   bool ok;
  QString robot_type;
  robot = 0;
  QDomElement e_type;
  QDomElement e_rob_cons;
  QDomElement e_vars;
  QDomElement e_s_groups;
  QDomElement e_s_mode; 
  QDomElement e_kin;
  QDomElement e_file = xml_operator->findTag("robot_file", e);
  QString file_name, old_file_name;
  
  if(e_file.isNull())
    file_name = QObject::tr("Untitled.robot");
  else
    file_name = e_file.text();
  old_file_name = file_name;
  file_name =  Settings::instance()->fillPath(file_name, Settings::instance()->getRobotDirectory());
  if(!xml_operator->insertDocContent(e, file_name, "robot", Robot::tr("Robot file loading")))
    return robot;
  if(!e.isNull()) {  
    e_type = xml_operator->findTag("robot_type", e);
    e_rob_cons = xml_operator->findTag("constructor", e);
    e_vars = xml_operator->findTag("Intrinsic_vars", e);
    e_s_groups = xml_operator->findTag("Sensors_groups", e);
    e_s_mode = xml_operator->findTag("sensorMode", e); 
    e_kin = xml_operator->findTag("Kinetics", e); 
    if(e_type.isNull() || e_rob_cons.isNull() || e_vars.isNull()
          || e_s_groups.isNull())
      return robot;
    robot_type = e_type.text();
  }
  else
    robot_type = "2FixedDrivingWheels"; 
  if(robot_type == "2FixedDrivingWheels")
    factory = new TotSimulRobotFactory();
  else if(robot_type == "AriaRobot")
    factory = new NDAriaRobotFactory();
    else if ( robot_type == NomadSimRobotFactory::TYPE_NAME )
        factory = new NomadSimRobotFactory();
    else if ( robot_type == NomadRobotFactory::TYPE_NAME )
        factory = new NomadRobotFactory();
  factory->setWorldName(world->getFileName());
  
  robot = factory->createRobot(e_rob_cons);

//   if(!robot || (!world && !e.isNull())) {
  if(!robot) {
    delete factory;
    return robot;
  }
  robot->setFileName(file_name);
  
//   if(old_file_name != QObject::tr("Untitled.robot")) { // No se crea simulacion
    if(file_name !=  old_file_name)
      robot->setFileChanged(true);  
    factory->readRobotKinetics(e_kin);
    setIntrinsicVars(e_vars);
    if(!robot) {
      delete factory;
      return robot;
    }
    robot->setMainMotor();
    setSensorsGroups(e_s_groups);
    if(!robot->sensorsSet) {
      delete factory;
      delete robot;
      robot = 0;
      return robot;
    }
    
    if(!e.isNull()) {
      factory->setSensorMode(e_s_mode, true);
      if(factory->setRobotInWorld())
        /*robot->initMotors()*/;
      else {
        delete robot;
        robot = 0;
      }
    }
//   }
  delete factory;
  return robot;
      
}

void RobotBuilder::setIntrinsicVars(const QDomElement& e)
{

  if(!e.isNull()) {
    QDomElement e_size = xml_operator->findTag("size", e);
    QDomElement e_img = xml_operator->findTag("Images", e);
    if(e_size.isNull() || e_img.isNull()) {
      delete robot;
      robot = 0;
    }
    robot->setIntrinsicVars(e_size);
  
    RobotDrawer* robotDrawer = factoryDrawer(robot->objectName());
    if(!robotDrawer->drawRobot(world->getCanvas(), robot, e_img)) {
      delete robot;
      robot = 0;
    }
    else {
    
      robot->setCanvas(world->getCanvas());
      factory->setRobotKinetics();    
    }
    delete robotDrawer;
  }
  
}

void RobotBuilder::setSensorsGroups(QDomElement& e)
{
  
  if(!e.isNull()) {
    robot->sensorsSet = new SensorsSet(robot->getClient());
    robot->sensorsSet->setActualSensorsList(robot->getActualSensorsList());
    SensorsBuilder* sensorsBuilder = new SensorsBuilder();
    sensorsBuilder->setWorld(world);
    sensorsBuilder->setRobotPosition(robot->robotPosition());
    sensorsBuilder->construct(robot->sensorsSet, e);
    robot->setFileChanged(robot->fileChanged() || sensorsBuilder->fileChanged());
    delete sensorsBuilder;
  }

}

RobotDrawer* RobotBuilder::factoryDrawer(const QString& r_name)
{

  RobotDrawer* robotDrawer;

  if(r_name == "Khepera" || r_name == "AmigoBot" )
    robotDrawer = new KheperaDrawer();
  else
    robotDrawer = new RobotDrawer();
  return robotDrawer;   

}
