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
#include "collisionsolver.h"
#include "../world/material.h"

CollisionSolver::CollisionSolver()
{
    robot_rad = 0;
    solver = 0;
    first_segment_solver = 0;
    incidence_angle = 0;
    lambda = 0;
    w_short = 0;
    cellSize = 50;
}

CollisionSolver::~CollisionSolver()
{
    removeMaterials(true);
}

void CollisionSolver::addMaterial(Material* material)
{
    if(materials.contains(material->getSensorType()))
        materials[material->getSensorType()] = material;
    else
        materials.insert(material->getSensorType(), material);
}

Material* CollisionSolver::getMaterial(int sensor_type)
{
    Material* material = 0;
    if(materials.contains(sensor_type))
        material = materials[sensor_type];
    else if(materials.contains(0))
        material = materials[0];
    return material;
}

void CollisionSolver::removeMaterials(bool del)
{
    QMapIterator<int, Material*> i(materials);
    if(del) {
        while (i.hasNext()) {
            i.next();
            delete i.value();
        }
    }
    materials.clear();
}

void CollisionSolver::addLista(QList< QPoint > listaPuntos)
{
    listasDePuntos = listaPuntos;
}

QList<QPoint>  CollisionSolver::getLista()
{
    return listasDePuntos;
}

QList<CollisionSolver*> CollisionSolver::getCollisionsSolver(){
    return segments_vertices;
}

void CollisionSolver::calculateCells(QList<QPoint > cells)
{
    int col = (pos.x() - topLeft.x())/cellSize;
    int row = (pos.y() - topLeft.y())/cellSize;
    cells.append(QPoint(row, col));
}

double CollisionSolver::getCellSize() const
{
    return cellSize;
}

void CollisionSolver::setCellSize(double value, QPoint topLeft)
{
    cellSize = value;
    this->topLeft = topLeft;
}
