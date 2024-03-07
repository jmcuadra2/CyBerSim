//
// C++ Implementation: areacenterreactivecontrol
//
// Description:
//
//
// Author: Jose Manuel Cuadra Troncoso <jmcuadra@dia.uned.es>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//

#include "areacenterreactivecontrol.h"
#include "inputfusionnode2d.h"
#include "smginterface.h"
#include "accontrolrecorder.h"
#include "netinputfusionnode.h"
#include "blobcamerafusionnode.h"
#include "arialaserfusionnode.h"
#include "../robot/ndariasick.h"
#include <QtGui>

#include <iostream>

#include "gui/accontroltab.h"
#include "../world/abstractworld.h"

AreaCenterReactiveControl::AreaCenterReactiveControl(): /*QObject(),*/ NullSimControl()
{
    robotDiameter = 0;
    robotScale= 1.0;
    advanceStatus = AreaCenterPolygon::All;
    localAngleStart = -90.5;
    localAngleEnd = 90.5;
    //   globalAreaCenter = new AreaCenter();
    globalAreaCenter = 0;
    advanceAreaCenter = new AreaCenter();
    world = 0;
    sensorMaxVal = 5000;
    //   numCollisionSectors = 5;
    //   detectedCollisionSectors.fill(false, numCollisionSectors);
    //
    //   collisionDistances << 600 << 700 << 900 << 700 << 600;
    //   currentCollisionDistances = collisionDistances;

    isRecording = false;
    recorder_on = false;
//    recorder = new ACControlRecorder("/home/jose/cybersim/examples/controls/acrecorder.dat");
    recorder = new ACControlRecorder("acrecorder.dat");
    connect(this, SIGNAL(sendToRecorder(double, int)), recorder, SLOT(writeValue(double ,int )));

    advanceRate = 1.0;

    goalSidePreference = AreaCenterPolygon::All;

    falseReadings = 0;

    suggestedSplit = false;
    advanceRateThresSplit = 0.85;
    stalled = NotStalled;
    countStall = 0;
    lvel = 0;
    rvel = 0;

    pan = 0.0;
    blobArea = -1;

    areaCenterPolygon = 0;

}


AreaCenterReactiveControl::~AreaCenterReactiveControl()
{
    if(globalAreaCenter)
        delete globalAreaCenter;
    delete advanceAreaCenter;
    delete recorder;
    delete areaCenterPolygon;
}


int AreaCenterReactiveControl::getSuperType(void)
{
    return CODE_CONTROL;
}

int AreaCenterReactiveControl::getType(void)
{
    return AREA_CENTER_REACTIVE_CONTROL;
}

void AreaCenterReactiveControl::readGlobalPolygon(void)
{

    double d, x, y;
    double max = -1;
    int numNodes = areaCenterNodeList.size();
    //   areaCenterPolygon->clear();
    for(int i = 0; i < numNodes; i++) {
        d = areaCenterNodeList[i]->result().getY().at(0);
        max = d > max ? d : max;
        //     NDPose* pose = areaCenterNodeList[i]->result().getPose();
        //     NDPose ppose = *pose;
        //     std::cout << ppose << std::endl;
    }

    falseReadings = 0;
    //   max = max < 1000 ? 1000 : max;
    for(int i = 0; i < numNodes; i++) {
        d = areaCenterNodeList[i]->result().getY().at(0);
        if(d < 0.0) {
            // //       d = max;
            //       falseReadings++;
            // /*      x = areaCenterNodeList[i]->result().getY().at(2)/1000.0*max; // 1000 viene asignacion de max y de InputFusionNode2D::proceessInformation()
            //       y = areaCenterNodeList[i]->result().getY().at(3)/1000.0*max; */
            x = areaCenterNodeList[i]->result().getY().at(2); // 1000 viene asignacion de max y de InputFusionNode2D::proceessInformation()
            y = areaCenterNodeList[i]->result().getY().at(3);
            //       areaCenterPolygon->setAdvanceStatus(AreaCenterPolygon::Conservative);
            //       continue;
        }
        else {
            x = areaCenterNodeList[i]->result().getY().at(2);
            y = areaCenterNodeList[i]->result().getY().at(3);
        }
        //     areaCenterPolygon->append(QPoint(x, y));
        (*areaCenterPolygon)[i].setX(x);
        (*areaCenterPolygon)[i].setY(y);
    }
    if(falseReadings) {
        if(advanceStatus == AreaCenterPolygon::All) {
            areaCenterPolygon->setAdvanceStatus(AreaCenterPolygon::Conservative);
            areaCenterPolygon->setAdvanceMaxRange(double(numNodes - falseReadings)/numNodes*sensorMaxVal);
        }
    }
}

