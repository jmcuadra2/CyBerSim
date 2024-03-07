//
// C++ Implementation: areacenterpolygon
//
// Description: 
//
//
// Author: Jose Manuel Cuadra Troncoso <jmcuadra@dia.uned.es>, (C) 2009
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "areacenterpolygon.h"
#include <iostream>
#include <QtGui>

#include "../world/abstractworld.h"

AreaCenterPolygon::AreaCenterPolygon() : QObject(), QPolygonF(), advanceAngles(-90.0/NDMath::RAD2GRAD, 90.0/NDMath::RAD2GRAD), currentAdvanceAngles(-90.0/NDMath::RAD2GRAD, 90.0/NDMath::RAD2GRAD)
{
  defaultValues();
}

AreaCenterPolygon::AreaCenterPolygon(QPointF advanceAngles) : QObject(), QPolygonF(), advanceAngles(advanceAngles), currentAdvanceAngles(advanceAngles)
{
  defaultValues();
}

AreaCenterPolygon::~AreaCenterPolygon()
{
}

void AreaCenterPolygon::defaultValues(void)
{
  world = 0;
  advanceStatus = All;
  sensorMaxRange  = 1000000.0;
  advanceMaxRange = sensorMaxRange;
  robotDiameter = 0.0;
  numAdvanceVertex = 0;
  deltaX = 0.0;
  deltaY = 0.0;
  deltaHeading = 0.0;
  showAreaCenter= true;
  lVel = 0.0;
  rVel = 0.0;
  suggestedSplit = false;
  corridorCollision = false;
}

void AreaCenterPolygon::setAdvanceAngles(QPointF advanceAngles)
{
  if(advanceAngles.x() <= advanceAngles.y()) {
    this->advanceAngles.setX(advanceAngles.x());
    this->advanceAngles.setY(advanceAngles.y());
  }
  else {
    this->advanceAngles.setX(advanceAngles.y());
    this->advanceAngles.setY(advanceAngles.x());
  }
  currentAdvanceAngles.setX(advanceAngles.x());
  currentAdvanceAngles.setY(advanceAngles.y());
  
  initPolygonSplitPointRight.setX(1000*cos(advanceAngles.x()));
  initPolygonSplitPointRight.setY(1000*sin(advanceAngles.x()));
  initPolygonSplitPointLeft.setX(1000*cos(advanceAngles.y()));
  initPolygonSplitPointLeft.setY(1000*sin(advanceAngles.y()));
  
  reset();

  int numVertex = size();
  numAdvanceVertex = 0;
  for(int i = 0; i < numVertex; i++) {
    double vertexAngle = atan2(at(i).y(), at(i).x());
    if((vertexAngle >= advanceAngles.x()) && (vertexAngle <= advanceAngles.y()))
      ++numAdvanceVertex;
    else if(vertexAngle > advanceAngles.y())
      break;
  }
}


void AreaCenterPolygon::calculateAdvancePolygon(void)
{
  if(advanceStatus == Lost)
    return;
    
  currentAdvanceAngles.setX(atan2(polygonSplitPointRight.y(), polygonSplitPointRight.x()));
  currentAdvanceAngles.setY(atan2(polygonSplitPointLeft.y(), polygonSplitPointLeft.x()));
  if((currentAdvanceAngles.x() < advanceAngles.x()) || (currentAdvanceAngles.x() > advanceAngles.y())) {
    currentAdvanceAngles.setX(advanceAngles.x());
    polygonSplitPointRight = initPolygonSplitPointRight;
  }
  if((currentAdvanceAngles.y() < advanceAngles.x()) || (currentAdvanceAngles.y() > advanceAngles.y())) {
    currentAdvanceAngles.setY(advanceAngles.y());
    polygonSplitPointLeft = initPolygonSplitPointLeft;
  }  
  if(currentAdvanceAngles.x() > currentAdvanceAngles.y()) {
    double swap = currentAdvanceAngles.x();
    currentAdvanceAngles.setX(currentAdvanceAngles.y());
    currentAdvanceAngles.setY(swap);
    // no deberia pasar por aqui
  }
   
  advancePolygon.clear();
  int numVertex = size();
  int numCurrentAdvanceVertex = 0;
  for(int i = 0; i < numVertex; i++) {
    double vertexAngle = atan2(at(i).y(), at(i).x());    
    if((vertexAngle >= currentAdvanceAngles.x()) && (vertexAngle <= currentAdvanceAngles.y())) {
    
//       std::cout << vertexAngle << endl;
    
      QPointF point(at(i));      
      double ratio = advanceMaxRange / sqrt(point.x()*point.x() + point.y()*point.y());
      ratio = ratio > 1.0 ? 1.0 : ratio;
      point.setX(point.x() * ratio);
      point.setY(point.y() * ratio);
      advancePolygon.append(point);
      if(showAreaCenter)
        world->changeRibColor(i, 1.5);
      ++numCurrentAdvanceVertex;
    }
    else if(((vertexAngle >= advanceAngles.x()) && (vertexAngle < currentAdvanceAngles.x())) || ((vertexAngle <= advanceAngles.y()) && (vertexAngle > currentAdvanceAngles.y()))) {
    
//       std::cout << vertexAngle << endl;
        
      if(showAreaCenter)
        world->changeRibColor(i, 0.3);
    }
    else {
    
//       std::cout << vertexAngle << "mal" << endl;
        
      if(showAreaCenter)
        world->changeRibColor(i, -0.5);
    }
  }
  if(numCurrentAdvanceVertex == 0)
    setLost();  
  else if(numCurrentAdvanceVertex == numAdvanceVertex) {
//     if(advanceStatus != All)
    if(advanceStatus != All) {
      if(advanceMaxRange >= sensorMaxRange)
        reset();
      else
        advanceStatus = Conservative;
      suggestedSplit = false;
    }
  }
  else {
    if(advanceStatus == All) {
      advanceStatus = Conservative;
    }
  }
}

