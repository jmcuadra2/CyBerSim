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
#include <QMouseEvent>
#include <Aria.h>
#include <ariaUtil.h>
#include <ArClientBase.h>
#include <ArClientSimpleConnector.h>

#include "../neuraldis/xmloperator.h"
#include "../world/ndariaworld.h"
#include "../world/worldview.h"
#include "gui/RobotConnectionDialog.h"
#include "ndariarobot.h"
#include "sensorsset.h"
#include "ndariarobotfactory.h"

NDAriaRobotFactory::NDAriaRobotFactory(): RobotFactory()
{
    nda_robot = 0;
}

NDAriaRobotFactory::~NDAriaRobotFactory()
{
}

Robot* NDAriaRobotFactory::createRobot(const QDomElement& e)
{
    QString servidor;
    QString user;
    QString host;
    QString password;
    QString terminal;
    QString mapa;
    QString remotePort;
    QString userRobot;
    QString dirRobot;
    bool realRobot;
    //   int modo;
    RobotConnectionDialog *robotConnection = new RobotConnectionDialog();
    robotConnection->setGeometry(50, 50, 400, 500);
    robotConnection->setMapa(world_name);
    robotConnection->show();

    int ret = robotConnection->exec();
    if(ret == QDialog::Accepted) {
        //       modo = robotConnection->getModo();
        servidor = robotConnection->getServidor();
        user = robotConnection->getUser();
        host = robotConnection->getHost();
        password = robotConnection->getPassword();
        terminal = robotConnection->getTerminal();
        mapa = robotConnection->getMapa();
        remotePort = robotConnection->getRemotePort();
        userRobot = robotConnection->getUserRobot();
        dirRobot = robotConnection->getDirRobot();
        //    realRobot = robotConnection->isRealRobot();
        realRobot = true;
        delete robotConnection;
    }
    else {
        delete robotConnection;
        return robot;
    }

//    QMessageBox::information(nullptr, world->tr("createRobot"),
//                             world->tr("Wait until ndguiServer is ready"),
//                             world->tr("&Continue"));
    QMessageBox::information(nullptr, world->tr("createRobot"),
                             world->tr("Wait until ndguiServer is ready"),
                             QMessageBox::Ok);

    Aria::init();

    ArClientBase* client = new ArClientBase();
    ArArgumentBuilder* argument_builder = new ArArgumentBuilder();
    if(!host.isEmpty()) {
        argument_builder->add("-host");
        //   argument_builder->add("192.168.0.2");
        argument_builder->add(host.toLatin1().data());
    }
    if(!user.isEmpty()) {
        argument_builder->add("-user");
        //   argument_builder->add("root");
        argument_builder->add(user.toLatin1().data());
    }
    if (password.isEmpty()) {
        argument_builder->add("-nopassword");
    }
    else{
        argument_builder->add("-pwd");
        argument_builder->add(password.toLatin1().data());
    }
    //   ArArgumentParser* parser = new ArArgumentParser(argument_builder);
    //   ArArgumentParser* parser = 0;
    //   ArClientSimpleConnector* clientConnector = new ArClientSimpleConnector (parser);
    ArClientSimpleConnector* clientConnector = new ArClientSimpleConnector (argument_builder);
    //   parser->loadDefaultArguments();

    //   if (!clientConnector->parseArgs() || !parser->checkHelpAndWarnUnparsed())
    if (!clientConnector->parseArgs())
    {
        clientConnector->logOptions();
        return robot;
    }

    if (!clientConnector->connectClient(client))
    {
        if (client->wasRejected())
//            QMessageBox::critical(nullptr, Robot::tr("createRobot"),
//                                  Robot::tr("Server rejected connection"), Robot::tr("&Cancel"));
            QMessageBox::critical(nullptr, Robot::tr("createRobot"),
                                  Robot::tr("Server rejected connection"), QMessageBox::Cancel);
        else
//            QMessageBox::critical(nullptr, Robot::tr("createRobot"),
//                                  Robot::tr("Could not connect to server"), Robot::tr("&Cancel"));
            QMessageBox::critical(nullptr, Robot::tr("createRobot"),
                                  Robot::tr("Could not connect to server"), QMessageBox::Cancel);
        delete client;
        delete clientConnector;
        return robot;

    }
//    QMessageBox::information(nullptr, Robot::tr("createRobot"),
//                             Robot::tr("Connected to server."), Robot::tr("&Ok"));
    QMessageBox::information(nullptr, Robot::tr("createRobot"),
                             Robot::tr("Connected to server."), QMessageBox::Ok);

    bool ok;
    int n_id = 1;
    QString r_name;
    if(!e.isNull()) {
        n_id = e.attribute("num_id").toUInt(&ok);
        r_name = e.attribute("name");
    }
    else
        r_name = "Pioneer-3at";
    nda_robot = new NDAriaRobot(n_id, 0 , r_name);
    //   delete guiServer;
    nda_robot->setAriaRobot(client, clientConnector, realRobot);
    robot = nda_robot;
    //   if(nda_robot)
    //     nda_robot->setMainMotor();

    return robot;
}

