/***************************************************************************
                          robotmonitorstechnician.h  -  description
                             -------------------
    begin                : Thu Mar 3 2005
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

#ifndef ROBOTMONITORSTECHNICIAN_H
#define ROBOTMONITORSTECHNICIAN_H

#include <QFile>
#include <QObject>
#include <QDomDocument>

#include "../neuraldis/xmloperator.h"
#include "../robot/gui/robotmonitorstechtab.h"

class Robot;
class SensorsSet;
class RobotLedMonitor;
class RobotGraphMonitor;
class GraphicMonitor;
class RobotRecorder;
class SensorsRecorder;
class QWidget;
class QMdiArea;
class SensorMonitorsContainer;

/*! \defgroup robot_monitor Monitorizacion del robot
    \ingroup robot
    Informacion sobre la monitorizacion robot
 */

/*! \ingroup robot_monitor 
    \brief Constructor y controlador de monitores graficos de las velocidades
     y grabador de las velocidades y posicion del robot.
  */

class RobotMonitorsTechnician  : public QObject{

    Q_OBJECT

    friend class RobotMonitorsTechTab;

public:
    RobotMonitorsTechnician(Robot* rob, QWidget* w_parent, QObject* parent = 0, char* name = 0);
    RobotMonitorsTechnician(Robot* rob, QMdiArea* w_parent, QObject* parent = 0, char* name = 0);
    ~RobotMonitorsTechnician();

    void constructLeds(void);
    void constructRecorder(void);
    void setRecording(bool on, double world_scale,int world_width, int world_height,
                      bool crono_reset, double world_xmin = 0, double world_ymin = 0,
                      bool inverseY = false, int toFigUnits = 1);
    void initSim(void);
    void save(void);
    void setFileVelsName(const QString &fname);
    const QString& getFileVelsName(void) { return file_vels_name ; }
    bool hasVelsMonitor(void);
    bool hasVelsRecorder(void);
    //     void edit(void);
    bool setFile(const QString& f_name);
    const QString& getFileName(void) { return file_name ; }
    void constructGraphMons(void);
    bool isFileReallyOpen(void) {return file_really_open ; }
    QList<QewExtensibleDialog*> tabsForEdit(QewExtensibleDialog* parent);
    void recordSensorsDialog(bool on);
    
public slots:
    void closeVelsMonitor(void);
    void constructLeds(QList<int> sensor_idents, bool alsoRecorders = true);
    void constructSensorsRecorders(QList<int> sensor_idents, QList< QewExtensibleDialog* > *containers = 0);

protected:
    void setDefaultParams(Robot* rob, QWidget* w_parent);
    void connectGraphMonsToRobot(bool on);
    void connectLedsToRobot(void);
    void connectLedsToSensors(void);
    void defaultVelsMonitors(void);
    void setMonitorsInfo(const QDomElement& e);
    QDomElement getMonitorsInfo(void);
    void setMonParams(const QDomElement& e, int index);
    void setLinearMonParams(const QDomElement& e);
    void setRadialMonParams(const QDomElement& e);
    void writeRobotMonitor(QDomDocument& doc, QDomElement& e);
    void writeLEDMonitors(QDomDocument& doc, QDomElement& e);
    void writeRobotRecorder(QDomDocument& doc, QDomElement& e);
    template<class LM>
    void writeLEDMonitor(LM* monitor, QDomDocument& doc, QDomElement& e);
    void writeVelMonsParams(QDomElement& e, int n_g, int xupd, int n_xlab, double y_min,
                            double y_max, int n_ylab, QString cap, bool bck_color);
    void writeRecorderParams(QDomElement& e, const QString& f_name, bool is_rec);

protected slots:
    void containerClosed(QObject* container);

private:
    RobotLedMonitor* led_monitors;
    RobotGraphMonitor* vel_monitors;
    GraphicMonitor* velocmonitor;
    GraphicMonitor* rotmonitor;
    QDomDocument doc_monitors;
    QDomElement info_monitors;
    XMLOperator* xml_operator;
    QWidget* parent_widget;
    QMdiArea* work_space;
    Robot* robot;
    RobotRecorder* recorder;

    QString file_name;
    QString file_vels_name;
    bool hasVelsFile, hasVelsMon;
    bool file_really_open;
    bool isRecording;

    QList<SensorMonitorsContainer*> sensorsMonitors;
    QList<SensorsRecorder*> sensorsRecorders;
    QewExtensibleDialog* noMonitorsDialog; //apano temporal

};

#endif