bool AreaCenterPolygon::addSplitPoint(QPointF const& areapoint, bool suggested, AdvanceStatus preference)  //calculateSplitPoint
{
  bool ret = false;
  bool collision = false;
  double angleAreaPoint = atan2(areapoint.y(), areapoint.x());
  /*QPolygonF polygonLeft, polygonRight;*/  

//   if(!suggested)
//     preference = All;

  if(!suggested && suggestedSplit) {
    conservativeAdvancePolygon();
    advanceStatus = Conservative;
    calculateAdvancePolygon();
//     suggestedSplit = suggested;
  }

//   if(suggested) {
//     setAdvanceMaxRange(sensorMaxRange/4);
//     calculateAdvancePolygon();
//   }

  if((angleAreaPoint < currentAdvanceAngles.x()) || (angleAreaPoint > currentAdvanceAngles.y())) {
    setLost();
    std::cerr << "Warning: AreaCenterPolygon::addSplitPoint() areapoint out of angle (1)" << std::endl;
    return ret;  
  }
  int numCurrentAdvanceVertex = advancePolygon.size();
  int splitIdx = 0;
  QPointF splitPoint;
//   setAdvanceMaxRange(0.5*advanceMaxRange);

//   for(int i = 0; i < numCurrentAdvanceVertex; i++) {
//     QPointF p(advancePolygon.at(i));
//     if(atan2(p.y(), p.x()) < angleAreaPoint) {
//       ++splitIdx;
//       polygonRight.append(0.5*p);
//     }
//     else
//       polygonLeft.append(0.5*p);
//   }
  polygonLeft.clear();
  polygonRight.clear();
  for(int i = 0; i < numCurrentAdvanceVertex; i++) {
    QPointF p(advancePolygon.at(i));
    if(atan2(p.y(), p.x()) < angleAreaPoint) {
      ++splitIdx;
      polygonRight.append(p);
    }
    else
      polygonLeft.append(p);
  }

  double advanceMaxRangeLeft = 0, advanceMaxRangeRight = 0;
  if(polygonRight.size())
    advanceMaxRangeRight = polygonRight.size()/(double)numAdvanceVertex;
  if(polygonLeft.size())
    advanceMaxRangeLeft = polygonLeft.size()/(double)numAdvanceVertex;

  if((advanceMaxRangeRight < 0.15) && (advanceMaxRangeLeft < 0.15)) {
    if(!suggested) {
      if(suggestedSplit) {
        conservativeAdvancePolygon();
        advanceStatus = Conservative;
        ret = true;
        suggestedSplit = suggested;
        return ret;
      }
      else  {
        setLost();
        suggestedSplit = suggested;
        return ret;
      }
    }
  }

  suggestedSplit = suggested;

  for(int i = 0; i < polygonRight.size(); i++)
      polygonRight[i] *= advanceMaxRangeRight;
  for(int i = 0; i < polygonLeft.size(); i++)
      polygonLeft[i] *= advanceMaxRangeLeft;

  if(splitIdx == numCurrentAdvanceVertex) {
    std::cerr << "Warning: AreaCenterPolygon::addSplitPoint() areapoint out of angle (2)" << std::endl;
    if(!suggested)
      setLost();
    return ret;
  } 

  if(splitIdx > 0 && splitIdx < numCurrentAdvanceVertex - 1) {
      splitPoint.setX(areapoint.x());
      splitPoint.setY(areapoint.y());
//     double x1 = advancePolygon.at(splitIdx - 1).x();
//     double y1 = advancePolygon.at(splitIdx - 1).y();
//     double x2 = advancePolygon.at(splitIdx).x();
//     double y2 = advancePolygon.at(splitIdx).y();
//     if(splitIdx == 1){
//       splitPoint.setX(x2);
//       splitPoint.setY(y2);
//     }
// //     else if(splitIdx == numCurrentAdvanceVertex - 1) {
// //       splitPoint.setX(x1);
// //       splitPoint.setY(y1);
// //     }
//     else {
//       if((x1*x1 + y1*y1) < (x2*x2 + y2*y2)) {
//         splitPoint.setX(x1);
//         splitPoint.setY(y1);
//       }
//       else {
//         splitPoint.setX(x2);
//         splitPoint.setY(y2);
//       }      
//     }
  }
  else if(splitIdx == 0) {
    splitPoint.setX(advancePolygon.at(1).x());
    splitPoint.setY(advancePolygon.at(1).y());
  }
  else if(splitIdx == numCurrentAdvanceVertex - 1) {
    splitPoint.setX(advancePolygon.at(splitIdx-1).x());
    splitPoint.setY(advancePolygon.at(splitIdx-1).y());    
  }

//   if(suggested)
//     splitPoint *= 0.375;
  
  QPointF areaCenterCoordsRight = calculateAreaCenter(polygonRight, QPointF(-2*sensorMaxRange, 0));
  bool accesibleRight = AreaCenterPolygon::isAdvanceAccesible(areaCenterCoordsRight, robotDiameter/2, Right);
  collision = corridorCollision;
  
  QPointF areaCenterCoordsLeft = calculateAreaCenter(polygonLeft, QPointF(-2*sensorMaxRange, 0));  
  bool accesibleLeft = AreaCenterPolygon::isAdvanceAccesible(areaCenterCoordsLeft, robotDiameter/2, Left);
  collision = collision || corridorCollision;
  
  if(advanceStatus == Lost) { // Colision con robotCorridor
    setLost();
    return ret;
  }
  
  if(accesibleRight && accesibleLeft) {
//     double areaRight = polygonArea(polygonRight);
//     double areaLeft = polygonArea(polygonLeft);
    double areaRight = polygonRight.size() * polygonArea(polygonRight);
    double areaLeft = polygonLeft.size() * polygonArea(polygonLeft);
//     double areaRight = double(polygonArea(polygonRight)) / polygonRight.size();
//     double areaLeft = double(polygonArea(polygonLeft)) / polygonLeft.size();

    double chooseLeft = 0.5;
    if(areaRight + areaLeft > 0) {
      chooseLeft = areaLeft/(areaRight + areaLeft);
      chooseLeft = chooseLeft < 0.1 ? 0.0 : chooseLeft;
      chooseLeft = chooseLeft > 0.9 ? 1.0 : chooseLeft;
    }

    if((preference == Left) && (chooseLeft > 0.15)) {
      polygonSplitPointRight = splitPoint;
      setAdvanceMaxRange(advanceMaxRangeLeft * sensorMaxRange);
      advancePolygon = polygonLeft;
      advanceStatus = Left;
      ret = true;
    }
    else if((preference == Right) && (chooseLeft < 0.85)) {
      polygonSplitPointLeft = splitPoint;
      setAdvanceMaxRange(advanceMaxRangeRight * sensorMaxRange);
      advancePolygon = polygonRight;
      advanceStatus = Right;
      ret = true;
    }

//     if((preference == Left) && suggestedSplit && (chooseLeft > 0.75)) {
//       polygonSplitPointRight = splitPoint;
//       setAdvanceMaxRange(advanceMaxRangeLeft * sensorMaxRange);
//       advancePolygon = polygonLeft;
//       advanceStatus = Left;
//       ret = true;
//     }
// //     if((preference == Left) && suggestedSplit && (areaLeft > 100000)) {
// //       polygonSplitPointRight = splitPoint;
// //       setAdvanceMaxRange(advanceMaxRangeLeft * sensorMaxRange);
// //       advancePolygon = polygonLeft;
// //       advanceStatus = Left;
// //       ret = true;
// //     }
//     else if((preference == Left) && !suggestedSplit && (chooseLeft >= 0.1)) {
//       polygonSplitPointRight = splitPoint;
//       setAdvanceMaxRange(advanceMaxRangeLeft * sensorMaxRange);
//       advancePolygon = polygonLeft;
//       advanceStatus = Left;
//       ret = true;
//     }
//     else if((preference == Right) && suggestedSplit && (chooseLeft < 0.25)) {
//       polygonSplitPointLeft = splitPoint;
//       setAdvanceMaxRange(advanceMaxRangeRight * sensorMaxRange);
//       advancePolygon = polygonRight;
//       advanceStatus = Right;
//       ret = true;
//     }
// /*    else if((preference == Right) && suggestedSplit && (areaRight < 100000)) {
//       polygonSplitPointLeft = splitPoint;
//       setAdvanceMaxRange(advanceMaxRangeRight * sensorMaxRange);
//       advancePolygon = polygonRight;
//       advanceStatus = Right;
//       ret = true;
//     } */         
//     else if((preference == Right) && !suggestedSplit && (chooseLeft <= 0.9)) {
//       polygonSplitPointLeft = splitPoint;
//       setAdvanceMaxRange(advanceMaxRangeRight * sensorMaxRange);
//       advancePolygon = polygonRight;
//       advanceStatus = Right;
//       ret = true;
//     }
    else {
      if(NDMath::randDouble4(0, 1) < chooseLeft) { // se elige Left
        polygonSplitPointRight = splitPoint;
        if(collision) {
          double r = sqrt(polygonSplitPointRight.x()*polygonSplitPointRight.x() + polygonSplitPointRight.y()*polygonSplitPointRight.y());
          if(r){
            polygonSplitPointRight *= 50/r;
          }
        }         
        setAdvanceMaxRange(advanceMaxRangeLeft * sensorMaxRange);
        advancePolygon = polygonLeft;
        advanceStatus = Left;
        ret = true;
      }
      else {  // se elige Right
        polygonSplitPointLeft = splitPoint;
        if(collision) {
          double r = sqrt(polygonSplitPointLeft.x()*polygonSplitPointLeft.x() + polygonSplitPointLeft.y()*polygonSplitPointLeft.y());
          if(r){
            polygonSplitPointLeft *= 50/r;
          }
        }           
        setAdvanceMaxRange(advanceMaxRangeRight * sensorMaxRange);
        advancePolygon = polygonRight;
        advanceStatus = Right;
        ret = true;
      }
    }
  }
  else if(accesibleLeft) {
    if((preference == Right)  && suggestedSplit)
      return ret;
    
    polygonSplitPointRight = splitPoint;
    
    if(collision) {
      double r = sqrt(polygonSplitPointRight.x()*polygonSplitPointRight.x() + polygonSplitPointRight.y()*polygonSplitPointRight.y());
      if(r){
        polygonSplitPointRight *= 50/r;
      }
    }  
    setAdvanceMaxRange(advanceMaxRangeLeft * sensorMaxRange);
    
    advancePolygon = polygonLeft;
    advanceStatus = Left;
    ret = true;
  }
  else if(accesibleRight){
    if((preference == Left)  && suggestedSplit)
      return ret;
        
    polygonSplitPointLeft = splitPoint;
    if(collision) {
      double r = sqrt(polygonSplitPointLeft.x()*polygonSplitPointLeft.x() + polygonSplitPointLeft.y()*polygonSplitPointLeft.y());
      if(r){
        polygonSplitPointLeft *= 50/r;
      }
    }    
    setAdvanceMaxRange(advanceMaxRangeRight * sensorMaxRange);
    advancePolygon = polygonRight;
    advanceStatus = Right;
    ret = true;
  }
  else if(!accesibleRight && !accesibleLeft) {
    if(!suggested) {
      if(advanceStatus == All) {
        conservativeAdvancePolygon();
        advanceStatus = Conservative;
        ret = true;
      }
      else 
        setLost();
   }
   else {
      conservativeAdvancePolygon();
      advanceStatus = Conservative;
      ret = true;
   }
 }

 return ret;   
}