// void AreaCenterReactiveControl::readPolygons(bool advanceToo)
// {
//   double d, x, y;
//   int numNodes = areaCenterNodeList.size();
//   
// //   double sensorOrientation, frontAngle = 60/NDMath::RAD2GRAD;
// //   detectedCollisionSectors.fill(false, detectedCollisionSectors.size());
// //   currentCollisionDistances = collisionDistances;
// 
//   for(int i = 0; i < numNodes; i++) {
//     d = areaCenterNodeList[i]->result().getY().at(0);
//     if(d < 0.0)
//       continue;
//     x = areaCenterNodeList[i]->result().getY().at(2);
//     y = areaCenterNodeList[i]->result().getY().at(3);
//     areaCenterPolygon[i].setX(x);
//     areaCenterPolygon[i].setY(y);
// 
//     if(advanceToo) {
//       areaCenterPolygon->calculateAdvancePolygon();
//          
// //       sensorOrientation = areaCenterNodeList[i]->result().getPose()->getOrientation().at(0);
// //       if((sensorOrientation < localAngleStart)) {
// //         if(d <= currentCollisionDistances.at(0)) {
// //           currentCollisionDistances[0] = d;
// //           detectedCollisionSectors[0] = true;
// //         }
// //       }
// //       else if((sensorOrientation >= localAngleStart) && (sensorOrientation < -frontAngle/2)) {
// //         if(d <= currentCollisionDistances.at(1)) {
// //           currentCollisionDistances[1] = d;
// //           detectedCollisionSectors[1] = true;
// //         }
// //       }
// //       else if((sensorOrientation >= -frontAngle/2) && (sensorOrientation <= frontAngle/2)) {
// //         if(d <= currentCollisionDistances.at(2)) {
// //           currentCollisionDistances[2] = d;
// //           detectedCollisionSectors[2] = true;
// //         }
// //       }
// //       else if((sensorOrientation > frontAngle/2) && (sensorOrientation <= localAngleEnd)) {
// //         if(d <= currentCollisionDistances.at(3)) {
// //           currentCollisionDistances[3] = d;
// //           detectedCollisionSectors[3] = true;
// //         }
// //       }
// //       else if(sensorOrientation > localAngleEnd) {
// //         if(d <= currentCollisionDistances.at(4)) {
// //           currentCollisionDistances[4] = d;
// //           detectedCollisionSectors[4] = true;
// //         }
// //       }     
//     }
//   }
// }

AreaCenterReactiveControl::StallStatus AreaCenterReactiveControl::getStalled(void)
{
    stalled = NotStalled;
    if(inputNodeMap.contains("Bumper_sensors")) {
        QVector< AbstractFusionNode* > inputNodeList = inputNodeMap.value("Bumper_sensors");
        int num_nodes = inputNodeList.size();
        for(int i = 0; i < num_nodes; i++) {
            if(inputNodeList[i]->result().getY().at(0) > 0.01) {
                if(i >= 2 && i <=6) {
                    if(stalled == BackStalled || stalled == ForeBackStalled)
                        stalled = ForeBackStalled;
                    else
                        stalled = ForeStalled;
                }
                else {
                    if(stalled == ForeStalled || stalled == ForeBackStalled)
                        stalled = ForeBackStalled;
                    else
                        stalled = BackStalled;
                }

            }
        }
    }
    return stalled;
}

void AreaCenterReactiveControl::calculateAreaCenters(void)
{

    QPointF advanceAreaCenterCoords;
    bool accesible = false;
    double advanceRateIncrement = 1.025;

    if(getStalled() || countStall) {
        if(advanceStatus == AreaCenterPolygon::Lost)  // escapando
            advanceAreaCenter->setPoint(areaCenterPolygon->advanceAreaCenter());
        return;
    }

    readGlobalPolygon();
    //   qDebug() << areaCenterPolygon;
    if(globalAreaCenter)
        globalAreaCenter->setPoint(areaCenterPolygon->globalAreaCenter());

    //   advanceStatus = areaCenterPolygon->getAdvanceStatus();
    if(advanceStatus == AreaCenterPolygon::Lost) { // escapando
        advanceAreaCenter->setPoint(areaCenterPolygon->advanceAreaCenter());
        return;
    }

    do {
        //     if(advanceStatus != AreaCenterPolygon::All)
        advanceRate = areaCenterPolygon->advanceRate();
        if(advanceStatus == AreaCenterPolygon::All || advanceStatus == AreaCenterPolygon::Conservative) {
            if(advanceRate < 1) {
                advanceRate *= advanceRateIncrement;
                if(advanceRate > 1) {
                    advanceRate = 1;
                }
            }
        }
        areaCenterPolygon->setAdvanceMaxRange(advanceRate*sensorMaxVal);

        areaCenterPolygon->calculateAdvancePolygon();
        advanceStatus = areaCenterPolygon->getAdvanceStatus();
        if(advanceStatus == AreaCenterPolygon::Lost) { // escapando
            advanceAreaCenter->setPoint(areaCenterPolygon->advanceAreaCenter());
            break;
        }

        advanceAreaCenterCoords = areaCenterPolygon->advanceAreaCenter();

        if(suggestedSplit) {
            //       if(advanceRate > 0.75)
            if(advanceRate > advanceRateThresSplit)
                accesible = false;
            else {
                accesible = areaCenterPolygon->isAdvanceAccesible(advanceAreaCenterCoords, robotDiameter/2, advanceStatus);
                suggestedSplit = false;
            }
        }
        else
            accesible = areaCenterPolygon->isAdvanceAccesible(advanceAreaCenterCoords, robotDiameter/2, advanceStatus);

        if(!accesible) {
            bool ret = areaCenterPolygon->addSplitPoint(advanceAreaCenterCoords, suggestedSplit, goalSidePreference);
            if(suggestedSplit) {
                suggestedSplit = false;
                if(!ret)
                    break;
            }
            advanceStatus = areaCenterPolygon->getAdvanceStatus();
            if(advanceStatus == AreaCenterPolygon::Lost) {
                advanceAreaCenterCoords = areaCenterPolygon->advanceAreaCenter();
                break;
            }

            areaCenterPolygon->calculateAdvancePolygon();
            advanceStatus = areaCenterPolygon->getAdvanceStatus();
            if(advanceStatus == AreaCenterPolygon::Lost) {
                advanceAreaCenterCoords = areaCenterPolygon->advanceAreaCenter();
                break;
            }

            advanceAreaCenterCoords = areaCenterPolygon->advanceAreaCenter();
            advanceStatus = areaCenterPolygon->getAdvanceStatus();
            if(advanceStatus == AreaCenterPolygon::Lost) {
                advanceAreaCenterCoords = areaCenterPolygon->advanceAreaCenter();
                break;
            }

        }

    } while(!accesible);

    advanceAreaCenter->setPoint(advanceAreaCenterCoords);

    switch (advanceStatus) {
    case AreaCenterPolygon::All:
        advanceAreaCenter->setColor(QColor("magenta"));
        break;
    case AreaCenterPolygon::Left:
        advanceAreaCenter->setColor(QColor("orange"));
        break;
    case AreaCenterPolygon::Right:
        advanceAreaCenter->setColor(QColor("darkred"));
        break;
    case AreaCenterPolygon::Lost:
        advanceAreaCenter->setColor(QColor("red"));
        break;
    case AreaCenterPolygon::Conservative:
        advanceAreaCenter->setColor(QColor("darkmagenta"));
        break;
    }

    return;
}


