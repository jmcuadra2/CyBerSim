/***************************************************************************
                          robotmonitorstechnician.cpp  -  description
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

#include <QWidget>
#include <QDomDocument>
#include <QDir>
#include <QMessageBox>
#include <QMdiSubWindow>

#include "../recmon/graphicmonitor.h"
#include "../recmon/recmonfactory.h"
#include "../sim/clocksdispatcher.h"
#include "../neuraldis/settings.h"
#include "../neuraldis/windowoperations.h"
#include "../neuraldis/neuraldisdoc.h"
#include "gui/sensormonitorscontainer.h"
#include "robotmonitorstechnician.h"
#include "robot.h"
#include "robotrecorder.h"
#include "sensorsrecorder.h"
#include "robotledmonitor.h"
#include "sensorsset.h"
#include "robotgraphmonitor.h"

RobotMonitorsTechnician::RobotMonitorsTechnician(Robot* rob,
                              QWidget* w_parent, QObject* parent, char* name) :
                    QObject(parent)
{
    setDefaultParams(rob, w_parent);
}

RobotMonitorsTechnician::RobotMonitorsTechnician(Robot* rob,
                                 QMdiArea* workspace, QObject* parent, char* name) :
                    QObject(parent)
{
    setDefaultParams(rob, workspace);
    work_space = workspace;
}              

RobotMonitorsTechnician::~RobotMonitorsTechnician()
{
    setParent(0);
    if(led_monitors) {
        Settings::instance()->getWorkspace()->removeSubWindow(led_monitors);
        delete led_monitors;
    }
    if(vel_monitors) {
        closeVelsMonitor();
        Settings::instance()->getWorkspace()->removeSubWindow(vel_monitors);
        delete vel_monitors;
    }
    if(recorder)
        delete recorder;
    
    for(int i = 0; i < sensorsMonitors.size(); i++) {
        QObject::disconnect(sensorsMonitors[i], SIGNAL(destroyed(QObject* )),
                            this, SLOT(containerClosed(QObject*)));
        delete sensorsMonitors[i];
    }
    for(int i = 0; i < sensorsRecorders.size(); i++)
        delete sensorsRecorders[i];
}

void RobotMonitorsTechnician::setDefaultParams(Robot* rob, QWidget* w_parent)
{
    robot = rob;
    led_monitors = 0;
    vel_monitors = 0;
    recorder = 0;
    velocmonitor = 0;
    rotmonitor = 0;
    parent_widget = w_parent;
    hasVelsFile = false;
    hasVelsMon = false;
    xml_operator = XMLOperator::instance();
    file_really_open = false;
    work_space = 0;
    isRecording = false;

    noMonitorsDialog = nullptr;
}

void RobotMonitorsTechnician::constructLeds()
{

    QDomElement e_led =
            xml_operator->findTag("LED_monitors", info_monitors);

    led_monitors = new RobotLedMonitor(0);
    if(work_space) {
        QMdiSubWindow* subw = work_space->addSubWindow(led_monitors);
        led_monitors->setMdiSubWindow(subw);
        led_monitors->getMdiSubWindow()->setMaximumSize(led_monitors->getMdiSubWindow()->size());
    }
    WindowOperations* w_oper = WindowOperations::instance();
    QDomElement e_geom = xml_operator->findTag("geometry", e_led);
    if(!e_geom.isNull()) {
        QWidget* w = led_monitors;
        w_oper->setPosSize(w, e_geom);
    }
    connectLedsToRobot();
    connectLedsToSensors();

}

void RobotMonitorsTechnician::constructLeds(QList<int> sensor_idents, bool alsoRecorders)
{
    if(sensor_idents.isEmpty()) return;

    AbstractSensor* sen;
    SensorMonitorsContainer* container;
    SensorsSet* sensorsSet = robot->getSensorsSet();
    QHash<QString, SensorGroup*> sensorsGroups = sensorsSet->getSensorGroups();
    QList< QewExtensibleDialog* > containers;

    QHashIterator<QString, SensorGroup*> i(sensorsGroups);
    while (i.hasNext()) {
        i.next();
        QListIterator<AbstractSensor*> it(sensorsSet->getSensors(i.key()));
        bool newMonitor = true;
        while(it.hasNext()) {
            sen = it.next();
            int idx = sensor_idents.indexOf(sen->getIdent());
            if(idx > -1) {
                if(newMonitor) {
                    container = new SensorMonitorsContainer(0);
                    sensorsMonitors.append(container);
                    container->setWindowTitle(i.key());
                    if(work_space) {
                        QMdiSubWindow* subw = work_space->addSubWindow(container);
                        container->setMdiSubWindow(subw);
                    }
                    connect(container, SIGNAL(destroyed(QObject* )),
                                            SLOT(containerClosed(QObject*)));
                    containers.append(container);
                    newMonitor = false;
                }
                container->addMonitor(sen);
            }
        }
    }
    
    for(int i = 0; i < sensorsMonitors.size(); i++)  {
        sensorsMonitors[i]->show();
        sensorsMonitors[i]->adjustSize();
        sensorsMonitors[i]->resizeMdi(QSize(sensorsMonitors[i]->width(),
                                            sensorsMonitors[i]->height()));
        sensorsMonitors[i]->getMdiSubWindow()->showNormal();
    }
    if(alsoRecorders)
        constructSensorsRecorders(sensor_idents, &containers);
}

void RobotMonitorsTechnician::containerClosed(QObject* container)
{
    if(!container->isWidgetType()) return;
    int idx = sensorsMonitors.indexOf((SensorMonitorsContainer*)container);
    if(idx >= 0) {
        sensorsMonitors.removeAt(idx);
        delete sensorsRecorders[idx];
        sensorsRecorders.removeAt(idx);
    }
    else
        qDebug("RobotMonitorsTechnician::containerClosed() container is not in list");
}

void RobotMonitorsTechnician::constructSensorsRecorders(QList<int> sensor_idents, QList< QewExtensibleDialog* > *containers)
{
    if(sensor_idents.isEmpty()) return;

    AbstractSensor* sen;
    SensorsRecorder* sens_recorder = nullptr;
    SensorsSet* sensorsSet = robot->getSensorsSet();
    QHash<QString, SensorGroup*> sensorsGroups = sensorsSet->getSensorGroups();
    if(!containers) {
        QewDialogFactory *factory = new QewDialogFactory();
        noMonitorsDialog = factory->createDialog(QewExtensibleDialog::QewTabWidget);
        if(!noMonitorsDialog->setUpDialog())
            return;
        noMonitorsDialog->setWindowTitle("Sensors recorder");
    }

    int j = 0;
    QHashIterator<QString, SensorGroup*> i(sensorsGroups);
    while (i.hasNext()) {
        i.next();
        QListIterator<AbstractSensor*> it(sensorsSet->getSensors(i.key()));
        bool newRecorder = true;
        QStringList head;
        while(it.hasNext()) {
            sen = it.next();
            int idx = sensor_idents.indexOf(sen->getIdent());
            if(idx > -1) {
                if(newRecorder) {
                    sens_recorder = new SensorsRecorder(idx);
                    sensorsRecorders.append(sens_recorder);

                    head.append(i.key().left(i.key().lastIndexOf('_')));
                    head.append(QString::number(sen->getSensorMaxVal()));
                    sens_recorder->setFileName(QDir::currentPath() + QDir::separator() + "recs" + QDir::separator() + head.at(0) + ".dat");
                    if(containers) {
                        QewExtensibleDialog* container = containers->at(j);
                        QewExtensibleDialog* dialog = sens_recorder->createDialog(container);
                        if(dialog)
                            container->addExtensibleChild(dialog, "");
                        j++;
                    }
                    else  {
                        QewExtensibleDialog* dialog = sens_recorder->createDialog(noMonitorsDialog);
                        if(dialog)
                            noMonitorsDialog->addExtensibleChild(dialog, i.key());
                    }
                    newRecorder = false;
                }

                head.append(QString::number(sen->getIdent()) + " " +
                            QString::number(sen->getOrientation()));
                sens_recorder->addChannel(sen->getIdent());
                QObject::connect(sen, SIGNAL(getSensorFValue(double, int)),
                                 sens_recorder, SLOT(writeValue(double, int)));
                QObject::connect(sen, SIGNAL(disconnectRecord(int)),
                                 sens_recorder, SLOT(delChannel(int)));
                sen->setEmitSignal(true);
            }
        }
        if(head.size()) {
            QString h = head.join("\n");
            sens_recorder->setHeader(head.join("\n"));
        }
    }

}

void RobotMonitorsTechnician::connectLedsToRobot(void)
{

    ClocksDispatcher* advance_timer = ClocksDispatcher::instance();

    if(!led_monitors) return;
    QObject::connect(robot, SIGNAL(rVel(double)),
                     led_monitors->irsmonitor, SLOT(setRVel(double)));
    QObject::connect(robot, SIGNAL(lVel(double)),
                     led_monitors->irsmonitor, SLOT(setLVel(double)));
    QObject::connect(robot, SIGNAL(signalBump()),
                     led_monitors->lightmonitor, SLOT(addBumpNoDetect()));

    QObject::connect(advance_timer, SIGNAL(Crono(int)),
                     led_monitors->lightmonitor, SLOT(initBumpDetect()));
    QObject::connect(advance_timer, SIGNAL(initCrono(int)),
                     led_monitors->lightmonitor, SLOT(initBumpDetect()));
    QObject::connect(advance_timer, SIGNAL(Crono(int)),
                     led_monitors->lightmonitor, SLOT(initBumpNoDetect()));
    QObject::connect(advance_timer, SIGNAL(initCrono(int)),
                     led_monitors->lightmonitor, SLOT(initBumpNoDetect()));

}

void RobotMonitorsTechnician::connectLedsToSensors(void)
{

    if(!led_monitors) return;
    AbstractSensor* sen;
    SensorsSet* sensorsSet = robot->getSensorsSet();

    QListIterator<AbstractSensor*> itp(sensorsSet->getSensors("Infrared_sensors"));
    if(itp.hasNext())
        led_monitors->irsmonitor->setIdOffset(itp.next()->getGroupIdent()); // o getGroupIdent()
    itp.toFront();
    while(itp.hasNext()) {
        sen = itp.next();
        QObject::connect(sen, SIGNAL(getSensorVal(int, int)),
                         led_monitors->irsmonitor, SLOT(setLCD(int, int)));
        led_monitors->irsmonitor->startLCD(sen->getGroupIdent());
    }

    QListIterator<AbstractSensor*> itl(sensorsSet->getSensors("Light_sensors"));
    if(itl.hasNext())
        led_monitors->lightmonitor->setIdOffset(itl.next()->getGroupIdent()); // o getGroupIdent()
    itl.toFront();
    while(itl.hasNext()) {
        sen = itl.next();
        QObject::connect(sen, SIGNAL(getSensorVal(int, int)),
                         led_monitors->lightmonitor, SLOT(setLCD(int, int)));
        led_monitors->lightmonitor->startLCD(sen->getGroupIdent());
    }

    QListIterator<AbstractSensor*> itrf(sensorsSet->getSensors("RadialField_sensors"));
    if(itrf.hasNext()) {
        itrf.toBack();
        sen = itrf.previous();
        led_monitors->lightmonitor->setIdOffset(sen->getGroupIdent() + 3); // o getGroupIdent()
    }
    itrf.toFront();
    while(itrf.hasNext()) {
        sen = itrf.next();
        QObject::connect(sen, SIGNAL(getSensorVal(int, int)),
                         led_monitors->lightmonitor, SLOT(setLCD(int, int)));
        led_monitors->lightmonitor->startLCD(sen->getGroupIdent());
    }

    QListIterator<AbstractSensor*> itb(sensorsSet->getSensors("Bumpers_sensors"));
    if(itb.hasNext())
        led_monitors->lightmonitor->setIdOffset(itb.next()->getGroupIdent());
    itb.toFront();
    while(itb.hasNext()) {
        sen = itb.next();
        QObject::connect(sen, SIGNAL(getSensorVal(int, int)),
                         led_monitors->lightmonitor, SLOT(setLCD(int, int)));
        QObject::connect(sen, SIGNAL(getSensorVal(int, int)),
                         led_monitors->lightmonitor, SLOT(addBumpDetect(int, int)));
        led_monitors->lightmonitor->startLCD(sen->getGroupIdent());
    }

    led_monitors->resetBump();
    led_monitors->show();
    led_monitors->getMdiSubWindow()->showNormal();

}

void RobotMonitorsTechnician::setMonitorsInfo(const QDomElement& e)
{

    QDomNode n(e.cloneNode());
    if(info_monitors.isNull()) {
        info_monitors = doc_monitors.createElement("Robot_monitoring");
    }
    QDomElement e_mons = xml_operator->findTag("RobotMonitor", info_monitors);
    if(e_mons.isNull())
        info_monitors.appendChild(n.toElement());
    else
        e_mons = n.toElement();

}

QDomElement RobotMonitorsTechnician::getMonitorsInfo(void)
{

    QDomElement e_rob_mon =
            xml_operator->findTag("RobotMonitor", info_monitors);
    return e_rob_mon;

}        

void RobotMonitorsTechnician::constructGraphMons(void)
{

    if(!parent_widget) return;
    WindowOperations* w_oper = WindowOperations::instance();
    bool ok;
    GraphicMonitor* monitor = 0;
    QString name;

    QDomElement e_rob_mon = getMonitorsInfo();
    QDomElement e_is_mon =
            xml_operator->findTag("is_monitoring", e_rob_mon);
    QDomElement e_construct =
            xml_operator->findTag("constructor", e_rob_mon);
    QDomElement e_geom = xml_operator->findTag("geometry", e_rob_mon);
    QDomElement e_mons = xml_operator->findTag("Monitors", e_rob_mon);
    if(e_construct.isNull() || e_mons.isNull()) return;
    hasVelsMon = bool(e_is_mon.text().toInt(&ok));
    if(vel_monitors) {
        closeVelsMonitor();
        Settings::instance()->getWorkspace()->removeSubWindow(vel_monitors);
        delete vel_monitors;
    }
    vel_monitors = new RobotGraphMonitor(e_construct, parent_widget);
    if(work_space) {
        QMdiSubWindow* subw = work_space->addSubWindow(vel_monitors);
        vel_monitors->setMdiSubWindow(subw);
    }
    if(!e_geom.isNull()) {
        QWidget* w = vel_monitors;
        w_oper->setPosSize(w, e_construct);
    }
    QDomNode n_mon = e_mons.firstChild();
    while( !n_mon.isNull() ) {
        QDomElement e_mon = n_mon.toElement();
        QDomElement e_id =
                xml_operator->findTag("mon_identification", e_mon);
        QDomElement e_cons =
                xml_operator->findTag("constructor", e_mon);
        QDomElement e_grs =
                xml_operator->findTag("graphs", e_mon);
        if(e_cons.isNull() || e_grs.isNull()) {
            Settings::instance()->getWorkspace()->removeSubWindow(vel_monitors);
            delete vel_monitors;
            vel_monitors = 0;
            return;
        }
        name = e_id.text();
        monitor = new GraphicMonitor(e_cons, vel_monitors, name);
        monitor->addGraphs(e_grs);
        if(monitor)
            vel_monitors->addMon(monitor);
        n_mon = n_mon.nextSibling();
    }

    if(vel_monitors) {
        velocmonitor = vel_monitors->gmons.at(0);
        rotmonitor = vel_monitors->gmons.at(1);
        QObject::connect(vel_monitors, SIGNAL(closeNumId(int)),
                         this, SLOT(closeVelsMonitor(void)));
        vel_monitors->init();
        vel_monitors->reset();
        if(!isRecording)
            vel_monitors->getMdiSubWindow()->hide();
        hasVelsMon = true;
    }
}

void RobotMonitorsTechnician::connectGraphMonsToRobot(bool on)
{

    if(!vel_monitors) return;
    if(!hasVelsMon) return;
    ClocksDispatcher* advance_timer = ClocksDispatcher::instance();
    if(on) {
        QObject::connect(robot, SIGNAL(velMonitor(double, int)),
                         velocmonitor, SLOT(paintVals(double, int)));
        QObject::connect(robot, SIGNAL(rotMonitor(double, int)),
                         rotmonitor, SLOT(paintVals(double, int)));

        QObject::connect(advance_timer, SIGNAL(Crono(int)),
                         velocmonitor, SLOT(Crono(int)));
        QObject::connect(advance_timer, SIGNAL(Crono(int)),
                         rotmonitor, SLOT(Crono(int)));

        QObject::connect(advance_timer, SIGNAL(initCrono(int)),
                         velocmonitor, SLOT(initCrono(int)));
        QObject::connect(advance_timer, SIGNAL(initCrono(int)),
                         rotmonitor, SLOT(initCrono(int)));
    }
    else {
        QObject::disconnect(robot, SIGNAL(velMonitor(double, int)),
                            velocmonitor, SLOT(paintVals(double, int)));
        QObject::disconnect(robot, SIGNAL(rotMonitor(double, int)),
                            rotmonitor, SLOT(paintVals(double, int)));

        QObject::disconnect(advance_timer, SIGNAL(Crono(int)),
                            velocmonitor, SLOT(Crono(int)));
        QObject::disconnect(advance_timer, SIGNAL(Crono(int)),
                            rotmonitor, SLOT(Crono(int)));

        QObject::disconnect(advance_timer, SIGNAL(initCrono(int)),
                            velocmonitor, SLOT(initCrono(int)));
        QObject::disconnect(advance_timer, SIGNAL(initCrono(int)),
                            rotmonitor, SLOT(initCrono(int)));
    }

}

void RobotMonitorsTechnician::setRecording(bool on, double world_scale, int world_width, int world_height, bool crono_reset, double world_xmin, double world_ymin, bool inverseY, int toFigUnits)
{

    if(isRecording == on) return;
    isRecording = on;

    QStringList header;
    header.append(QString::number(world_scale));
    header.append(QString::number(world_width));
    header.append(QString::number(world_height));
    //       header.append(QString::number(robot->robotScale()));
    header.append(QString::number(1)); // provisional, al exportar un world no hay por que saber ni depender de robotScale

    header.append(QString::number(world_xmin));
    header.append(QString::number(world_ymin));
    header.append(QString::number(inverseY ? -1 :1));
    header.append(QString::number(toFigUnits));

    if(isRecording) {
        if(hasVelsFile) {
            recorder = new RobotRecorder(file_vels_name);
            recorder->setCronoReset(crono_reset);
            if(!recorder->init(header)) {
                QMessageBox::warning(nullptr, tr("Init record"),
                                     tr("Couldn't open path file,") + "\n" +
                                     tr("path will not be recorded"),
                                     QMessageBox::Ok);
                hasVelsFile = false;
            }
        }
        robot->setRecording(hasVelsFile);

        if(hasVelsMon) {
            if(!vel_monitors)
                constructGraphMons();
            else
                vel_monitors->reset();
            connectGraphMonsToRobot(true);
            vel_monitors->getMdiSubWindow()->showNormal();

        }
        else
            closeVelsMonitor();
        robot->setMonitoring(hasVelsMon);
        if(hasVelsFile) {
            recorder->connectToCrono(isRecording);
            QObject::connect(robot, SIGNAL(sendToRecorder(double, int)),
                             recorder, SLOT(writeValue(double, int)));
        }
        QObject::connect(robot->getSprite(),
                         SIGNAL(sigPosVelRot(double, double, double, double,
                                             double, double, bool)),
                         robot,
                         SLOT(recordRobot(double, double, double, double,
                                          double, double, bool)));
    }
    else {
        QObject::disconnect(robot->getSprite(),
                            SIGNAL(sigPosVelRot(double, double, double,
                                                double, double, double, bool)),
                            robot,
                            SLOT(recordRobot(double, double, double,
                                             double, double, double, bool)));
        closeVelsMonitor();
        robot->setMonitoring(false);
        robot->setRecording(false);
        if(recorder){
            delete recorder;
            recorder = 0;
        }
    }

    for(int i = 0; i < sensorsRecorders.size(); i++) {
        if(sensorsRecorders[i]->getRecording()) {
            sensorsRecorders[i]->connectToCrono(isRecording);
            if(isRecording) {
                sensorsRecorders[i]->setCronoReset(crono_reset);
                QStringList data;
                data << header;
                data.append(sensorsRecorders[i]->getHeader());
                sensorsRecorders[i]->init(data);
            }
            else
                sensorsRecorders[i]->closeFile();
        }
    }
}

void RobotMonitorsTechnician::setFileVelsName(const QString &fname)
{

    if(file_vels_name == fname) return;
    file_vels_name = fname;
    if(recorder)
        recorder->setFileName(file_vels_name);

}   

void RobotMonitorsTechnician::closeVelsMonitor(void)
{

    if(vel_monitors) {
        QDomDocument doc("velmonitor");
        QDomElement root = doc.createElement("vels");
        doc.appendChild(root);
        vel_monitors->write(doc, root);
        setMonitorsInfo(root);
        connectGraphMonsToRobot(false);
        vel_monitors->getMdiSubWindow()->hide();
    }

}

void RobotMonitorsTechnician::initSim(void)
{
    if(led_monitors)
        led_monitors->resetBump();
}

void RobotMonitorsTechnician::setLinearMonParams(const QDomElement& e)
{
    setMonParams(e, 0);
    if(vel_monitors)
        velocmonitor->setParams(e);
}

void RobotMonitorsTechnician::setRadialMonParams(const QDomElement& e)
{
    setMonParams(e, 1);
    if(vel_monitors)
        rotmonitor->setParams(e);
}

void RobotMonitorsTechnician::setMonParams(const QDomElement& e, int index)
{

    QDomNodeList n_list;
    QDomElement e_mon;
    n_list = info_monitors.elementsByTagName("Monitor");
    e_mon = n_list.item(index).toElement();
    QString cap = e_mon.attribute("titulo");
    e_mon.clear();
    e_mon = e.cloneNode().toElement();
    e_mon.setAttribute("caption", cap);

}

void RobotMonitorsTechnician::save(void)
{

    xml_operator->removeChildren(info_monitors);
    QDomElement tag_leds = doc_monitors.createElement("LED_monitors");
    info_monitors.appendChild(tag_leds);
    writeLEDMonitors(doc_monitors, tag_leds);

    QDomElement tag_rec = doc_monitors.createElement("RobotRecorder");
    info_monitors.appendChild(tag_rec);
    tag_rec.appendChild(xml_operator->createTextElement(doc_monitors,
                                           "is_recording", QString::number(int(
                                                                                            hasVelsRecorder()))));
    tag_rec.appendChild(xml_operator->createTextElement(doc_monitors,
                                           "file_name", getFileVelsName()));

    QDomElement tag_mon = doc_monitors.createElement("RobotMonitor");
    info_monitors.appendChild(tag_mon);
    tag_mon.appendChild(xml_operator->
                        createTextElement(doc_monitors, "is_monitoring",
                                          QString::number(hasVelsMonitor())));
    writeRobotMonitor(doc_monitors, tag_mon);

    NeuralDisDoc* doc_manager = NeuralDisDoc::instance();
    QString ret_name = doc_manager->saveDocument(file_name, doc_monitors.toString(4),
                                        "robot_monitor_record", "robmon",
                                        tr("Saving robot monitors and recorders"), true);
    if(!ret_name.isEmpty()) {
        file_name = ret_name;
        file_really_open = true;  //  por si fue creado desde new sim
    }
}

void RobotMonitorsTechnician::writeRobotMonitor(QDomDocument& doc, QDomElement& e)
{
    if(vel_monitors)
        vel_monitors->write(doc, e);
}

void RobotMonitorsTechnician::writeLEDMonitors(QDomDocument& doc, QDomElement& e)
{
    if(led_monitors)
        led_monitors->write(doc, e);

}

void RobotMonitorsTechnician::writeRobotRecorder(QDomDocument& doc, QDomElement& e)
{
    if(recorder)
        recorder->write(doc, e);
}

bool RobotMonitorsTechnician::hasVelsMonitor(void)
{
    return hasVelsMon;
}

bool RobotMonitorsTechnician::hasVelsRecorder(void)
{
    return hasVelsFile;
}

void RobotMonitorsTechnician::defaultVelsMonitors(void) {

    QDomDocument doc("velmonitor");
    QDomElement root = doc.createElement("RobotMonitor");
    doc.appendChild(root);

    QDomElement tag_cons = doc.createElement( "constructor" );
    root.appendChild(tag_cons);
    tag_cons.setAttribute("num_id", "1");
    tag_cons.setAttribute("caption", tr("Velocities monitor"));
    tag_cons.setAttribute("back_color", "0");

    QDomElement tag_mons = doc.createElement( "Monitors" );
    root.appendChild(tag_mons);

    QDomElement tag_lin = doc.createElement( "Monitor" );
    tag_mons.appendChild(tag_lin);

    tag_lin.appendChild(xml_operator->createTextElement(doc,
                                                        "mon_identification", "Linear"));

    QDomElement tag_lin_cons = doc.createElement( "constructor" );
    tag_lin.appendChild(tag_lin_cons);
    writeVelMonsParams(tag_lin_cons, 1, 60, 6, -100, 100,
                       7, tr("Linear Velocity (cm/s)"), false);
    QDomElement tag_lin_graph = doc.createElement( "graphs" );
    tag_lin.appendChild(tag_lin_graph);
    //  for(uint i = 0; i < graphs_id.count(); ++i) {
    tag_lin_graph.appendChild(xml_operator->createTextElement(doc,
                                                              "graph_id", "-1"));

    QDomElement tag_rad = doc.createElement( "Monitor" );
    tag_mons.appendChild(tag_rad);


    QDomElement tag_rad_id = doc.createElement( "mon_identification" );
    tag_rad.appendChild(tag_rad_id);
    QDomText text_rad_id = doc.createTextNode("Radial");
    tag_rad_id.appendChild(text_rad_id);

    QDomElement tag_rad_cons = doc.createElement( "constructor" );
    tag_rad.appendChild(tag_rad_cons);
    writeVelMonsParams(tag_rad_cons, 1, 60, 7, -400, 400,
                       7, tr("Radial Velocity (dg/s)"), false);
    QDomElement tag_rad_graph = doc.createElement( "graphs" );
    tag_rad.appendChild(tag_rad_graph);
    tag_rad_graph.appendChild(xml_operator->createTextElement(doc,
                                                              "graph_id", "-1"));
    setMonitorsInfo(root);

}

void RobotMonitorsTechnician::writeRecorderParams(QDomElement& e,
                                                  const QString& f_name, bool is_rec)
{                              
    e.setAttribute("file_name", f_name);
    e.setAttribute("is_recording", QString::number(is_rec));
}

void RobotMonitorsTechnician::writeVelMonsParams(QDomElement& e,
                                                 int , int xupd, int n_xlab,
                                                 double y_min, double y_max, int n_ylab,
                                                 QString cap, bool bck_color)
{
    e.setAttribute("caption", cap);
    e.setAttribute("back_color", QString::number(bck_color));
    e.setAttribute("xupdate", QString::number(xupd));
    e.setAttribute("num_xlabels", QString::number(n_xlab));
    e.setAttribute("num_ylabels", QString::number(n_ylab));
    e.setAttribute("y_min", QString::number(y_min));
    e.setAttribute("y_max", QString::number(y_max));

}

void RobotMonitorsTechnician::constructRecorder(void)
{

    bool ok;
    QDomElement e_rec =
            xml_operator->findTag("RobotRecorder", info_monitors);

    QDomElement e_file = xml_operator->findTag("file_name", e_rec);
    QDomElement e_isrec = xml_operator->findTag("is_recording", e_rec);
    setFileVelsName(e_file.text());
    hasVelsFile = bool(e_isrec.text().toInt(&ok));

}

bool RobotMonitorsTechnician::setFile(const QString& f_name)
{

    bool ret = false;
    file_name = f_name;
    file_really_open = false;
    doc_monitors.clear();
    info_monitors.clear();
    if(file_name.trimmed().isEmpty()) {
        QMessageBox msgBox;
        msgBox.setIcon(QMessageBox::Question);
        msgBox.setText(tr("Monitors and recorder"));
        msgBox.setInformativeText(tr("Load from file or use defaults"));
        QPushButton *loadButton = msgBox.addButton(tr("Load"), QMessageBox::ActionRole);
        QPushButton *defaultButton = msgBox.addButton(tr("Default"), QMessageBox::ActionRole);
        msgBox.exec();
//        if(QMessageBox::information(nullptr, tr("Monitors and recorder"),
//                                    tr("Load from file or use defaults"),
//                                    tr("&Load"), tr("&Default")) == 1) {
        if(msgBox.clickedButton() == defaultButton) {
            QString ext = robot->getFileName().section(QDir::toNativeSeparators("/"), -1).
                    section('.', -1);
            file_name = robot->getFileName().left(robot->getFileName().length() -
                                                  ext.length()) + "robmon";
            doc_monitors = QDomDocument("robot_monitor_record");
            defaultVelsMonitors();
            doc_monitors.appendChild(info_monitors);
            ret = true;
            return ret;
        }
        else
            file_name = tr("Untitled") + "." + tr("robmon");
    }
    NeuralDisDoc* doc_manager = NeuralDisDoc::instance();
    file_name =  Settings::instance()->fillPath(file_name,
                                                Settings::instance()->getRobotDirectory());
    doc_monitors = doc_manager->loadXML(file_name, "robot_monitor_record",
                                        tr("Robot recorder and monitors loading"), "robmon");
    if(!doc_monitors.isNull()) {
        QDomNode n = doc_monitors.firstChild();
        info_monitors = n.toElement();
        ret = true;
        file_really_open = true;
    }
    return ret;

}

QList< QewExtensibleDialog* > RobotMonitorsTechnician::
                    tabsForEdit(QewExtensibleDialog* parent)
{
    QList<QewExtensibleDialog*> add_dialogs;
    if(vel_monitors) {
        RobotMonitorsTechTab* monstech_tab = new RobotMonitorsTechTab(parent);
        if(monstech_tab->setUpDialog()) {
            add_dialogs.append(monstech_tab);
            monstech_tab->init(this);
        }
        else
            delete monstech_tab;
    }
    return add_dialogs;

}

void RobotMonitorsTechnician::recordSensorsDialog(bool on)
{
    if(on) {
        SensorsSet* sensorsSet = robot->getSensorsSet();
        QList<AbstractSensor*> slist = sensorsSet->getSensors();
        QList<int> idx;
        for(int i = 0; i < slist.count(); i++)
            idx.append(slist.at(i)->getIdent());
        constructSensorsRecorders(idx);
        if(noMonitorsDialog) {
            if(noMonitorsDialog->exec() == QewExtensibleDialog::Accepted) {

            }
        }
    }
    else {
        for(int i = 0; i < sensorsRecorders.size(); i++)
            delete sensorsRecorders[i];
        sensorsRecorders.clear();
    }
}
