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

#include <QPolygonF>
#include <QPainterPath>

#include "polygoncollisionsolver.h"
#include "segmentcollisionsolver.h"
#include "singularpointcollisionsolver.h"

PolygonCollisionSolver::PolygonCollisionSolver()
    : CollisionSolver()
{
}


PolygonCollisionSolver::~PolygonCollisionSolver()
{
    removeMaterials(true);
    for(int i = 0; i < segments_vertices.size(); i++){
        delete segments_vertices[i];
    }
}

void PolygonCollisionSolver::setParameters(const QDomElement& params)
{

    bool ok = false;
    CollisionSolver* coll_solver = 0;

    paramsPoints.clear();
    QPolygon pa;

    double xx, yy;
    XMLOperator* xml_operator = XMLOperator::instance();
    QDomElement num_points = xml_operator->findTag("num_points", params);
    int n_points = 0;
    if(!num_points.isNull()) {
        n_points = num_points.text().toInt(&ok);
        pa.fill(QPoint(0,0), n_points);
        for(int i = 0; i < n_points; i++) {
            QDomElement point = xml_operator->findTag("point" +
                                     QString::number(i).rightJustified(4,'0'), params);
            if(!point.isNull()) {
                pa.setPoint(i, point.attribute("x").toInt(), point.attribute("y").toInt());
            }
        }
        int first_index = 0;
        for(int i = 1; i < n_points; i++) {
            if(pa.point(i).x() < pa.point(first_index).x())
                first_index = i;
            else if(pa.point(i).x() == pa.point(first_index).x()) {
                if(pa.point(i).y() < pa.point(first_index).y())
                    first_index = i;
            }
        }

        QPointF prev_point, post_point;
        prev_point = pa.point((first_index - 1 + n_points)%n_points);
        post_point = pa.point((first_index + 1)%n_points);

        if(prev_point.y() >= post_point.y()) {
            xx = double(pa.point(first_index).x());
            yy = double(pa.point(first_index).y());

            paramsPoints.append(QPointF(xx, yy));
            for(int i = 1; i < n_points; i++) {
                xx = double(pa.point((first_index + i + n_points)%n_points).x());
                yy = double(pa.point((first_index + i + n_points)%n_points).y());
                paramsPoints.append(QPointF(xx, yy));
            }
        }
        else {
            first_index = (first_index - 1 + n_points)%n_points;
            xx = double(pa.point(first_index).x());
            yy = double(pa.point(first_index).y());
            paramsPoints.append(QPointF(xx, yy));
            for(int i = 1; i < n_points; i++) {
                xx = double(pa.point((first_index - i + n_points)%n_points).x());
                yy = double(pa.point((first_index - i + n_points)%n_points).y());
                paramsPoints.append(QPointF(xx, yy));
            }
        }

        QDomDocument doc_collision("collision");
        QDomElement collision_params = doc_collision.createElement("parameters");
        for(int i = 0; i < n_points; i++) {
            collision_params.setAttribute("x_start", paramsPoints.at(i%n_points).x());
            collision_params.setAttribute("y_start", paramsPoints.at(i%n_points).y());
            collision_params.setAttribute("x_end", paramsPoints.at((i + 1)%n_points).x());
            collision_params.setAttribute("y_end", paramsPoints.at((i + 1)%n_points).y());
            coll_solver = new SegmentCollisionSolver();
            if(!i)
                first_segment_solver = coll_solver;
            segments_vertices.append(coll_solver);

            coll_solver->setParameters(collision_params);
        }
        for(int i = 0; i < n_points; i++) {
            collision_params.setAttribute("x", paramsPoints.at(i).x());
            collision_params.setAttribute("y", paramsPoints.at(i).y());
            double start_angle = segments_vertices.at((i - 1 + n_points) % n_points)->getCollisionAngle(0, 0) - M_PI/2;
            if(start_angle < -M_PI)
                start_angle += 2*M_PI;
            double end_angle = segments_vertices.at(i)->getCollisionAngle(0, 0) - M_PI/2;
            if(end_angle < -M_PI)
                end_angle += 2*M_PI;
            collision_params.setAttribute("start_angle", start_angle);
            collision_params.setAttribute("end_angle", end_angle);
            coll_solver = new SingularPointCollisionSolver();
            segments_vertices.append(coll_solver);
            coll_solver->setParameters(collision_params);
        }
    }

}

void PolygonCollisionSolver::setRobotRadius(const double& rad) 
{ 

    robot_rad = rad;
    QListIterator<CollisionSolver*> it(segments_vertices);
    while(it.hasNext()){
        it.next()->setRobotRadius(robot_rad);
    }

}  

double PolygonCollisionSolver::getCollisionValue(double x_p, double y_p, double vx_p, double vy_p)
{
    double ret = -1000.0;  // no collision
    double val, min_val;
    min_val = 1.0;
    solver = 0;
    QListIterator<CollisionSolver*> it(segments_vertices);
    while(it.hasNext()) {
        solver = it.next();
        val = solver->getCollisionValue(x_p, y_p, vx_p, vy_p);
        if(val >= -1.0 && val <= min_val) {
            min_val = val;
        }
        else {
            solver=0;
        }
    }
    if(solver)
        ret = min_val;
    return ret;

}

double PolygonCollisionSolver::getCollisionAngle(const double& x_p, const double& y_p)
{

    double ret = 0.0;
    if(solver)
        ret = solver->getCollisionAngle(x_p, y_p);
    return ret;

}

double PolygonCollisionSolver::getLineIntersectionValue(const double& line_x_start, const double& line_y_start, const double& line_x_end, const double& line_y_end)
{ 
    double ret = -1000.0;  // no intersection
    double val = 1000.0;
    double min_val = val;
    solver = 0;

    QListIterator<CollisionSolver*> it(segments_vertices);
    CollisionSolver* tmp_solver = 0;

    while(it.hasNext()) {
        tmp_solver = it.next();
        val = tmp_solver->getLineIntersectionValue(line_x_start, line_y_start, line_x_end, line_y_end);
        if(val >= 0 && val <= 1) {
            if(val <= min_val) {
                solver = tmp_solver;
                min_val = val;
            }
        }
    }
    if(solver) {
        intersection_point = solver->getIntersectionPoint();
        incidence_angle = solver->getIncidenceAngle();
        ret = min_val;
    }
    return ret;
}

void PolygonCollisionSolver::addMaterial(Material* material) {
    CollisionSolver::addMaterial(material);
    for(int i = 0; i < getCollisionsSolver().size(); i++)
        getCollisionsSolver().at(i)->addMaterial(material);
}

void PolygonCollisionSolver::removeMaterials(bool del) {
    for(int i = 0; i < getCollisionsSolver().size(); i++)
        getCollisionsSolver().at(i)->removeMaterials(false);
    CollisionSolver::removeMaterials(del);
}
