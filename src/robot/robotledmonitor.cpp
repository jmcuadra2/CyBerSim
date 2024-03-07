/***************************************************************************
                          robotmonitor.cpp  -  description
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

#include "robotledmonitor.h"
#include <QHBoxLayout>
#include <QMdiSubWindow>
#include <QCloseEvent>

#include "../neuraldis/windowoperations.h"


RobotLedMonitor::RobotLedMonitor(QWidget* w_parent, const char *name)
    :QWidget(w_parent)
{

    setAttribute(Qt::WA_DeleteOnClose);
    irsmonitor = new IRSMonitor(this, "Proximity sensors monitor");
    lightmonitor = new lightMonitor(this, "Light and bump sensors monitor");
    setObjectName(name);

    QHBoxLayout *layout = new QHBoxLayout;
    layout->setSpacing(10);
    layout->addWidget(irsmonitor);
    layout->addWidget(lightmonitor);
    setLayout(layout);
    resize(irsmonitor->width() + lightmonitor->width() + 20, irsmonitor->height() + 10);
    setMinimumSize(size());
    setWindowTitle(tr("Sensors monitor"));
    
}

RobotLedMonitor::~RobotLedMonitor(){

}

void RobotLedMonitor::closeEvent(QCloseEvent* e)
{

    if(getMdiSubWindow()) {
        getMdiSubWindow()->hide();
        e->ignore();
    }

}

void RobotLedMonitor::resetBump(void)
{    
    lightmonitor->resetBump();
}


void RobotLedMonitor::write(QDomDocument& doc, QDomElement& e)
{

    WindowOperations* w_oper = WindowOperations::instance();
    QDomElement tag_geom = doc.createElement( "geometry" );
    e.appendChild(tag_geom);
    QWidget* w = this;
    w_oper->writePosSize(w, tag_geom);

    QDomElement tag_leds = doc.createElement("Leds");
    e.appendChild(tag_leds);
    writeLED(irsmonitor, doc, tag_leds);
    writeLED(lightmonitor, doc, tag_leds);

}

template<class LM>
void RobotLedMonitor::writeLED(LM* monitor,
                               QDomDocument& doc, QDomElement& e)
{

    QDomElement tag_name = doc.createElement("Led");
    e.appendChild(tag_name);
    tag_name.setAttribute("name", monitor->objectName());
    
}

