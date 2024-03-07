/***************************************************************************
                          robotsimulation.h  -  description
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

#ifndef ROBOTSIMULATION_H
#define ROBOTSIMULATION_H

#include "basesimulation.h"
#include "../world/world.h"
#include "../neuraldis/settings.h"

class Robot;
class RobotMonitorsTechnician;
class ClocksDispatcher;

/**
  *@author Jose M. Cuadra Troncoso
  */

class RobotSimulation : public BaseSimulation  {

    Q_OBJECT

    friend class SimulationBuilder;
    friend class RobotSimulationFactory;

public:
    //  	RobotSimulation(World *wor, Settings *prog_sets,
    //  								QObject *parent = 0, const char *name = 0);
    RobotSimulation(QObject *parent = 0, const char *name = 0);
    ~RobotSimulation();

    //     bool edit(int page = 0);
    void go(void);
    void stop(void);
    void step(void);
    void viewSensors(bool show);
    //     virtual void setSensorMode(int s_mode);
    void setRecording(bool on);
    void init();
    int simulationType(void) const;
    bool needWorld(void) { return true ;};
    void centerInRobot(bool on);
    void setDriving(bool on);
    QList<QString> getSensorGroupsNames();
    void showSensors(bool checked, QString const& data);
    void recordSensorsDialog(bool on);

public slots:
    virtual void pauseOutWorld(void);
    //    bool isRunning(void);
    void pause(void);

protected :
    void stopReset(void);
    void setClocks(void);
    virtual void setWorld_Settings(AbstractWorld *wor, Settings *prog_sets);
    void save_special(QDomDocument& doc, QDomElement& e);
    //     void setNoise(int nois);
    void initRecordersMonitors(void);
    QewExtensibleDialog* createDialog(void);

protected slots:
    virtual void slotStep(void);

protected:
    Robot* robot;
    RobotMonitorsTechnician* monitorsTech;
    AbstractWorld* world;
    //     int type_iner;
    //     int sensorMode;
    double init_rvel, init_lvel;
    bool driving;

};

#endif

