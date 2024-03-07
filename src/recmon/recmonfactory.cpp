/***************************************************************************
                          recmonfactory.cpp  -  description
                             -------------------
    begin                : Sat May 21 2005
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

#include <QMdiArea>
#include <QMdiSubWindow>

#include "recmonfactory.h"
#include "boxmonitor.h"
#include "graphicmonitor.h"
#include "devicerecorder.h"
#include "../robot/robotrecorder.h"
#include "../robot/sensorsrecorder.h"
#include "../control/accontrolrecorder.h"
#include "../neuraldis/settings.h"
#include "../neuraldis/windowoperations.h"
#include "../neuraldis/xmloperator.h"

RecMonFactory* RecMonFactory::factory = nullptr;

RecMonFactory* RecMonFactory::instance(void)
{

  if(factory == 0)
    factory = new RecMonFactory;
  return factory;

}

RecMonFactory::RecMonFactory()
{

  xml_operator = XMLOperator::instance();
    
}

RecMonFactory::~RecMonFactory(){
}

BoxMonitor* RecMonFactory::factoryBoxMonitor(QDomElement& e)
{

  QString name;
  GraphicMonitor* monitor = nullptr;
  BoxMonitor* box_mon = nullptr;
  Settings* prog_settings = Settings::instance();
  bool ok;
 
  QDomElement e_type = xml_operator->findTag("type", e);
  int type = e_type.text().toInt(&ok);
  if(!ok) type = 1;
  QDomElement e_cons = xml_operator->findTag("constructor", e);
  QDomElement e_geom = xml_operator->findTag("geometry", e);
  QDomElement e_mons = xml_operator->findTag("Monitors", e);
  
  
  if(e_cons.isNull() || e_mons.isNull()) return box_mon;
  if(type == 1) {
    box_mon = new BoxMonitor(e_cons, prog_settings->getWorkspace());
//D//     prog_settings->getWorkspace()->addWindow(box_mon);
    QMdiSubWindow* subw = prog_settings->getWorkspace()->addSubWindow(box_mon);
    box_mon->setMdiSubWindow(subw);
  }
  WindowOperations* w_oper = WindowOperations::instance();
  w_oper->setPosSize(box_mon, e_geom);

  QDomNode n_mon = e_mons.firstChild();
  while(!n_mon.isNull()) {
    QDomElement e_mon = n_mon.toElement();
    if(!e_mon.isNull()) {
      monitor = factoryGraphicMonitor(e_mon, box_mon);
      if(monitor)
        box_mon->addMon(monitor);
    }
    n_mon = n_mon.nextSibling();
  }
  
  if(box_mon) {
    box_mon->init();
    box_mon->reset();
  }
  return box_mon;

}

GraphicMonitor* RecMonFactory::factoryGraphicMonitor(QDomElement& e,
                              BoxMonitor* box_mon)
{

  GraphicMonitor* monitor = 0;
//  if(e.tagName() != "Monitor") return monitor;

  QString name;
  QDomElement e_ident = xml_operator->findTag("mon_identification", e);
  QDomElement e_cons = xml_operator->findTag("constructor", e);
  QDomElement e_graphs = xml_operator->findTag("graphs", e);
  if(e_ident.isNull() || e_cons.isNull() || e_graphs.isNull())
    return monitor;
  name = e_ident.text();
  monitor = new GraphicMonitor(e_cons, box_mon, name);
  monitor->addGraphs(e_graphs);
  return monitor;
}

FileRecorder* RecMonFactory::factoryRecorder(QDomElement& e)
{

  FileRecorder* recorder = nullptr;
  bool ok;

//  if(e.tagName() != "Recorder") return recorder;  
  
  QDomElement e_type = xml_operator->findTag("type", e);
  int type = e_type.text().toInt(&ok);
  if(!ok) type = FileRecorder::ROBOT_RECORDER; 
  QDomElement e_cons = xml_operator->findTag("constructor", e);
  QDomElement e_channels = xml_operator->findTag("channels", e);
  if(type != FileRecorder::ROBOT_RECORDER) {
    if(e_cons.isNull() || e_channels.isNull())
      return recorder;
  }
  if(type == FileRecorder::NEURON_RECORDER)
    recorder = new DeviceRecorder(e_cons);
  else if(type == FileRecorder::ROBOT_RECORDER) {
    QString f_name = xml_operator->findTag("file_name", e).text();
    if(f_name.isEmpty()) f_name = "Untitled";
    recorder = new RobotRecorder(f_name);
    return recorder;
  }
  else if(type == FileRecorder::SENSORS_RECORDER)
    recorder = new SensorsRecorder(e_cons);
  else if(type == FileRecorder::ACCONTROL_RECORDER)
    recorder = new ACControlRecorder(e_cons);
    
  recorder->addChannels(e_channels); 
  return recorder;

}
