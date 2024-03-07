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
#ifndef ELLIPSECOLLISIONSOLVER_H
#define ELLIPSECOLLISIONSOLVER_H

#include "collisionsolver.h"

/**
@author jose manuel
*/

class EllipseCollisionSolver : public CollisionSolver
{
public:
    EllipseCollisionSolver();
    ~EllipseCollisionSolver();
    
    double getCollisionValue(double x_p, double y_p, double vx_p, double vy_p);
    double getCollisionAngle(const double& x_p, const double& y_p);
    void setParameters(const QDomElement& params);
    void setRobotRadius(const double& rad);
    double getLineIntersectionValue(const double& line_x_start, const double& line_y_start, const double& line_x_end, const double& line_y_end);

private:
    double a2_coll, b2_coll, a2b_coll;
    double a2, b2, a2b;
    double x_center, y_center, x_radius, y_radius, x_radius2, y_radius2, angle;
    double ext_x_radius2, ext_y_radius2, ext_x_radius, ext_y_radius;
};

#endif
