//
// C++ Interface: outputmotorplan
//
// Description:
//
//
// Author: Jose Manuel Cuadra Troncoso <jmcuadra@dia.uned.es>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef OUTPUTMOTORPLAN_H
#define OUTPUTMOTORPLAN_H

#include "abstractmotorplan.h"

class AbstractMotor;

/**
  @author Jose Manuel Cuadra Troncoso <jmcuadra@dia.uned.es>
*/
class OutputMotorPlan : public AbstractMotorPlan
{
  public:
    OutputMotorPlan(AbstractMotor* motor);

    virtual ~OutputMotorPlan();

    virtual void setMotor(AbstractMotor* motor);
    virtual void sendCommand(void);
    void addCommand(MotorCommand const& command);
    virtual void getInitialCommand(double& vel1, double& vel2); // provisional
    void initilizaOutputCommands(MotorCommand const& command, size_t memorySize);

  protected:
    AbstractMotor* motor;    
};

#endif
