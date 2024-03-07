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
#include "singularpointcollisionsolver.h"

SingularPointCollisionSolver::SingularPointCollisionSolver()
    : CollisionSolver()
{
}


SingularPointCollisionSolver::~SingularPointCollisionSolver()
{
}

void SingularPointCollisionSolver::setParameters(const QDomElement& params)
{

    bool ok = false;
    x = params.attribute("x").toDouble(&ok);
    y = params.attribute("y").toDouble(&ok);
    start_angle = params.attribute("start_angle").toDouble(&ok);
    end_angle = params.attribute("end_angle").toDouble(&ok);

    paramsPoints.append(QPointF(x, y));
    paramsPoints.append(QPointF(start_angle, end_angle));
}

void SingularPointCollisionSolver::setParameters(double x, double y, double start_angle, double end_angle)
{
    this->x = x;
    this->y = y;
    this->start_angle = start_angle;
    this->end_angle = end_angle;
}

void SingularPointCollisionSolver::setRobotRadius(const double& rad) 
{    
    robot_rad = rad;
}

double SingularPointCollisionSolver::getCollisionValue(double x_p, double y_p,
                                                       double vx_p, double vy_p)
{

    double ret = -1000.0;  // no collision
    double x_robot = x_p - x;
    double y_robot = y_p - y;
    if((vx_p*x_robot + vy_p*y_robot) > 0)
        return ret; // saliendo del objeto

    double angle = atan2(y_robot, x_robot);
    if(start_angle <= end_angle) {
        if(angle < start_angle || angle > end_angle )
            return ret;
    }
    else {
        if((0 <= angle && angle < start_angle) || (0 > angle && angle > end_angle))
            return ret;
    }
    double d1 = vx_p*vx_p + vy_p*vy_p;
    if(d1) {
        double d = robot_rad*robot_rad*d1 - (x_robot*vy_p - y_robot*vx_p)*(x_robot*vy_p - y_robot*vx_p);
        if(d >= 0.0) {
            double s = x_robot*vx_p + y_robot*vy_p;
            double t1 = (sqrt(d) - s)/d1;
            double t2 = (-sqrt(d) - s)/d1;
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
    return ret;
}

double SingularPointCollisionSolver::getCollisionAngle(const double& x_p, const double& y_p)
{   
    return atan2(x_p - x, y - y_p);

}
