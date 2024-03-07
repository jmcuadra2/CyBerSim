/***************************************************************************
                          robotsprite.h  -  description
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

#ifndef ROBOTSPRITE_H
#define ROBOTSPRITE_H

#include <QObject>
#include <QGraphicsScene>
#include <QGraphicsSvgItem>
#include <QDir>

#include "../world/rtti.h"
#include "../world/drawings.h"
#include "spritecollisioner.h"

class RobotSprite;
class DrawingPath;

/*! \ingroup robot_base
    \brief Clase derivada de QCanvasSprite que contiene pixmaps
      rotados del robot como cuadros de la animacion.
     Controla los movimientos del robot en el mundo :
     traslaciones, rotaciones y colisiones.
  */

class RobotSprite : public QGraphicsSvgItem
{
    Q_OBJECT

    friend class RobotBuilder;
    friend class Robot;
    friend class RobotFactory;
    friend class NDAriaRobot;
    friend class NDAriaRobotFactory;
    friend class TotallySimulatedRobot;
    friend class TotSimulRobotFactory;
    friend class RobotPosition;
    friend class RobotKineticsTab;
    friend class NDAriaRobotKineticsTab;
    friend class SpriteCollisioner;
    friend class NomadSimRobotFactory;
    friend class NomadSimRobot;
    friend class NomadRobot;


public:
    RobotSprite(QGraphicsScene* w_canvas, const QString& fileName, QGraphicsItem * parent = 0);
    virtual ~RobotSprite();
    
    qreal x() const;
    qreal y() const;
    void setX(qreal x_, bool initPos = true);
    void setY(qreal y_, bool initPos = true);
    
    QList<QGraphicsItem *> collidingItems ( Qt::ItemSelectionMode mode = Qt::IntersectsItemShape ) const;
    
protected:
    void advance(int stage);
    int type(void) const;

    void setVel(double rv, double lv, double rot);
    void setVelocity(double xVelo, double yVelo);
    void setXVelocity(double xVelo);
    void setYVelocity(double yVelo);
    double xVelocity();
    double yVelocity();
    
    void setLVel(double lv);
    void setRVel(double rv);
    double getLVel(void) {
        return lVel ;
    }
    double getRVel(void) {
        return rVel ;
    }

    void calcAdvance(double x_, double y_, double rotation, bool forward);
    void setRotation(double rot);
    double getRotation(void) {
        return rotation ;
    }
    double noiseRatio(void) {
        return noise_ratio ;
    }
    void setMaxVel(double m_vel) {
        max_vel = m_vel ;
    }
    double maxVel(void) {
        return max_vel ;
    }

    bool getDrived() const {
        return drived;
    }

    void setVx(double vx) {
        this->vx = vx;
    }

    double getVx() const {
        return vx;
    }

    void setVy(double vy) {
        this->vy = vy;
    }

    double getVy() const {
        return vy;
    }

    void setCollisioner(SpriteCollisioner* collisioner) {
        this->collisioner = collisioner;
    }

    void clearPath(bool reset);
    void updatePath();
    void updateSlamOdometry(double x, double y);
    void updateRawOdometry(double x, double y);

signals:
    void outWorld(void);
    void sigPosVelRot(double, double, double,
                      double, double, double, bool);
    void intPosition(int, int);

protected:
    double rotation;
    double rVel, lVel;
    bool forw;
    int advance_period;
    double noise_ratio, max_vel;
    double vx, vy, add_rotation;

private:
    int diameter;
    double rot;
    double wheelsSep;
    double robot_scale; // mm por pixel
    //     double vv_x, vv_y;
    long double cos_rotation, sin_rotation;
    DrawingPath* pathDrawing, * slamOdometryDrawing, * rawOdometryDrawing;
    bool drived;

    SpriteCollisioner* collisioner;
    QRectF collidingRect;

};

inline int RobotSprite::type(void) const
{    
    return ROBOT_RTTI;
}

inline qreal RobotSprite::x() const
{
    return QGraphicsItem::x() + transformOriginPoint().x();
}

inline qreal RobotSprite::y() const
{
    qreal gy = QGraphicsItem::y();
    qreal ty = transformOriginPoint().y();
    return QGraphicsItem::y() + transformOriginPoint().y();
}
#endif