//IMPLEMENTACION JAVI
bool AreaCenterPolygon::addSplitPointCenter(QPointF const& areapoint, bool suggested, AdvanceStatus preference)  //calculateSplitPointCenter
{
  bool ret = false;
  bool collision = false;
  double angleAreaPoint = atan2(areapoint.y(), areapoint.x());

  if(!suggested && suggestedSplit) {
    conservativeAdvancePolygon();
    advanceStatus = Conservative;
    calculateAdvancePolygon();
  }

  if((angleAreaPoint < currentAdvanceAngles.x()) || (angleAreaPoint > currentAdvanceAngles.y())) {
    setLost();
    std::cerr << "Warning: AreaCenterPolygon::addSplitPoint() areapoint out of angle (1)" << std::endl;
    return ret;
  }
  int numCurrentAdvanceVertex = advancePolygon.size();
  int splitIdx = 0;
  QPointF splitPoint;

  polygonLeft.clear();
  polygonRight.clear();
  for(int i = 0; i < numCurrentAdvanceVertex; i++) {
    QPointF p(advancePolygon.at(i));
    if(atan2(p.y(), p.x()) < angleAreaPoint) {
      ++splitIdx;
      polygonRight.append(p);
    }
    else
      polygonLeft.append(p);
  }

  double advanceMaxRangeLeft = 0, advanceMaxRangeRight = 0;
  if(polygonRight.size())
    advanceMaxRangeRight = polygonRight.size()/(double)numAdvanceVertex;
  if(polygonLeft.size())
    advanceMaxRangeLeft = polygonLeft.size()/(double)numAdvanceVertex;

  std::cerr << "Center advanceMaxRangeRight:" << advanceMaxRangeRight << " Center advanceMaxRangeLeft:" << advanceMaxRangeLeft << std::endl;

  //BEFORE 0,15; NOW 0,10
  if(advanceMaxRangeRight < 0.05 && advanceMaxRangeLeft < 0.05) {
    if(!suggested) {
      if(suggestedSplit) {
        conservativeAdvancePolygon();
        advanceStatus = Conservative;
        ret = true;
        suggestedSplit = suggested;
        return ret;
      }
      else  {
        setLost();
        suggestedSplit = suggested;
        return ret;
      }
    }
  }

  suggestedSplit = suggested;

  for(int i = 0; i < polygonRight.size(); i++)
      polygonRight[i] *= advanceMaxRangeRight;
  for(int i = 0; i < polygonLeft.size(); i++)
      polygonLeft[i] *= advanceMaxRangeLeft;

  if(splitIdx == numCurrentAdvanceVertex) {
    std::cerr << "Warning: AreaCenterPolygon::addSplitPoint() areapoint out of angle (2)" << std::endl;
    if(!suggested)
      setLost();
    return ret;
  }

  if(splitIdx > 0 && splitIdx < numCurrentAdvanceVertex - 1) {
      splitPoint.setX(areapoint.x());
      splitPoint.setY(areapoint.y());
  }
  else if(splitIdx == 0) {
    splitPoint.setX(advancePolygon.at(1).x());
    splitPoint.setY(advancePolygon.at(1).y());
  }
  else if(splitIdx == numCurrentAdvanceVertex - 1) {
    splitPoint.setX(advancePolygon.at(splitIdx-1).x());
    splitPoint.setY(advancePolygon.at(splitIdx-1).y());
  }

  QPointF areaCenterCoordsRight = calculateAreaCenter(polygonRight, QPointF(-2*sensorMaxRange, 0));
  bool accesibleRight = AreaCenterPolygon::isAdvanceAccesible(areaCenterCoordsRight, robotDiameter/2, Right);
  collision = corridorCollision;

  QPointF areaCenterCoordsLeft = calculateAreaCenter(polygonLeft, QPointF(-2*sensorMaxRange, 0));
  bool accesibleLeft = AreaCenterPolygon::isAdvanceAccesible(areaCenterCoordsLeft, robotDiameter/2, Left);
  collision = collision || corridorCollision;

  if(advanceStatus == Lost) { // Colision con robotCorridor
    setLost();
    return ret;
  }

  if(accesibleRight && accesibleLeft) {
    double areaRight = polygonRight.size() * polygonArea(polygonRight);
    double areaLeft = polygonLeft.size() * polygonArea(polygonLeft);

    double chooseLeft = 0.5;
    if(areaRight + areaLeft > 0) {
      chooseLeft = areaLeft/(areaRight + areaLeft);
      chooseLeft = chooseLeft < 0.1 ? 0.0 : chooseLeft;
      chooseLeft = chooseLeft > 0.9 ? 1.0 : chooseLeft;
    }

    if((preference == Left) && (chooseLeft > 0.15)) {
      polygonSplitPointRight = splitPoint;
      setAdvanceMaxRange(advanceMaxRangeLeft * sensorMaxRange);
      advancePolygon = polygonLeft;
      advanceStatus = Left;
      ret = true;
    }
    else if((preference == Right) && (chooseLeft < 0.85)) {
      polygonSplitPointLeft = splitPoint;
      setAdvanceMaxRange(advanceMaxRangeRight * sensorMaxRange);
      advancePolygon = polygonRight;
      advanceStatus = Right;
      ret = true;
    }
    else {
      if(NDMath::randDouble4(0, 1) < chooseLeft) { // se elige Left
        polygonSplitPointRight = splitPoint;
        if(collision) {
          double r = sqrt(polygonSplitPointRight.x()*polygonSplitPointRight.x() + polygonSplitPointRight.y()*polygonSplitPointRight.y());
          if(r){
            polygonSplitPointRight *= 50/r;
          }
        }
        setAdvanceMaxRange(advanceMaxRangeLeft * sensorMaxRange);
        advancePolygon = polygonLeft;
        advanceStatus = Left;
        ret = true;
      }
      else {  // se elige Right
        polygonSplitPointLeft = splitPoint;
        if(collision) {
          double r = sqrt(polygonSplitPointLeft.x()*polygonSplitPointLeft.x() + polygonSplitPointLeft.y()*polygonSplitPointLeft.y());
          if(r){
            polygonSplitPointLeft *= 50/r;
          }
        }
        setAdvanceMaxRange(advanceMaxRangeRight * sensorMaxRange);
        advancePolygon = polygonRight;
        advanceStatus = Right;
        ret = true;
      }
    }
  }
  else if(accesibleLeft) {
    if((preference == Right)  && suggestedSplit)
      return ret;

    polygonSplitPointRight = splitPoint;

    if(collision) {
      double r = sqrt(polygonSplitPointRight.x()*polygonSplitPointRight.x() + polygonSplitPointRight.y()*polygonSplitPointRight.y());
      if(r){
        polygonSplitPointRight *= 50/r;
      }
    }
    setAdvanceMaxRange(advanceMaxRangeLeft * sensorMaxRange);

    advancePolygon = polygonLeft;
    advanceStatus = Left;
    ret = true;
  }
  else if(accesibleRight){
    if((preference == Left)  && suggestedSplit)
      return ret;

    polygonSplitPointLeft = splitPoint;
    if(collision) {
      double r = sqrt(polygonSplitPointLeft.x()*polygonSplitPointLeft.x() + polygonSplitPointLeft.y()*polygonSplitPointLeft.y());
      if(r){
        polygonSplitPointLeft *= 50/r;
      }
    }
    setAdvanceMaxRange(advanceMaxRangeRight * sensorMaxRange);
    advancePolygon = polygonRight;
    advanceStatus = Right;
    ret = true;
  }
  else if(!accesibleRight && !accesibleLeft) {
    if(!suggested) {
      if(advanceStatus == All) {
        conservativeAdvancePolygon();
        advanceStatus = Conservative;
        ret = true;
      }
      else
        setLost();
   }
   else {
      conservativeAdvancePolygon();
      advanceStatus = Conservative;
      ret = true;
   }
 }

 return ret;
}

