/***************************************************************************
                          robotdrawer.cpp  -  description
                             -------------------
    begin                : Tue Apr 12 2005
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

#include "robotdrawer.h"
#include "robot.h"
#include "../neuraldis/xmloperator.h"
#include "../neuraldis/ndmath.h"
#include "../neuraldis/settings.h"

#include <QPixmap>

RobotDrawer::RobotDrawer()
{   
    diamtr = 0;
    file_changed = false;
}

RobotDrawer::~RobotDrawer(){
}

bool RobotDrawer::drawRobot(QGraphicsScene* w_canvas, Robot* robot, const QDomElement& e)
{

    XMLOperator* xml_operator = XMLOperator::instance();
    bool ret = false;

//    setPaintingDiameter(NDMath::roundInt(robot->diamtr));
    setPaintingDiameter(robot->diamtr);
    QDomElement e_r_img = xml_operator->findTag("robot_image_file", e);
    fileName = e_r_img.text();
    if(!fileName.isEmpty()) {
        robot->robot_image_file = QDir::cleanPath(Settings::instance()->getAppDirectory() + QDir::separator() + fileName);
        fileName = robot->robot_image_file;
    }

    if(!robot->robot_image_file.isEmpty()) {
        robot->setRobotSprite(creaRobotSprite(w_canvas));
        ret = true;
    }
    else {
        robot->setRobotSprite(dibujaRobotSprite(w_canvas, "", ""));
        if(!fileName.isEmpty()) {
            file_changed = true;
            robot->robot_image_file = fileName;
            ret = true;
        }
    }
    if(file_changed)
        robot->setFileChanged(true);
    return ret;

} 

RobotSprite* RobotDrawer::creaRobotSprite(QGraphicsScene* w_canvas)
{
    RobotSprite* robot_sprite = new RobotSprite(w_canvas, fileName, 0);
    robot_sprite->setData(1, "RobotSprite");
    return robot_sprite;
}

void RobotDrawer::setPaintingDiameter(double robot_diamter)
{
//    diamtr = int(robot_diamter*sqrt(2)) + 1;
    diamtr = robot_diamter*sqrt(2);
}

RobotSprite* RobotDrawer::dibujaRobotSprite(QGraphicsScene* , const QString& , const QString&)
{
    fileName = "";
    RobotSprite* p = 0;
    return p ;

}

QString RobotDrawer::loadImage (QString& image_file)
{
    bool to_continue = true;
    QFile file(image_file);
    if(!file.exists()) {
        NeuralDisDoc* doc_manager = NeuralDisDoc::instance();
        QString sender = "RobotDrawer";
        QString type_file = "svg";
        while(to_continue) {
            to_continue = doc_manager->getFileDialog(image_file, sender, type_file);
            if(to_continue) {
                if(file.exists()) {
                    to_continue = false;
                    file_changed = true;
                }
            }
        }
    }
    return file.fileName();
}
