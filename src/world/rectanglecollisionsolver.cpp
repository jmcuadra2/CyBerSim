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

#include "rectanglecollisionsolver.h"
#include "segmentcollisionsolver.h"
#include "singularpointcollisionsolver.h"
#include <QPolygonF>

RectangleCollisionSolver::RectangleCollisionSolver()
    : CollisionSolver()
{

}

RectangleCollisionSolver::~RectangleCollisionSolver()
{

    for(int i = 0; i < segments_vertices.size(); i++){
        delete segments_vertices[i];
    }
    
}

void RectangleCollisionSolver::setParameters(const QDomElement& params)
{

    bool ok = false;
    CollisionSolver* coll_solver = 0;
    QPolygonF points;
    double x = params.attribute("x").toDouble(&ok);
    double y = params.attribute("y").toDouble(&ok);
    int width = params.attribute("width").toInt(&ok);
    int height = params.attribute("height").toInt(&ok);
    points.append(QPointF(x, y));
    points.append(QPointF(x + width, y));
    points.append(QPointF(x + width, y + height));
    points.append(QPointF(x, y + height));
    QDomDocument doc_collision("collision");
    QDomElement collision_params = doc_collision.createElement("parameters");
    int num_points = points.count();
    for(int i = 0; i < num_points; i++) {
        collision_params.setAttribute("x_start", points.at(i%num_points).x());
        collision_params.setAttribute("y_start", points.at(i%num_points).y());
        collision_params.setAttribute("x_end", points.at((i + 1)%num_points).x());
        collision_params.setAttribute("y_end", points.at((i + 1)%num_points).y());
        coll_solver = new SegmentCollisionSolver();
        segments_vertices.append(coll_solver);

        coll_solver->setParameters(collision_params);
        collision_params.setAttribute("x", points.at(i%num_points).x());
        collision_params.setAttribute("y", points.at(i%num_points).y());
        collision_params.setAttribute("start_angle", ((i - 2)%num_points)*M_PI/2);
        collision_params.setAttribute("end_angle", ((i - 1)%num_points)*M_PI/2);
        coll_solver = new SingularPointCollisionSolver();
        segments_vertices.append(coll_solver);

        coll_solver->setParameters(collision_params);
    }

}  

void RectangleCollisionSolver::setRobotRadius(const double& rad) 
{ 

    robot_rad = rad;
    QListIterator<CollisionSolver*> it(segments_vertices);
    while(it.hasNext()){
        it.next()->setRobotRadius(robot_rad);
    }

}

double RectangleCollisionSolver::getCollisionValue(double x_p, double y_p, double vx_p, double vy_p)
{
    double ret = -1000.0;  // no collision
    double val, min_val;
    min_val = 1.0;
    solver = 0;
    QListIterator<CollisionSolver*> it(segments_vertices);
    while(it.hasNext()) {
        solver=it.next();
        val = solver->getCollisionValue(x_p, y_p, vx_p, vy_p);
        if(val >= -1.0 && val <= min_val)
            min_val = val;
        else
            solver=0;
    }
    if(solver)
        ret = min_val;
    return ret;

}
double RectangleCollisionSolver::getCollisionAngle(const double& x_p, const double& y_p)
{

    double ret = 0.0;
    if(solver)
        ret = solver->getCollisionAngle(x_p, y_p);
    return ret;

}
