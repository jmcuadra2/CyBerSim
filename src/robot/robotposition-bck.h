/***************************************************************************
                          robotposition.h  -  description
                             -------------------
    begin                : Fri Jul 15 2005
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

#ifndef ROBOTPOSITION_H
#define ROBOTPOSITION_H

#include "robotsprite.h"
#include "../neuraldis/ndmath.h"


/**
  *@author Jose M. Cuadra Troncoso
  */

class RobotPosition {

public:
    RobotPosition(RobotSprite* rob_sprite);
    ~RobotPosition();
    double x(void);
    double y(void);
    double rotation(void);
    double xVelocity(void);
    double yVelocity(void);
    double noiseRatio(void);
    QPointF& rotateWithRobot(double xx, double yy);

protected:
    RobotSprite* robot_sprite;

};

inline double RobotPosition::x(void)
{
    return robot_sprite ? robot_sprite->x() : 0.0;
};

inline double RobotPosition::y(void)
{
    return robot_sprite ? robot_sprite->y() : 0.0;
};

inline double RobotPosition::rotation(void)
{
    double rot = 0.0;
    if(robot_sprite)
        rot = robot_sprite->getRotation();
    return rot;
};

inline double RobotPosition::xVelocity(void)
{
    return robot_sprite ? robot_sprite->xVelocity() : 0.0;
};

inline double RobotPosition::yVelocity(void)
{
    return robot_sprite ? robot_sprite->yVelocity() : 0.0;
};

inline double RobotPosition::noiseRatio(void)
{
    return robot_sprite ? robot_sprite->noiseRatio() : 0.0;
};  

inline QPointF& RobotPosition::rotateWithRobot(double xx, double yy)
{
    static QPointF new_p;
    if(robot_sprite) {
        new_p.setX(robot_sprite->x() + xx*robot_sprite->cos_rotation - yy*robot_sprite->sin_rotation);
        new_p.setY(robot_sprite->y() + xx*robot_sprite->sin_rotation + yy*robot_sprite->cos_rotation);
    }
    else {
        new_p.setX(xx);
        new_p.setY(yy);
    }
    return new_p;
}

#endif
