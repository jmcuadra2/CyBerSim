//
// C++ Implementation: ariasimulation
//
// Description: 
//
//
// Author: Jose Manuel Cuadra Troncoso <jmcuadra@dia.uned.es>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "ariasimulation.h"
#include "clocksdispatcher.h"
#include "../robot/robot.h"
#include "../world/worldview.h"

AriaSimulation::AriaSimulation(QObject* parent, const char* name): RobotSimulation(parent, name)
{
}


AriaSimulation::~AriaSimulation()
{
}

int AriaSimulation::simulationType(void) const
{

  return BaseSimulation::ARIA_SIM;

}

void AriaSimulation::stop(void)
{
//     RobotSimulation::stop(); // no cambiar el orden de estas dos sentencias
//     noCommandsMode = false;
    noCommandsMode = true;
    RobotSimulation::stop(); // no cambiar el orden de estas dos sentencias para dejar parar al robot por inercia
    noCommandsMode = false;
}

void AriaSimulation::pause(void)
{
  world->view()->setFocus();
  robot->setPaused(true);
  noCommandsMode = true;
}

void AriaSimulation::slotStep(void)
{
//   crono += real_advance_period;
//   if(crono < 15000) return; // para camara real
  
  advance_timer->startClock(robot->fullName(), sim_name_timer);
  crono += real_advance_period;
  
  advance_timer->startClock(robot->fullName(), tr("Input"));
//   robot->sensorsVisible(false);
  robot->readSensors();
  control->sampleInputs();
//   robot->sensorsVisible(true);
  advance_timer->pauseClock(robot->fullName(), tr("Input"));

  advance_timer->startClock(robot->fullName(), tr("Propagate"));
  control->think();
  advance_timer->pauseClock(robot->fullName(), tr("Propagate"));

  advance_timer->startClock("Control", tr("Backpropagate"));
  control->backPropagate();
  advance_timer->pauseClock("Control", tr("Backpropagate"));

  advance_timer->startClock(robot->fullName(), tr("Output"));
//   if(!driving)
    control->sendOutputs();
  if(!noCommandsMode)
    robot->outputMotors();
  advance_timer->pauseClock(robot->fullName(), tr("Output"));

  advance_timer->startClock(robot->fullName(), tr("Bump"));
//   robot->sensorsVisible(false);
  robot->advance(0);
  advance_timer->pauseClock(robot->fullName(), tr("Bump"));

  advance_timer->startClock(robot->fullName(), tr("Advance"));
//   robot->sensorsVisible(true);
  robot->advance(1);
  advance_timer->pauseClock(robot->fullName(), tr("Advance"));

  advance_timer->startClock(robot->fullName(), tr("Monitor"));
  control->doMonitor();
  advance_timer->pauseClock(robot->fullName(), tr("Monitor"));
      
  advance_timer->pauseClock(robot->fullName(), sim_name_timer);

  if(type_scr_update) {
    if(scr_update) {
      if(!(crono%(100*scr_update)))  // decimas de segundo
//         world->getCanvas()->update();
        world->update(crono, real_advance_period);
    }
  }
  else
//     world->getCanvas()->update();
    world->update(crono, real_advance_period);
  if(crono > duration)
    setAdvancePeriod(-1);
  else
    emit Crono(crono);
}