bool AreaCenterPolygon::addSplitPoints(QPointF const& polygonSplitPointL,QPointF const& polygonSplitPointR){
    QPolygonF polygon;
    bool ret = false;
    double angleAreaPointLeft = atan2(polygonSplitPointL.y(), polygonSplitPointL.x());
    double angleAreaPointRight = atan2(polygonSplitPointR.y(), polygonSplitPointR.x());

    int numCurrentAdvanceVertex = advancePolygon.size();
    int splitIdx = 0;

    for(int i = 0; i < numCurrentAdvanceVertex; i++) {
      QPointF p(advancePolygon.at(i));
      if(atan2(p.y(), p.x()) <= angleAreaPointLeft && atan2(p.y(), p.x()) >= angleAreaPointRight) {
        ++splitIdx;
        polygon.append(p);
      }
    }

    double advanceMaxRangePolygon = 0;
    if(polygon.size()){
        advanceMaxRangePolygon=polygon.size()/(double)numAdvanceVertex;
    }

    std::cerr << "AddSplitPoints advanceMaxRangePolygon:" << advanceMaxRangePolygon << std::endl;

    ///AQUI ES CUANDO HACE SET LOST!!(antes 0.15, yo 0,07)
    if(advanceMaxRangePolygon < 0.07) {
      std::cerr << "addSplitPoints::  Warning: AreaCenterPolygon::addSplitPoint() small" << std::endl;
      //setLost();
      return ret;
    }

    if(splitIdx == numCurrentAdvanceVertex) {
      std::cerr << "addSplitPoints::  Warning: AreaCenterPolygon::addSplitPoint() areapoint out of angle (2)" << std::endl;
      //setLost();
      return ret;
    }

    polygonSplitPointLeft = polygonSplitPointL;
    polygonSplitPointRight = polygonSplitPointR;
    setAdvanceMaxRange(advanceMaxRangePolygon*sensorMaxRange);
    advancePolygon=polygon;
    ret = true;

    return ret;
}