// void AreaCenterReactiveControl::calculateAreaCenters(void)
// {
// 
//   QPointF advanceAreaCenterCoords;
//   bool accesible = false;
//   double advanceRateIncrement = 1.025;
// 
//   readPolygons();
//   globalAreaCenter->setPoint(areaCenterPolygon->globalAreaCenter());
//   
//   advanceStatus = areaCenterPolygon->getAdvanceStatus();
//   if(advanceStatus == AreaCenterPolygon::Lost) { // escapando
//     advanceAreaCenter->setPoint(areaCenterPolygon->advanceAreaCenter());
//     return;
//   }  
// 
//   do {
// //     if(advanceStatus != AreaCenterPolygon::All)
//     if(advanceStatus != AreaCenterPolygon::All && advanceStatus != AreaCenterPolygon::Conservative)
// //       advanceRate = areaCenterPolygon->advanceRate()/2;
//       advanceRate = areaCenterPolygon->advanceRate();
//     else {
//       if(advanceRate < 1) {
//         advanceRate *= advanceRateIncrement;
//         if(advanceRate > 1) {
//           advanceRate = 1;
//         }
//       }
//     }
//     areaCenterPolygon->setAdvanceMaxRange(advanceRate*sensorMaxVal);
//     advanceAreaCenterCoords = areaCenterPolygon->advanceAreaCenter();
// 
//     accesible = areaCenterPolygon->isAdvanceAccesible(advanceAreaCenterCoords, robotDiameter/2);
//     if(!accesible) {
//       areaCenterPolygon->addSplitPoint(advanceAreaCenterCoords, goalSidePreference);
//       advanceStatus = areaCenterPolygon->getAdvanceStatus();
//       if(advanceStatus == AreaCenterPolygon::Lost) {
//         advanceAreaCenterCoords = areaCenterPolygon->advanceAreaCenter();
//         break;
//       }
//       
//       areaCenterPolygon->calculateAdvancePolygon();
//       advanceStatus = areaCenterPolygon->getAdvanceStatus();
//       if(advanceStatus == AreaCenterPolygon::Lost) {
//         advanceAreaCenterCoords = areaCenterPolygon->advanceAreaCenter();
//         break;
//       }
//       
//       advanceAreaCenterCoords = areaCenterPolygon->advanceAreaCenter();
//       advanceStatus = areaCenterPolygon->getAdvanceStatus();
//       if(advanceStatus == AreaCenterPolygon::Lost) {
//         advanceAreaCenterCoords = areaCenterPolygon->advanceAreaCenter();
//         break;
//       }       
//       
//     }
// 
//   } while(!accesible);
//        
//   advanceAreaCenter->setPoint(advanceAreaCenterCoords);
//   
// //   record();
//   
//   switch (advanceStatus) {    void processCollisions(void);
//     case AreaCenterPolygon::All:
//       advanceAreaCenter->setColor(QColor("magenta"));
//       break;
//     case AreaCenterPolygon::Left:
//       advanceAreaCenter->setColor(QColor("orange"));
//       break;
//     case AreaCenterPolygon::Right:
//       advanceAreaCenter->setColor(QColor("darkred"));
//       break;
//     case AreaCenterPolygon::Lost:
//       advanceAreaCenter->setColor(QColor("red"));
//       break;
//     case AreaCenterPolygon::Conservative:
//       advanceAreaCenter->setColor(QColor("darkmagenta"));    
//       break;
//   }
// 
//   return;
// }

// void AreaCenterReactiveControl::calculateAreaCenters(void)
// {
// 
//   QPointF advanceAreaCenterCoords;
//   bool accesible;
// 
//   readPolygons();
//   advanceStatus = areaCenterPolygon->getAdvanceStatus();
// 
//   if(advanceStatus == AreaCenterPolygon::Lost) { // escapando
//       return;        
//   }
//   globalAreaCenter->setPoint(areaCenterPolygon->globalAreaCenter());
//   advanceAreaCenter->setPoint(areaCenterPolygon->advanceAreaCenter());
//   advanceAreaCenterCoords = *advanceAreaCenter;
//   
//   if(advanceStatus == AreaCenterPolygon::All) {
// 
//     accesible = areaCenterPolygon->isAdvanceAccesible(advanceAreaCenterCoords, robotDiameter);
// 
//     if(!accesible) {
//       areaCenterPolygon->addSplitPoint(advanceAreaCenterCoords);
//       areaCenterPolygon->calculateAdvancePolygon();
// 
//       advanceAreaCenterCoords = areaCenterPolygon->advanceAreaCenter();
// 
//       advanceStatus = areaCenterPolygon->getAdvanceStatus();
//       switch (advanceStatus) {
//         case AreaCenterPolygon::All:
//           break;
//         case AreaCenterPolygon::Left:
//           advanceAreaCenter->setColor(QColor("orange"));
//           break;
//         case AreaCenterPolygon::Right:
//           advanceAreaCenter->setColor(QColor("darkred"));
//           break;
//         case AreaCenterPolygon::Lost:
//           int kk = 0;
//           break;
//       }
//     }
//     else
//       advanceAreaCenter->setColor(QColor("magenta"));
//   }
//   else {
//     double min = 0.5;
//     double k = areaCenterPolygon->advanceRate();
//     k = 4*(1 - min)*(k*k - k) + 1;
//     areaCenterPolygon->setAdvanceMaxRange(k*sensorMaxVal);
// 
//     accesible = areaCenterPolygon->isAdvanceAccesible(advanceAreaCenterCoords, robotDiameter);
//     if(!accesible) {
//       areaCenterPolygon->addSplitPoint(advanceAreaCenterCoords);
//       areaCenterPolygon->calculateAdvancePolygon();
//       advanceStatus = areaCenterPolygon->getAdvanceStatus();
//       if(advanceStatus == AreaCenterPolygon::Lost) {
//         advanceAreaCenterCoords = areaCenterPolygon->advanceAreaCenter();
//         advanceAreaCenter->setColor(QColor("red"));
//       }
//     }
// 
//   }
// 
//   advanceAreaCenter->setPoint(advanceAreaCenterCoords);
//   return;
// }

