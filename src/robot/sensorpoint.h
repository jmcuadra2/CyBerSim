/***************************************************************************
                          sensorpoint.h  -  description
                             -------------------
    begin                : vie feb 6 2004
    copyright            : (C) 2004 by Jose M. Cuadra Troncoso
    email                : jmcuadra@dia.uned.es
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef SENSORPOINT_H
#define SENSORPOINT_H

#include "robotposition.h"

#include "../neuraldis/ndmath.h"
#include "../world/drawings.h"
#include "../world/abstractworld.h"
#include "../world/calculadorbresenham.h"

/*! \ingroup sensors
    \brief Clase de los puntos que definen el campo de los sensores derivados
    de PointedSampledSensor.
  */

class SensorPoint
{

public:
    SensorPoint(int id, double x_org, double y_org, double org_rotation, int val,
                RobotPosition *rob_pos, const QString &visible_color,
                const QString &second_color, AbstractWorld* world,
                bool rotateWithRobot = true);

    ~SensorPoint();
    void advance(int stage);
    int type(void) const;
    void setVisible(bool on);
    void rotate(void);
    void rotate(double rx, double ry, double ang);
//    void rotate(double ang);
//    void rotateTo(double ang);
    void moveBy(qreal dx, qreal dy);

//    void showInWorld(bool show);
    int sample(void);
    bool collidesWithItem(const QGraphicsItem* i) const;
    double x(void);
    double y(void);
    void setX(double x_);
    void setY(double y_);
    void changeRelativePos(double x_, double y_);

    QuadTree* getQuadtree();

public:
    enum noSample {SP_NO_SAMPLE = -1000000000};

private:
    RobotPosition *robot_position;
    double x_old_trans, y_old_trans;
    AbstractWorld* world;
    double x_org_old_trans, y_org_old_trans;
    double rotation, originalRotation;
    int ident, val;
//    int x_org, yy_org;
    double xx, yy;
    bool rotateWithRobot;
    DrawingItem* drawing;
    int size;

    CollisionSolver* solver;
    CalculadorBresenham *calculador;
    QuadTree *qTree;

};

inline double SensorPoint::x(void)
{
    return drawing->canvasItem()->x() + double(2/2.0);
}

inline double SensorPoint::y(void)
{
    return drawing->canvasItem()->y() + double(2/2.0);
}

inline void SensorPoint::setX(double x_)
{   
    xx = x_ ;
//    drawing->canvasItem()->setX(xx - double(2/2.0));
}

inline void SensorPoint::setY(double y_)
{   
    yy = y_;
//    drawing->canvasItem()->setY(yy - double(2/2.0));
}

inline int SensorPoint::type(void) const
{    
    return SPOINT_RTTI;
}

inline void SensorPoint::setVisible(bool on) {
    if (drawing) {
        drawing->setVisible(on);
        drawing->setShowColor(on);
    }
}

inline void SensorPoint::rotate(double rx, double ry, double ang)
{    
    QPointF n_p;
    n_p = NDMath::rotateGrad(x_old_trans, y_old_trans, rx, ry, ang);
    setX(n_p.x());
    setY(n_p.y());
}

#endif