void NDAriaRobotFactory::readRobotKinetics(void)
{
    //   x_robot = aria_robot->getX();
    //   y_robot = aria_robot->getY();
    //   init_rotation = aria_robot->getTh();
}

void NDAriaRobotFactory::readRobotKinetics(const QDomElement& e)
{
    RobotFactory::readRobotKinetics(e);

    bool ok;
    XMLOperator* xml_operator = XMLOperator::instance();
    QDomElement e_x = xml_operator->findTag("x_robot", e);
    QDomElement e_y = xml_operator->findTag("y_robot", e);
    QDomElement e_rot = xml_operator->findTag("init_rotation", e);


    x_robot = e_x.text().toDouble(&ok);
    if(!ok) x_robot = 100;
    y_robot = e_y.text().toDouble(&ok);
    if(!ok) y_robot = 100;
    init_rotation = e_rot.text().toDouble(&ok);

    //   x_robot = aria_robot->getX();
    //   y_robot = aria_robot->getY();
    //   init_rotation = aria_robot->getTh();
}

void NDAriaRobotFactory::setRobotKinetics(void)
{
    RobotFactory::setRobotKinetics();
}

bool NDAriaRobotFactory::setRobotInWorld(void)
{

    // definir las traslaciones
    //   NDAriaWorld* nda_world;
    //   if(world->isA("NDAriaWorld"))
    //     nda_world = (NDAriaWorld*) world;
    //   nda_robot->world_x_min_position = nda_world->getXMinPosition();
    //   nda_robot->world_y_min_position = nda_world->getYMinPosition();
    //   nda_robot->world_height = nda_world->getHeight();
    //   nda_robot->world_scale = (double) nda_world->getWorldScale();

    //   ArPose robot_home(nda_world->getXRobotHome(), nda_world->getYRobotHome(), nda_world->getHeadingRobotHome());
    //   aria_robot->setDeadReconPose(robot_home);
    robot->setWorld(world);
    //  nda_robot->setX(nda_robot->x);
    //  nda_robot->setY(nda_robot->y);
    //  nda_robot->rotation(nda_robot->init_rot);
    nda_robot->setX(x_robot);
    nda_robot->setY(y_robot);
    nda_robot->rotation(init_rotation);

    nda_robot->setXYInit(x_robot, y_robot);
    nda_robot->setRotInit(init_rotation);

    nda_robot->robot_sprite->clearPath(true);

    QObject::connect(world->view(), SIGNAL(mouseEventToRobot(QMouseEvent* )), nda_robot, SLOT(getMouseEvent(QMouseEvent* )));
    /*  QObject::connect(world->view(), SIGNAL(keyToRobot(int , bool )), nda_robot, SLOT(getKeyEvent(int , bool ))); */
    QObject::connect(world->view(), SIGNAL(keyToRobot(QKeyEvent* , bool )), nda_robot, SLOT(getKeyEvent(QKeyEvent* , bool )));

    SensorsSet* sensorsSet = nda_robot->getSensorsSet();
    if(sensorsSet) {
        QObject::connect(world, SIGNAL(worldSources(const QHash<QString, QList<RadialFieldSource*> >&)), sensorsSet, SLOT(setRadialFieldSources(const QHash<QString, QList<RadialFieldSource*> >&)));
        world->sendSources();
    }

    world->setRobotRadius(robot->diamtr/2.0);

    world->addRobotItem(robot->getSprite());

    //   nda_robot->setX(nda_world->getXRobotHome());
    //   nda_robot->setY(nda_world->getYRobotHome());
    //   nda_robot->rotation(nda_world->getHeadingRobotHome());

    
    //   RobotFactory::setRobotInWorld(world);
    //   nda_robot->rotation(init_rotation);
    return true;
}

void NDAriaRobotFactory::setSensorMode(const QDomElement& , bool )
{
    //     RobotFactory::setSensorMode(e, load_first);
}
