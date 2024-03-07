//
// C++ Implementation: areacenterpolygonsegmented
//
// Description:
//
//
// Author: Jose Manuel Cuadra Troncoso <jmcuadra@dia.uned.es>, (C) 2011
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "areacenterpolygonsegmented.h"
#include <iostream>

AreaCenterPolygonSegmented::AreaCenterPolygonSegmented(): AreaCenterPolygon()
{
}


AreaCenterPolygonSegmented::AreaCenterPolygonSegmented(QPointF advanceAngles): AreaCenterPolygon(advanceAngles)
{
}


AreaCenterPolygonSegmented::~AreaCenterPolygonSegmented()
{
}

bool AreaCenterPolygonSegmented::addSplitPoint(QPointF const& areapoint, bool suggested, AdvanceStatus preference)
{  
  int end_i = endPoints.size();

  int offset = suggested ? 0 : 20;
  if(!end_i)
    return AreaCenterPolygon::addSplitPoint(areapoint, suggested, preference);
  
  QPointF newAreapoint = areapoint;
  double central_angle = (currentAdvanceAngles.x() + currentAdvanceAngles.y())/2;  
  double d, r, x, y, min_ang, ang, x1, y1;
  double min_d = 1e12, min_x, min_y;
  int min_i = end_i;
  min_x = newAreapoint.x();
  min_y = newAreapoint.y();
  double angleAreaPoint = atan2(min_y, min_x);

  for(int i = 0; i < end_i; i++) {
    ang = (endPoints[i].first - 90)/NDMath::RAD2GRAD;
    r = abs(rhoEst[i]/cos(ang - phiEst[i]));
    x = r * cos(ang);
    y = r * sin(ang);
    x1 = x - newAreapoint.x();
    y1 = y - newAreapoint.y();
    d = x1 * x1 + y1 * y1;

    if(d < min_d) {
      min_d = d;
      min_i = i;
      min_ang = ang;
    }

    ang = (endPoints[i].second - 90)/NDMath::RAD2GRAD;
    r = abs(rhoEst[i]/cos(ang - phiEst[i]));
    x = r * cos(ang);
    y = r * sin(ang);
    x1 = x - newAreapoint.x();
    y1 = y - newAreapoint.y();
    d = x1 * x1 + y1 * y1;

    if(d < min_d) {
      min_d = d;
      min_i = i;
      min_ang = ang;
    }  
  }

  if(suggested && min_d > 2*robotDiameter) {
    r = sqrt(newAreapoint.x()*newAreapoint.x() + newAreapoint.y()*newAreapoint.y());
    if(preference == Right) 
      angleAreaPoint = angleAreaPoint > -15/NDMath::RAD2GRAD ? angleAreaPoint : -15/NDMath::RAD2GRAD;
    if(preference == Left)
      angleAreaPoint = angleAreaPoint < 15/NDMath::RAD2GRAD ? angleAreaPoint : 15/NDMath::RAD2GRAD;
    newAreapoint.setX(r*cos(angleAreaPoint));
    newAreapoint.setY(r*sin(angleAreaPoint));
    return AreaCenterPolygon::addSplitPoint(newAreapoint, suggested, preference);
  }
  
  min_d = 1e12;
  int i = min_i;
  int start_i = min_i;
/*  if(min_ang <= central_angle) {*/
  if(angleAreaPoint <= central_angle) {
    while(i < end_i) {

//         if(i == start_i) {
      ang = (endPoints[i].first - 90)/NDMath::RAD2GRAD;
      r = abs(rhoEst[i]/cos(ang - phiEst[i]));

      if(r < min_d) {
        min_d = r;
        min_i = i;
        min_ang = ang;
      }
//         }
      if(endPoints[i].first > round(central_angle*NDMath::RAD2GRAD) + 90 + offset/*90*/) {
        break;
      }
      if(endPoints[i].second > round(central_angle*NDMath::RAD2GRAD) + 90 + offset/*90*/) {
        if(endPoints[i].second > round(currentAdvanceAngles.y()*NDMath::RAD2GRAD) + 90 || endPoints[i].second > round(central_angle*NDMath::RAD2GRAD) + 90 + 2*offset/*110*/)
          break;
      }
      
      ang = (endPoints[i].second - 90)/NDMath::RAD2GRAD;
      r = abs(rhoEst[i]/cos(ang - phiEst[i]));

      if(r < min_d) {
        min_d = r;
        min_i = i;
        min_ang = ang;
      }
      i++;
    }

  }
  else {
    while(i >= 0) {


//         if(i == start_i) {
      ang = (endPoints[i].second - 90)/NDMath::RAD2GRAD;
      r = abs(rhoEst[i]/cos(ang - phiEst[i]));

      if(r < min_d) {
        min_d = r;
        min_i = i;
        min_ang = ang;
      }
//         }
      if(endPoints[i].second < round(central_angle*NDMath::RAD2GRAD) + 90 - offset/*90*/) {
        break;
      }
      
      if(endPoints[i].first < round(central_angle*NDMath::RAD2GRAD) + 90 - offset/*90*/) {
        if(endPoints[i].first < round(currentAdvanceAngles.x()*NDMath::RAD2GRAD) + 90 || endPoints[i].first < round(central_angle*NDMath::RAD2GRAD) + 90 - 2*offset/*70*/)
          break;
      }
      ang = (endPoints[i].first - 90)/NDMath::RAD2GRAD;
      r = abs(rhoEst[i]/cos(ang - phiEst[i]));

      if(r < min_d) {
        min_d = r;
        min_i = i;
        min_ang = ang;
      }
      
      i--;
    }
  }

//   r = abs(rhoEst[min_i]/cos(min_ang - phiEst[min_i])) - robotDiameter/2;
//   r = r < robotDiameter/2 ? robotDiameter/2 : r;

  r = abs(rhoEst[min_i]/cos(min_ang - phiEst[min_i]));
  min_x = r * cos(min_ang) - abs(robotDiameter/2*cos(M_PI/2 - phiEst[min_i]));
  min_y = r * sin(min_ang);
  min_y = min_y < 0 ? min_y + robotDiameter/2*sin(M_PI/2 - phiEst[min_i]) : min_y - robotDiameter/2*sin(M_PI/2 - phiEst[min_i]);

  newAreapoint.setX(min_x);
  newAreapoint.setY(min_y);

  angleAreaPoint = atan2(newAreapoint.y(), newAreapoint.x());
  if((angleAreaPoint < currentAdvanceAngles.x()) || (angleAreaPoint > currentAdvanceAngles.y())) {
    std::cout << "Warning: AreaCenterPolygon::addSplitPoint() areapoint out of angle (1)" << std::endl;
    newAreapoint = areapoint;
  }

  bool ret = AreaCenterPolygon::addSplitPoint(newAreapoint, suggested, preference);
  return ret;
}

