//
// C++ Interface: spritecollisioner
//
// Description:
//
//
// Author: Jose Manuel Cuadra Troncoso <jmcuadra@dia.uned.es>, (C) 2009
//
// Copyright: See COPYING file that comes with this distribution
//
//

#ifndef SPRITECOLLISIONER_H
#define SPRITECOLLISIONER_H
#include <QGraphicsItem>

class QGraphicsItem;
class RobotSprite;

/**
  @author Jose Manuel Cuadra Troncoso <jmcuadra@dia.uned.es>
*/
class SpriteCollisioner
{
public:
    SpriteCollisioner(RobotSprite * robot_sprite);

    ~SpriteCollisioner();

    bool collides(void);

    void setCollision(bool collision) {
        this->collision = collision;
    }

    bool getCollision() const {
        return collision;
    }

    void setResolvCollision(bool resolv_collision) {
        this->resolv_collision = resolv_collision;
    }

    bool getResolvCollision() const {
        return resolv_collision;
    }

protected:
    bool collidesWith(const QGraphicsItem* i);
    double getCollisionAngle(const QGraphicsItem* i);

protected:
    bool collision, resolv_collision;
    double min_prop_collision, collision_angle;
    QGraphicsItem* min_prop_canvasitem;
    double vv_x, vv_y;
    RobotSprite * robot_sprite;

};

#endif
