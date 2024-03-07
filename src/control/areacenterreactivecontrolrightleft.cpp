//
// C++ Implementation: areacenterreactivecontrolrightleft
//
// Description:
//
//
// Author: Javier Fernandez Perez
//
// Copyright: See COPYING file that comes with this distribution
//
//


#include "areacenterreactivecontrolrightleft.h"
#include "smginterface.h"

AreaCenterReactiveControlRightLeft::AreaCenterReactiveControlRightLeft(): /*QObject(),*/ AreaCenterSegmentedControl()
{
  //centralSector=181 -> areacenterreactivecontrol
  centralSector=1;
  middleAngle=(localAngleStart+localAngleEnd)/2;
  startAngleRight=localAngleStart;
  endAngleRight=(middleAngle-(centralSector/2));
  startAngleLeft=(middleAngle+(centralSector/2));
  endAngleLeft=localAngleEnd;
  advanceAreaCenterLeft = new AreaCenter();
  advanceAreaCenterRight = new AreaCenter();
  areaCenterPolygonLeft = 0;
  areaCenterPolygonRight = 0;
  behaviour=SearchCrossing;
}


AreaCenterReactiveControlRightLeft::~AreaCenterReactiveControlRightLeft()
{
  delete advanceAreaCenterLeft;
  delete advanceAreaCenterRight;
  delete areaCenterPolygonLeft;
  delete areaCenterPolygonRight;
}

int AreaCenterReactiveControlRightLeft::getType(void)
{
  return AREA_CENTER_RIGHT_LEFT_REACTIVE_CONTROL;
}

void AreaCenterReactiveControlRightLeft::setWorld(AbstractWorld* world)
{
  AreaCenterReactiveControl::setWorld(world);
  setWorldsAdvanceAreaCenter();
  createAreaCenterPolygons();
}

void AreaCenterReactiveControlRightLeft::setWorldsAdvanceAreaCenter(void){
    QString c1="yellow";
    QString c2="red";
    QString c3="blue";
    QString c4="orange";
    advanceAreaCenterLeft->setWorld(world,c1,c2);
    advanceAreaCenterRight->setWorld(world,c3,c4);
}

void AreaCenterReactiveControlRightLeft::createAreaCenterPolygons(void)
{
  areaCenterPolygonLeft = new AreaCenterPolygon();
  areaCenterPolygonLeft->setWorld(world);
  areaCenterPolygonRight = new AreaCenterPolygon();
  areaCenterPolygonRight->setWorld(world);
}

void AreaCenterReactiveControlRightLeft::connectIO(void)
{
   AreaCenterSegmentedControl::connectIO();

   connectIO(advanceAreaCenterLeft,areaCenterPolygonLeft);
   connectIO(advanceAreaCenterRight,areaCenterPolygonRight);
}

void AreaCenterReactiveControlRightLeft::connectIO(AreaCenter *advanceAreaCent,AreaCenterPolygon *areaCenterPoly)
{
  advanceAreaCent->setRobotPosition(advanceAreaCenter->getRobotPosition());
  areaCenterPoly->setSensorMaxRange(areaCenterPolygon->getSensorMaxRange());
}


void AreaCenterReactiveControlRightLeft::initElements()
{
    initElementsReactiveControlBeforePropagate();

    initElements(areaCenterPolygonLeft,startAngleLeft,endAngleLeft);
    initElements(areaCenterPolygonRight,startAngleRight,endAngleRight);

    propagate();

    initElementsReactiveControlAfterPropagate();

    advanceAreaCenterLeft->resetPath();
    advanceAreaCenterRight->resetPath();
}

void AreaCenterReactiveControlRightLeft::initElementsReactiveControlBeforePropagate()
{
    NullSimControl::initElements();
    segmentation->setSensorMaxVal(sensorMaxVal*2); // ñapa *2

    robotScale = getSMGInterface()->getRobot()->robotScale();
    robotDiameter = getSMGInterface()->getRobot()->getDiameter()*robotScale;
    areaCenterPolygon->setRobotDiameter(robotDiameter);

    sampleInputs();

    AreaCenterSegmentedControl::readGlobalPolygon();

    areaCenterPolygon->setAdvanceAngles(QPointF(localAngleStart/NDMath::RAD2GRAD, localAngleEnd/NDMath::RAD2GRAD));
    areaCenterPolygon->reset();
    advanceStatus = areaCenterPolygon->getAdvanceStatus();
    goalSidePreference = advanceStatus;
}

void AreaCenterReactiveControlRightLeft::initElements(AreaCenterPolygon *areaCenterPoly,
                              double angleStart,double angleEnd)
{
  areaCenterPoly->setRobotDiameter(robotDiameter);

  sampleInputs();

  readGlobalPolygon(angleStart,angleEnd,areaCenterPoly);

  areaCenterPoly->setAdvanceAngles(QPointF(angleStart/NDMath::RAD2GRAD,angleEnd/NDMath::RAD2GRAD));

  areaCenterPoly->reset();
}

void AreaCenterReactiveControlRightLeft::readGlobalPolygon(double angleStart,double angleEnd, AreaCenterPolygon *areaCenterPol)
{
  double x, y;
  int numNodes = areaCenterNodeList.size();
  int numNodesSide=0;
  int position=0;

  double boundaryLeft=(angleStart/NDMath::RAD2GRAD);
  double boundaryRight=(angleEnd/NDMath::RAD2GRAD);

  for(int i = 0; i < numNodes; i++) {
    x = areaCenterNodeList[i]->result().getY().at(2);
    y = areaCenterNodeList[i]->result().getY().at(3);
    double vertexAngle = atan2(y, x);

    if((vertexAngle >= boundaryLeft) && (vertexAngle <= boundaryRight))
      numNodesSide=numNodesSide+1;
    else if(vertexAngle > boundaryRight)
      break;
  }

  areaCenterPol->resize(numNodesSide);

  for(int i = 0; i < numNodes; i++) {
    x = areaCenterNodeList[i]->result().getY().at(2);
    y = areaCenterNodeList[i]->result().getY().at(3);
    double vertexAngle = atan2(y, x);

    if((vertexAngle >= boundaryLeft) && (vertexAngle <= boundaryRight)){
      (*areaCenterPol)[position].setX(x);
      (*areaCenterPol)[position].setY(y);
      position++;
    }
    else if(vertexAngle > boundaryRight)
      break;
  }
}

