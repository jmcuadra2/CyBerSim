//
// C++ Implementation: sensorline
//
// Description:
//
//
// Author: Jose Manuel Cuadra Troncoso <jmcuadra@dia.uned.es>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//

#include <QColor>

#include "../world/rtti.h"
#include "../world/world.h"
#include "../world/material.h"
#include "sensorline.h"

SensorLine::SensorLine(int id, double x_org, double y_org, double x_end, double y_end,
                       double org_rotation, AbstractWorld* world, RobotPosition *rob_pos,
                       const QString& visible_color, const QString& second_color,
                       DrawingItem::Drawing type_draw, int sensorType,bool rotateWithRobot)
{

    this->sensorType = sensorType;
    robot_position = rob_pos;
    rotateWithRobot_ = rotateWithRobot;
    ident = id;
    val = 0;
    typeDraw = type_draw;
    this->world = world;

    material = 0;

    x_org_old_trans = x_org - robot_position->x();
    y_org_old_trans = y_org - robot_position->y();
    x_end_old_trans = x_end - robot_position->x();
    y_end_old_trans = y_end - robot_position->y();

    drawing = dynamic_cast<DrawingItem*> (DrawingsFactory::factoryDrawingItem(type_draw,
                                        world->getCanvas(), visible_color, second_color));
    if(drawing) {
        world->addSensorLine(this, drawing->canvasItem());
        drawing->setGeometry(NDMath::roundInt(x_org), NDMath::roundInt(y_org),
                             NDMath::roundInt(x_end), NDMath::roundInt(y_end));

        drawing->setReadingCoords(NDMath::roundInt(x_end), NDMath::roundInt(y_end));
    }

    setVisible(false);
    max_size = 1000.0;
    originalRotation = org_rotation;
    if (rotateWithRobot_)
        rotation  = originalRotation + robot_position->rotation();
    else
        rotation  = originalRotation;

    sigma = 0;
    lambda = 0;
    w_short = 0;
    w_rand = 0;
    validReading = false;
    solver = 0;
    calculador = world->getBresenhamCalculador();
    qTree = getQuadtree();
}


SensorLine::~SensorLine()
{
    if(drawing)
        delete drawing;
}

void SensorLine::advance(int stage)
{
    switch ( stage ) {
    case 0:
        break;
    case 1:
        rotate();
        break;
    }
}

void SensorLine::rotate(double ang)
{
    QPointF n_p;
    NDMath::selfRotateGrad(x_end_old_trans, y_end_old_trans, x_org_old_trans,
                           y_org_old_trans, ang);
    originalRotation += ang;
    rotate();
}

void SensorLine::rotateTo(double ang)
{
    rotate(ang - originalRotation);
}

void SensorLine::rotate()
{

    QPointF n_end_p;
    QPointF n_org_p;
    if (rotateWithRobot_) {
        n_org_p = robot_position->rotateWithRobot(x_org_old_trans, y_org_old_trans);
        n_end_p = robot_position->rotateWithRobot(x_end_old_trans, y_end_old_trans);
        rotation  = originalRotation + robot_position->rotation();
    } else {
        n_org_p.setX(robot_position->x() + x_org_old_trans);
        n_org_p.setY(robot_position->y() + y_org_old_trans);
        n_end_p.setX(robot_position->x() + x_end_old_trans);
        n_end_p.setY(robot_position->y() + y_end_old_trans);
    }

    if(drawing) {
        drawing->setGeometry(NDMath::roundInt(n_org_p.x()), NDMath::roundInt(n_org_p.y()),
                             NDMath::roundInt(n_end_p.x()), NDMath::roundInt(n_end_p.y()));
        drawing->setReadingCoords(n_end_p.x(), n_end_p.y());
    }

}

