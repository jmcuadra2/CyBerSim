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
#include "ellipsecollisionsolver.h"

EllipseCollisionSolver::EllipseCollisionSolver()
 : CollisionSolver()
{
  x_center = 0;
  y_center = 0;
  x_radius = 0;
  y_radius = 0;
  angle = 0;
  
  a2 = 0;
  b2 = 0;
  a2b = 0;
}


EllipseCollisionSolver::~EllipseCollisionSolver()
{
}

void EllipseCollisionSolver::setParameters(const QDomElement& params)
{

  bool ok = false;
  x_center = params.attribute("center_x").toDouble(&ok);
  y_center = params.attribute("center_y").toDouble(&ok);
  x_radius = params.attribute("radius_x").toDouble(&ok);
  y_radius = params.attribute("radius_y").toDouble(&ok);
  angle = params.attribute("angle").toDouble(&ok);
  
  paramsPoints.append(QPointF(x_center, y_center));
  paramsPoints.append(QPointF(x_radius, y_radius));
  paramsPoints.append(QPointF(angle, 0));
  

  x_radius2 = x_radius*x_radius;
  y_radius2 = y_radius*y_radius;
  
  a2 = cos(angle)*cos(angle)/x_radius2 + sin(angle)*sin(angle)/y_radius2;
  b2 = sin(angle)*sin(angle)/x_radius2 + cos(angle)*cos(angle)/y_radius2;
  a2b = 2*(-sin(angle)*cos(angle)/x_radius2 + sin(angle)*cos(angle)/y_radius2);
}

void EllipseCollisionSolver::setRobotRadius(const double& rad) 
{ 

  robot_rad = rad;
  ext_x_radius = x_radius + robot_rad;
  ext_y_radius = y_radius + robot_rad;  
  ext_x_radius2 = ext_x_radius*ext_x_radius;
  ext_y_radius2 = ext_y_radius*ext_y_radius;
  a2_coll = cos(angle)*cos(angle)/ext_x_radius2+sin(angle)*sin(angle)/ext_y_radius2;
  b2_coll = sin(angle)*sin(angle)/ext_x_radius2+cos(angle)*cos(angle)/ext_y_radius2;
  a2b_coll = 2*(-sin(angle)*cos(angle)/ext_x_radius2 + sin(angle)*cos(angle)/ext_y_radius2);  
  
}  

double EllipseCollisionSolver::getCollisionValue(double x_p, double y_p, double vx_p, double vy_p)
{
  double ret = -1000.0;  // no collision
  double x_robot = x_p - x_center;
  double y_robot = y_p - y_center;
  
  double dx = 2*a2_coll*x_robot - a2b_coll*y_robot;
  double dy = 2*b2_coll*y_robot - a2b_coll*x_robot;
  if((vx_p*dx + vy_p*dy) > 0)
    return ret; // saliendo del objeto     
  
  double t1, t2, d, d1;
  NDMath::selfRotateRad(x_robot, y_robot, 0, 0, -angle); 
  NDMath::selfRotateRad(vx_p, vy_p, 0, 0, -angle);
  d1 = vx_p*vx_p*ext_y_radius2 + vy_p*vy_p*ext_x_radius2;
  if(d1) {
    d = d1 - (vx_p*y_robot - vy_p*x_robot)*(vx_p*y_robot - vy_p*x_robot);
    if(d < 0)
      return ret;
    double ttt = sqrt(d)*ext_x_radius * ext_y_radius; 
    double sss = vx_p*x_robot*ext_y_radius2 + vy_p*y_robot*ext_x_radius2;  
    t1 = -(sss + ttt)/d1;
    t2 = -(sss - ttt)/d1;
    if(t1 <= 1.0 && t1 >= -1.0) {
      if(t2 < t1 && t2 >= -1.0) 
        ret = t2;
      else
        ret = t1;
    }
    else if(t2 <= 1.0 && t2 >= -1.0) 
      ret = t2;
  }
  return ret;
    
}

double EllipseCollisionSolver::getCollisionAngle(const double& x_p, const double& y_p)
{

  double dx, dy;
  dx = 2*a2_coll*(x_p - x_center) - a2b_coll*(y_p - y_center);
  dy = -(2*b2_coll*(y_p - y_center) - a2b_coll*(x_p - x_center)); // giro vector normal 90 gr y obtengo tangente
  return atan2(dx, dy);  
  
}

double EllipseCollisionSolver::getLineIntersectionValue(const double& line_x_start, const double& line_y_start, const double& line_x_end, const double& line_y_end)
{
  double ret = -1000.0;  // no intersection
  double vx_p = line_x_end - line_x_start;
  double vy_p = line_y_end - line_y_start; 

  double x_end = line_x_start - x_center;
  double y_end = line_y_start - y_center;
  
  double dx = 2*a2*x_end - a2b*y_end;
  double dy = 2*b2*y_end - a2b*x_end;

  double denom = vx_p*dy - vy_p*dx;  
  if((vx_p*dx + vy_p*dy) > 0)
    return ret; // saliendo del objeto
 
  if(denom != 0.0) {

    double t1, t2, d, d1;
    NDMath::selfRotateRad(x_end, y_end, 0, 0, -angle);
    NDMath::selfRotateRad(vx_p, vy_p, 0, 0, -angle);
    d1 = vx_p*vx_p*y_radius2 + vy_p*vy_p*x_radius2;
    if(d1) {
      d = d1 - (vx_p*y_end - vy_p*x_end)*(vx_p*y_end - vy_p*x_end);
      if(d < 0)
        return ret;
      double ttt = sqrt(d)*x_radius * y_radius;
      double sss = vx_p*x_end*y_radius2 + vy_p*y_end*x_radius2;
      t1 = -(sss + ttt)/d1;
      t2 = -(sss - ttt)/d1;
      if(t1 <= 1.0 && t1 >= -1.0) {
        if(t2 < t1 && t2 >= -1.0) 
          ret = t2;
        else
          ret = t1;
      }
      else if(t2 <= 1.0 && t2 >= -1.0) 
        ret = t2;
    }
  }

  if(ret <= 1.0 && ret >= -1.0) {
    intersection_point.setX(line_x_start + (line_x_end - line_x_start)*ret);
    intersection_point.setY(line_y_start + (line_y_end - line_y_start)*ret);

    double ax, ay;
    ax = 2*a2*(intersection_point.x() - x_center) - a2b*(intersection_point.y() - y_center);
    ay = 2*b2*(intersection_point.y() - y_center) - a2b*(intersection_point.x() - x_center); // vector normal

    double lx = line_x_start-intersection_point.x();
    double ly = line_y_start-intersection_point.y();
    incidence_angle =  acos((ax*lx + ay*ly) / sqrt((ax*ax + ay*ay)*(lx*lx+ly*ly)));

  }

  return ret;
     
}