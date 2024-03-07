//
// C++ Implementation: outputmotorplan
//
// Description: 
//
//
// Author: Jose Manuel Cuadra Troncoso <jmcuadra@dia.uned.es>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//


#include <QtGui>

#include "outputmotorplan.h"
#include "../robot/abstractmotor.h"
#include "../neuraldis/ndpose2d.hpp"
#include "../neuraldis/ndmath.h"

OutputMotorPlan::OutputMotorPlan(AbstractMotor* motor)
 : AbstractMotorPlan()
{
  this->motor = motor;
//   priority = 1;
}

OutputMotorPlan::~OutputMotorPlan()
{
}

void OutputMotorPlan::setMotor(AbstractMotor* motor)
{ 
  this->motor = motor;
//   messages.copyFields(motor->getMessage());

  
// // //   driver = dynamic_cast<DriveControl*>(BaseSimControlFactory::createDriveControl(motor)); 
}

void OutputMotorPlan::sendCommand(void)
{
//   MotorCommand& command = commands.at(0);
  if(commands.isEmpty()) return;
  double lvel, rvel;
  
  MotorCommand command;
  QList<double> priorities;
  QList<double> rvels;
  QList<double> lvels;
  QList<size_t> idxs;

//   command = commands.at(0);
  double pri, lv, rv;
  int i;
  for(i = 0; i < commands.size(); i++) {
    priorities.append(commands.at(i).getPriority());
    pri = commands.at(i).getPriority();
    NDPose* commandValue = commands.at(i).getCommandValue();
    lvels.append(commandValue->getPosition().at(0));
    rvels.append(commandValue->getPosition().at(1));
    lv = commandValue->getPosition().at(0);
    rv = commandValue->getPosition().at(1);
  }

  idxs = NDMath::sortContainer(priorities, false);

  command.setCommandId(MotorCommand::WheelsVel);
  if(i)
    command.setPriority(priorities.at(0));
  else
    command.setPriority(1);
  
  double acumPriority = 0;
  int indexAcumPriority = -1;
  for(int i = 0; i < commands.size(); i++) {
    acumPriority += priorities.at(i);
    if(acumPriority >= 1) {
      indexAcumPriority = i;
      break; 
    }
  }

  if(indexAcumPriority == -1)
    indexAcumPriority = commands.size() - 1; // si size= 0 --> indexAcumPriority == -1
  double mean_lvel = 0;
  double mean_rvel = 0;
  double prod;
  
  for(int i = 0; i <= indexAcumPriority; i++) {
    priorities[i] /= acumPriority; // --> acumPriority = 1;
    pri = priorities[i];
    lv = lvels.at(idxs.at(i));
    rv = rvels.at(idxs.at(i));
    prod = lvels.at(idxs.at(i))*priorities.at(i);
    mean_lvel += prod;

    prod = rvels.at(idxs.at(i))*priorities.at(i);
    mean_rvel += prod;
  }

  if(indexAcumPriority == -1) { // no hay commands.size()
    mean_rvel = 0;
    mean_lvel = 0;
  }

  NDPose* commandValue = new NDPose2D();

  commandValue->setCoordinates(mean_lvel, mean_rvel, 0);
  command.setCommandValue(commandValue, false);
  int po = prevOutputCommands.size();
  if(prevOutputCommands.size()) {
    MotorCommand prevCommand = prevOutputCommands.takeLast();
    if(prevCommand.getCommandValue() && (command.getPriority() < 0.5)) {
      lvel = command.getCommandValue()->getPosition().at(0)*.25 + prevCommand.getCommandValue()->getPosition().at(0)*.75;
      rvel = command.getCommandValue()->getPosition().at(1)*.25 + prevCommand.getCommandValue()->getPosition().at(1)*.75;
    }
    else {
      lvel = command.getCommandValue()->getPosition().at(0);
      rvel = command.getCommandValue()->getPosition().at(1);
    }
  }
  else {
    lvel = command.getCommandValue()->getPosition().at(0);
    rvel = command.getCommandValue()->getPosition().at(1);
  }
  
  switch(command.getCommandId()) {
    case MotorCommand::Stop:
       lvel = 0;
       rvel = 0;
       motor->setVel1(lvel);
       motor->setVel2(rvel);
       break;
    case MotorCommand::PolarVel:  // cambiar
       motor->setVel1(command.getCommandValue()->getPosition().at(0));
       motor->setVel2(command.getCommandValue()->getPosition().at(1));
       break;
    case MotorCommand::WheelsVel:
       motor->setVel1(lvel);
       motor->setVel2(rvel);
       break;
    case MotorCommand::CartesianVel:
       break;
    case MotorCommand::Pose:
       break;                         
  }

  commandValue->setCoordinates(lvel, rvel, 0);
  command.setCommandValue(commandValue, false);  
  prevOutputCommands.push_front(command);
  commands.clear();

}

void OutputMotorPlan::getInitialCommand(double& vel1, double& vel2)
{
  vel1 = motor->getInitVel1();
  vel2 = motor->getInitVel2();
}

void OutputMotorPlan::addCommand(MotorCommand const& command)
{
  commands.push_front(command);
}

void OutputMotorPlan::initilizaOutputCommands(MotorCommand const& command, size_t memorySize)
{
  prevOutputCommands.clear();
  for(size_t i = 0; i < memorySize; i++)
    prevOutputCommands.push_front(command);
}
