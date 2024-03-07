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
#ifndef SEGMENTCOLLISIONSOLVER_H
#define SEGMENTCOLLISIONSOLVER_H

#include "collisionsolver.h"

/**
@author jose manuel
*/
class SegmentCollisionSolver : public CollisionSolver
{

  public:
    SegmentCollisionSolver();
    ~SegmentCollisionSolver();
    
    void setParameters(const QDomElement& params);
    void setParameters(double x_start, double y_start, double x_end, double y_end);
    void setRobotRadius(const double& rad);
    double getCollisionValue(double , double , double , double );
    double getCollisionAngle(const double& , const double& ) ; 
    double getLineIntersectionValue(const double& line_x_start, const double& line_y_start, const double& line_x_end, const double& line_y_end);
        
  protected:
    void setParameters(void);

  private:
    double x_start, y_start, x_end, y_end, x_obj_start, y_obj_start, x_obj_end, y_obj_end; 
    QPointF normal_vector; 
    double dx, dy;  
};

#endif