void AreaCenterReactiveControl::propagate(void)
{
    calculateGoalSide();
    calculateAreaCenters();
    if(areaCenterPolygon->showAreaCenterPath()) {
        advanceAreaCenter->updatePath();
        if(globalAreaCenter)
            globalAreaCenter->updatePath();
    }
}

void AreaCenterReactiveControl::readInputs(void)
{
    //  sampleInputs();
    readRobotData();
}

void AreaCenterReactiveControl::readRobotData(void)
{
    Robot* robot = getSMGInterface()->getRobot();
    double leftVel = robot->getLeftVel();
    double rightVel = robot->getRightVel();

    double deltaHeading = (leftVel - rightVel) / robot->getRealWheelsSep() * getSamplingPeriod(); // WheelsSep cm (velocidades en cm)
    double traslation = (leftVel + rightVel)/2 * getSamplingPeriod() * 10; // 10 mm por cm (velocidades en cm)
    double deltaX = traslation *cos(deltaHeading);
    double deltaY = traslation *sin(deltaHeading);

    areaCenterPolygon->setRobotMovement(deltaX, deltaY, deltaHeading, leftVel, rightVel);
    areaCenterPolygon->moveBySplitPoints();

    ///////////////// aqui calcular el pasillo de colision del robot

}

void AreaCenterReactiveControl::sendOutputs(void)
{

    DriveControl* drive = getSMGInterface()->getDrive();
    //   double lvel, rvel;
    double priority = 0.5;

    countStall = countStall > 20 ? 0 : countStall;

    if(drive->isActive()) {
        QList<MotorCommand> driveCommands = drive->getMotorPlan()->getCommands();
        NDPose* commandValue = driveCommands[0].getCommandValue();
        lvel = commandValue->getPosition().at(0);
        rvel = commandValue->getPosition().at(1);
    }
    else if(stalled || countStall) {
        priority = 1;
        ++countStall;
        if(stalled == ForeStalled) {
            lvel = -0.1;
            rvel = -0.1;
            advanceStatus = AreaCenterPolygon::Lost;
        }
        else if(stalled == BackStalled) {
            lvel = 0.1;
            rvel = 0.1;
            advanceStatus = AreaCenterPolygon::Conservative;
        }
        else if(stalled == ForeBackStalled){
            lvel = 0;
            rvel = 0;
            countStall = 1;
        } // si es countStall > 0 se conservan la rvel y lvel anteriores
    }
    else {
        double x = advanceAreaCenter->x();
        double y = advanceAreaCenter->y();
        double areaCenterAngle = atan2(y, x);
        //     double areaCenterNormalizedDistance = sqrt(x*x + y*y)/areaCenterPolygon->getAdvanceMaxRange()*2.0;  // advance  max/2
        //     double normalizer = 2.0;
        //     double areaCenterNormalizedDistance = sqrt(x*x + y*y)/sensorMaxVal*normalizer;  // advance  max/2
        double amplitude = fabs(areaCenterPolygon->getCurrentAdvanceAngles().x() - areaCenterPolygon->getCurrentAdvanceAngles().y())/2;
        double normalizer = amplitude ? 2.0*sensorMaxVal/3*sin(amplitude)/amplitude : 2.0*sensorMaxVal/3; // tomado de la formula del CA de un sector circular
        normalizer *= 65/50; // he observado que las velocidades maximas suelen ser de 65 y deseamos 50
        double areaCenterNormalizedDistance = sqrt(x*x + y*y)/normalizer;  // advance  max/2

        double transformedAngle;
        double stopTransAngle = 100/NDMath::RAD2GRAD;
        //     double startMoveAngle = 1/NDMath::RAD2GRAD;
        double startMoveAngle = 2/NDMath::RAD2GRAD;
        double absAreaCenterAngle = fabs(areaCenterAngle);
        double velLimiter = 0.5;

        if(absAreaCenterAngle > startMoveAngle) {

            if(areaCenterAngle < stopTransAngle)
                transformedAngle =  areaCenterAngle*180/NDMath::RAD2GRAD/stopTransAngle;
            else
                transformedAngle = 180/NDMath::RAD2GRAD;

            if(areaCenterAngle >= 0) {
                if(advanceStatus == AreaCenterPolygon::Lost) {
                    if(absAreaCenterAngle > 0.4) {// 0.4 rad aprox= 23 grados
                        lvel = -0.1;
                        rvel = 0.1;
                    }
                    else {
                        lvel = -0.2*sin(absAreaCenterAngle + 0.1);
                        rvel = 0.2*sin(areaCenterAngle);
                    }
                }
                else {
                    lvel = cos(transformedAngle)*areaCenterNormalizedDistance*velLimiter*cos(areaCenterAngle);
                    rvel = areaCenterNormalizedDistance*velLimiter*cos(areaCenterAngle);
                }
            }
            else {
                if(advanceStatus == AreaCenterPolygon::Lost) {
                    if(absAreaCenterAngle > 0.4) {// 0.4 rad aprox= 23 grados
                        lvel = 0.1;
                        rvel = -0.1;
                    }
                    else {
                        lvel = 0.2*sin(absAreaCenterAngle + 0.1);
                        rvel = -0.2*sin(areaCenterAngle + 0.1);
                    }
                }
                else {
                    lvel = areaCenterNormalizedDistance*velLimiter*cos(areaCenterAngle);
                    rvel = cos(transformedAngle)*areaCenterNormalizedDistance*velLimiter*cos(areaCenterAngle);
                }
            }
        }
        else {
            if(advanceStatus == AreaCenterPolygon::Lost) {
                areaCenterPolygon->resetLost();
                advanceStatus = areaCenterPolygon->getAdvanceStatus(); // All
                lvel = 0;
                rvel = 0;
            }
            else {
                lvel = areaCenterNormalizedDistance*velLimiter;
                rvel = areaCenterNormalizedDistance*velLimiter;
            }
        }

        //     if(advanceStatus != AreaCenterPolygon::Lost)
        //       processCollisions();
        //     else
        //       priority = 1;
    }

    OutputMotorPlan* outputMotorPlan = getSMGInterface()->getOutputMotorPlan();
    if(outputMotorPlan) {
        //     addCommand(MotorCommand::WheelsVel, priority, lvel*0.4, rvel*0.4, 0);
        addCommand(MotorCommand::WheelsVel, priority, lvel, rvel, 0);
        outputMotorPlan->sendCommand();
    }

}