double AreaCenterPolygon::advanceRate(void)
{
  if(advanceStatus == All)
    return 1;
  else if(advanceStatus == Conservative)
    return advanceMaxRange/sensorMaxRange;
//     return advanceMaxRange/sensorMaxRange*(double)advancePolygon.size()/numAdvanceVertex; // el segundo factor para cuando se pierden medidas
  else
    return (double)advancePolygon.size()/numAdvanceVertex;
}

void AreaCenterPolygon::setRobotMovement(double deltaX, double deltaY, double deltaHeading, double lVel, double rVel)
{
  this->deltaX = deltaX;
  this->deltaY = deltaY;
  this->deltaHeading = deltaHeading;
  this->lVel  = lVel;
  this->rVel  = rVel;
}

void AreaCenterPolygon::moveBySplitPoints(void)
{
  if(polygonSplitPointRight != initPolygonSplitPointRight) {
    polygonSplitPointRight.setX(polygonSplitPointRight.x() - deltaX);
    polygonSplitPointRight.setY(polygonSplitPointRight.y() - deltaY);

    NDMath::selfRotateRad(polygonSplitPointRight, deltaHeading);
    emit changeSplitPoint(polygonSplitPointRight, Right - 1);
    
     // es -deltaHeading*world::headTransform() ?
  }
  else
    emit hideSplitPoint(Right - 1);
    
  if(polygonSplitPointLeft != initPolygonSplitPointLeft) {
    polygonSplitPointLeft.setX(polygonSplitPointLeft.x() - deltaX);
    polygonSplitPointLeft.setY(polygonSplitPointLeft.y() - deltaY);

    NDMath::selfRotateRad(polygonSplitPointLeft, deltaHeading);  // es -deltaHeading*world::headTransform() ?
    emit changeSplitPoint(polygonSplitPointLeft, Left - 1);
  }
  else
    emit hideSplitPoint(Left - 1);
}

double AreaCenterPolygon::polygonArea(QPolygonF const& poly)
{
  double area = 0;
  int polySize = poly.size();

  for(int i = 0; i < polySize - 1; i++)
    area += (poly.at(i).x()*poly.at(i + 1).y() - poly.at(i + 1).x()*poly.at(i).y())/2;
    
  return area;
}

QPointF AreaCenterPolygon::calculateAreaCenter(QPolygonF const & polygon, QPointF defaultValue)
{
  QPointF areaC(defaultValue.x(), defaultValue.y());
  int numVertex = polygon.size();
  if(numVertex < 2) {
//     std::cerr << "Warning: AreaCenterReactiveControl::calculateAreaCenter() too few points" << std::endl;
    return areaC;
  }
  
  double x_center = 0;
  double y_center = 0;
  double sum_area = 0;
//   double cos_aperture;
  QPointF p1, p2;
  for(int i = 0; i < numVertex - 1; i++) {
    double d1, d2, area, xc,yc;
    p1 = polygon.at(i);
    p2 = polygon.at(i + 1);
    d1 = sqrt(p1.x()*p1.x() + p1.y()*p1.y());
    d2 = sqrt(p2.x()*p2.x() + p2.y()*p2.y());
    if(!d1 || !d2)
      continue;
    area = (p1.x()*p2.y() - p2.x()*p1.y())/2;  // no necesita apertura fija
    if(abs(area) < 1e-6) continue;
    if(area < 0)
      ++area;  // para punto de interrupcion en debugger
    area = area*log(1 + (d1 + d2)/2);  // log es la densidad de area
//     area = area/log(1 + (d1 + d2)/2);  // log es la densidad de area
    
    xc = (p1.x() + p2.x())/3;
    yc = (p1.y() + p2.y())/3;

    x_center = x_center * sum_area;
    y_center = y_center * sum_area;
    sum_area += area;

    x_center = x_center/sum_area + xc*area/sum_area;
    y_center = y_center/sum_area + yc*area/sum_area;
    areaC.setX(x_center);
    areaC.setY(y_center);
  }
  return areaC;
}

bool AreaCenterPolygon::polygonContains(QPointF const& point, QPolygonF const & polygon)
{
  bool ret = false;
  double x1, y1, xp, yp, x0, y0, x0p, y0p, x01, y01;
  int numLados = polygon.size();
  
  if(numLados < 2)
    return ret;
    
  int i;
  xp = point.x();
  yp = point.y();
  for (i = 0; i < numLados; i++) {
    x1 = polygon.at(i).x();
    y1 = polygon.at(i).y();
    if((x1*yp - xp*y1) <= 0)
      break;
  }

  if(i == 0)
    ret = ((x1*yp - xp*y1) == 0); // producto vectorial
  else if(i == numLados); // no se cumplio la condicion x1 >= xp
  else {
    x0 = polygon.at(i - 1).x();
    y0 = polygon.at(i - 1).y();
    x01 = x1 - x0;
    y01 = y1 - y0;
    x0p = xp - x0;
    y0p = yp - y0;
    
    ret = ((x0p*y01 - x01*y0p) <= 0);
    
  }

  return ret;
}

