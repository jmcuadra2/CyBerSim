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
#ifndef SINGULARPOINTCOLLISIONSOLVER_H
#define SINGULARPOINTCOLLISIONSOLVER_H

#include "collisionsolver.h"

/**
@author jose manuel
*/
class SingularPointCollisionSolver : public CollisionSolver
{

public:
    SingularPointCollisionSolver();
    ~SingularPointCollisionSolver();
    
    void setParameters(const QDomElement& params);
    void setParameters(double x, double y, double start_angle, double end_angle);
    void setRobotRadius(const double& rad);
    double getCollisionValue(double x_p, double y_p, double vx_p, double vy_p);
    double getCollisionAngle(const double& , const double& );
    double getLineIntersectionValue(const double& /*line_x_start*/,
                                    const double& /*line_y_start*/,
                                    const double& /*line_x_end*/,
                                    const double& /*line_y_end*/) { return -1000.0 ; }

private:
    double x, y, start_angle, end_angle;
};

#endif