// provisional aqui, buscar sitio mejor 
void AreaCenterReactiveControl::addCommand(MotorCommand::MotorCommands type, double priority, double commandParam1, double commandParam2, double commandParam3)
{
    OutputMotorPlan* outputMotorPlan = getSMGInterface()->getOutputMotorPlan();
    if(outputMotorPlan) {
        MotorCommand command = writeCommand(type, priority, commandParam1, commandParam2, commandParam3);
        outputMotorPlan->addCommand(command);
    }
    else
        cerr << "Warning: AreaCenterReactiveControl::addCommand(), no motor plan" << endl;
}

// provisional aqui, buscar sitio mejor 
MotorCommand AreaCenterReactiveControl::writeCommand(MotorCommand::MotorCommands /*type*/, double priority, double commandParam1, double commandParam2, double commandParam3)
{
    MotorCommand command;
    command.setPriority(priority);
    command.setCommandId(MotorCommand::WheelsVel);
    NDPose* commandValue = new NDPose2D();

    commandValue->setCoordinates(commandParam1, commandParam2, commandParam3);
    command.setCommandValue(commandValue, false);
    return command;
}

// void AreaCenterReactiveControl::processCollisions(void)
// {
//   double lvel, rvel, priority, deltaV;
//   double noise_l = NDMath::randDouble4(-0.05, 0.05);
//   double noise_r = NDMath::randDouble4(-0.05, 0.05);
// //   qDebug() << currentCollisionDistances;
// //   qDebug() << collisionDistances;
//   for(int i = 0; i < detectedCollisionSectors.size(); i++) {
//     if(detectedCollisionSectors.at(i)) {
//     double prop = (currentCollisionDistances.at(i) - sensorSaturationVal)/(collisionDistances.at(i) - sensorSaturationVal); 
//       priority = (1 - sqrt(prop))*0.95;
//       if(i==0) { // derecha atras
//         deltaV = (1 - sqrt(prop))*0.6;
//         rvel = deltaV + noise_r;
//         if(priority < 0.5)
//           lvel = noise_l;
//         else
//           lvel = deltaV + noise_l;
//         rvel = rvel > 1 ? 1 : rvel;
//         lvel = lvel < -1 ? -1 : lvel;
//         addCommand(MotorCommand::WheelsVel, priority, lvel, rvel, 0);
//       }      
//       if(i==1) { // derecha
//         deltaV = (1 - sqrt(prop));
//         rvel = deltaV + noise_r;
//         if(priority < 0.5)
//           lvel = noise_l;
//         else
//           lvel = -deltaV + noise_l;
//         rvel = rvel > 1 ? 1 : rvel;
//         lvel = lvel < -1 ? -1 : lvel;
//         addCommand(MotorCommand::WheelsVel, priority, lvel, rvel, 0);
//       }
//       else if(i==2) { // frente
//         deltaV = (1 - sqrt(prop)); ///////// quizas /2 para menor retroceso
// //         lvel = -deltaV + noise_l;
// //         rvel = -deltaV + noise_r;
//         if(priority < 0.5) {
//           lvel = noise_l;
//           rvel = noise_r;
//         }
//         else {
//           lvel = -deltaV + noise_l;
//           rvel = -deltaV + noise_r;
//         }
//         lvel = lvel < -1 ? -1 : lvel;
//         rvel = rvel < -1 ? -1 : rvel;
//         addCommand(MotorCommand::WheelsVel, priority, lvel, rvel, 0);
//       }    
//       else if(i==3) { // izquierda
//         deltaV = (1 - sqrt(prop));
//         if(priority < 0.5)
//           rvel =  noise_r;
//         else
//           rvel = -deltaV + noise_r;
//         lvel = deltaV + noise_l;
//         rvel = rvel < -1 ? -1 : rvel;
//         lvel = lvel > 1 ? 1 : lvel;
//         addCommand(MotorCommand::WheelsVel, priority, lvel, rvel, 0);
//       }
//       if(i==4) { // izquierda atras
//         deltaV = (1 - sqrt(prop))*0.8;
//         rvel = deltaV + noise_r;
//         if(priority < 0.5)
//           lvel = noise_l;
//         else
//           lvel = deltaV + noise_l;
//         rvel = rvel > 1 ? 1 : rvel;
//         lvel = lvel < -1 ? -1 : lvel;
//         addCommand(MotorCommand::WheelsVel, priority, lvel, rvel, 0);
//       }       
//     }    
//   }
//   
// }

