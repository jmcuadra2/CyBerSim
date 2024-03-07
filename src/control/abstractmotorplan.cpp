//
// C++ Implementation: abstractmotorplan
//
// Description:
//
//
// Author: Jose Manuel Cuadra Troncoso <jmcuadra@dia.uned.es>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "abstractmotorplan.h"

AbstractMotorPlan::AbstractMotorPlan()
{
}

AbstractMotorPlan::~AbstractMotorPlan()
{
}

void AbstractMotorPlan::initilizaOutputCommands(MotorCommand const& command, size_t memorySize)
{
  for(int i = 0; i < memorySize; i++) 
    prevOutputCommands.append(command);
}
