/***************************************************************************
                          basesimcontrolfactory.cpp  -  description
                             -------------------
    begin                : Wed Apr 27 2005
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

#include "basesimcontrolfactory.h"

BaseSimControlFactory::BaseSimControlFactory(){
}

BaseSimControlFactory::~BaseSimControlFactory(){
}

// BaseSimControl* BaseSimControlFactory::createSensorGroupControl(SensorGroup* sensorGroup) {
// 
//   SensorGroupControl* sgc = new SensorGroupControl();
//   sgc->setSensorGroup(sensorGroup);
//   sgc->setControlName(sensorGroup->getGroupName());
//   sgc->connectIO();
// 
//   return sgc;
// }

BaseSimControl* BaseSimControlFactory::createMotorControl(AbstractMotor* motor) {

  MotorControl* mc = new MotorControl();
  mc->setMotor(motor);
  mc->setControlName(motor->getMotorName()+"Control");
  mc->connectIO();

  return mc;
}

// BaseSimControl* BaseSimControlFactory::createDriveControl(AbstractMotor* motor) {
// 
//   DriveControl* dc = new DriveControl();
//   dc->setMotor(motor);
//   dc->setControlName("Drive" + motor->getMotorName());
//   dc->connectIO();
// 
//   return dc;
// }
