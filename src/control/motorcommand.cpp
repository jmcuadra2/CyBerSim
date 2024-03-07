//
// C++ Implementation: motorcommand
//
// Description: 
//
//
// Author: Jose Manuel Cuadra Troncoso <jmcuadra@dia.uned.es>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//



#include "../neuraldis/ndpose.hpp"
#include "motorcommand.h"

MotorCommand::MotorCommand() : commandValue(0)
{
}

MotorCommand::MotorCommand(MotorCommand const& motorcommand) : priority(motorcommand.priority), commandId(motorcommand.commandId), commandValue(motorcommand.commandValue->deepCopy())
{
}

void MotorCommand::operator =(MotorCommand const& motorcommand)
{
  if(NDPose::safeCopy(motorcommand.commandValue, this->commandValue)) {
    priority = motorcommand.priority;
    commandId = motorcommand.commandId;
  }
}

MotorCommand::~MotorCommand()
{
  if(commandValue)
    delete commandValue;
}

void MotorCommand::setCommandValue(NDPose* commandValue, bool copyPose)
{
  NDPose::safeCopy(commandValue, this->commandValue, copyPose);
}
