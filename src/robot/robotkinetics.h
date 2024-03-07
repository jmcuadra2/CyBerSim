/***************************************************************************
                          robotkinetics.h  -  description
                             -------------------
    begin                : Fri Apr 1 2005
    copyright            : (C) 2005 by Jose M. Cuadra
    email                : jose@portatil-host
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef ROBOTKINETICS_H
#define ROBOTKINETICS_H


/**
  *@author Jose M. Cuadra
  */

class RobotKinetics {
  
  public:
  	RobotKinetics();
  	~RobotKinetics();

    double getXRobot(void) { return x_robot ; };
    double getYRobot(void) { return y_robot ; };
    double getInitRotation(void) { return init_rotation ; };
    double getInitRightVel(void) { return init_right_vel ; };
    double getInitLeftVel(void) { return init_left_vel ; };
    void setXRobot(double x_rob)
                              { x_robot  = x_rob ;};
    void setYRobot(double y_rob)
                              { y_robot = y_rob ;};
    void setInitRotation(double init_rot)
                              { init_rotation = init_rot ;};
    void setInitRightVel(double init_r_vel)
                              { init_right_vel = init_r_vel ;};
    void setInitLeftVel(double init_l_vel)
                              { init_left_vel = init_l_vel ;};
    void write(QDomDocument& doc, QDomElement& e);
   
  protected:
    double x_robot, y_robot, init_rotation;
    double init_right_vel, init_left_vel;
    int robot_advance_period;
    QDomElement robotInfo;
  
};

#endif