void AreaCenterReactiveControlRightLeft::initElementsReactiveControlAfterPropagate()
{
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

void AreaCenterReactiveControlRightLeft::propagate(void)
{
    std::cerr << "Behaviour: " << behaviour << std::endl;

    switch (behaviour) {
        case SearchCrossing:
            searchCrossing();
            break;
        case AvoidObstacle:
            avoidObstacle();
            break;
        case DetectCrossing:
            detectCrossing();
            break;
        case GoCrossing:
           goCrossing();
            break;
    }

    if(areaCenterPolygon->showAreaCenterPath()) {
      advanceAreaCenter->updatePath();
      if(globalAreaCenter)
        globalAreaCenter->updatePath();
    }

    if(areaCenterPolygonLeft->showAreaCenterPath()) {
      advanceAreaCenterLeft->updatePath();
    }
    if(areaCenterPolygonRight->showAreaCenterPath()) {
      advanceAreaCenterRight->updatePath();
    }

}

void AreaCenterReactiveControlRightLeft::searchCrossing(void)
{
    QPointF advanceAreaCenterCoords,advanceAreaCenterCoordsLeft,advanceAreaCenterCoordsRight;
    bool accesible,accesibleLeft,accesibleRight;

    if(getStalled() || countStall) {
      if(advanceStatus == AreaCenterPolygon::Lost)  // escapando
        advanceAreaCenter->setPoint(areaCenterPolygon->advanceAreaCenter());
      return;
    }

    AreaCenterSegmentedControl::readGlobalPolygon();

    readGlobalPolygon(startAngleLeft,endAngleLeft,areaCenterPolygonLeft);
    areaCenterPolygonLeft->setAdvanceAngles(QPointF(startAngleLeft/NDMath::RAD2GRAD,endAngleLeft/NDMath::RAD2GRAD));

    readGlobalPolygon(startAngleRight,endAngleRight,areaCenterPolygonRight);
    areaCenterPolygonRight->setAdvanceAngles(QPointF(startAngleRight/NDMath::RAD2GRAD,endAngleRight/NDMath::RAD2GRAD));

    if(globalAreaCenter)
      globalAreaCenter->setPoint(areaCenterPolygon->globalAreaCenter());

    if(advanceStatus == AreaCenterPolygon::Lost) { // escapando
      advanceAreaCenter->setPoint(areaCenterPolygon->advanceAreaCenter());
      return;
    }

    areaCenterPolygon->calculateAdvancePolygon();
    areaCenterPolygonLeft->calculateAdvancePolygon();
    areaCenterPolygonRight->calculateAdvancePolygon();

    advanceStatus = areaCenterPolygon->getAdvanceStatus();
    if(advanceStatus == AreaCenterPolygon::Lost) { // escapando
      advanceAreaCenter->setPoint(areaCenterPolygon->advanceAreaCenter());
      return;
    }

    advanceAreaCenterCoords = areaCenterPolygon->advanceAreaCenter();
    advanceAreaCenterCoordsLeft = areaCenterPolygonLeft->advanceAreaCenter();
    advanceAreaCenterCoordsRight = areaCenterPolygonRight->advanceAreaCenter();

    accesible = areaCenterPolygon->isAdvanceAccesible(advanceAreaCenterCoords, robotDiameter/3, advanceStatus);
    accesibleLeft = areaCenterPolygonLeft->isAdvanceAccesible(advanceAreaCenterCoordsLeft, robotDiameter/3, advanceStatus,false);
    accesibleRight = areaCenterPolygonRight->isAdvanceAccesible(advanceAreaCenterCoordsRight, robotDiameter/3, advanceStatus,false);

    if(!accesible){
        behaviour=AvoidObstacle;
    }

    if(accesible&&(!accesibleLeft||!accesibleRight)){
        behaviour=DetectCrossing;
    }

    advanceAreaCenter->setPoint(advanceAreaCenterCoords);
    advanceAreaCenterLeft->setPoint(advanceAreaCenterCoordsLeft);
    advanceAreaCenterRight->setPoint(advanceAreaCenterCoordsRight);

    switch (advanceStatus) {
        case AreaCenterPolygon::All:
            advanceAreaCenter->setColor(QColor("magenta"));
            advanceAreaCenterLeft->setColor(QColor("magenta"));
            advanceAreaCenterRight->setColor(QColor("magenta"));
            break;
        case AreaCenterPolygon::Left:
            advanceAreaCenter->setColor(QColor("orange"));
            advanceAreaCenterLeft->setColor(QColor("orange"));
            advanceAreaCenterRight->setColor(QColor("orange"));
            break;
        case AreaCenterPolygon::Right:
            advanceAreaCenter->setColor(QColor("darkred"));
            advanceAreaCenterLeft->setColor(QColor("darkred"));
            advanceAreaCenterRight->setColor(QColor("darkred"));
            break;
        case AreaCenterPolygon::Lost:
            advanceAreaCenter->setColor(QColor("red"));
            advanceAreaCenterLeft->setColor(QColor("red"));
            advanceAreaCenterRight->setColor(QColor("red"));
            break;
        case AreaCenterPolygon::Conservative:
            advanceAreaCenter->setColor(QColor("darkmagenta"));
            advanceAreaCenterLeft->setColor(QColor("darkmagenta"));
            advanceAreaCenterRight->setColor(QColor("darkmagenta"));
            break;
    }

    return;
}

void AreaCenterReactiveControlRightLeft::avoidObstacle(void)
{
    QPointF advanceAreaCenterCoords,advanceAreaCenterCoordsLeft,advanceAreaCenterCoordsRight;
    bool accesible;
    double advanceRateIncrement = 1.025;

    if(getStalled() || countStall) {
      if(advanceStatus == AreaCenterPolygon::Lost)  // escapando
        advanceAreaCenter->setPoint(areaCenterPolygon->advanceAreaCenter());
      return;
    }

    AreaCenterSegmentedControl::readGlobalPolygon();

    readGlobalPolygon(startAngleLeft,endAngleLeft,areaCenterPolygonLeft);
    areaCenterPolygonLeft->setAdvanceAngles(QPointF(startAngleLeft/NDMath::RAD2GRAD,endAngleLeft/NDMath::RAD2GRAD));

    readGlobalPolygon(startAngleRight,endAngleRight,areaCenterPolygonRight);
    areaCenterPolygonRight->setAdvanceAngles(QPointF(startAngleRight/NDMath::RAD2GRAD,endAngleRight/NDMath::RAD2GRAD));

    if(globalAreaCenter)
      globalAreaCenter->setPoint(areaCenterPolygon->globalAreaCenter());

    if(advanceStatus == AreaCenterPolygon::Lost) { // escapando
      advanceAreaCenter->setPoint(areaCenterPolygon->advanceAreaCenter());
      return;
    }

    do {
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
        areaCenterPolygonLeft->calculateAdvancePolygon();
        areaCenterPolygonRight->calculateAdvancePolygon();

        advanceStatus = areaCenterPolygon->getAdvanceStatus();
        if(advanceStatus == AreaCenterPolygon::Lost) { // escapando
            advanceAreaCenter->setPoint(areaCenterPolygon->advanceAreaCenter());
           break;
        }

        advanceAreaCenterCoords = areaCenterPolygon->advanceAreaCenter();
        advanceAreaCenterCoordsLeft = areaCenterPolygonLeft->advanceAreaCenter();
        advanceAreaCenterCoordsRight = areaCenterPolygonRight->advanceAreaCenter();

        accesible = areaCenterPolygon->isAdvanceAccesible(advanceAreaCenterCoords, robotDiameter/3, advanceStatus);

        if(!accesible) {

            areaCenterPolygon->addSplitPointCenter(advanceAreaCenterCoords, suggestedSplit, goalSidePreference);

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

    if(advanceStatus==AreaCenterPolygon::All||(advanceStatus==AreaCenterPolygon::Conservative&&advanceRate>0.85)){
        behaviour=SearchCrossing;
    }

    advanceAreaCenter->setPoint(advanceAreaCenterCoords);
    advanceAreaCenterLeft->setPoint(advanceAreaCenterCoordsLeft);
    advanceAreaCenterRight->setPoint(advanceAreaCenterCoordsRight);


    //behaviour=SearchCrossing;

    switch (advanceStatus) {
        case AreaCenterPolygon::All:
            advanceAreaCenter->setColor(QColor("magenta"));
            advanceAreaCenterLeft->setColor(QColor("magenta"));
            advanceAreaCenterRight->setColor(QColor("magenta"));
            break;
        case AreaCenterPolygon::Left:
            advanceAreaCenter->setColor(QColor("orange"));
            advanceAreaCenterLeft->setColor(QColor("orange"));
            advanceAreaCenterRight->setColor(QColor("orange"));
            break;
        case AreaCenterPolygon::Right:
            advanceAreaCenter->setColor(QColor("darkred"));
            advanceAreaCenterLeft->setColor(QColor("darkred"));
            advanceAreaCenterRight->setColor(QColor("darkred"));
            break;
        case AreaCenterPolygon::Lost:
            advanceAreaCenter->setColor(QColor("red"));
            advanceAreaCenterLeft->setColor(QColor("red"));
            advanceAreaCenterRight->setColor(QColor("red"));
            break;
         case AreaCenterPolygon::Conservative:
            advanceAreaCenter->setColor(QColor("darkmagenta"));
            advanceAreaCenterLeft->setColor(QColor("darkmagenta"));
            advanceAreaCenterRight->setColor(QColor("darkmagenta"));
            break;
    }

    return;

}

//void AreaCenterReactiveControlRightLeft::calculateAreaCentersRightLeft(void)
//{
//    QPointF advanceAreaCenterCoords,advanceAreaCenterCoordsLeft,advanceAreaCenterCoordsRight;
//    AreaCenterPolygon::AdvanceStatus advanceStatusLeft,advanceStatusRight;
//    QPointF polygonSplitPointLeft,polygonSplitPointRight;
//    bool accesible = false;
//    bool accesibleLeft = false;
//    bool accesibleRight = false;
//    double advanceRateIncrement = 1.025;
//    double advanceRateLeft,advanceRateRight;

//    if(getStalled() || countStall) {
//      if(advanceStatus == AreaCenterPolygon::Lost)  // escapando
//        advanceAreaCenter->setPoint(areaCenterPolygon->advanceAreaCenter());
//      return;
//    }

//    readGlobalPolygon(startAngleLeft,endAngleLeft,areaCenterPolygonLeft);
//    areaCenterPolygonLeft->setAdvanceAngles(QPointF(startAngleLeft/NDMath::RAD2GRAD,endAngleLeft/NDMath::RAD2GRAD));

//    readGlobalPolygon(startAngleRight,endAngleRight,areaCenterPolygonRight);
//    areaCenterPolygonRight->setAdvanceAngles(QPointF(startAngleRight/NDMath::RAD2GRAD,endAngleRight/NDMath::RAD2GRAD));

//    if(globalAreaCenter)
//      globalAreaCenter->setPoint(areaCenterPolygon->globalAreaCenter());

//    if(advanceStatus == AreaCenterPolygon::Lost) { // escapando
//      advanceAreaCenter->setPoint(areaCenterPolygon->advanceAreaCenter());
//      return;
//    }

//    do {
//      advanceRate = areaCenterPolygon->advanceRate();
//      advanceRateLeft = areaCenterPolygonLeft->advanceRate();
//      advanceRateRight = areaCenterPolygonRight->advanceRate();
//      if(advanceStatus == AreaCenterPolygon::All || advanceStatus == AreaCenterPolygon::Conservative) {
//        if(advanceRate < 1) {
//          advanceRate *= advanceRateIncrement;
//          if(advanceRate > 1) {
//            advanceRate = 1;
//          }
//        }
//      }

//      areaCenterPolygon->setAdvanceMaxRange(advanceRate*sensorMaxVal);
//      areaCenterPolygonLeft->setAdvanceMaxRange(advanceRateLeft*sensorMaxVal);
//      areaCenterPolygonRight->setAdvanceMaxRange(advanceRateRight*sensorMaxVal);

//      areaCenterPolygon->calculateAdvancePolygon();
//      areaCenterPolygonLeft->calculateAdvancePolygon();
//      areaCenterPolygonRight->calculateAdvancePolygon();

//      advanceStatus = areaCenterPolygon->getAdvanceStatus();
//      if(advanceStatus == AreaCenterPolygon::Lost) { // escapando
//        advanceAreaCenter->setPoint(areaCenterPolygon->advanceAreaCenter());
//        break;
//      }

//      advanceAreaCenterCoords = areaCenterPolygon->advanceAreaCenter();
//      advanceAreaCenterCoordsLeft = areaCenterPolygonLeft->advanceAreaCenter();
//      advanceAreaCenterCoordsRight = areaCenterPolygonRight->advanceAreaCenter();

//      accesible = areaCenterPolygon->isAdvanceAccesible(advanceAreaCenterCoords, robotDiameter/3, advanceStatus);
//      accesibleLeft = areaCenterPolygonLeft->isAdvanceAccesible(advanceAreaCenterCoordsLeft, robotDiameter/3, advanceStatus,false);
//      accesibleRight = areaCenterPolygonRight->isAdvanceAccesible(advanceAreaCenterCoordsRight, robotDiameter/3, advanceStatus,false);

//      if(!accesible) {

//        allAccesible=false;

//        areaCenterPolygon->addSplitPointCenter(advanceAreaCenterCoords, suggestedSplit, goalSidePreference);

//        advanceStatus = areaCenterPolygon->getAdvanceStatus();
//        if(advanceStatus == AreaCenterPolygon::Lost) {
//          advanceAreaCenterCoords = areaCenterPolygon->advanceAreaCenter();
//          break;
//        }

//        areaCenterPolygon->calculateAdvancePolygon();
//        advanceStatus = areaCenterPolygon->getAdvanceStatus();
//        if(advanceStatus == AreaCenterPolygon::Lost) {
//          advanceAreaCenterCoords = areaCenterPolygon->advanceAreaCenter();
//          break;
//        }

//        advanceAreaCenterCoords = areaCenterPolygon->advanceAreaCenter();
//        advanceStatus = areaCenterPolygon->getAdvanceStatus();
//        if(advanceStatus == AreaCenterPolygon::Lost) {
//          advanceAreaCenterCoords = areaCenterPolygon->advanceAreaCenter();
//          break;
//        }

//      }
//      else{
//  //        if(advanceStatus==AreaCenterPolygon::All){
//          if(advanceStatus==AreaCenterPolygon::All||
//                  (advanceStatus==AreaCenterPolygon::Conservative&&advanceRate>0.85)){

//             if(!accesibleLeft&accesibleRight&&allAccesible){
//                  std::cerr << "Detectado cruce a la izquierda!!!!!!" << std::endl;

//                  allAccesible=false;

//                  areaCenterPolygonLeft->addSplitPointRightLeft(advanceAreaCenterCoordsLeft);
//                  advanceStatusLeft=areaCenterPolygonLeft->getAdvanceStatus();
//                  if(advanceStatusLeft==AreaCenterPolygon::Lost){
//                      break;
//                  }
//                  areaCenterPolygon->setAdvanceStatus(AreaCenterPolygon::Left);
//                  polygonSplitPointLeft=areaCenterPolygonLeft->getPolygonSplitPointLeft();
//                  polygonSplitPointRight=areaCenterPolygonLeft->getPolygonSplitPointRight();
//                  areaCenterPolygon->addSplitPoints(polygonSplitPointLeft,polygonSplitPointRight);
//                  advanceStatus = areaCenterPolygon->getAdvanceStatus();
//                  areaCenterPolygon->calculateAdvancePolygon();
//                  advanceStatus = areaCenterPolygon->getAdvanceStatus();
//                  if(advanceStatus==AreaCenterPolygon::Lost){
//                      advanceAreaCenter->setPoint(areaCenterPolygon->advanceAreaCenter());
//                      break;
//                  }
//                  areaCenterPolygon->advanceAreaCenter();
//                  advanceStatus = areaCenterPolygon->getAdvanceStatus();
//                  if(advanceStatus==AreaCenterPolygon::Lost){
//                      advanceAreaCenter->setPoint(areaCenterPolygon->advanceAreaCenter());
//                      break;
//                  }
//                  accesible=false;
//              }
//              if(accesibleLeft&!accesibleRight&&allAccesible){
//                  std::cerr << "Detectado cruce a la derecha!!!!!!" << std::endl;

//                  allAccesible=false;

//                  areaCenterPolygonRight->addSplitPointRightLeft(advanceAreaCenterCoordsRight);
//                  advanceStatusRight=areaCenterPolygonRight->getAdvanceStatus();
//                  if(advanceStatusRight==AreaCenterPolygon::Lost){
//                      break;
//                  }
//                  areaCenterPolygon->setAdvanceStatus(AreaCenterPolygon::Right);
//                  polygonSplitPointLeft=areaCenterPolygonRight->getPolygonSplitPointLeft();
//                  polygonSplitPointRight=areaCenterPolygonRight->getPolygonSplitPointRight();
//                  areaCenterPolygon->addSplitPoints(polygonSplitPointLeft,polygonSplitPointRight);
//                  advanceStatus = areaCenterPolygon->getAdvanceStatus();
//                  areaCenterPolygon->calculateAdvancePolygon();
//                  advanceStatus = areaCenterPolygon->getAdvanceStatus();
//                  if(advanceStatus==AreaCenterPolygon::Lost){
//                      advanceAreaCenter->setPoint(areaCenterPolygon->advanceAreaCenter());
//                      break;
//                  }
//                  areaCenterPolygon->advanceAreaCenter();
//                  advanceStatus = areaCenterPolygon->getAdvanceStatus();
//                  if(advanceStatus==AreaCenterPolygon::Lost){
//                      advanceAreaCenter->setPoint(areaCenterPolygon->advanceAreaCenter());
//                      break;
//                  }
//                  accesible=false;
//              }
//              if(!accesibleLeft&!accesibleRight&&allAccesible){
//                  std::cerr << "Detectado cruce a la derecha e izquierda!!!!!!" << std::endl;

//                  allAccesible=false;

//                  if(NDMath::randDouble4(0, 1) < 0.5) {
//                      allAccesible=false;
//                      areaCenterPolygonLeft->addSplitPointRightLeft(advanceAreaCenterCoordsLeft);
//                      advanceStatusLeft=areaCenterPolygonLeft->getAdvanceStatus();
//                      if(advanceStatusLeft==AreaCenterPolygon::Lost){
//                          break;
//                      }
//                      areaCenterPolygon->setAdvanceStatus(AreaCenterPolygon::Left);
//                      polygonSplitPointLeft=areaCenterPolygonLeft->getPolygonSplitPointLeft();
//                      polygonSplitPointRight=areaCenterPolygonLeft->getPolygonSplitPointRight();
//                      areaCenterPolygon->addSplitPoints(polygonSplitPointLeft,polygonSplitPointRight);
//                      advanceStatus = areaCenterPolygon->getAdvanceStatus();
//                      areaCenterPolygon->calculateAdvancePolygon();
//                      advanceStatus = areaCenterPolygon->getAdvanceStatus();
//                      if(advanceStatus==AreaCenterPolygon::Lost){
//                          advanceAreaCenter->setPoint(areaCenterPolygon->advanceAreaCenter());
//                          break;
//                      }
//                      areaCenterPolygon->advanceAreaCenter();
//                      advanceStatus = areaCenterPolygon->getAdvanceStatus();
//                      if(advanceStatus==AreaCenterPolygon::Lost){
//                          advanceAreaCenter->setPoint(areaCenterPolygon->advanceAreaCenter());
//                          break;
//                      }
//                      accesible=false;
//                  }
//                  else{
//                      allAccesible=false;
//                      areaCenterPolygonRight->addSplitPointRightLeft(advanceAreaCenterCoordsRight);
//                      advanceStatusRight=areaCenterPolygonRight->getAdvanceStatus();
//                      if(advanceStatusRight==AreaCenterPolygon::Lost){
//                          break;
//                      }
//                      areaCenterPolygon->setAdvanceStatus(AreaCenterPolygon::Right);
//                      polygonSplitPointLeft=areaCenterPolygonRight->getPolygonSplitPointLeft();
//                      polygonSplitPointRight=areaCenterPolygonRight->getPolygonSplitPointRight();
//                      areaCenterPolygon->addSplitPoints(polygonSplitPointLeft,polygonSplitPointRight);
//                      advanceStatus = areaCenterPolygon->getAdvanceStatus();
//                      areaCenterPolygon->calculateAdvancePolygon();
//                      advanceStatus = areaCenterPolygon->getAdvanceStatus();
//                      if(advanceStatus==AreaCenterPolygon::Lost){
//                          advanceAreaCenter->setPoint(areaCenterPolygon->advanceAreaCenter());
//                          break;
//                      }
//                      areaCenterPolygon->advanceAreaCenter();
//                      advanceStatus = areaCenterPolygon->getAdvanceStatus();
//                      if(advanceStatus==AreaCenterPolygon::Lost){
//                          advanceAreaCenter->setPoint(areaCenterPolygon->advanceAreaCenter());
//                          break;
//                      }
//                      accesible=false;
//                  }
//              }
//              if(accesibleLeft&&accesibleRight){
//                  allAccesible=true;
//              }
//          }
//      }

//    } while(!accesible);

//    advanceAreaCenter->setPoint(advanceAreaCenterCoords);
//    advanceAreaCenterLeft->setPoint(advanceAreaCenterCoordsLeft);
//    advanceAreaCenterRight->setPoint(advanceAreaCenterCoordsRight);

//    switch (advanceStatus) {
//      case AreaCenterPolygon::All:
//        advanceAreaCenter->setColor(QColor("magenta"));
//        advanceAreaCenterLeft->setColor(QColor("magenta"));
//        advanceAreaCenterRight->setColor(QColor("magenta"));
//        break;
//      case AreaCenterPolygon::Left:
//        advanceAreaCenter->setColor(QColor("orange"));
//        advanceAreaCenterLeft->setColor(QColor("orange"));
//        advanceAreaCenterRight->setColor(QColor("orange"));
//        break;
//      case AreaCenterPolygon::Right:
//        advanceAreaCenter->setColor(QColor("darkred"));
//        advanceAreaCenterLeft->setColor(QColor("darkred"));
//        advanceAreaCenterRight->setColor(QColor("darkred"));
//        break;
//      case AreaCenterPolygon::Lost:
//        advanceAreaCenter->setColor(QColor("red"));
//        advanceAreaCenterLeft->setColor(QColor("red"));
//        advanceAreaCenterRight->setColor(QColor("red"));
//        break;
//      case AreaCenterPolygon::Conservative:
//        advanceAreaCenter->setColor(QColor("darkmagenta"));
//        advanceAreaCenterLeft->setColor(QColor("darkmagenta"));
//        advanceAreaCenterRight->setColor(QColor("darkmagenta"));
//        break;
//    }

//    return;
//}

void AreaCenterReactiveControlRightLeft::detectCrossing(void)
{
    AreaCenterSegmentedControl::readGlobalPolygon();

    std::cerr << std::endl;
    std::cerr << "Sensors;";
    QList<double> sensorsDistances=createSensorsDistances();
    for(int i=0;i<sensorsDistances.size();i++){
        std::cerr << i << ";";
    }
    std::cerr << std::endl;

    std::cerr << "Distances;";
    for(int i=0;i<sensorsDistances.size();i++){
        std::cerr << sensorsDistances[i] << ";";
    }
    std::cerr << std::endl;

    std::cerr << std::endl;
    std::cerr << "Zone;StartSegment;EndSegment;Phi;Rho" << std::endl;

    QVector< pair < int , int > > endPointsZones=segmentation->getEndPoints();
    QVector< double > phisZones=segmentation->getPhiEst();
    QVector< double > rhosZones=segmentation->getRhoEst();

    for(int i=0;i<endPointsZones.size();i++){
        int numZone=i+1;
        pair<int,int> endPointsZone=endPointsZones.at(i);
        int startZone=endPointsZone.first;
        int endZone=endPointsZone.second;
        double phiZone=phisZones.at(i);
        double rhoZone=rhosZones.at(i);
        std::cerr << numZone << ";" << startZone << ";" << endZone << ";" << phiZone << ";" << rhoZone << std::endl;
    }
    std::cerr << std::endl;

    std::cerr << std::endl;
    std::cerr << "Free zone;StartSegment;EndSegment" << std::endl;

    QVector< pair < int , int > > freeZones=getFreeZones(endPointsZones);
    for(int i=0;i<freeZones.size();i++){
        int numZone=i+1;
        pair<int,int> freeZone=freeZones.at(i);
        int startFreeZone=freeZone.first;
        int endFreeZone=freeZone.second;
        std::cerr << numZone << ";" << startFreeZone << ";" << endFreeZone << std::endl;
    }
    std::cerr << std::endl;

    std::cerr << std::endl;
    std::cerr << "Big free zone;StartSegment;EndSegment" << std::endl;

    deleteSmallZones(freeZones);
    for(int i=0;i<freeZones.size();i++){
        int numZone=i+1;
        pair<int,int> freeZone=freeZones.at(i);
        int startFreeZone=freeZone.first;
        int endFreeZone=freeZone.second;
        std::cerr << numZone << ";" << startFreeZone << ";" << endFreeZone << std::endl;
    }
    std::cerr << std::endl;

    std::cerr << std::endl;
    if(freeZones.size()>0){
        pair<int,int> freeZone=getRandomFreeZone(freeZones);
        std::cerr << "Random free zone;" << freeZone.first << ";" << freeZone.second << std::endl;
        calculateSplitPoints(freeZone);
    }
    else{
        deleteSmallZones(endPointsZones);
        pair<int,int> bestZone=getBestZone(endPointsZones);
        if(bestZone.first==-1&&bestZone.second==-1){
            behaviour=SearchCrossing;
            std::cerr << "Best zone not found" << std::endl;
        }
        else{
            std::cerr << "Best zone;" << bestZone.first << ";" << bestZone.second << std::endl;
            calculateSplitPoints(bestZone);
        }
    }

    //HASTA AQUIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIII

//    QList<double> secondDerivateSensorsDistances=calculateSecondDerivateSensorsDistances(sensorsDistances);
//    for(int i=0;i<secondDerivateSensorsDistances.size();i++){
//        std::cerr << secondDerivateSensorsDistances[i] << ";";
//    }
//    std::cerr << std::endl;

//    QList<QString> zoneTags=calculateZoneTags(secondDerivateSensorsDistances);
//    for(int i=0;i<zoneTags.size();i++){
//        std::cerr << zoneTags[i].toStdString() << ";";
//    }
//    std::cerr << std::endl;

//    QList<QPair<int,int> > zonesLimits=calculateZonesLimits(zoneTags);
//    std::cerr << "Number of zones: " << zonesLimits.size() << std::endl;
//    for(int i=0;i<zonesLimits.size();i++){
//        std::cerr << "Zone " << i <<": from " << zonesLimits[i].first << " to " << zonesLimits[i].second  << std::endl;
//    }

//    QPair<int,int> bestZoneLimits=calculateBestZoneLimits(zonesLimits);
//    //QPair<int,int> bestZoneLimits=zonesLimits[0];
//    //QPair<int,int> bestZoneLimits(15,40);
//    std::cerr << "Best from " << bestZoneLimits.first << " to " << bestZoneLimits.second  << std::endl;

//    calculateSplitPoints(bestZoneLimits);

    std::cerr << "Fin"  << std::endl;
}

QList<double> AreaCenterReactiveControlRightLeft::createSensorsDistances(void){
    double d, x, y;
    QList<double> sensorsDistances;

    int numNodes = areaCenterNodeList.size();
    double boundaryLeft=(localAngleStart/NDMath::RAD2GRAD);
    double boundaryRight=(localAngleEnd/NDMath::RAD2GRAD);

    for(int i = 0; i < numNodes; i++) {
      d = areaCenterNodeList[i]->result().getY().at(0);
      x = areaCenterNodeList[i]->result().getY().at(2);
      y = areaCenterNodeList[i]->result().getY().at(3);
      double vertexAngle = atan2(y, x);

      if((vertexAngle >= boundaryLeft) && (vertexAngle <= boundaryRight))
        sensorsDistances.append(d);
      else if(vertexAngle > boundaryRight)
        break;
    }

    return sensorsDistances;
}

QVector< pair < int , int > > AreaCenterReactiveControlRightLeft::getFreeZones(QVector< pair < int , int > > endPointsZones){
    QVector< pair < int , int > > freeZones;
    int position=0;
    for(int i=0;i<endPointsZones.size() - 1;i++){ //  -1 jose 09-2018
        pair< int , int > endPointsZone=endPointsZones.at(i);
        int startZone=endPointsZone.first;
            int endZone=endPointsZone.second;
        if(position>=startZone&&position<=endZone){
            position=endZone+1;
        }
        else{
            pair< int , int > nextEndPointsZone=endPointsZones.at(i+1);
            int nextStartZone=nextEndPointsZone.first;
            int nextEndZone=nextEndPointsZone.second;
                pair< int , int > freeZone(position,nextStartZone-1);
            freeZones.append(freeZone);
            position=nextEndZone+1;
        }
    }

    int numSensors=filterData.size();
    if(position<numSensors){
        pair< int , int > freeZone(position,numSensors-1);
        freeZones.append(freeZone);
    }
    return freeZones;
}

void AreaCenterReactiveControlRightLeft::deleteSmallZones(QVector< pair < int , int > > zones){
    QVector< pair < int , int > > zonesSmall;
    for(int i=0;i<zones.size();i++){
        pair < int , int > zone=zones.at(i);
        int startZone=zone.first;
        int endZone=zone.second;
        double distanceStartZone=filterData[startZone];
        double distanceEndZone=filterData[endZone];
        double angleStartZone=angle.at(startZone);
        double angleEndZone=angle.at(endZone);
        double xStartZone=distanceStartZone*cos(angleStartZone);
        double yStartZone=distanceStartZone*sin(angleStartZone);
        double xEndZone=distanceEndZone*cos(angleEndZone);
        double yEndZone=distanceEndZone*sin(angleEndZone);
        double amplitudeZone = sqrt(pow((xEndZone-xStartZone),2)+pow((yEndZone-yStartZone),2));
        if(amplitudeZone<(1.5*robotDiameter)){
            zonesSmall.append(zone);
        }
    }

    for(int i=0;i<zonesSmall.size();i++){
        pair<int , int> zoneSmall=zonesSmall.at(i);
        int positionZoneSmall=zones.indexOf(zoneSmall);
        zones.remove(positionZoneSmall);
    }
}

pair<int,int> AreaCenterReactiveControlRightLeft::getRandomFreeZone(QVector< pair < int , int > > freeZones){
    int numFreeZones=freeZones.size();
    int randomNumber=rand()%numFreeZones;
    pair<int,int> randomFreeZone=freeZones.at(randomNumber);
    return randomFreeZone;
}

void AreaCenterReactiveControlRightLeft::calculateSplitPoints(pair<int,int> zone)
{
    QPointF advanceAreaCenterCoords;

    //Not found zone, follow
    if(zone.first==-1&&zone.second==-1){
        behaviour=SearchCrossing;
    }
    else{
        //Found zone
        int startZone=zone.first;
        int endZone=zone.second;
        double distanceStartZone=filterData[startZone];
        double distanceEndZone=filterData[endZone];
        double angleStartZone=angle.at(startZone);
        double angleEndZone=angle.at(endZone);
        double xStartZone=distanceStartZone*cos(angleStartZone);
        double yStartZone=distanceStartZone*sin(angleStartZone);
        double xEndZone=distanceEndZone*cos(angleEndZone);
        double yEndZone=distanceEndZone*sin(angleEndZone);
        QPointF splitPointRight(xStartZone,yStartZone);
        QPointF splitPointLeft(xEndZone,yEndZone);
        areaCenterPolygon->addSplitPoints(splitPointLeft,splitPointRight);
        areaCenterPolygon->calculateAdvancePolygon();
        advanceStatus = areaCenterPolygon->getAdvanceStatus();
        if(advanceStatus == AreaCenterPolygon::Lost) {
            advanceAreaCenterCoords = areaCenterPolygon->advanceAreaCenter();
            return;
        }

        advanceAreaCenterCoords = areaCenterPolygon->advanceAreaCenter();
        advanceStatus = areaCenterPolygon->getAdvanceStatus();
        if(advanceStatus == AreaCenterPolygon::Lost) {
            advanceAreaCenterCoords = areaCenterPolygon->advanceAreaCenter();
            return;
        }

        behaviour=GoCrossing;
    }
}

pair<int,int> AreaCenterReactiveControlRightLeft::getBestZone(QVector< pair < int , int > > endPointsZones){
    pair<int,int> bestZone(-1,-1);
    double maxDepth=0;
    for(int i=0;i<endPointsZones.size();i++){
        pair<int , int> endPointsZone=endPointsZones.at(i);
        int startZone=endPointsZone.first;
        int endZone=endPointsZone.second;
        for(int j=startZone;j<=endZone;j++){
            double depth=filterData[j];
            if(depth>maxDepth){
                maxDepth=depth;
                bestZone=endPointsZone;
            }
        }
    }
    return bestZone;
}

//QList<double> AreaCenterReactiveControlRightLeft::calculateSecondDerivateSensorsDistances(QList<double> sensorsDistances){
//    QList<double> secondDerivateSensorsDistances;
//    //It is not possible to calculate second derivates of two first sensors (filled by 0)
//    secondDerivateSensorsDistances.append(0);
//    secondDerivateSensorsDistances.append(0);

//    int numSensorsDistances = sensorsDistances.size();
//    for(int i=2;i<numSensorsDistances;i++){
//        double secondDerivate=sensorsDistances[i]-2*sensorsDistances[i-1]+sensorsDistances[i-2];
//        secondDerivateSensorsDistances.append(secondDerivate);
//    }
//    return secondDerivateSensorsDistances;
//}

//QList<QString> AreaCenterReactiveControlRightLeft::calculateZoneTags(QList<double> secondDerivateSensorsDistances){
//    QList<QString> zoneTags;
//    //It is not possible to calculate tags of two third sensors (filled by DC)
//    zoneTags.append("DC");
//    zoneTags.append("DC");
//    zoneTags.append("DC");

//    int numSecondDerivateSensorsDistances = secondDerivateSensorsDistances.size();
//    for(int i=3;i<numSecondDerivateSensorsDistances-1;i++){
//        QString zoneTag="DC";
//        //SZ
//        if(secondDerivateSensorsDistances[i-1]>=0&&
//           secondDerivateSensorsDistances[i]>0&&
//           secondDerivateSensorsDistances[i+1]<0){
//            zoneTag="SZ";
//        }
//        //EZ
//        if(secondDerivateSensorsDistances[i-1]<=0&&
//           secondDerivateSensorsDistances[i]>0&&
//           secondDerivateSensorsDistances[i+1]>=0){
//            zoneTag="EZ";
//        }
//        //IZ
//        if(secondDerivateSensorsDistances[i]<0){
//            zoneTag="IZ";
//        }

//        zoneTags.append(zoneTag);
//    }

//    //It is not possible to calculate tags of last sensor (filled by DC)
//    zoneTags.append("DC");

//    return zoneTags;
//}

//QList<QPair<int,int> > AreaCenterReactiveControlRightLeft::calculateZonesLimits(QList<QString> zoneTags){
//    QList<QPair<int,int> > zonesLimits;
//    int numZoneTags = zoneTags.size();
//    int searchTagFromPosition=0;
//    int positionTagSZ;
//    int positionTagEZ;
//    QString searchTag="SZ";
//    while(searchTagFromPosition<numZoneTags&&
//          zoneTags.indexOf(searchTag,searchTagFromPosition)!=-1){
//        if(searchTag=="SZ"){
//            positionTagSZ=zoneTags.indexOf(searchTag,searchTagFromPosition);
//            searchTag="EZ";
//            searchTagFromPosition=positionTagSZ+1;
//        }
//        else{
//            positionTagEZ=zoneTags.indexOf(searchTag,searchTagFromPosition);
//            QPair<int,int> zoneLimits(positionTagSZ,positionTagEZ);
//            zonesLimits.append(zoneLimits);
//            searchTag="SZ";
//            searchTagFromPosition=positionTagEZ+1;
//        }
//    }
//    return zonesLimits;
//}

//QPair<int,int> AreaCenterReactiveControlRightLeft::calculateBestZoneLimits(QList<QPair<int,int> > zonesLimits){
//    int numZones = zonesLimits.size();
//    double maxZone=0;
//    QPair<int,int> bestZone(-1,-1);
//    for(int i=0;i<numZones;i++){
//        QPair<int,int> zoneLimits=zonesLimits[i];
//        int zoneLimit1=zoneLimits.first;
//        int zoneLimit2=zoneLimits.second;
//        double xZoneLimit1 = areaCenterNodeList[zoneLimit1]->result().getY().at(2);
//        double yZoneLimit1 = areaCenterNodeList[zoneLimit1]->result().getY().at(3);
//        double xZoneLimit2 = areaCenterNodeList[zoneLimit2]->result().getY().at(2);
//        double yZoneLimit2 = areaCenterNodeList[zoneLimit2]->result().getY().at(3);
//        double zoneAmplitude = sqrt(pow((xZoneLimit2-xZoneLimit1),2)+pow((yZoneLimit2-yZoneLimit1),2));
//        //check amplitude
//        std::cerr << "Zone amplitude from from " << zoneLimit1 << " to " << zoneLimit2 << " is " <<  zoneAmplitude << std::endl;
//        std::cerr << "robotDiameter " << robotDiameter << std::endl;
//        if(zoneAmplitude>=(1.5*robotDiameter)){
//            //check depth
//            double zoneDepth=calculateZoneDepth(zoneLimits);
//            std::cerr << "Zone depth from " << zoneLimit1 << " to " << zoneLimit2 << " is " <<  zoneDepth << std::endl;
//            if(zoneDepth>maxZone){
//                maxZone=zoneDepth;
//                bestZone=zoneLimits;
//            }
//        }
//    }
//    return bestZone;
//}

//double AreaCenterReactiveControlRightLeft::calculateZoneDepth(QPair<int,int> zoneLimits){
//    double maxDepth=0;
//    int zoneLimit1=zoneLimits.first;
//    int zoneLimit2=zoneLimits.second;
//    for(int i=zoneLimit1;i<=zoneLimit2;i++){
//        double depthPointZone = areaCenterNodeList[i]->result().getY().at(0);
//        std::cerr << "Depth sensor " << i << " is " << depthPointZone << std::endl;
//        if(depthPointZone>maxDepth){
//           maxDepth=depthPointZone;
//        }
//    }
//    return maxDepth;
//}

//void AreaCenterReactiveControlRightLeft::calculateSplitPoints(QPair<int,int> bestZoneLimits)
//{
//    QPointF advanceAreaCenterCoords;

//    //Not found bestZone, follow
//    if(bestZoneLimits.first==-1&&bestZoneLimits.second==-1){
//        behaviour=SearchCrossing;
//    }
//    else{
//        //Found bestZone
//        double xZoneLimit1 = areaCenterNodeList[bestZoneLimits.first]->result().getY().at(2);
//        double yZoneLimit1 = areaCenterNodeList[bestZoneLimits.first]->result().getY().at(3);
//        double xZoneLimit2 = areaCenterNodeList[bestZoneLimits.second]->result().getY().at(2);
//        double yZoneLimit2 = areaCenterNodeList[bestZoneLimits.second]->result().getY().at(3);
//        QPointF splitPointRight(xZoneLimit1,yZoneLimit1);
//        QPointF splitPointLeft(xZoneLimit2,yZoneLimit2);
//        areaCenterPolygon->addSplitPoints(splitPointLeft,splitPointRight);
//        areaCenterPolygon->calculateAdvancePolygon();
//        advanceStatus = areaCenterPolygon->getAdvanceStatus();
//        if(advanceStatus == AreaCenterPolygon::Lost) {
//            advanceAreaCenterCoords = areaCenterPolygon->advanceAreaCenter();
//            return;
//        }

//        advanceAreaCenterCoords = areaCenterPolygon->advanceAreaCenter();
//        advanceStatus = areaCenterPolygon->getAdvanceStatus();
//        if(advanceStatus == AreaCenterPolygon::Lost) {
//            advanceAreaCenterCoords = areaCenterPolygon->advanceAreaCenter();
//            return;
//        }

//        behaviour=GoCrossing;
//    }
//}

void AreaCenterReactiveControlRightLeft::goCrossing(void)
{
    AreaCenterReactiveControl::calculateAreaCenters();

    if(advanceStatus==AreaCenterPolygon::All||(advanceStatus==AreaCenterPolygon::Conservative&&advanceRate>0.85)){
        behaviour=SearchCrossing;
    }

}

void AreaCenterReactiveControlRightLeft::doMonitor(void)
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
    emit sendToRecorder(advanceAreaCenterLeft->getXWorld(), 8);
    emit sendToRecorder(advanceAreaCenterLeft->getYWorld(), 9);
    emit sendToRecorder(advanceAreaCenterRight->getXWorld(), 10);
    emit sendToRecorder(advanceAreaCenterRight->getYWorld(), 11);
    }
}
