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
#include "segmentcollisionsolver.h"

SegmentCollisionSolver::SegmentCollisionSolver()
 : CollisionSolver()
{
  dx = 0;
  dy = 0;
}


SegmentCollisionSolver::~SegmentCollisionSolver()
{
}

void SegmentCollisionSolver::setParameters(const QDomElement& params)
{

  bool ok = false;
  x_obj_start = params.attribute("x_start").toDouble(&ok);
  y_obj_start = params.attribute("y_start").toDouble(&ok);
  x_obj_end = params.attribute("x_end").toDouble(&ok);
  y_obj_end = params.attribute("y_end").toDouble(&ok);
  setParameters();

}

void SegmentCollisionSolver::setParameters(double x_start, double y_start, double x_end, double y_end)
{
  x_obj_start = x_start;
  y_obj_start = y_start;
  x_obj_end = x_end;
  y_obj_end = y_end;
  setParameters();
//   setRobotRadius(robot_rad);
}

void SegmentCollisionSolver::setParameters(void)
{
  double x_norm, y_norm, mod;
  x_norm = x_obj_end - x_obj_start;
  y_norm = y_obj_end - y_obj_start;
  mod = sqrt(x_norm*x_norm + y_norm*y_norm);
  mod = mod == 0.0 ? 1 : mod;
  normal_vector.setX(y_norm/mod);
  normal_vector.setY(-x_norm/mod);
  
    paramsPoints.append(QPointF(x_obj_start, y_obj_start));
    paramsPoints.append(QPointF(x_obj_end, y_obj_end));
}

void SegmentCollisionSolver::setRobotRadius(const double& rad) 
{ 

  robot_rad = rad;
  x_start = x_obj_start + robot_rad*normal_vector.x();
  y_start = y_obj_start + robot_rad*normal_vector.y();
  x_end = x_obj_end + robot_rad*normal_vector.x();
  y_end = y_obj_end + robot_rad*normal_vector.y();
  
  dx = x_obj_end - x_obj_start;
  dy = y_obj_end - y_obj_start;  
   
}

double SegmentCollisionSolver::getCollisionValue(double x_p, double y_p, double vx_p, double vy_p)
{

  double ret = -1000.0;  // no collision
  if((vx_p*normal_vector.x()) + (vy_p*normal_vector.y()) > 0)
    return ret; // saliendo del objeto

  double denom = vx_p*dy - vy_p*dx;
  if(denom != 0.0) {
    double s_param, rob_param;
    s_param = (vx_p*(y_p - y_start) - vy_p*(x_p - x_start))/denom;
    if(s_param >= 0.0 && s_param <= 1.0) {
      rob_param = -(x_p*dy - y_p*dx -x_start*y_end + x_end*y_start)/denom;
/*    if(rob_param >= 0.0 && rob_param <= 1.0)
      ret = rob_param;*/      
    if(rob_param >= -1.0 && rob_param <= 1.0)
      ret = rob_param;
    }
  }
  return ret;
  
}

double SegmentCollisionSolver::getCollisionAngle(const double& , const double& )
{
  
  return atan2(normal_vector.x(), -normal_vector.y()); 

}

double SegmentCollisionSolver::getLineIntersectionValue(const double& line_x_start, const double& line_y_start, const double& line_x_end, const double& line_y_end)
{
  double ret = -1000.0;  // no intersection
  double vx_p = line_x_end - line_x_start;
  double vy_p = line_y_end - line_y_start; 
  double denom = vx_p*dy - vy_p*dx;
  
  if(fabs(denom) > 1.0e-10) {
  double s_param, line_param;
    s_param = (vx_p*(line_y_start - y_obj_start) - vy_p*(line_x_start - x_obj_start))/denom;
    if(s_param >= 0.0 && s_param <= 1.0) {
      line_param = -(line_x_start*dy - line_y_start*dx -x_obj_start*y_obj_end + x_obj_end*y_obj_start)/denom;
/*    if(rob_param >= 0.0 && rob_param <= 1.0)
      ret = rob_param;*/      
      if(line_param >= 0 && line_param <= 1.0) {
        ret = line_param;
        intersection_point.setX(line_x_start + (line_x_end - line_x_start)*ret);
        intersection_point.setY(line_y_start + (line_y_end - line_y_start)*ret);
        incidence_angle = M_PI/2 - acos(sqrt((vx_p*dx + vy_p*dy)*(vx_p*dx + vy_p*dy)/(vx_p*vx_p + vy_p*vy_p)/(dx*dx + dy*dy)));       
      }
    }
  }
  return ret;   
}

