//
// C++ Interface: nomadsprite
//
// Description: 
//
//
// Author: Jose Manuel Cuadra Troncoso <jmcuadra@dia.uned.es>, (C) 2010
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef NOMADSPRITE_H
#define NOMADSPRITE_H

#include "robotsprite.h"

class DrawingLine;
/**
*/
class NomadSprite : public RobotSprite
{

public:
    NomadSprite(QGraphicsScene* w_canvas,const QString & fileName, QGraphicsItem * parent);
    void updatePathLine(double dir);

    virtual ~NomadSprite();

private:
    DrawingLine * pathLine;
    int tmp;
};

#endif
