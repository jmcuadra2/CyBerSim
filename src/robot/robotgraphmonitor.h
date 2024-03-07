/***************************************************************************
                          robotgraphmonitor.h  -  description
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

#ifndef ROBOTGRAPHMONITOR_H
#define ROBOTGRAPHMONITOR_H



#include "../recmon/boxmonitor.h"

/*! \ingroup robot_monitor
    \brief Caja de monitores de las velocidades lineal y radial del robot.
  */

class RobotGraphMonitor : public BoxMonitor  {

  Q_OBJECT
  public:
    RobotGraphMonitor(const QDomElement& e,
                       QWidget *parent = 0, const char *name = 0);
  	RobotGraphMonitor(int id, QString cap, bool color_pap = true,
               QWidget *parent = 0, const char *name = 0);
  	~RobotGraphMonitor();
   
};

#endif