void AreaCenterReactiveControl::connectIO(void)
{
    int numAreaCenterGroups = 0;
    SensorsSet* sensorsSet = getSMGInterface()->getSensorsSet();
    //   if(sensorsSet->getSensorGroups().size() > 1) {
    //     cerr << "Warning: AreaCenterReactiveControl::connectIO() More than one sensors group not yet implemented";
    // //     return;
    //   }

    QHashIterator<QString, SensorGroup*> itg(sensorsSet->getSensorGroups());
    int i = 0;
    int numSensors = 0;
    //   double swap_rot;
    //   int swap_idx;
    QVector<double> rotations;
    QVector<size_t> idxs;

    QMap<double, QPair<AbstractFusionNode*, SensorLine*> > areaCenterNodeMap;

    while (itg.hasNext()) {
        itg.next();
        if(itg.key().contains("AreaCenter")) {
            ++numAreaCenterGroups;
        }
        QVector< AbstractFusionNode* > inputNodeList;
        SensorGroup* group = itg.value();

        QList<AbstractSensor*>& sensors = group->getSensors();
        numSensors = sensors.count();
        rotations.resize(numSensors);

        for(i = 0; i < numSensors; i++)
            idxs.append(i);
        QListIterator<AbstractSensor*> its(sensors);
        i = 0;
        while(its.hasNext()) {
            rotations[i] = its.next()->getRotationOrg();
            i++;
        }
        idxs = NDMath::sortContainer(rotations);

        if(itg.key().contains("AreaCenter")) {
            if(numAreaCenterGroups == 1) {
                world-> clearFreeAreaRibbing();
            }
        }

        for(i = 0; i < numSensors; i++) {
//            if(!i && itg.key().contains("AreaCenter") && numAreaCenterGroups == 2) {
            if(!i && itg.key().contains("AreaCenter")) {
                advanceAreaCenter->setRobotPosition(sensors.at(idxs.at(i))->getRobotPosition());
                if(globalAreaCenter)
                    globalAreaCenter->setRobotPosition(sensors.at(idxs.at(i))->getRobotPosition());
//              sensorMaxVal = sensors.at(idxs.at(i))->getSensorMaxVal();
//              sensorSaturationVal = sensors.at(idxs.at(i))->getSensorCriticVal();
                // solo pruebas de slam, hay que incluirlo en los archivos de conf del control
                sensorMaxVal = sensors.at(idxs.at(i))->getSensorMaxVal()/2;
                sensorSaturationVal = sensors.at(idxs.at(i))->getSensorCriticVal()/2;
                areaCenterPolygon->setSensorMaxRange(sensorMaxVal);
            }

            AbstractFusionNode *node;
            if(itg.key().contains("RadialField")) {
                NetInputFusionNode* n = new NetInputFusionNode(sensors.at(idxs.at(i)));
                node = n;
            }
            else if(itg.key().contains("Bumper")) {
                NetInputFusionNode* n = new NetInputFusionNode(sensors.at(idxs.at(i)));
                node = n;
            }
            else if(itg.key().contains("BlobCamera")) {
                BlobCameraFusionNode* n = new BlobCameraFusionNode(sensors.at(idxs.at(i)));
                node = n;
            }
            else {
                if(itg.key().contains("AriaLaser")) {
                    NDAriaSick* sen = (NDAriaSick*)sensors.at(idxs.at(i));
                    AriaLaserFusionNode* n = new AriaLaserFusionNode(sen);
                    node = n;
                }
                else {
                    InputFusionNode2D* n = new InputFusionNode2D(sensors.at(idxs.at(i)));
                    node = n;
                }
                if(itg.key().contains("AreaCenter")) {
                    //           world->addFreeAreaRib(sensors.at(idxs.at(i))->getSensorLine());
                    areaCenterNodeMap[sensors.at(idxs.at(i))->getRotationOrg()] = QPair<AbstractFusionNode*, SensorLine*>(node, sensors.at(idxs.at(i))->getSensorLine());
                }
            }
            node->initialize();
            inputNodeList.push_back(node);
        }

        inputNodeMap.insert(itg.key(), inputNodeList);
        //     if(itg.key().contains("AreaCenter")) {
        //       areaCenterNodeList = inputNodeList;
        //     }

    }
    areaCenterNodeList.clear();
    areaCenterPolygon->resize(areaCenterNodeMap.size());
    QMapIterator<double, QPair<AbstractFusionNode*, SensorLine*> > it(areaCenterNodeMap);
    while (it.hasNext()) {
        it.next();
        //     std::cout << it.key() << std::endl;
        areaCenterNodeList.append(it.value().first);
        world->addFreeAreaRib(it.value().second);
    }

    diamondConnectIO();

}

