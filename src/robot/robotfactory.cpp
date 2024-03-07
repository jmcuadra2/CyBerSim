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



#include "../neuraldis/xmloperator.h"
#include "../neuraldis/settings.h"
#include "../world/abstractworld.h"
#include "../world/worldview.h" 
#include "robotfactory.h"
#include "robot.h"

RobotFactory::RobotFactory()
{
  robot = 0;
  x_robot = 100;
  y_robot = 100;
  init_rotation = 0;
  init_right_vel = 0;
  init_left_vel = 0;
  max_vel = 100;
  robot_advance_period = 20;
  Settings* settings = Settings::instance();  
  world = settings->getWorld();
}

RobotFactory::~RobotFactory()
{
}

void RobotFactory::setSensorMode(const QDomElement&  , bool ) {}

void RobotFactory::readRobotKinetics(const QDomElement& e)
{
  if(!e.isNull()) {
    bool ok;
    XMLOperator* xml_operator = XMLOperator::instance();  
    QDomElement e_max_vel = xml_operator->findTag("max_vel", e);
    max_vel = e_max_vel.text().toDouble(&ok);
    if(!ok) max_vel = 100;
    QDomElement e_adv = xml_operator->findTag("robot_advance_period", e);
    robot_advance_period = e_adv.text().toInt(&ok);  
  }   
}

void RobotFactory::setRobotKinetics(void)
{
  if(robot) {
    robot->setMaxVel(max_vel);
    robot->setAdvancePeriod(robot_advance_period);
  }
}

bool RobotFactory::setRobotInWorld(void)
{

  QObject::connect(world, SIGNAL(worldCleared(void)), robot, SLOT(worldCleared(void)));
  QObject::connect(world, SIGNAL(worldLoaded(void)), robot, SLOT(worldLoaded(void)));
  
  QObject::connect(world->view(), SIGNAL(mouseEventToRobot(QMouseEvent* )),
                   robot, SLOT(getMouseEvent(QMouseEvent* )));
  QObject::connect(world->view(), SIGNAL(keyToRobot(QKeyEvent* , bool )),
                   robot, SLOT(getKeyEvent(QKeyEvent* , bool )));
//   QObject::connect(world->view(), SIGNAL(keyToRobot(int , bool )), robot, SLOT(getKeyEvent(int , bool )));
     
  QObject::connect(robot, SIGNAL(sendRadius(const double&)),
                   world, SLOT(setRobotRadius(const double& )));
  robot->setWorld(world);
//   robot->init_rot = init_rotation;
  robot->x = NDMath::roundInt(x_robot);
  robot->y = NDMath::roundInt(y_robot);
  
  robot->setX(robot->x);
  robot->setY(robot->y);
  robot->robot_sprite->clearPath(true);
  robot->rotation(init_rotation);
  
  world->addRobotItem(robot->getSprite());
//   robot->robot_sprite->setRotation(robot->init_rot);
//   robot->robot_sprite->setLVel(robot->init_left_vel/robot->robotScale());
//   robot->robot_sprite->setRVel(robot->init_right_vel/robot->robotScale());

  return true;
  
}