bool AreaCenterPolygon::isAccesible(QPointF const& point, QPolygonF const & polygon, QPolygonF const& polygonGlobal, double robotRoom, AdvanceStatus side, bool isPolygonMain)
{

  double d2;
  robotRoom = robotRoom < 0 ? 0 : robotRoom;
  double robotRoom2 = robotRoom * robotRoom;

  bool ret = false;
  corridorCollision = false;
  double x1, y1, xp, yp, x0, y0, x0p, y0p, x1p, y1p, x01, y01;
  int numPoints = polygon.size();
  double acAngle;
  if(numPoints < 2)
    return ret;
 
  int i;
  xp = point.x();
  yp = point.y();
  if(((xp*xp + yp*yp) < 4*robotRoom2) && advanceStatus == All ) { // AC muy cerca de robot
//     setLost();

//    for(int i = 0; i < numPoints; i++)
//      cout << "(" << polygon[i].x() << ", " << polygon[i].y() << ")";
//    cout << endl;
    return ret;
  }
  acAngle = atan2(yp,xp);
  
  for (i = 0; i < numPoints; i++) {
    x0 = polygon.at(i).x();
    y0 = polygon.at(i).y();
    x0p = xp - x0;
    y0p = yp - y0;
    ret = (x0p*x0p + y0p*y0p > robotRoom2); // distancia al primer extremo del segmento (SingularPointCollision)

    if(ret && i < numPoints - 1) {  // distancia del punto al segmento
      x1 = polygon.at(i + 1).x();
      y1 = polygon.at(i + 1).y();
      x01 = x1 - x0;
      y01 = y1 - y0;      
      x1p = xp - x1;
      y1p = yp - y1;    
      double segment_long2 = x01*x01 + y01*y01;
      if(segment_long2 > 0.0) {
        d2 = xp*y01 - yp*x01 - x0*y01 + y0*x01;
        d2 *= d2/(segment_long2);
        ret = d2 > robotRoom2;
        if(!ret)
          ret = (x0p*x0p + y0p*y0p + x1p*x1p + y1p*y1p > robotRoom2 + segment_long2); // el punto está entre las perpendiculares al segmento
      }
    }
    if(!ret)
      break;
  }
  
  if(ret && isPolygonMain) {
//     double velFactorAdv = 10*(lVel + rVel)/2/50;  
    double velFactorAdv = 10*(lVel + rVel)/2/50;  // aqui hay que mandar (50) la vel max? del robot en el experimento
    double velFactorWidth =.5*fabs(lVel - rVel)/2/50;
     // 100 >= |lVel - rVel|
//     double velFactorAdv = (lVel + rVel)/2/50;  // aqui hay que mandar (50) la vel max? del robot en el experimento
//     double velFactorWidth =0.02*fabs(lVel - rVel)/2/50; // 100 >= |lVel - rVel|
    QPointF roomAdvanceVector = QPointF(robotRoom * 1.25, 0);
    QPointF roomWidthVector = QPointF(0, robotRoom * 1.1);
    QPointF translationWidth = QPointF(0, robotRoom*velFactorWidth);
        
//     QPointF translationAdvance = NDMath::rotateRad(deltaX, deltaY, 0, 0, deltaHeading)*velFactorAdv + roomAdvanceVector; // 10 steps.
// 
//     QPolygonF robotCorridor, intersection;
//     robotCorridor.append(-roomWidthVector - translationWidth);
//     robotCorridor.append(-roomWidthVector + translationAdvance);
//     robotCorridor.append(roomWidthVector + translationAdvance);
//     robotCorridor.append(roomWidthVector + translationWidth);

    // El hexagono robotCorridor no debe ser nunca un rectangulo, ya
    // que en ese caso intersection tendría 4 puntos y no 6, por lo que
    // nunca seria == a robotCorridor aunque delimitaran la misma zona
    double factorBendCorridor = 0.5;
    QPointF translationAdvance1 = NDMath::rotateRad(deltaX, deltaY, 0, 0, deltaHeading)*velFactorAdv*factorBendCorridor + roomAdvanceVector; // 10 steps.
    QPointF translationAdvance2 = NDMath::rotateRad(deltaX, deltaY, 0, 0, acAngle)*velFactorAdv*(1 - factorBendCorridor)/* + roomAdvanceVector*1.01*/; // 10 steps, 1.01 Ñapa para que la interseccion tenga 6 puntos

//     QPolygonF robotCorridor, intersection;
//     robotCorridor.append(-roomWidthVector - translationWidth);
//     robotCorridor.append(-roomWidthVector + factorBendCorridor*translationAdvance1);
//     robotCorridor.append(-0.99*roomWidthVector + factorBendCorridor*translationAdvance1 + (1 - factorBendCorridor)*translationAdvance2 + QPointF(1, 0));
// 
//     robotCorridor.append(0.99*roomWidthVector + factorBendCorridor*translationAdvance1 + (1 - factorBendCorridor)*translationAdvance2 + QPointF(1, 0));
//     // 0.99 Ñapa para que la interseccion tenga 6 puntos
//     robotCorridor.append(roomWidthVector + factorBendCorridor*translationAdvance1);
//     robotCorridor.append(roomWidthVector + translationWidth);
    
    QPolygonF robotCorridor, intersection;
    if(side == Left) {
      robotCorridor.append(-roomWidthVector/* - translationWidth*/);
//       robotCorridor.append(QPointF(0, 0) + translationAdvance1);
      robotCorridor.append(-roomWidthVector + roomAdvanceVector);
      robotCorridor.append(QPointF(0, 0) + translationAdvance1 +translationAdvance2 + QPointF(1, 0));
      robotCorridor.append(0.99*roomWidthVector + translationAdvance1 + translationAdvance2 + QPointF(1, 0));
      // 0.99 Ñapa para que la interseccion tenga 6 puntos
      robotCorridor.append(roomWidthVector + translationAdvance1);
      robotCorridor.append(roomWidthVector + translationWidth);
    }
    else if(side == Right) {
      robotCorridor.append(-roomWidthVector - translationWidth);
      robotCorridor.append(-roomWidthVector + translationAdvance1);
      robotCorridor.append(-0.99*roomWidthVector + translationAdvance1 + translationAdvance2 + QPointF(1, 0));

      robotCorridor.append(QPointF(0, 0) + translationAdvance1 + translationAdvance2 + QPointF(1, 0));
      // 0.99 Ñapa para que la interseccion tenga 6 puntos
//       robotCorridor.append(QPointF(0, 0) + translationAdvance1);
      robotCorridor.append(roomWidthVector + roomAdvanceVector);
      robotCorridor.append(roomWidthVector/* + translationWidth*/);
    }
    else {
      robotCorridor.append(-roomWidthVector - translationWidth);
      robotCorridor.append(-roomWidthVector + translationAdvance1);
      robotCorridor.append(-0.99*roomWidthVector + translationAdvance1 + translationAdvance2 + QPointF(1, 0));

      robotCorridor.append(0.99*roomWidthVector + translationAdvance1 + translationAdvance2 + QPointF(1, 0));
      // 0.99 Ñapa para que la interseccion tenga 6 puntos
      robotCorridor.append(roomWidthVector + translationAdvance1);
      robotCorridor.append(roomWidthVector + translationWidth);
    }

//     if(polygonContains(point, robotCorridor)) {
//       ret = false;
//       setLost();
//     }
//     else {
      intersection = polygonGlobal.intersected(robotCorridor);
   

      if(!intersection.isEmpty())/* {
	if(full360)*/
/*	  intersection.pop_back(); */     
// 	else
	  intersection.pop_back();
//       }
      ret = (robotCorridor == intersection);
      if(!ret) {
/*        qDebug() << polygonGlobal;
        std::cout << endl;   */   
//         qDebug() << robotCorridor;
//        std::cout << endl;
//         qDebug() <<intersection;
//        std::cout << endl<<endl;
        corridorCollision = true;
      }
    }
//   }
  
  return ret;
}


