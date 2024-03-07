/***************************************************************************
                          robotposition.cpp  -  description
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

#include "robotposition.h"
#include <iomanip>
#include <QFileDialog>
using std::cout;
using std::setw;

void RobotPosition::testSlot()
{

}

RobotPosition::RobotPosition(RobotSprite *rob_sprite) {

    robot_sprite = rob_sprite;

    queueLength = 10;
    robotDataDirectory= QFileDialog::getExistingDirectory(0, ("Select Output Folder"), QDir::currentPath());
}

RobotPosition::~RobotPosition() {

}

/**
  * @brief RobotPosition::xAcceleration
  * @return the mean acceleration of the robot in the x-axis
  */
double RobotPosition::xAcceleration() {
    if(lastNXVelocities.size()>0){
        double previousVelocity = lastNXVelocities.back();
        return (robot_sprite->xVelocity() - previousVelocity) / advancePeriod();
    }
    return 0;
}

double RobotPosition::yAcceleration() {
    if(lastNYVelocities.size()>0){
        double previousVelocity = lastNYVelocities.back();
        return (robot_sprite->yVelocity() - previousVelocity) / advancePeriod();
    }
      return 0;
}
/**
 * @brief RobotPosition::xAccelerated
 * @return the x Position of the robot having the accleration into account
 */
double RobotPosition::xAccelerated(){
//x0(t)=x0+vx0*t+1/2ax0t²

return lastNXPositions.back()+lastNXVelocities.back()*advancePeriod()+0.5*xAcceleration()*advancePeriod()*advancePeriod();
}
/**
 * @brief RobotPosition::yAccelerated
 * @return the Y position of the robot having the acceleration into account
 */
double RobotPosition::yAccelerated(){
    return lastNYPositions.back()+lastNYVelocities.back()*advancePeriod()+0.5*yAcceleration()*advancePeriod()*advancePeriod();;
}
/**
 * @brief RobotPosition::saveInQueue
 * @param value to insert in the queue(x,y,vx,vy)
 * @param queue the queue where the value is intended for(velocities, positions)
 */
void RobotPosition::saveInQueue(double value,queue<double>& queue){
    //save velocity of x in the queue if the size is less than N

    if(queue.size() <= queueLength){
         queue.push(value);

    }else{
        queue.pop();
        queue.push(value);
    }
}

void RobotPosition::writeInFile(){
    QString fileName = robotDataDirectory+"/Data.txt";
    QFile robotPositionFile(fileName);
    QString title = "x\ty\txVelocity\tyVelocity\txAcceleration\tyAcceleration\txAccelerated\tyAccelerated\n";;
    if(!robotPositionFile.exists()){
        QTextStream outStream(&robotPositionFile);
        if (robotPositionFile.open(QIODevice::ReadWrite)) {
            QTextStream stream(&robotPositionFile);
            outStream.setFieldAlignment(QTextStream::AlignLeft);
            outStream <<  title;
        }


    }else{
        robotPositionFile.open(QIODevice::WriteOnly | QIODevice::Append);
        QTextStream ts(&robotPositionFile);
        double xPosition = robot_sprite->x();
        double yPosition = robot_sprite->y();

        double xA = xAcceleration();
        double yA = yAcceleration();
        double xV = xVelocity() ;
        double yV = yVelocity() ;

        ts.setFieldAlignment(QTextStream::AlignLeft);
        ts.setFieldWidth(5);


         ts << xPosition << "\t" << qSetFieldWidth(0);
         ts << "\t";
         ts << yPosition;
         ts << "\t";
         ts << xV;
         ts << "\t";
         ts << yV;
         ts << "\t";
         ts << xA;
         ts << "\t";
         ts << yA;
         ts << "\n";

    }
}



