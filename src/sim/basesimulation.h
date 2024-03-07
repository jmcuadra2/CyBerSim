/***************************************************************************
                          basesimulation.h  -  description
                             -------------------
    begin                : Wed Nov 10 2004
    copyright            : (C) 2004 by Jose M. Cuadra Troncoso
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

#ifndef BASESIMULATION_H
#define BASESIMULATION_H

#include <QObject>
#include <QDomDocument>
#include <QTimer>
#include <QTextStream>
#include <QFile>
#include <QPointer>
#include <QDateTime>
#include <qewextensibledialog.h>

#include "../control/basesimcontrol.h"
#include "../neuraldis/settings.h"

class ClocksDispatcher;

class BaseSimulation : public QObject {

    Q_OBJECT

    friend class baseSimulationDialog;
    friend class SimulationBuilder;
    friend class BaseSimulationFactory;

public:
    BaseSimulation(QObject *parent = 0, const char *name = 0);
    ~BaseSimulation();

    virtual bool edit(int page = 0);
    virtual void save(QTextStream &ts);
    virtual QString save(void);
    virtual void go(void);
    virtual void step(void);
    virtual void stop(void);
    virtual bool isRecording(void);
    virtual void setRecording(bool on);
    virtual void setAdvancePeriod(int ms);
    virtual bool isRunning(void);
    virtual int simulationType(void) const;
    virtual void init();
    virtual void viewSensors(bool ) {}
    virtual void setDuration(int ms);
    virtual int getDuration(void);
    virtual bool getViewSensors(void);
    virtual QString& getFileName(void) { return file_name; }
    virtual void setFileName(const QString& f_name) { file_name = f_name ; }
    virtual bool needWorld(void) { return false ;}
    bool hasChanged(void) { return has_changed ; }
    virtual void centerInRobot(bool /* on */) {}
    virtual void setDriving(bool /*on*/) {}
    virtual QList<QString> getSensorGroupsNames();
    virtual void showSensors(bool /*checked*/, QString const& /*data*/) {}

    virtual void recordSensorsDialog(bool on);

public slots:
    virtual void pause(void);

protected :
    virtual void stopReset(void);
    virtual void setClocks(void);
    virtual void addControlTabs(QewExtensibleDialog* dialog);
    virtual void save_special(QDomDocument&, QDomElement&) {}
    virtual void initRecordersMonitors(void);
    virtual QewExtensibleDialog* createDialog(void);
    
protected slots:
    virtual void slotStep(void);

signals:
    void statusGo(bool);
    void scrUpdate(bool);
    void Crono(int);
    void initCrono(int);
    void CronoReset(bool);

public:
    enum SimType{BASE_SIM, ROBOT_SIM = 5, ARIA_SIM = 10};
    enum TimeConst{max_duration = 86400000}; // 1 dia = 86400000 ms.

protected:
    QString file_name;
    QTextStream sim_ts;

    int real_advance_period, sim_advance_period, type_sim_advper;
    bool is_running, recording;
    ClocksDispatcher* advance_timer;
    int crono;
    bool crono_reset;
    QString sim_name_timer;
    BaseSimControl* control;
    int scr_update, type_scr_update;
    int duration;
    bool view_sensors;
    Settings *prog_settings;
    bool has_changed;
    bool noCommandsMode;

};

#endif
