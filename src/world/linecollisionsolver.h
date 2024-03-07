//
// C++ Interface: linecollisionsolver
//
// Description: 
//
//
// Author: Jose Manuel Cuadra Troncoso <jmcuadra@dia.uned.es>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef LINECOLLISIONSOLVER_H
#define LINECOLLISIONSOLVER_H

#include "polygoncollisionsolver.h"

/**
  @author Jose Manuel Cuadra Troncoso <jmcuadra@dia.uned.es>
*/
class LineCollisionSolver : public PolygonCollisionSolver
{
  public:
    LineCollisionSolver();

    ~LineCollisionSolver();

     double getLineIntersectionValue(const double& line_x_start, const double& line_y_start, const double& line_x_end, const double& line_y_end);

};

#endif