bool AreaCenterPolygonSegmented::isAdvanceAccesible(QPointF const& areaCenter, double robotRoom, AdvanceStatus side)
{
  int i = 0;
  int end_i = endPoints.size();
  double min_d = 1e12;
  double min_right, min_left, ang, r;
  double central_angle = (currentAdvanceAngles.x() + currentAdvanceAngles.y())/2;
  bool toParent = true;
  double startAngle;
  
  if(initPolygonSplitPointRight != polygonSplitPointRight) {
    startAngle = round(atan2(polygonSplitPointRight.y(), polygonSplitPointRight.x())*NDMath::RAD2GRAD) + 90;
    i = 0;
    while(i < end_i) {
      if(endPoints[i].first < startAngle) {
        i++;
        continue;
      }
      if((endPoints[i].first - 90)/NDMath::RAD2GRAD <= central_angle) {
        ang = (endPoints[i].first - 90)/NDMath::RAD2GRAD;
        r = abs(rhoEst[i]/cos(ang - phiEst[i]));
        if(r < min_d)
          min_d = r;
      }
      else
        break;

      if(endPoints[i].second < startAngle) {
        i++;
        continue;
      }        
        
      if((endPoints[i].second - 90)/NDMath::RAD2GRAD < currentAdvanceAngles.y() && (endPoints[i].second - 90)/NDMath::RAD2GRAD <= central_angle) {
        ang = (endPoints[i].second - 90)/NDMath::RAD2GRAD;
        r = abs(rhoEst[i]/cos(ang - phiEst[i]));
        if(r < min_d) 
          min_d = r;
      }
      else
        break;
      i++;
    }
    min_right = min_d;
  
    if(min_right < sqrt(polygonSplitPointRight.x()*polygonSplitPointRight.x() + polygonSplitPointRight.y()*polygonSplitPointRight.y()) -100)
      toParent =  false;
  }

  min_d = 1e12;
  
  if(initPolygonSplitPointLeft != polygonSplitPointLeft) {
    i = end_i - 1;
    startAngle = round(atan2(polygonSplitPointLeft.y(), polygonSplitPointLeft.x())*NDMath::RAD2GRAD) + 90;
    while(i >= 0) {
      if(endPoints[i].second > startAngle) {
        i--;
        continue;
      }
    
      if((endPoints[i].second - 90)/NDMath::RAD2GRAD >= central_angle) {
        ang = (endPoints[i].second - 90)/NDMath::RAD2GRAD;
        r = abs(rhoEst[i]/cos(ang - phiEst[i]));
        if(r < min_d)
          min_d = r;
      }
      else
        break;

      if(endPoints[i].first > startAngle) {
        i--;
        continue;
      }         
      if((endPoints[i].first - 90)/NDMath::RAD2GRAD > currentAdvanceAngles.x() && (endPoints[i].first - 90)/NDMath::RAD2GRAD >= central_angle) {
        ang = (endPoints[i].first - 90)/NDMath::RAD2GRAD;
        r = abs(rhoEst[i]/cos(ang - phiEst[i]));
        if(r < min_d)
          min_d = r;
      }
      else
        break;
      i--;
    }

    min_left = min_d;
  
    if(min_left < sqrt(polygonSplitPointLeft.x()*polygonSplitPointLeft.x() + polygonSplitPointLeft.y()*polygonSplitPointLeft.y()) - 100)
      toParent = false;
  }
  if(!toParent)
    return toParent;
  else
    return AreaCenterPolygon::isAdvanceAccesible(areaCenter, robotRoom, side);
}