int SensorLine::sample(bool fullProb)
{
    QPointF n_end_p;
    QPointF n_org_p;
    double ratio = 0.0;
    double max_val_ratio = 1.0;
    int ret = wrong_val;

    if (rotateWithRobot_) {
        n_org_p = robot_position->rotateWithRobot(x_org_old_trans, y_org_old_trans);
        n_end_p = robot_position->rotateWithRobot(x_end_old_trans, y_end_old_trans);
    }
    else {
        n_org_p.setX(robot_position->x() + x_org_old_trans);
        n_org_p.setY(robot_position->y() + y_org_old_trans);
        n_end_p.setX(robot_position->x() + x_end_old_trans);
        n_end_p.setY(robot_position->y() + y_end_old_trans);
    }

    lineaBresenham = calculador->BresenhamLine(n_org_p.toPoint().x(), n_org_p.toPoint().y(),
                                       n_end_p.toPoint().x(), n_end_p.toPoint().y(), 2);
    solver = 0;
    int cnt = 0;
    for(int h = 0; h<lineaBresenham.size(); h++) {
        uint fila = lineaBresenham.at(h).x();
        uint columna = lineaBresenham.at(h).y();

        QList<CollisionSolver*> *world_objects = qTree->getCollisionSolvers(fila,columna);
        int s = world_objects->size();
        if(!s)
            continue;

        for(int it = 0; it < s; it++) {
            CollisionSolver* solverWorld = world_objects->at(it);
            material = solverWorld->getMaterial(sensorType);
            if(material) {
                if(material->isTransparent())
                    continue;
            }

            ratio = solverWorld->getLineIntersectionValue(n_org_p.x(), n_org_p.y(),
                                                          n_end_p.x(), n_end_p.y());

            if(material) {
                if(ratio >= 0.0 && ratio <= max_val_ratio) {
                    solver = solverWorld;
                    max_val_ratio = ratio;
                }
            }
            else {
                if(ratio >= 0.0 && ratio <= max_val_ratio) {
                    solver = solverWorld;
                    max_val_ratio = ratio;
                }
            }
        }
        
        if(solver) {
            if(cnt > 10)
                break;
            else
                cnt++;
        }
    }
    double z_mix = 0;
    z_star = 0;
    if(solver) {
        material = solver->getMaterial(sensorType);
        z_star = max_val_ratio*max_val;
        z_mix = z_star;
        double incidence_angle = solver->getIncidenceAngle(); // segments no usan los parámetros de la función
        lambda = solver->getLambda();
        w_short = solver->getW_short();
        if(material) {
            w_rand = material->getW_rand();
            if(fullProb)
                z_mix = material->randomize(z_star, incidence_angle, lambda, sigma,
                                            w_short, max_val);
            else
                z_mix = material->randomBounce(z_star, incidence_angle, max_val);
        }
        else
            w_rand = world->getW_rand();
        ret = NDMath::roundInt(z_mix);
        if(ret >= max_val) {
            ret = wrong_val;
            global_intersection_point = n_end_p;
        }
        else {
            ratio = (z_mix - z_star)/max_val;
            if(ratio >= 0)
                global_intersection_point = n_end_p * ratio + (1 - ratio) *
                        solver->getIntersectionPoint();
            else
                global_intersection_point = -n_org_p * ratio + (1 + ratio) *
                        solver->getIntersectionPoint();
        }
    }
    else {
        lambda = world->getLambda();
        w_short = world->getW_short();
        w_rand = world->getW_rand();
        if(fullProb)
            z_mix = NDMath::randBeamModel(wrong_val, lambda, 0, world->getW_short(),
                                          0, 1 - w_short - w_rand, w_rand, max_val);
        else
            z_mix = max_val;
        ret = NDMath::roundInt(z_mix);
        if(ret >= max_val) {
            ret = wrong_val;
            global_intersection_point = n_end_p;
        }
        else {
            ratio = z_mix/max_val;
            global_intersection_point = (n_end_p - n_org_p)*ratio + n_org_p;
        }
    }

    local_intersection_point = global_intersection_point - n_org_p;
    local_intersection_point = NDMath::rotateGradCanvas(local_intersection_point.x(),
                                            local_intersection_point.y(), 0, 0, -rotation);

    return ret;

}

int SensorLine::sampleSimple(bool fullProb)
{
    QPointF n_end_p;
    QPointF n_org_p;
    int ret = fullProb;

    if (rotateWithRobot_) {
        n_org_p = robot_position->rotateWithRobot(x_org_old_trans, y_org_old_trans);
        n_end_p = robot_position->rotateWithRobot(x_end_old_trans, y_end_old_trans);
    } else {
        n_org_p.setX(robot_position->x() + x_org_old_trans);
        n_org_p.setY(robot_position->y() + y_org_old_trans);
        n_end_p.setX(robot_position->x() + x_end_old_trans);
        n_end_p.setY(robot_position->y() + y_end_old_trans);
    }

    global_intersection_point = n_end_p;

    local_intersection_point = global_intersection_point - n_org_p;
    local_intersection_point = NDMath::rotateGradCanvas(local_intersection_point.x(),
                                            local_intersection_point.y(), 0, 0, -rotation);

    return ret;
}

void SensorLine::setShowColor(bool show)
{
    if(drawing)
        drawing->setShowColor(show);
}

void  SensorLine::setColor(const QColor& color)
{
    if(drawing)
        drawing->setColor(color);
}

void SensorLine::scale (double x_reading, double y_reading)
{
    if(drawing)
        drawing->setReadingCoords(NDMath::roundInt(world->xRobotTransform(x_reading)),
                                  NDMath::roundInt(world->yRobotTransform(y_reading)));
}

void SensorLine::setSensorVals(int maxval, int wrongval)
{
    max_val = maxval;
    wrong_val = wrongval;
    max_size = (double)max_val/world->getWorldScale();

}

void SensorLine::setProbabilisticParams(double sigma)
{
    this->sigma = sigma;
}

bool SensorLine::isUserVisible(void) const
{
    bool ret = false;
    if(drawing)
        ret = drawing->isUserVisible();
    return ret;
}

void SensorLine::saturateColor(double value)
{
    if(drawing)
        drawing->saturateColor(value);
}

void SensorLine::setLength(int lenght)
{
    QPointF n_end_p;
    QPointF n_org_p;
    n_org_p = robot_position->rotateWithRobot(x_org_old_trans, y_org_old_trans);
    n_end_p = robot_position->rotateWithRobot(x_end_old_trans, y_end_old_trans);

    double ratio = (double)lenght/max_val;
    global_intersection_point = (n_end_p - n_org_p)*ratio + n_org_p;

    scale(getGlobalIntersectionPoint().x(), getGlobalIntersectionPoint().y());

    local_intersection_point = global_intersection_point - n_org_p;
    local_intersection_point = NDMath::rotateGradCanvas(local_intersection_point.x(),
                                    local_intersection_point.y(), 0, 0, -rotation);

}

void SensorLine::setValidReading(bool valid)
{
    if(drawing)
        drawing->setValidReading(valid);

    validReading = valid;
}

bool SensorLine::isValidReading(void)
{
    return validReading;
}

QuadTree* SensorLine::getQuadtree()
{
    return world->getQuadTree();
}
