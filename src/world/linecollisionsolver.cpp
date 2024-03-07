//
// C++ Implementation: linecollisionsolver
//
// Description: 
//
//
// Author: Jose Manuel Cuadra Troncoso <jmcuadra@dia.uned.es>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "linecollisionsolver.h"

LineCollisionSolver::LineCollisionSolver(): PolygonCollisionSolver()
{
}

LineCollisionSolver::~LineCollisionSolver()
{
}

double LineCollisionSolver::getLineIntersectionValue(const double& line_x_start, const double& line_y_start, const double& line_x_end, const double& line_y_end)
{
  double ret = -1000.0;  // no intersection
  double val = -1000.0;
  solver = 0;  
  val = first_segment_solver->getLineIntersectionValue(line_x_start, line_y_start, line_x_end, line_y_end);    
  if(val >= 0 && val <= 1) {
    solver = first_segment_solver;
    intersection_point = solver->getIntersectionPoint();   
    incidence_angle = solver->getIncidenceAngle();        
    ret = val; 
  }
  return ret;
}

