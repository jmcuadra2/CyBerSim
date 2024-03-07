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
#ifndef NDARIAROBOTFACTORY_H
#define NDARIAROBOTFACTORY_H

#include "robotfactory.h"
#include <QApplication>

class NDAriaRobot;
class ArRobot;
/**
@author jose manuel
*/

class NDAriaRobotFactory : public RobotFactory
{
public:
    NDAriaRobotFactory();

    ~NDAriaRobotFactory();

    bool setRobotInWorld(void);
    Robot* createRobot(const QDomElement& e);
    Robot* createRobot(void) { return (Robot*)0 ;}
    void readRobotKinetics(const QDomElement& e);
    void readRobotKinetics(void);
    void setRobotKinetics(void);
    void setSensorMode(const QDomElement& e, bool load_first);

protected:
    NDAriaRobot* nda_robot;
       
};

#endif
