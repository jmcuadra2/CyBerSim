//
// C++ Interface: motorcommand
//
// Description:
//
//
// Author: Jose Manuel Cuadra Troncoso <jmcuadra@dia.uned.es>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef MOTORCOMMAND_H
#define MOTORCOMMAND_H

class NDPose;

/**
  @author Jose Manuel Cuadra Troncoso <jmcuadra@dia.uned.es>
*/
class MotorCommand
{
  public:
    enum MotorCommands {Stop = 1, PolarVel = 2, WheelsVel = 3, CartesianVel = 4, Pose = 5};
  
    MotorCommand();
    MotorCommand(MotorCommand const& motorcommand);
    void operator = (MotorCommand const& motorcommand);

    ~MotorCommand();
    void setPriority(double priority) { this->priority = priority ; }
    double getPriority() const { return priority ; }
    void setCommandId(const MotorCommands& commandId) { this->commandId = commandId ; }
    MotorCommands getCommandId() const { return commandId ; }
    void setCommandValue(NDPose* commandValue, bool copyPose = true);
    NDPose* getCommandValue() const { return commandValue ; }

  protected:
    double priority;
    MotorCommands commandId;
    NDPose* commandValue;
};

#endif
