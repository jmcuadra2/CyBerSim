/***************************************************************************
                          robotsimulation.cpp  -  description
                             -------------------
    begin                : Thu Apr 14 2005
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

#include <QDir>
#include <QMessageBox>
#include <QSize>
#include <QApplication>
#include <QString>
#include <QColor>

#include "robotsimulation.h"
#include "../control/basesimcontrol.h"
#include "clocksdispatcher.h"
#include "gui/basesimulationdialog.h"
#include "../world/rtti.h"
#include "../world/worldview.h"
#include "../robot/robotmonitorstechnician.h"
#include "../robot/robot.h"
#include "../neuraldis/ndmath.h"
#include "../neuraldis/windowoperations.h"
#include "../neuraldis/xmloperator.h"
#include "../robot/gui/robotkineticstab.h"

RobotSimulation::RobotSimulation(QObject *parent, const char *name)
    : BaseSimulation(parent, name){

    world = 0;
    robot = 0;
    monitorsTech = 0;

    type_scr_update = 0; // wcanvas->update continuo
    scr_update = 10;

    //   sensorMode  = Robot::KHEPERA_II;
    //   view_sensors = false;
    //   type_iner = 2;
    init_rvel = 0.0;
    init_lvel = 0.0;
    driving = false;

}

void RobotSimulation::setWorld_Settings(AbstractWorld *wor, Settings *prog_sets)
{

    world = wor;
    prog_settings = prog_sets;

}

RobotSimulation::~RobotSimulation(){

    if(monitorsTech)
        delete monitorsTech;
    if(robot)
        delete robot;
    //  emit simEnd();
    if(world)
        world->getCanvas()->update();

}

QewExtensibleDialog* RobotSimulation::createDialog(void)
{

    QewExtensibleDialog* dialog = BaseSimulation::createDialog();
    if(dialog) {
        QList<QewExtensibleDialog*> add_dialogs = robot->createKineticsDialog(dialog);
        QList<QewExtensibleDialog*> mon_dialogs = monitorsTech->tabsForEdit(dialog);
        QList<QewExtensibleDialog*> it(mon_dialogs);
        for(int j = 0;j<it.size();++j)
            add_dialogs.append(it.value(j));
        dialog->addExtensibleChildrenList(add_dialogs,"Robot");
    }

    else {
        delete dialog;
        dialog = 0;
    }
    return dialog;

}

void RobotSimulation::init()
{

    //   BaseSimulation::init();
    robot->initMotors();
    BaseSimulation::init();
    robot->setAdvancePeriod(real_advance_period);
    robot->initSim(crono_reset);
    if(crono_reset) {
        if(monitorsTech)
            monitorsTech->initSim();
    }

    world->update(crono, real_advance_period);

}

void RobotSimulation::save_special(QDomDocument& doc, QDomElement& e)
{

    WindowOperations* w_oper = WindowOperations::instance();
    XMLOperator* xml_operator = XMLOperator::instance();
    
    QDomElement tag_world = doc.createElement("World_geometry");
    e.appendChild(tag_world);
    if(world) {
        QWidget* w = world->view();
        w_oper->writePosSize(w, tag_world);
    }
    else
        w_oper->writePosSize(0, tag_world);

    QDomElement tag_robot = doc.createElement("Robot");
    e.appendChild(tag_robot);
    robot->write(doc, tag_robot);

    QDomElement tag_robot_mon = doc.createElement("Robot_monitoring");
    e.appendChild(tag_robot_mon);
    if(monitorsTech) {
        QString relativeName = monitorsTech->getFileName();
        tag_robot_mon.appendChild(xml_operator->createTextElement(doc,
                "robot_monrec_file", relativeName.remove(Settings::instance()->getRobotDirectory()
                                                         + QDir::separator())));
        if(!monitorsTech->isFileReallyOpen())
            monitorsTech->save();
    }
}

void RobotSimulation::go(void)
{

    world->view()->setFocus();
    is_running = true;
    robot->setPaused(false);
    if(!robot->walking()) {
        if(crono_reset) {
            if(monitorsTech)
                monitorsTech->initSim();
        }
    }
    robot->readSensors();
    control->sampleInputs();
    robot->setStatus(is_running);
    BaseSimulation::go();
    //   robot->sensorsVisible(true);
    world->update(crono, real_advance_period);

}

void RobotSimulation::initRecordersMonitors(void)
{
    BaseSimulation::initRecordersMonitors();
    robot->initRecord();

}  

void RobotSimulation::step(void)
{

    world->view()->setFocus();
    if(!robot->okPosition()) return;
    emit statusGo(true);
    slotStep();
    robot->sensorsVisible(false);
    robot->readSensors();
    control->sampleInputs();
    robot->sensorsVisible(true);
}

void RobotSimulation::slotStep(void)
{
    //   crono += real_advance_period;
    //   if(crono < 15000) return; // para camara real

    advance_timer->startClock(robot->fullName(), sim_name_timer);
    crono += real_advance_period;

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


    advance_timer->startClock(robot->fullName(), tr("Input"));
    //   robot->sensorsVisible(false);
    robot->setTimePoseTaken(TimeStamp::nowMicro());
    robot->readSensors();
    control->sampleInputs();
    //   robot->sensorsVisible(true);
    advance_timer->pauseClock(robot->fullName(), tr("Input"));

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
        //     setAdvancePeriod(-1);
        stop();
    else
        emit Crono(crono);



    /*void RobotSimulation::slotStep(void)
{

  advance_timer->startClock(robot->fullName(), sim_name_timer);
  crono += real_advance_period;

  advance_timer->startClock(robot->fullName(), tr("Input"));
  robot->sensorsVisible(false);
  control->sampleInputs();
  advance_timer->pauseClock(robot->fullName(), tr("Input"));

  advance_timer->startClock(robot->fullName(), tr("Propagate"));
  control->think();
  advance_timer->pauseClock(robot->fullName(), tr("Propagate"));

  advance_timer->startClock("Control", tr("Backpropagate"));
  control->backPropagate();
  advance_timer->pauseClock("Control", tr("Backpropagate"));

  advance_timer->startClock(robot->fullName(), tr("Output"));
  control->sendOutputs();
  if(!noCommandsMode)
    robot->outputMotors();
  advance_timer->pauseClock(robot->fullName(), tr("Output"));

  advance_timer->startClock(robot->fullName(), tr("Bump"));
  robot->advance(0);
  advance_timer->pauseClock(robot->fullName(), tr("Bump"));

  advance_timer->startClock(robot->fullName(), tr("Advance"));
  robot->sensorsVisible(true);
  robot->advance(1);
  advance_timer->pauseClock(robot->fullName(), tr("Advance"));

  advance_timer->startClock(robot->fullName(), tr("Monitor"));
  control->doMonitor();
  advance_timer->pauseClock(robot->fullName(), tr("Monitor"));

  advance_timer->pauseClock(robot->fullName(), sim_name_timer);

  if(type_scr_update) {
    if(scr_update) {
      if(!(crono%(100*scr_update)))  // decimas de segundo
        world->getCanvas()->update();
    }
  } else
    world->getCanvas()->update();
  if(crono > duration)
    setAdvancePeriod(-1);
  else
    emit Crono(crono);
*/
}

