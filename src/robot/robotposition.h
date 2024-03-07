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
#include <queue>
#include <QTextStream>
#include <iostream>
using namespace std;

/**
  *@author Jose M. Cuadra Troncoso
  */

class RobotPosition : public QObject {

    Q_OBJECT

public slots:
    void testSlot();
public:

    RobotPosition(RobotSprite* rob_sprite);
    virtual ~RobotPosition();
    double x(void);
    double y(void);
    double rotation(void);
    double xVelocity(void);
    double yVelocity(void);
    double xAcceleration(void);
    double yAcceleration(void);
    double xAccelerated(void);
    double yAccelerated(void);

    int advancePeriod(void);

    double noiseRatio(void);
    QPointF& rotateWithRobot(double xx, double yy);

protected:
    RobotSprite* robot_sprite;
    int queueLength;
private:
   void saveInQueue(double value,std::queue<double>& myQueue);
   void writeInFile(void);
   QString robotDataDirectory="";
   queue<double> lastNXPositions;
   queue<double> lastNYPositions;
   queue<double> lastNXVelocities;
   queue<double> lastNYVelocities;
};

inline double RobotPosition::x(void)
{
    saveInQueue(robot_sprite ? robot_sprite->x() : 0.0,lastNXPositions);
    writeInFile();
    return robot_sprite ? robot_sprite->x() : 0.0;
};

inline double RobotPosition::y(void)
{
    saveInQueue(robot_sprite ? robot_sprite->y() : 0.0,lastNYPositions);

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
        saveInQueue(robot_sprite ? robot_sprite->xVelocity() : 0.0,lastNXVelocities);
        return robot_sprite ? robot_sprite->xVelocity() : 0.0;
};

inline double RobotPosition::yVelocity(void)
{
        saveInQueue(robot_sprite ? robot_sprite->yVelocity() : 0.0,lastNYVelocities);
        return robot_sprite ? robot_sprite->yVelocity() : 0.0;
};

inline double RobotPosition::noiseRatio(void)
{
    return robot_sprite ? robot_sprite->noiseRatio() : 0.0;
};
inline int RobotPosition::advancePeriod(void)
{
 return robot_sprite->advance_period;
}

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
