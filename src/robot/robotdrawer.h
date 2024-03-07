/***************************************************************************
                          robotdrawer.h  -  description
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

#ifndef ROBOTDRAWER_H
#define ROBOTDRAWER_H

#include <QDomDocument>
#include <QBitmap>
#include <QPixmap>
#include <QGraphicsItem>
#include <QFile>

#include "../neuraldis/neuraldisdoc.h"
#include "robotsprite.h"

class Robot;

/*! \ingroup robot_base 
    \brief Dibuja o lee de un fichero el pixmap y la
    mascara de colision del robot y crea el correspondiente
    sprite para la animacion.
  */

class RobotDrawer {
public:
    RobotDrawer();
    virtual ~RobotDrawer();

    virtual bool drawRobot(QGraphicsScene* w_canvas, Robot* robot, const QDomElement& e);

protected:
    virtual void setPaintingDiameter(double robot_diamter);
    
protected:
    qreal diamtr;
    double rot;
    bool file_changed;

protected:
    QString loadImage (QString& image_file);
    virtual RobotSprite* creaRobotSprite(QGraphicsScene* w_canvas);
    virtual RobotSprite* dibujaRobotSprite(QGraphicsScene* w_canvas, const QString& s_inner_color,
                                           const QString& s_sensor_color);

protected:
    QString fileName;


};

#endif
