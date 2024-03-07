/***************************************************************************
 *   Copyright (C) 2004 by jose manuel                                     *
 *   jose@pcjose                                                           *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
#ifndef ROBOTFACTORY_H
#define ROBOTFACTORY_H

/**
@author jose manuel
*/


#include <QDomDocument>

class AbstractWorld;
class Robot;
// class RobotDrawer;
// class XMLOperator;

class RobotFactory{
  
  public:
    RobotFactory();

    virtual ~RobotFactory();
    
    virtual void readRobotKinetics(const QDomElement& e);
    virtual void readRobotKinetics(void) {};
    virtual void setRobotKinetics(void);
    virtual Robot* createRobot(const QDomElement&) = 0;
    virtual Robot* createRobot(void) { return (Robot*)0; };
    virtual void setSensorMode(const QDomElement& e, bool load_first = false);
    virtual bool setRobotInWorld(void);
    virtual void setWorldName(const QString& w_name) {world_name = w_name ; };
    const QString& getWorldName(void){ return world_name ; };
    
  protected:
    double x_robot, y_robot, init_rotation;
    double init_right_vel, init_left_vel, max_vel;
    Robot* robot;
    AbstractWorld* world;
    int robot_advance_period;
    QString world_name;

};

#endif
