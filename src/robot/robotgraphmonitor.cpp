/***************************************************************************
                          robotgraphmonitor.cpp  -  description
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

#include "robotgraphmonitor.h"

RobotGraphMonitor::RobotGraphMonitor(const QDomElement& e,
                       QWidget *parent, const char *name)
        : BoxMonitor(e, parent, name)
{
}            

RobotGraphMonitor::RobotGraphMonitor(int id, QString cap, bool color_pap,
                         QWidget *parent, const char *name)
                  : BoxMonitor(id, cap, color_pap, parent, name)       
{
}

RobotGraphMonitor::~RobotGraphMonitor(){
}