// bool AreaCenterPolygon::isAccesible(QPointF const& point, QPolygonF const & polygon, QPolygonF const& polygonGlobal, double robotRoom, AdvanceStatus side)
// {
// 
//   QPointF p(0, 0);
//   double d2;
//   robotRoom = robotRoom < 0 ? 0 : robotRoom;
//   double robotRoom2 = robotRoom * robotRoom;
// 
//   bool ret = false;
//   double x1, y1, xp, yp, x0, y0, x0p, y0p, x1p, y1p, x01, y01;
//   int numLados = polygon.size();
//   
//   if(numLados < 2)
//     return ret;
//     
//   int i;
//   xp = point.x();
//   yp = point.y();
//   for (i = 0; i < numLados; i++) {
//     x1 = polygon.at(i).x();
//     y1 = polygon.at(i).y();
//     if((x1*yp - xp*y1) <= 0)
//       break;
//   }
// 
//   if(i == 0)
//     ret = ((x1*yp - xp*y1) == 0); // producto vectorial
//   else if(i == numLados); // no se cumplio la condicion x1 >= xp
//   else {
//     x0 = polygon.at(i - 1).x();
//     y0 = polygon.at(i - 1).y();
//     x01 = x1 - x0;
//     y01 = y1 - y0;
//     x0p = xp - x0;
//     y0p = yp - y0;
//     x1p = xp - x1;
//     y1p = yp - y1;
// 
//     ret = ((x0p*y01 - x01*y0p) <= 0); // comprobar si esta dentro del triangulo
//     if(ret) {                               ;
//       ret = (x0p*x0p + y0p*y0p > robotRoom2) && (x1p*x1p + y1p*y1p > robotRoom2); // distancia a extremos del segmento (SingularPointCollision)
//       
//       if(ret) {  // distancia del punto al segmento
//         double segment_long2 = x01*x01 + y01*y01;
//         if(segment_long2 > 0.0) {
//           d2 = xp*y01 - yp*x01 - x0*y01 + y0*x01;
//           d2 *= d2/(segment_long2);
//           ret = d2 > robotRoom2;
//           if(!ret)
// //             ret = (x0p*x0p + y0p*y0p > robotRoom2 + segment_long2) && (x1p*x1p + y1p*y1p > robotRoom2 + segment_long2); // para incluir puntos cerca del segmento pero fuera de las perpendiculares a sus extremos
//             ret = ((x0p*x0p + y0p*y0p > robotRoom2) && (x1p*x1p + y1p*y1p > robotRoom2 + segment_long2)) || ((x0p*x0p + y0p*y0p > robotRoom2 + segment_long2) && (x1p*x1p + y1p*y1p > robotRoom2));
// 
//             if(ret) {
//               QPointF translation = NDMath::rotateRad(deltaX, deltaY, 0, 0, deltaHeading)*10 + QPointF(robotRoom * 1.25, 0); // 10 steps.
//             
//               QPolygonF robotCorridor, intersection;
//               robotCorridor.append(QPointF(0, robotRoom * 1.25));
//               robotCorridor.append(QPointF(0, -robotRoom * 1.25));
//               robotCorridor.append(robotCorridor.at(1) + translation);
//               robotCorridor.append(robotCorridor.at(0) + translation);
// //               robotCorridor.append(QPointF(robotRoom * 3 , -robotRoom * 1.25));
// //               robotCorridor.append(QPointF(robotRoom * 3 , robotRoom * 1.25));
// 
// //               QPointF normal(-sin(deltaHeading), cos(deltaHeading));
// // //               double modulus = sqrt(normal.x()*normal.x() + normal.y()*normal.y());
// // //               if(modulus > 0)
// // //                 normal /= modulus;
// //               if(side != Left)
// //                 robotCorridor.append(normal * robotRoom*1.5);
// //               else
// //                 robotCorridor.append(QPointF(0,0));
// //               if(side != Right)
// //                 robotCorridor.append(-normal * robotRoom*1.5);
// //               else
// //                 robotCorridor.append(QPointF(0,0));
// //
// // //               robotRoom = robotRoom > modulus ? modulus : robotRoom;
// //               if(side != Right) // aqui Right antes que Left para no cruzar el rectangulo
// //                 robotCorridor.append(point/modulus*robotRoom * 2.0 - normal * robotRoom*1.5);
// //               else
// //                 robotCorridor.append(point/modulus*robotRoom * 2.0);
// //               if(side != Left)
// //                 robotCorridor.append(point/modulus*robotRoom * 2.0 + normal * robotRoom*1.5);
// //               else
// //                 robotCorridor.append(point/modulus*robotRoom * 2.0);
// 
// 
//               intersection = polygonGlobal.intersected(robotCorridor);
// //
// // //               qDebug() << "robotCorridor " << robotCorridor;
// // //               qDebug() << "intersection " << intersection;
// // //               for(int i = 0; i < 4; i++) {
// // //                 std::cout << "robotCorridor X " << robotCorridor.at(i).x() << std::endl;
// // //                 std::cout << "robotCorridor Y " << robotCorridor.at(i).y() << std::endl;
// // //                 if(intersection.size() > i) {
// // //                   std::cout << "intersection X " << intersection.at(i).x() << std::endl;
// // //                   std::cout << "intersection Y " << intersection.at(i).y() << std::endl << std::endl;
// // //                 }
// // //               }
// //
//               if(intersection.isEmpty())
//                 int j = 0;
//               else
//                 intersection.pop_back();
//               ret = (robotCorridor == intersection);
//             }
// 
//         }
//       }
//     }
//   }
// 
//   return ret;
// }