void AreaCenterReactiveControl::initElements(void)
{
    NullSimControl::initElements();

    robotScale = getSMGInterface()->getRobot()->robotScale();
    robotDiameter = getSMGInterface()->getRobot()->getDiameter()*robotScale;
    areaCenterPolygon->setRobotDiameter(robotDiameter);

    sampleInputs();

    readGlobalPolygon();
    areaCenterPolygon->setAdvanceAngles(QPointF(-90.5/NDMath::RAD2GRAD, 90.5/NDMath::RAD2GRAD));
    areaCenterPolygon->reset();
    advanceStatus = areaCenterPolygon->getAdvanceStatus();
    goalSidePreference = advanceStatus;

    //   sampleInputs();
    propagate();

    advanceAreaCenter->resetPath();
    if(globalAreaCenter)
        globalAreaCenter->resetPath();

    connect(areaCenterPolygon, SIGNAL(changeSplitPoint(const QPointF&, int )), advanceAreaCenter, SLOT(setSplitPoint(const QPointF&, int)));
    connect(areaCenterPolygon, SIGNAL(hideSplitPoint(int )), advanceAreaCenter, SLOT(hideSplitPoint(int )));

    int commandsSize = 2;
    OutputMotorPlan* outputMotorPlan = getSMGInterface()->getOutputMotorPlan();
    MotorCommand command = writeCommand(MotorCommand::WheelsVel, .3, 0, 0, 0);
    if(outputMotorPlan)
        outputMotorPlan->initilizaOutputCommands(command, commandsSize);
}

void AreaCenterReactiveControl::setWorld(AbstractWorld* world)
{
    this->world = world;
    advanceAreaCenter->setWorld(world);
    if(globalAreaCenter) {
        globalAreaCenter->setWorld(world);
        globalAreaCenter->setColor(QColor("dimgray"));
        globalAreaCenter->setVisible(false);
    }
    createAreaCenterPolygon();
    //   areaCenterPolygon->setWorld(world);
}

void AreaCenterReactiveControl::setRecording(bool on, bool crono_reset)
{
    //   on = false;  // provisional
//    isRecording = false; // provisional
    if(isRecording != on) {
        if(recorder_on == on) return;
        isRecording = on;
        recorder->connectToCrono(isRecording);
        if(on)  {
            recorder->setCronoReset(crono_reset);
            QStringList header;
            header.append(QString::number(world->getWorldScale()));
            header.append(QString::number(NDMath::roundInt(world->getWidth())));
            header.append(QString::number(NDMath::roundInt(world->getHeight())));
            header.append(QString::number(1)); // era robotScale
            header.append(QString::number(world->getXMinPosition()));
            header.append(QString::number(world->getYMinPosition()));
            bool inverseY = world->headingTransform(1) > 0 ? false : true;
            header.append(QString::number(inverseY ? -1 :1));
            header.append(QString::number(world->toFigUnits()));
            recorder->init(header);
        }
        else {
            recorder->closeFile();
        }
        recorder_on = on;
    }

}  

void AreaCenterReactiveControl::doMonitor(void)
{

    if(isRecording) {
        QPointF angles = areaCenterPolygon->getCurrentAdvanceAngles();
        emit sendToRecorder(advanceAreaCenter->getXWorld(), 1);
        emit sendToRecorder(advanceAreaCenter->getYWorld(), 2);
        emit sendToRecorder(angles.x(), 3);
        emit sendToRecorder(angles.y(), 4);
        emit sendToRecorder(areaCenterPolygon->getAdvanceStatus(), 5);
        emit sendToRecorder(pan, 6);
        emit sendToRecorder(blobArea, 7);
    }

}

QList<QewExtensibleDialog*> AreaCenterReactiveControl::tabsForEdit(QewExtensibleDialog* parent)
{
    QList<QewExtensibleDialog*> add_dialogs;
    ACControlTab* control_tab = new ACControlTab(parent);
    if(control_tab->setUpDialog()) {
        add_dialogs.append(control_tab);
        control_tab->init(this);
    }
    QewExtensibleDialog* rec_dialog = recorder->createDialog(control_tab);
    control_tab->addExtensibleChild(rec_dialog, "");
    return add_dialogs;

}

