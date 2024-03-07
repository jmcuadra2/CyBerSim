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
#ifndef POLYGONCOLLISIONSOLVER_H
#define POLYGONCOLLISIONSOLVER_H

#include "collisionsolver.h"
#include "calculadorbresenham.h"

/**
@author jose manuel
*/
class PolygonCollisionSolver : public CollisionSolver
{
public:
    PolygonCollisionSolver();
    ~PolygonCollisionSolver();
    
    void setParameters(const QDomElement& params);
    void setRobotRadius(const double& rad);
    double getCollisionValue(double x_p, double y_p, double vx_p, double vy_p);
    double getCollisionAngle(const double& x_p, const double& y_p);
    double getLineIntersectionValue(const double& line_x_start, const double& line_y_start, const double& line_x_end, const double& line_y_end);
    
    void addMaterial(Material* material);
    void removeMaterials(bool del);
};

#endif
