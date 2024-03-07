/***************************************************************************
                          robotsimulationfactory.h  -  description
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

#ifndef ROBOTSIMULATIONFACTORY_H
#define ROBOTSIMULATIONFACTORY_H

#include "basesimulationfactory.h"

class RobotSimulation;
class Robot;
class BaseSimControl;

/**
  *@author Jose M. Cuadra Troncoso
  */

class RobotSimulationFactory : public BaseSimulationFactory {
  
  public:
    RobotSimulationFactory();
    ~RobotSimulationFactory();

    BaseSimulation* factorySim(int sim_type, const QDomElement& e,
                                const QString& sim_name);
    BaseSimulation* factorySim(int sim_type);                                
    bool construct(void);

  protected:
    virtual Robot* buildRobot(QDomElement& e);
    void setClocks(void);
    virtual void buildRobotMonitors(QDomElement& e);
    virtual void ConnectRobotToControl(BaseSimControl* control);
    
  protected:
    RobotSimulation* r_sim;
    Robot* robot;
    
};

#endif
