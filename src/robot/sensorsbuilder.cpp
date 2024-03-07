/***************************************************************************
                          sensorsbuilder.cpp  -  description
                             -------------------
    begin                : Thu Jul 21 2005
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

#include "sensorsbuilder.h"
#include "sensorsset.h"
#include "pointssampledsensorgroup.h"
#include "radialfieldsensorgroup.h"
#include "ariasensorgroup.h"
#include "linegroup.h"
#include "nomadsensorgroup.h"
#include "../world/abstractworld.h"
#include "../neuraldis/settings.h"
#include "../neuraldis/xmloperator.h"
#include "../neuraldis/neuraldisdoc.h"

SensorsBuilder::SensorsBuilder()
{

  sensorsSet = 0;
  world = 0;
  robot_position = 0;
  file_changed = false;

}

SensorsBuilder::~SensorsBuilder(){
}

void SensorsBuilder::setWorld(AbstractWorld* world_)
{

  world = world_;

}

void SensorsBuilder::setRobotPosition(RobotPosition* robot_pos)
{

  robot_position= robot_pos;

}

void SensorsBuilder::construct(SensorsSet* set, QDomElement& e)
{

  sensorsSet = set;
  QDomNode n = e.firstChild();
  while(!n.isNull()) {
    QDomElement e_part = n.toElement();
    if(e_part.isNull()) {
      n = n.nextSibling();
      continue;
    }
    if(e_part.tagName().contains("_sensors"))
      factorySensorGroup(e_part);
    n = n.nextSibling();
  }

}

void SensorsBuilder::factorySensorGroup(QDomElement& e)
{

  bool ok;
  XMLOperator* xml_operator = XMLOperator::instance();
  QString groupName = e.tagName();
  QDomElement e_file = xml_operator->findTag("sensor_file", e);
  SensorGroup *sensor_group = nullptr;
//   if(e_file.isNull()) {
//     delete sensorsSet;
//     sensorsSet = 0;
//     return;
//   }
  QString file_name = e_file.text();
  file_name = Settings::instance()->fillPath(file_name, Settings::instance()->getRobotDirectory());
  
//   QTextStream lTS(stdout);
//   lTS << e;
  
  if(!xml_operator->insertDocContent(e, file_name, "sensor", SensorsSet::tr("Sensor file loading"))) {
//     delete sensorsSet;
//     sensorsSet = 0;
    return;
  }

  if(file_name != e_file.text()) {
    file_changed = true;
    xml_operator->changeTextInElement(e_file, file_name);
  }
//   lTS << e;
//   lTS << endl << endl;
  QDomElement e_super = xml_operator->findTag("sensor_supertype", e);
  if(e_super.isNull()) {
//     delete sensorsSet;
//     sensorsSet = 0;
    return;
  }
  sensor_supertype = e_super.text().toInt(&ok);
  if(sensor_supertype == AbstractSensor::POINTS_SAMPLED) 
    sensor_group = (SensorGroup *)new PointsSampledSensorGroup(e);
  else if(sensor_supertype == AbstractSensor::RADIAL_FIELD) 
    sensor_group = (SensorGroup *)new RadialFieldSensorGroup(e);
  else if(sensor_supertype == AbstractSensor::ARIA_SENSOR) {
    void* v_client = sensorsSet->getClient();
    ArClientBase* robot_client = (ArClientBase*) (v_client);
    if(robot_client)
      sensor_group = (SensorGroup *)new AriaSensorGroup(e, robot_client, sensorsSet->getActualSensorsList());
  } 
  else if(sensor_supertype == AbstractSensor::LINEAR_SENSOR) 
    sensor_group = (SensorGroup *)new LineGroup(e);
  else if(sensor_supertype == AbstractSensor::NOMAD_SENSOR) 
    sensor_group = (SensorGroup *)new NomadSensorGroup(e,sensorsSet->getClient());
  else
    cerr << "Warning: SensorsBuilder::factorySensorGroup() unknown sensor supertype" << endl;
  
  if(sensor_group) {
//     QDomElement e_name = xml_operator->findTag("group_name", e);
//     sensor_group->setGroupName(e_name.text());
    sensor_group->setGroupName(groupName);
    sensor_group->setRobotPosition(robot_position);
    if(!sensor_group->createSensors())
      delete sensor_group;
    else {
      sensor_group->setInRobotPosition();
//       sensor_group->setIOControlMessages();
      sensorsSet->addSensorGroup(e.tagName(), sensor_group);
    }
  }

}
