//
// C++ Interface: abstractmotorplan
//
// Description:
//
//
// Author: Jose Manuel Cuadra Troncoso <jmcuadra@dia.uned.es>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef ABSTRACTMOTORPLAN_H
#define ABSTRACTMOTORPLAN_H

#include <QList>
#include "motorcommand.h"

using namespace std;

/**
  @author Jose Manuel Cuadra Troncoso <jmcuadra@dia.uned.es>
*/
class AbstractMotorPlan
{
    
  public:
    AbstractMotorPlan();

    virtual ~AbstractMotorPlan();

    virtual void addCommand(MotorCommand const& command) { commands.push_front(command) ; }
    QList<MotorCommand>& getCommands(void) { return commands ; }
    void initilizaOutputCommands(MotorCommand const& command, size_t memorySize);

  protected:
    QList<MotorCommand> commands;
    QList<MotorCommand> prevOutputCommands;
    
};

#endif
