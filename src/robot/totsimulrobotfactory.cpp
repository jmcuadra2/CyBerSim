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


#include <QMessageBox>

#include "../neuraldis/xmloperator.h"
#include "../world/world.h"
#include "totsimulrobotfactory.h"
#include "totallysimulatedrobot.h"
#include "sensorsset.h"

TotSimulRobotFactory::TotSimulRobotFactory()
    : RobotFactory()
{
    robot_advance_period = 20;
    ts_robot = 0;
}


TotSimulRobotFactory::~TotSimulRobotFactory()
{
}

void TotSimulRobotFactory::setRobotKinetics(void)
{
    RobotFactory::setRobotKinetics();
    if(ts_robot) {
        //     ts_robot->advance_period = robot_advance_period;
        //     ts_robot->init_rot = init_rotation;
        //     ts_robot->x = int(x_robot + 0.5);
        //     ts_robot->y = int(y_robot + 0.5);
        ts_robot->init_left_vel = init_left_vel;
        ts_robot->init_right_vel = init_right_vel;
        ts_robot->old_left_vel = init_left_vel;
        ts_robot->old_right_vel = init_right_vel;
        ts_robot->setTypeIner(type_iner);
        ts_robot->setNoise(noise);
    }

}

void TotSimulRobotFactory::readRobotKinetics(const QDomElement& e)
{

    RobotFactory::readRobotKinetics(e);
    bool ok;
    XMLOperator* xml_operator = XMLOperator::instance();
    QDomElement e_x = xml_operator->findTag("x_robot", e);
    QDomElement e_y = xml_operator->findTag("y_robot", e);
    QDomElement e_rot = xml_operator->findTag("init_rotation", e);
    QDomElement e_rv = xml_operator->findTag("init_right_vel", e);
    QDomElement e_lv = xml_operator->findTag("init_left_vel", e);
    //   QDomElement e_adv = xml_operator->findTag("robot_advance_period", e);

    QDomElement e_inrt = xml_operator->findTag("inertia", e);

    type_iner = e_inrt.text().toInt(&ok);
    if(!ok) type_iner = 2;

    QDomElement e_noise = xml_operator->findTag("noise", e);
    noise = e_noise.text().toInt(&ok);
    if(!ok) noise = 10;

    x_robot = e_x.text().toDouble(&ok);
    if(!ok) x_robot = 100;
    y_robot = e_y.text().toDouble(&ok);
    if(!ok) y_robot = 100;
    init_rotation = e_rot.text().toDouble(&ok);
    init_right_vel = e_rv.text().toDouble(&ok);
    init_left_vel = e_lv.text().toDouble(&ok);
    /*  robot_advance_period = e_adv.text().toInt(&ok); */

}

Robot* TotSimulRobotFactory::createRobot(const QDomElement& e)
{

    bool ok;
    int n_id = 1;
    QString r_name;
    if(!e.isNull()) {
        n_id = e.attribute("num_id").toUInt(&ok);
        r_name = e.attribute("name");
    }
    else
        r_name = "Khepera";
    ts_robot = new TotallySimulatedRobot(n_id, 0, r_name);
    if(ts_robot) {
        robot = ts_robot;
        //     ts_robot->setMainMotor();
    }
    else
        robot = 0;
    return robot;

}

void TotSimulRobotFactory::setSensorMode(const QDomElement& e, bool load_first)
{
    bool ok;
    ts_robot->setSensorMode(e.text().toInt(&ok), load_first);
}

bool TotSimulRobotFactory::setRobotInWorld()
{
    bool ret = RobotFactory::setRobotInWorld();

    ts_robot->robot_sprite->setLVel(robot->init_left_vel/robot->robotScale());
    ts_robot->robot_sprite->setRVel(robot->init_right_vel/robot->robotScale());

    if(ret) {
        SensorsSet* sensorsSet = ts_robot->getSensorsSet();
        if(sensorsSet) {
            QObject::connect(world,
                             SIGNAL(worldSources(const QHash<QString, QList<RadialFieldSource*> >&)),
                             sensorsSet, SLOT(setRadialFieldSources(const QHash<QString, QList<RadialFieldSource*> >&)));
            world->sendSources();
        }

        world->setRobotRadius(robot->diamtr/2.0);
    }
    else {
//        QMessageBox::critical(nullptr, World::tr("Connect world to robot"),
//                              World::tr("It is not an appropriate world type for this robot"), World::tr("&Cancel"));
        QMessageBox::critical(nullptr, World::tr("Connect world to robot"),
                              World::tr("It is not an appropriate world type for this robot"), QMessageBox::Cancel);
        ret = false;
    }
    return ret;
}
