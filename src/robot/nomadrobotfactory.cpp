//
// C++ Implementation: nomadrobotfactory
//
// Description: 
//
//
// Author: Javier Garcia Misis <>, (C) 2010
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include <QMessageBox>
#include <QMouseEvent>

#include "nomadrobotfactory.h"
#include "gui/nomadconnectiondialog.h"
#include <iostream>

#include "nomadrobot.h"

QString NomadRobotFactory::TYPE_NAME = "RealNomadRobot";

NomadRobotFactory::NomadRobotFactory(): RobotFactory()
{
}

NomadRobotFactory::~NomadRobotFactory()
{
}


bool NomadRobotFactory::setRobotInWorld()
{
    return RobotFactory::setRobotInWorld();
}

Robot* NomadRobotFactory::createRobot(const QDomElement& e)
{
    QString servidor;
    QString host;
    QString mapa;
    QString port;
    bool isDirect;
    int id;
    bool createRobot;
    bool robotReal;
    
    NomadConnectionDialog *robotConnection = new NomadConnectionDialog();
    robotConnection->show();
    
    int ret = robotConnection->exec();
    if(ret == QDialog::Accepted) {
        host = robotConnection->getHost();
        port = robotConnection->getPort();
        isDirect = robotConnection->isDirectConnection();
        id = robotConnection->getID();
        createRobot = robotConnection->createNewRobot();
        robotReal =  robotConnection->connectToReal();
        delete robotConnection;
    }
    else {
        delete robotConnection;
        return robot;
    }
    
    bool ok;
    int n_id = 1;
    QString r_name;

    if(!e.isNull()) {
        n_id = e.attribute("num_id").toUInt(&ok);
        r_name = e.attribute("name");
    }

    NomadRobot * nrobot = new NomadRobot(n_id, 0 , r_name);

    if(!nrobot->Connet(id,host,port,isDirect,createRobot)){
        //fallo al conectar
        if(isDirect)
//            QMessageBox::critical(nullptr, Robot::tr("createRobot"),
//                            Robot::tr("Could not connect to robot"), Robot::tr("&Ok"));
            QMessageBox::critical(nullptr, Robot::tr("createRobot"),
                                  Robot::tr("Could not connect to robot"), QMessageBox::Ok);
        else
//            QMessageBox::critical(nullptr, Robot::tr("createRobot"),
//                            Robot::tr("Could not connect to server"), Robot::tr("&Ok"));
            QMessageBox::critical(nullptr, Robot::tr("createRobot"),
                                  Robot::tr("Could not connect to server"), QMessageBox::Ok);
        delete nrobot;
    }
    else{
        if(!isDirect && robotReal){
            if(!nrobot->switchToReal()){
//                QMessageBox::critical(nullptr, Robot::tr("createRobot"),
//                                      Robot::tr("Connect to server. Not to real Robot"), Robot::tr("&Ok"));
                QMessageBox::critical(nullptr, Robot::tr("createRobot"),
                                      Robot::tr("Connect to server. Not to real Robot"), QMessageBox::Ok);
                delete nrobot;
                return robot;
            }
        }
        robot = nrobot;
        nrobot->MostrarVentana();
    }
    return robot;
}

Robot* NomadRobotFactory::createRobot()
{
    return 0;
}

void NomadRobotFactory::readRobotKinetics(const QDomElement& e)
{
    RobotFactory::readRobotKinetics(e);
    bool ok;
    XMLOperator* xml_operator = XMLOperator::instance();
    QDomElement e_vel_max = xml_operator->findTag("max_vel", e);
    QDomElement e_rot_max = xml_operator->findTag("max_rot", e);
    QDomElement e_x = xml_operator->findTag("x_robot", e);
    QDomElement e_y = xml_operator->findTag("y_robot", e);
    QDomElement e_rot = xml_operator->findTag("init_rotation", e);
    
    max_vel = e_vel_max.text().toDouble(&ok);
    if(!ok) max_vel = 609;// mm/s
    
    max_rot_ = e_rot_max.text().toDouble(&ok);
    if(!ok) max_rot_ = 45;// deg/s

    x_robot = e_x.text().toDouble(&ok);
    if(!ok) x_robot = 100;
    y_robot = e_y.text().toDouble(&ok);
    if(!ok) y_robot = 100;
    init_rotation = e_rot.text().toDouble(&ok);

}

void NomadRobotFactory::readRobotKinetics()
{
    
}

void NomadRobotFactory::setRobotKinetics()
{
    RobotFactory::setRobotKinetics();
    if(robot) {
        NomadRobot * nrobot = (NomadRobot *)robot;
        nrobot->setMaxRot(max_rot_);
        nrobot->setInitRot(init_rotation);
        nrobot->setInitX(x_robot);
        nrobot->setInitY(y_robot);
    }
}

void NomadRobotFactory::setSensorMode(const QDomElement& e, bool load_first)
{

}