void RobotSimulation::pause(void)
{

    world->view()->setFocus();
    BaseSimulation::pause();
    robot->setPaused(true);
    robot->sensorsVisible(false);
    robot->readSensors();
    control->sampleInputs();
    robot->sensorsVisible(true);

}

void RobotSimulation::pauseOutWorld(void)
{
    robot->sensorsVisible(false);
    robot->readSensors();
    control->sampleInputs();
    robot->sensorsVisible(true);
    robot->crash();
    setAdvancePeriod(-1);

}

void RobotSimulation::stop(void)
{

//    QTime timer;
    QElapsedTimer timer;
    if(!is_running)
        return;
    robot->setPaused(false);
    timer.start();
    while(!robot->stop()) {
        while(timer.elapsed() < real_advance_period) {};
        slotStep();
        timer.restart();
    }

    is_running = false;
    robot->setStatus(is_running);

    BaseSimulation::stop();

}

void RobotSimulation::stopReset(void)
{

    BaseSimulation::stopReset();
    world->getCanvas()->update();

}

void RobotSimulation::viewSensors(bool show)
{

    if(view_sensors != show) {
        view_sensors = show;
        robot->viewSensors(view_sensors);
        world->getCanvas()->update();
    }

}

void RobotSimulation::setRecording(bool on)
{

    if(recording == on) return;
    BaseSimulation::setRecording(on);
    if(monitorsTech) {
        bool inverseY = world->headingTransform(1) > 0 ? false : true; // hacemos depender que la y sea + hacia abajo o arriba de la transformacion de angulos
//        monitorsTech->setRecording(on, SCALE_FIG_2_PIXEL, world->getCanvas()->width(), world->getCanvas()->height(), crono_reset, world->getXMinPosition(), world->getYMinPosition(), inverseY, world->toFigUnits());
        monitorsTech->setRecording(on, SCALE_FIG_2_PIXEL, world->getWidth(), world->getHeight(), crono_reset, world->getXMinPosition(), world->getYMinPosition(), inverseY, world->toFigUnits());
    }

}

