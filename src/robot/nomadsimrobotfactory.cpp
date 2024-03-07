//
// C++ Implementation: nomadsimrobotfactory
//
// Description: 
//
//
// Author: Javier Garcia Misis, (C) 2010
//
// Copyright: See COPYING file that comes with this distribution
//
//

#include <QMessageBox>

#include "../neuraldis/xmloperator.h"
#include "../world/world.h"
#include "nomadsimrobotfactory.h"
#include "nomadsimrobot.h"

QString NomadSimRobotFactory::TYPE_NAME = "SimulatedNomadRobot";

NomadSimRobotFactory::NomadSimRobotFactory()
{
    max_rot_ =0;
}


NomadSimRobotFactory::~NomadSimRobotFactory()
{
}


Robot* NomadSimRobotFactory::createRobot(const QDomElement& e)
{
    bool ok;
    int n_id = 1;
    QString r_name;
    
    if(!e.isNull()) {
        n_id = e.attribute("num_id").toUInt(&ok);
        r_name = e.attribute("name");
    }
    else
        r_name = "Nomad";

    ns_robot = new NomadSimRobot(n_id, (QObject*)0, r_name.toLatin1().data());
    if(ns_robot) {
        robot = ns_robot;
    }
    else
        robot = 0;

    return robot;
}

void NomadSimRobotFactory::setRobotKinetics(void)
{
    RobotFactory::setRobotKinetics();
    if(ns_robot) {
        ns_robot->init_left_vel = init_left_vel;
        ns_robot->init_right_vel = init_right_vel;
        ns_robot->old_left_vel = init_left_vel;
        ns_robot->old_right_vel = init_right_vel;
        ns_robot->setTypeIner(type_iner);
        ns_robot->setNoise(noise);
        ns_robot->setMaxRot(max_rot_);
    }

}

void NomadSimRobotFactory::readRobotKinetics(const QDomElement& e)
{

    RobotFactory::readRobotKinetics(e);
    bool ok;
    XMLOperator* xml_operator = XMLOperator::instance();
    QDomElement e_x = xml_operator->findTag("x_robot", e);
    QDomElement e_y = xml_operator->findTag("y_robot", e);
    QDomElement e_rot = xml_operator->findTag("init_rotation", e);
    QDomElement e_rv = xml_operator->findTag("init_right_vel", e);
    QDomElement e_lv = xml_operator->findTag("init_left_vel", e);
    QDomElement e_vel_max = xml_operator->findTag("max_vel", e);
    QDomElement e_rot_max = xml_operator->findTag("max_rot", e);
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

    max_vel = e_vel_max.text().toDouble(&ok);
    if(!ok) max_vel = 609;// mm/s

    max_rot_ = e_rot_max.text().toDouble(&ok);
    if(!ok) max_rot_ = 45;// deg/s

}
void NomadSimRobotFactory::setSensorMode(const QDomElement& e, bool load_first)
{
    bool ok;
    ns_robot->setSensorMode(e.text().toInt(&ok), load_first);
}

bool NomadSimRobotFactory::setRobotInWorld()
{
    bool ret = RobotFactory::setRobotInWorld();

    ns_robot->robot_sprite->setLVel(robot->init_left_vel/robot->robotScale());
    ns_robot->robot_sprite->setRVel(robot->init_right_vel/robot->robotScale());

    if(ret) {
        SensorsSet* sensorsSet = ns_robot->getSensorsSet();
        if(sensorsSet) {
            QObject::connect(world, SIGNAL(worldSources(const QHash<QString, QList<RadialFieldSource*> >&)), sensorsSet, SLOT(setRadialFieldSources(const QHash<QString, QList<RadialFieldSource*> >&)));
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
