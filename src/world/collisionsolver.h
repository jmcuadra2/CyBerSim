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
#ifndef COLLISIONSOLVER_H
#define COLLISIONSOLVER_H

#include <QDomDocument>
#include <QMap>

#include "../neuraldis/xmloperator.h"
#include "../neuraldis/ndmath.h" 

class Material;

/**
@author jose manuel
*/
class CollisionSolver{

public:
    CollisionSolver();
    virtual ~CollisionSolver();
    
    virtual double getCollisionValue(double x_p, double y_p, double vx_p, double vy_p) = 0;
    virtual double getCollisionAngle(const double& , const double& ) = 0;
    virtual void setRobotRadius(const double& rad) = 0;
    virtual void setParameters(const QDomElement& params) = 0;
    virtual double getLineIntersectionValue(const double& line_x_start, const double& line_y_start, const double& line_x_end, const double& line_y_end) = 0;
    QPointF getIntersectionPoint(void) { return intersection_point ; }
    double getIncidenceAngle(void) { return incidence_angle ; }
    
    virtual void addMaterial(Material* material);
    Material* getMaterial(int sensor_type);

    QMap< int, Material * > getMaterials() const { return materials; }
    void setLambda(double lambda) { this->lambda = lambda; }
    double getLambda() const { return lambda; }
    void setW_short(double w_short) { this->w_short = w_short; }
    double getW_short() const { return w_short; }

    virtual void removeMaterials(bool del);

    QList<QPoint> getLista();
    void addLista(QList<QPoint> listaPuntos);
    virtual QList<CollisionSolver*> getCollisionsSolver();
    virtual void calculateCells(QList< QPoint > cells); // Bresenhem

    void setPos(QPoint pos) { this->pos = pos;}
    QPoint getPos(){return pos;}
    double getCellSize() const;
    void setCellSize(double value, QPoint topLeft);
    
    QList<QPointF> getParamsPoints() {return paramsPoints; }

public:
    enum Types {ELLIPSE_COLL = 1, POLYGON_COLL, SEGMENT_COLL, POINT_COLL};

protected:
    double robot_rad;
    CollisionSolver* solver;
    CollisionSolver* first_segment_solver; // para LineCollisionSolver::getLineIntersectionValue();
    QPointF intersection_point;
    double incidence_angle;
    QMap<int, Material*> materials;
    QPoint pos;
    int cellSize;
    
    QPoint topLeft;
    QList<QPoint> listasDePuntos;

    QList<CollisionSolver*> segments_vertices;
    QList<QPointF> paramsPoints;
    
private:
    double collision_val, collision_angle;
    double lambda, w_short;

};

#endif