// void RobotSimulation::setNoise(int nois)
// {
// 
//   robot->setNoise(double(nois)/100.0);
//   BaseSimulation::setNoise(nois);
// 
// }

//bool RobotSimulation::isRunning(void)
//{
//
//  return robot->walking();
//
//}

// void RobotSimulation::setSensorMode(int s_mode)
// {
// 
//  sensorMode = s_mode;
//  robot->setSensorMode(s_mode);
// 
// }

int RobotSimulation::simulationType(void) const
{

    return BaseSimulation::ROBOT_SIM;

}

void RobotSimulation::setClocks(void)
{

    advance_timer->addClock(robot->fullName(), tr("Bump"));
    advance_timer->addClock(robot->fullName(), tr("Input"));
    advance_timer->addClock(robot->fullName(), tr("Propagate"));
    advance_timer->addClock(robot->fullName(), tr("Backpropagate"));
    advance_timer->addClock(robot->fullName(), tr("Output"));
    advance_timer->addClock(robot->fullName(), tr("Advance"));
    advance_timer->addClock(robot->fullName(), tr("Monitor"));

}

void RobotSimulation::centerInRobot(bool on)
{
    if(world) {
        //     if(robot) {
        //       if(type_sim_advper < 2) {
        world->centerInRobot(on);
        //       if(on) {
        //         connect(robot->getSprite(), SIGNAL(intPosition(int, int)), world, SLOT(centerInRobot(int, int)));
        //         world->view()->center(robot->robotPosition()->x(), robot->robotPosition()->y());
        //       }
        //       else
        //         disconnect(robot->getSprite(), SIGNAL(intPosition(int, int)), world, SLOT(centerInRobot(int, int)));
        //     }
        //    }
    }
}

void RobotSimulation::setDriving(bool on) 
{
    if(driving == on)
        return;
    driving = on;
    robot->setDriving(driving);
}

QList<QString> RobotSimulation::getSensorGroupsNames()
{
    return robot->getSensorsSet()->getSensorGroups().keys();
}

void RobotSimulation::showSensors(bool checked, QString const& data)
{
    robot->getSensorsSet()->showSensors(checked, data);
    world->getCanvas()->update();
}

void RobotSimulation::recordSensorsDialog(bool on)
{
    if(monitorsTech)
        monitorsTech->recordSensorsDialog(on);
}