void AreaCenterReactiveControl::calculateGoalSide(void)
{

    double max = 0;
    int maxIdx =-1;
    double sum = 0;
    double data;
    goalSidePreference = AreaCenterPolygon::All;
    DriveControl* drive = getSMGInterface()->getDrive();
    Robot* robot = getSMGInterface()->getRobot();

    double sum_back = -1; // para camara

    if(inputNodeMap.contains("RadialField_sensors")) {
        QVector< AbstractFusionNode* > inputNodeList = inputNodeMap.value("RadialField_sensors");
        int num_nodes = inputNodeList.size(); // debe ser numero par o excluir el del centro
        sum_back = 0;
        for(int i = 0; i < num_nodes; i++) {
            data = inputNodeList[i]->result().getY().at(0);
            if(i == 0  || i == num_nodes - 1)
                sum_back += data;
            if(num_nodes%2 == 1) { // num_nodes impar
                if(i == num_nodes/2 + 1) // se excluye el central al determinar el lado pero no la suma
                    sum = 2*data;
                else if(data > max) {
                    maxIdx = i;
                    max = data;
                }
            }
            else {
                if((i == num_nodes/2 - 1) || (i == num_nodes/2))
                    sum += data;
                if(data > max) {
                    maxIdx = i;
                    max = data;
                }
            }
        }

        if(num_nodes%2 == 1) {
            if(maxIdx > -1) {
                if(maxIdx < num_nodes/2 - 1) {
                    goalSidePreference = AreaCenterPolygon::Right;
                    suggestedSplit = true;
                }
                else if(maxIdx > num_nodes/2 + 1) {
                    goalSidePreference = AreaCenterPolygon::Left;
                    suggestedSplit = true;
                }
            }
        }
        else {
            if(maxIdx > -1) {
                if(maxIdx < num_nodes/2) {
                    goalSidePreference = AreaCenterPolygon::Right;
                    if(maxIdx < num_nodes/2 - 1)
                        suggestedSplit = true;
                }
                else if(maxIdx >= num_nodes/2) {
                    goalSidePreference = AreaCenterPolygon::Left;
                    if(maxIdx >= num_nodes/2 + 1)
                        suggestedSplit = true;
                }
            }
        }
    }
    if(inputNodeMap.contains("BlobCamera_sensors")) { // esto debería estar en otro control junto con parte de AriaCamera::sample()
        QVector< AbstractFusionNode* > inputNodeList = inputNodeMap.value("BlobCamera_sensors");
        data = inputNodeList[0]->result().getY().at(0);
        max = data;

        //     printf("max %f\n", max);

        blobArea = -1;
        pan = inputNodeList[0]->result().getPose()->getOrientation().at(0);
        if(!robot->getRealRobot()) {
            if(max > 0.05) {
                blobArea = max*320*240;
                //         if(max > 0.8)
                pan /= 1 - max + 0.1;

                //         if(pan < -20)
                //           goalSidePreference = AreaCenterPolygon::Right;
                //         else if(pan > 20)
                //           goalSidePreference = AreaCenterPolygon::Left;

                if(pan < 0) { // para robots simulados y real
                    goalSidePreference = AreaCenterPolygon::Right;
                    if(pan < -20)
                        suggestedSplit = true;
                }
                else if(pan > 0) {
                    goalSidePreference = AreaCenterPolygon::Left;
                    if(pan > 20)
                        suggestedSplit = true;
                }

                //         if(pan > 0) { // para MobileSim
                //           goalSidePreference = AreaCenterPolygon::Right;
                //           if(pan > 20)
                //             suggestedSplit = true;
                //         }
                //         else if(pan < 0) {
                //           goalSidePreference = AreaCenterPolygon::Left;
                //           if(pan < -20)
                //             suggestedSplit = true;
                //         }


                //         printf("pan = %f\n\n", pan);
            }
        }
        else {
            if(max > 0.0065) {
                blobArea = max*320*240;
                if(max > 0.35)
                    pan /= 1 - max + 0.1;

                if(pan < 0) { // para robots simulados y real
                    goalSidePreference = AreaCenterPolygon::Right;
                    if(pan < -20)
                        suggestedSplit = true;
                }
                else if(pan > 0) {
                    goalSidePreference = AreaCenterPolygon::Left;
                    if(pan > 20)
                        suggestedSplit = true;
                }

                //         if(pan < -20)
                //           goalSidePreference = AreaCenterPolygon::Left;
                //         else if(pan > 20)
                //           goalSidePreference = AreaCenterPolygon::Right;
                //         if(pan < -20)
                //           goalSidePreference = AreaCenterPolygon::Right;
                //         else if(pan > 20)
                //           goalSidePreference = AreaCenterPolygon::Left;

                //         printf("pan = %f\n\n", pan);
            }
        }
    }


    //   if(sum >= 1.5 || max > 0.85 ) {
    //     if(goalSidePreference == AreaCenterPolygon::Right)
    //       addCommand(MotorCommand::WheelsVel, 0.7*max, 2*(1 - max), 0, 0);
    //     else if(goalSidePreference == AreaCenterPolygon::Left)
    //       addCommand(MotorCommand::WheelsVel, 0.7*max, 0, 2*(1 - max), 0);
    //     else
    //       addCommand(MotorCommand::WheelsVel, max, 0, 0, 0);
    //   }
    //
    // //   if(max >=0.925) {
    // //     addCommand(MotorCommand::WheelsVel, 1, 0, 0, 0);
    // //   }


    //   if(max >=0.5) {  // ajustado para un determinado experimento
    //     if(!drive->isActive())
    //       addCommand(MotorCommand::WheelsVel, 1, 0, 0, 0);
    //   }
    //   else if(max > 0.25) {
    //     if(!drive->isActive())
    //       addCommand(MotorCommand::WheelsVel, 4*max, 0, 0, 0);
    //   }
    if(robot->getRealRobot()) {
        if(max >=0.75) {  // ajustado para un determinado experimento
            if(!drive->isActive())
                addCommand(MotorCommand::WheelsVel, 1, 0, 0, 0);
        }
        else if(max > 0.35) {
            if(!drive->isActive())
                addCommand(MotorCommand::WheelsVel, 1/0.75*max, 0, 0, 0);
        }
        else {
            if(((goalSidePreference == AreaCenterPolygon::Left) || (goalSidePreference == AreaCenterPolygon::Right)) && (advanceStatus != goalSidePreference) && (advanceStatus != AreaCenterPolygon::Lost))
                suggestedSplit = true;
            if(sum_back > sum - sum_back)
                suggestedSplit = true;
        }
    }
    else {
        if(max >=0.95) {  // ajustado para un determinado experimento
            if(!drive->isActive())
                addCommand(MotorCommand::WheelsVel, 1, 0, 0, 0);
        }
        else if(max > 0.8) {
            if(!drive->isActive())
                addCommand(MotorCommand::WheelsVel, max, 0, 0, 0);
        }
        else {
            if(((goalSidePreference == AreaCenterPolygon::Left) || (goalSidePreference == AreaCenterPolygon::Right)) && (advanceStatus != goalSidePreference) && (advanceStatus != AreaCenterPolygon::Lost) && suggestedSplit)
                suggestedSplit = true;
            if(sum_back > sum - sum_back)
                suggestedSplit = true;
        }
    }

}

void AreaCenterReactiveControl::createAreaCenterPolygon(void )
{
    areaCenterPolygon = new AreaCenterPolygon();
    areaCenterPolygon->setWorld(world);
}
