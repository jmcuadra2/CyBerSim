/***************************************************************************
                          robotsprite.cpp  -  description
                             -------------------
    begin                : Thu Dec 11 2003
    copyright            : (C) 2003 by Jose M. Cuadra Troncoso
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

#include "robotsprite.h"
#include <iostream>

#include "../neuraldis/ndmath.h"
#include "../world/world.h"
#include "../world/drawings.h"

RobotSprite::RobotSprite(QGraphicsScene* w_canvas, const QString & fileName, QGraphicsItem * parent)
    : QGraphicsSvgItem(fileName, parent)
{
    int x = 0;
    int y = 0;
    diameter = 1;
    wheelsSep = 1;
    noise_ratio = 0.0;

    pathDrawing = DrawingsFactory::factoryDrawingPath(w_canvas, "green", 50);
    slamOdometryDrawing = nullptr;
    rawOdometryDrawing = nullptr;

    slamOdometryDrawing = DrawingsFactory::factoryDrawingPath(w_canvas,"black",50);
    rawOdometryDrawing = DrawingsFactory::factoryDrawingPath(w_canvas, "blue", 50);

    setTransformOriginPoint(sceneBoundingRect().center());
    setX(x);
    setY(y);
    setZValue(2001);
    vx = 0.0;
    vy = 0.0;
    setVelocity(vx, vy);
    rVel = 0.0;
    lVel = 0.0;
    forw = true;
    advance_period = 100;
    rotation = 0.0;
    add_rotation = 0.0;
    cos_rotation = 1.0;
    sin_rotation = 0.0;
    robot_scale = 2.0; // mm por pixel
    max_vel = 100;
    rot = 0;
    drived = false;

    collisioner= 0;

    collidingRect = boundingRect();
    double scale = sqrt(collidingRect.height() * collidingRect.height() + collidingRect.width() * collidingRect.width())/fmin(collidingRect.height(), collidingRect.width());
    NDMath::scaleRect(collidingRect, scale);
    collidingRect.moveTo(-collidingRect.center());

    show();

}

RobotSprite::~RobotSprite()
{
    if(pathDrawing)
        delete pathDrawing;

    if(slamOdometryDrawing)
        delete slamOdometryDrawing;

    if(rawOdometryDrawing)
        delete rawOdometryDrawing;
    
    if(collisioner)
        delete collisioner;
}

void RobotSprite::setX(qreal x_, bool initPos)
{
    QGraphicsItem::setX(x_ -transformOriginPoint().x());
    if(initPos) {
        emit intPosition((int)x(), (int)y());
        clearPath(true);
    }
}

void RobotSprite::setY(qreal y_, bool initPos)
{
    QGraphicsItem::setY(y_ - transformOriginPoint().y());
    if(initPos) {
        emit intPosition((int)x(), (int)y());
        clearPath(true);
    }
}

void RobotSprite::updatePath(void)
{
    if(pathDrawing) {
        pathDrawing->addPoint(QPoint(NDMath::roundInt(x()), NDMath::roundInt(y())));
        //     std::cout << "updatePath " << NDMath::roundInt(x()) << " " << NDMath::roundInt(y()) << endl;
    }
}

void RobotSprite::updateSlamOdometry(double x, double y)
{
    if(slamOdometryDrawing) {
        slamOdometryDrawing->addPoint(QPoint(NDMath::roundInt(x), NDMath::roundInt(y)));
        //     std::cout << x() << " " << y() << endl;
    }
}

void RobotSprite::updateRawOdometry(double x, double y)
{
    if(rawOdometryDrawing) {
        rawOdometryDrawing->addPoint(QPoint(NDMath::roundInt(x), NDMath::roundInt(y)));
        //    std::cout << "updateRawOdometry " << x << " " << y << endl;
    }
}

void RobotSprite::advance(int stage)
{
    double f_rot;
    switch ( stage ) {
    case 0:
        if(collisioner) {
            collisioner->collides();
            cos_rotation = cos(rotation/NDMath::RAD2GRAD);
            sin_rotation = sin(rotation/NDMath::RAD2GRAD);
        }

        break;
    case 1:
        f_rot = (rotation);
        if(int(f_rot > 360.0))
        {
            f_rot = -360.0;
        }
        setRotation(f_rot);

        QGraphicsItem::moveBy(vx,vy);// xVelocity, yVelocity son los parametros a poner.
        collidingRect.translate(vx,vy);
        emit sigPosVelRot(x()*robot_scale/10.0, y()*robot_scale/10.0,
                          rotation, // 0.01 = (10 mm por cm)/(1000 ms por s)
                          vx/(0.01*advance_period), vy/(0.01*advance_period),
                          add_rotation*(1000.0/advance_period), forw);
        emit intPosition(NDMath::roundInt(x()), NDMath::roundInt(y()));
        if(pathDrawing) {
            pathDrawing->addPoint(QPoint((int)x(), (int)y()));
            //       std::cout << "updatePath " << x() << " " << y() << endl;
        }
        break;

    }
}

void RobotSprite::setVel(double rv, double lv, double rot)
{

    QPointF p = NDMath::polarToCartesian((rVel + lVel)/2.0, rot);
    rVel = rv;
    lVel = lv;
    vx = p.x()*0.01*advance_period; // 0.01 = (10 mm por cm)/(1000 ms por s)
    vy = p.y()*0.01*advance_period;
    add_rotation = (lVel - rVel)/wheelsSep*advance_period*0.01;
    setVelocity(vx, vy);
    rotation = rot;
    cos_rotation = cos(rotation/NDMath::RAD2GRAD);
    sin_rotation = sin(rotation/NDMath::RAD2GRAD);

}
void RobotSprite::setVelocity(double xVelo, double yVelo)
{

    vx = xVelo;
    vy = yVelo;
}
void RobotSprite::setXVelocity(double xVelo)
{

    vx = xVelo;
}
void RobotSprite::setYVelocity(double yVelo)
{

    vy = yVelo;
}
double RobotSprite::xVelocity()
{
    return vx;
}
double RobotSprite::yVelocity()
{
    return vy;
}
void RobotSprite::setRVel(double rv)
{    
    rVel= rv;
}

void RobotSprite::setLVel(double lv)
{
    lVel= lv;
}

void RobotSprite::calcAdvance(double x_, double y_, double rotation, bool forward)
{

    forw = forward;

    while(rotation < 0.0) rotation += 360.0;
    while(rotation >= 360.0) rotation -= 360.0;
    //   add_rotation = this->rotation - rotation;
    add_rotation = this->rotation - rotation;
    add_rotation = add_rotation < -180 ? add_rotation + 360 : add_rotation;
    add_rotation = add_rotation > 180 ? add_rotation - 360 : add_rotation;
    this->rotation = rotation;
    cos_rotation = cos(rotation/NDMath::RAD2GRAD);
    sin_rotation = sin(rotation/NDMath::RAD2GRAD);

    vx = x_ - x();
    vy = y_ - y();
    if ( x() + vx <= scene()->sceneRect().left() || x() + vx >= scene()->sceneRect().right() || y() + vy <= scene()->sceneRect().top() || y() + vy >= scene()->sceneRect().bottom())
        emit outWorld();

}

void RobotSprite::setRotation(double rot)
{ 
    rotation = rot ;
    cos_rotation = cos(rotation/NDMath::RAD2GRAD);
    sin_rotation = sin(rotation/NDMath::RAD2GRAD);
    QGraphicsSvgItem::setRotation(rotation);
}

void RobotSprite::clearPath(bool reset)
{
    if(pathDrawing)
        pathDrawing->resetPath(NDMath::roundInt(x()), NDMath::roundInt(y()));
    if(slamOdometryDrawing) {
        if(reset)
            slamOdometryDrawing->resetPath(NDMath::roundInt(x()), NDMath::roundInt(y()));
        else
            slamOdometryDrawing->collapse();
    }
    if(rawOdometryDrawing) {
        if(reset)
            rawOdometryDrawing->resetPath(NDMath::roundInt(x()), NDMath::roundInt(y()));
        else
            rawOdometryDrawing->collapse();
    }
} 

QList<QGraphicsItem *> RobotSprite::collidingItems (Qt::ItemSelectionMode mode) const
{
    if(mode == Qt::IntersectsItemBoundingRect)
        return scene()->items(collidingRect, mode);
    else
        return QGraphicsItem::collidingItems(mode);
}