// bool AreaCenterPolygonSegmented::addSplitPoint(QPointF const& areapoint, bool suggested, AdvanceStatus preference)
// {
//   QPointF newAreapoint = areapoint;
//   int end_i = endPoints.size();
//   double d;
//   double r, r1, x, y, ang, x1, y1;
//   double min_d = 1000000, min_x, min_y;
//   int i1, i2;
//   int min_i = end_i;
//   for(int i = 0; i < end_i; i++) {
//     ang = (endPoints[i].first - 90)/NDMath::RAD2GRAD;
//     r = abs(rhoEst[i]/cos(ang - phiEst[i]));
//     x = r * cos(ang);
//     y = r * sin(ang);
//     x1 = x - newAreapoint.x();
//     y1 = y - newAreapoint.y();
//     d = sqrt(x1 * x1 + y1 * y1);
// 
//     if(d < min_d) {
//       min_d = d;
//       min_i = i;
//     }
// 
//     ang = (endPoints[i].second - 90)/NDMath::RAD2GRAD;
//     r = abs(rhoEst[i]/cos(ang - phiEst[i]));
//     x = r * cos(ang);
//     y = r * sin(ang);
//     x1 = x - newAreapoint.x();
//     y1 = y - newAreapoint.y();
//     d = sqrt(x1 * x1 + y1 * y1);
// 
//     if(d < min_d) {
//       min_d = d;
//       min_i = i;
//     }  
//   }
// 
//   for(int i = 0; i < connectedAreas.size(); i++) {
//     if(min_i >= connectedAreas[i].first && min_i <= connectedAreas[i].second) {
//       min_i = i;
//       break;
//     }
//   }
// 
//   i1 = connectedAreas[min_i].first;
//   i2 = connectedAreas[min_i].second;
// //   min_d = 1000000;
// // 
// //   for(int i = i1; i <= i2; i++) {
// //     ang = (endPoints[i].first - 90)/NDMath::RAD2GRAD;
// //     r = abs(rhoEst[i]/cos(ang - phiEst[i]));
// //     if(r < min_d) {
// //       min_d = r;
// //       min_x = r * cos(ang);
// //       min_y = r * sin(ang);
// //     }
// //     ang = (endPoints[i].second - 90)/NDMath::RAD2GRAD;
// //     r = abs(rhoEst[i]/cos(ang - phiEst[i]));
// //     if(r < min_d) {
// //       min_d = r;
// //       min_x = r * cos(ang);
// //       min_y = r * sin(ang);
// //     }
// //   }
// 
//   int min_ang = 1000000;
//   min_i = 1000000;
// 
//   for(int i = i1; i <= i2; i++) {
//     if(abs(min_ang) > abs(endPoints[i].first - 90)) {
//       min_ang = endPoints[i].first - 90;
//       min_i = i;
//     }
//     if(abs(min_ang) > abs(endPoints[i].second - 90)) {
//       min_ang = endPoints[i].second - 90;
//       min_i = i;
//     }    
//   }
// 
//   ang = min_ang/NDMath::RAD2GRAD;
//   r = abs(rhoEst[min_i]/cos(min_ang - phiEst[min_i]));
//   min_x = r * cos(ang);
//   min_y = r * sin(ang);
// 
//   newAreapoint.setX(min_x);
//   newAreapoint.setY(min_y);
// 
// 
//   return AreaCenterPolygon::addSplitPoint(newAreapoint, suggested, preference);
// }