// bool AreaCenterPolygon::isAccesible(QPointF const& point, QPolygonF const & polygon, double robotRoom)
// {
//   QPolygonF shrunkPolygon;
//   bool ret = false;
//   int numVertex = polygon.size();
//   QPointF p(0, 0);
//   double d, cos_d, sin_d;
//   double minDistance = 100;  // leer de sensor saturationVal ?
//   robotRoom = robotRoom < 0 ? 0 : robotRoom;
// 
//   for(int i = 0; i < numVertex; i++) {
//     p = polygon.at(i)                                       ;
//     d = sqrt(p.x()*p.x() + p.y()*p.y());
//     cos_d = p.x()/d;
//     sin_d = p.y()/d;
//     d -= robotRoom;
//     d = d < minDistance ? minDistance : d;
//     p.setX(d*cos_d);
//     p.setY(d*sin_d);
//     shrunkPolygon.append(p);
//   }
//   ret = polygonContains(point, shrunkPolygon);
// //   if(ret) {
// //     QPolygonF robotCorridor, intersection;
// // 
// //     QPointF normal(-point.y(), point.x());
// //     double modulus = sqrt(normal.x()*normal.x() + normal.y()*normal.y());
// //     if(modulus > 0)
// //       normal /= modulus;
// //     robotCorridor.append(-normal*robotRoom/10);
// //     robotCorridor.append(normal*robotRoom/10);
// //     robotCorridor.append(point + normal*robotRoom/10);
// //     robotCorridor.append(point - normal*robotRoom/10);
// //     intersection = shrunkPolygon.intersected(robotCorridor);
// // 
// // //     qDebug() << "robotCorridor " << robotCorridor;
// // //     qDebug() << "intersection " << intersection;
// // //     for(int i = 0; i < 4; i++) {
// // //       std::cout << "robotCorridor X " << robotCorridor.at(i).x() << std::endl;
// // //       std::cout << "robotCorridor Y " << robotCorridor.at(i).y() << std::endl;
// // //       if(intersection.size() > i) {
// // //         std::cout << "intersection X " << intersection.at(i).x() << std::endl;
// // //         std::cout << "intersection Y " << intersection.at(i).y() << std::endl << std::endl;
// // //       }
// // //     }
// // 
// //     if(intersection.isEmpty())
// //       int j = 0;
// //     else
// //       intersection.pop_back();
// //     ret = (robotCorridor == intersection);
// //   }
//   return ret;
// }

QPointF AreaCenterPolygon::advanceAreaCenter(void)
{
  QPointF point;
  if(advanceStatus == Lost) {      
    lostAreaCenter.setX(lostAreaCenter.x() - deltaX);
    lostAreaCenter.setY(lostAreaCenter.y() - deltaY);
    NDMath::selfRotateRad(lostAreaCenter, deltaHeading);  // es -deltaHeading*world::headTransform() ?
    point = lostAreaCenter;
  }
  else {
    point = calculateAreaCenter(advancePolygon, QPointF(-2*sensorMaxRange, 0));
    if(!(point.x() || point.y())) {
      setLost();
      point = calculateAreaCenter(advancePolygon, QPointF(-2*sensorMaxRange, 0));
    }
  }
  return point;
}

void AreaCenterPolygon::setLost(void)
{
  if(advanceStatus == Lost) return;
  advanceStatus = Lost;
//   conservativeAdvancePolygon();
  setAdvanceMaxRange(sensorMaxRange/2);
  lostAreaCenter = calculateAreaCenter(inverseAdvancePolygon(), QPointF(-0.5*sensorMaxRange,0)); 
  suggestedSplit = false;
}

void AreaCenterPolygon::conservativeAdvancePolygon(void)
{
  setAdvanceMaxRange(sensorMaxRange/4);
  currentAdvanceAngles = advanceAngles/*/2*/;
//   polygonSplitPointRight.setX(2000*cos(currentAdvanceAngles.x())); // 1000 representa una distancia no muy lejana al robot
//   polygonSplitPointRight.setY(2000*sin(currentAdvanceAngles.x()));
//   polygonSplitPointLeft.setX(2000*cos(currentAdvanceAngles.y()));
//   polygonSplitPointLeft.setY(2000*sin(currentAdvanceAngles.y()));
  polygonSplitPointRight = initPolygonSplitPointRight;
  polygonSplitPointLeft = initPolygonSplitPointLeft;
}

QPolygonF AreaCenterPolygon::inverseAdvancePolygon(void) const
{
  QPolygonF inversePolygon, auxPoligon;
  int numVertex = size();
  
  for(int i = 0; i < numVertex; i++) {
    double vertexAngle = atan2(at(i).y(), at(i).x());
    if(((vertexAngle < 0) && (vertexAngle <= advanceAngles.x())) || ((vertexAngle  >= 0) && (vertexAngle >= advanceAngles.y()))) {
      QPointF point(at(i));
      double ratio = advanceMaxRange / sqrt(point.x()*point.x() + point.y()*point.y());
      ratio = ratio > 1.0 ? 1.0 : ratio;
      point.setX(point.x() * ratio);
      point.setY(point.y() * ratio);
      inversePolygon.append(point);
    }
    else if(((vertexAngle < 0) && (vertexAngle <= advanceAngles.x())) || ((vertexAngle  >= 0) && (vertexAngle >= advanceAngles.y()))) {
      QPointF point(at(i));
      double ratio = advanceMaxRange / sqrt(point.x()*point.x() + point.y()*point.y());
      ratio = ratio > 1.0 ? 1.0 : ratio;
      point.setX(point.x() * ratio);
      point.setY(point.y() * ratio);
      auxPoligon.append(point);
    }
        
//     numVertex = auxPoligon.size();
//     for(int i = 0; i < numVertex; i++) {
//       inversePolygon.append(auxPoligon.at(numVertex - 1 - i));
//     }
    
  }

  numVertex = auxPoligon.size();
  for(int i = 0; i < numVertex; i++) {
    inversePolygon.append(auxPoligon.at(numVertex - 1 - i));
  }
  return inversePolygon;
}

void AreaCenterPolygon::reset(void)
{
  polygonSplitPointRight = initPolygonSplitPointRight;
  polygonSplitPointLeft = initPolygonSplitPointLeft;
  advanceStatus = All;
//   setAdvanceMaxRange(sensorMaxRange);
}

void AreaCenterPolygon::resetLost(void)
{ 
//   polygonSplitPointRight.setX(1000*cos(advanceAngles.x()/2));
//   polygonSplitPointRight.setY(1000*sin(advanceAngles.x()/2));
//   polygonSplitPointLeft.setX(1000*cos(advanceAngles.y()/2));
//   polygonSplitPointLeft.setY(1000*sin(advanceAngles.y()/2));
  polygonSplitPointRight = initPolygonSplitPointRight;
  polygonSplitPointLeft = initPolygonSplitPointLeft;
  
  advanceStatus = Conservative;
//   setAdvanceMaxRange(sensorMaxRange/2);
}

void AreaCenterPolygon::setAdvanceMaxRange(double advanceMaxRange)
{
  advanceMaxRange = advanceMaxRange < robotDiameter ? robotDiameter : advanceMaxRange;
  advanceMaxRange = advanceMaxRange > sensorMaxRange ? sensorMaxRange : advanceMaxRange;
  this->advanceMaxRange = advanceMaxRange;
}

void AreaCenterPolygon::setSensorMaxRange(double sensorMaxRange)
{
  this->sensorMaxRange = sensorMaxRange;
  setAdvanceMaxRange(sensorMaxRange);
}

double AreaCenterPolygon::minimunDistance(QPolygonF const & poly)
{
  int sz = poly.size();
  double min = sensorMaxRange*sensorMaxRange, d;
  for(int i = 0; i < sz; i++) {
    d = poly[i].x()*poly[i].x() + poly[i].y()*poly[i].y();
    if(d < min)
      min = d;
  }
  return sqrt(min);
}
