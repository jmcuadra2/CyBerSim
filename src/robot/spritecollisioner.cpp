//
// C++ Implementation: spritecollisioner
//
// Description: 
//
//
// Author: Jose Manuel Cuadra Troncoso <jmcuadra@dia.uned.es>, (C) 2009
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "spritecollisioner.h"
#include "robotsprite.h"
#include "../neuraldis/ndmath.h"
#include "../world/solidobject.h"
#include "../world/collisionsolver.h"

SpriteCollisioner::SpriteCollisioner(RobotSprite * robot_sprite)
{
    min_prop_collision = 1.0;
    min_prop_canvasitem = 0;
    collision = false;
    resolv_collision = false;
    collision_angle = 0.0;
    this->robot_sprite = robot_sprite;
}


SpriteCollisioner::~SpriteCollisioner()
{
}

bool SpriteCollisioner::collides(void)
{
    double old_min_prop_collision = 1.0;
    bool off_resolv_collision = false;

    if(collision && !robot_sprite->getDrived()) {
        if(!resolv_collision) {
            NDMath::selfRotateRad(vv_x, vv_y, 0.0, 0.0, -collision_angle);
            vv_x *= 2;
            vv_y *= -2;
            NDMath::selfRotateRad(vv_x, vv_y, 0.0, 0.0, collision_angle);
            double noise_v = 1 + NDMath::randDouble4(0, 2.0)*robot_sprite->noiseRatio();
            vv_x *= noise_v;
            vv_y *= noise_v;
            robot_sprite->setRotation(robot_sprite->getRotation() + NDMath::randDouble(-150.0, 150.0, 0)
                                      *sqrt(vv_x*vv_x +vv_y*vv_y)/robot_sprite->maxVel()*robot_sprite->noiseRatio());
            resolv_collision = true;
        }
        else
            off_resolv_collision = true;
        collision = false;
    }
    else {
        resolv_collision = false;
        vv_x = robot_sprite->getVx();
        vv_y = robot_sprite->getVy();
    }

    robot_sprite->setVelocity(vv_x, vv_y);
    if(robot_sprite->getDrived())
        return resolv_collision;

    QList<QGraphicsItem*> l = robot_sprite->collidingItems(Qt::IntersectsItemBoundingRect);

    int s = l.size();
    for (QList<QGraphicsItem *>::Iterator it=l.begin();it!=l.end(); ++it) {
        QGraphicsItem *hit = *it;

        int type = hit->type();
        if (type / 100 != RADIALFIELDSOURCE_RTTI && (type > FIRST_COLLISION_RTTI) &&
             (type < LAST_COLLISION_RTTI)) {
            if(collidesWith(hit)) {
                if(min_prop_collision <= old_min_prop_collision) {
                    min_prop_canvasitem = hit;
                    old_min_prop_collision = min_prop_collision;
                }
            }
        }
    }
    min_prop_collision = old_min_prop_collision;

    if(min_prop_canvasitem) {
        collision = true;
        collision_angle = getCollisionAngle(min_prop_canvasitem);
        if(min_prop_collision >= 0.0) {
            robot_sprite->setVx(vv_x* min_prop_collision*0.9);
            robot_sprite->setVy(vv_y* min_prop_collision*0.9);
        }
        else if(min_prop_collision > -1.0){
            robot_sprite->setVx(vv_x* min_prop_collision*1.1);
            robot_sprite->setVy(vv_y* min_prop_collision*1.1);
        }
        robot_sprite->setVelocity(robot_sprite->getVx(), robot_sprite->getVy());
        min_prop_canvasitem = 0;
        min_prop_collision = 1.0;
    }

    if(off_resolv_collision) {
        resolv_collision = false;
        robot_sprite->setVx(vv_x);
        robot_sprite->setVy(vv_y);
        vv_x /= 4;
        vv_y /= 4;
    }

    return resolv_collision;
}

bool SpriteCollisioner::collidesWith(const QGraphicsItem* i)
{
    bool resp = false;
    double prop_collision = 1.1;

    if((i->type() > FIRST_COLLISION_RTTI)  && (i->type() < LAST_COLLISION_RTTI)) {
        const SolidObject* solid_object = dynamic_cast<const SolidObject*> (i);
        prop_collision = solid_object->getCollisionSolver()->getCollisionValue(robot_sprite->x(), robot_sprite->y(), vv_x, vv_y);
    }

    if(prop_collision <= 1.0 && prop_collision >= 0.0) {
        min_prop_collision = prop_collision;
        resp = true;
    }
    else if(prop_collision < 0.0 && prop_collision > -1.0 && !resolv_collision ) {
        min_prop_collision = prop_collision;
        resp = true;
    }
    return resp;
}

double SpriteCollisioner::getCollisionAngle(const QGraphicsItem* i)
{
    if((i->type() > FIRST_COLLISION_RTTI)  && (i->type() < LAST_COLLISION_RTTI)) {
        const SolidObject* solid_object = dynamic_cast<const SolidObject*> (i);
        return solid_object->getCollisionSolver()->getCollisionAngle(robot_sprite->x() + robot_sprite->getVx()*min_prop_collision, robot_sprite->y() + robot_sprite->getVy()*min_prop_collision);
    }
    else
        return 0.0;  // este valor es arbitrario
}
