/***************************************************************************
                          kheperadrawer.h  -  description
                             -------------------
    begin                : Tue Apr 12 2005
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

#ifndef KHEPERADRAWER_H
#define KHEPERADRAWER_H

#include "robotdrawer.h"

/**
  *@author Jose M. Cuadra Troncoso
  */

class KheperaDrawer : public RobotDrawer  {

public:
    KheperaDrawer();
    ~KheperaDrawer();

protected:
    RobotSprite* dibujaRobotSprite(QGraphicsScene* w_canvas,
                           const QString& s_inner_color, const QString& s_sensor_color);
    void setPaintingDiameter(double robot_diamter);

};

#endif
