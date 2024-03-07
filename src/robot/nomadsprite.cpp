//
// C++ Implementation: nomadsprite
//
// Description: 
//
//
// Author: Jose Manuel Cuadra Troncoso <jmcuadra@dia.uned.es>, (C) 2010
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "nomadsprite.h"
#include "../world/drawings.h"
#include "../neuraldis/ndmath.h"

#include <iostream>

NomadSprite::NomadSprite(QGraphicsScene* w_canvas, const QString & fileName,
                         QGraphicsItem * parent) :
            RobotSprite(w_canvas, fileName, parent),pathLine(0)
{
    pathLine = dynamic_cast<DrawingLine *>(DrawingsFactory::factoryDrawingItem(DrawingItem::Line,scene(), "black",""));
    show();
}


NomadSprite::~NomadSprite()
{
    if(pathLine!=0){
        delete pathLine;
        pathLine = 0;
    }
}

void NomadSprite::updatePathLine(double dir)
{
    QPointF d  = NDMath::polarToCartesian(50, dir * NDMath::RAD2GRAD);

    pathLine->setValidReading(true); 
    pathLine->setShowColor(true);           
    pathLine->setGeometry(x(),y(),x()+ d.x(), y() - d.y());
    pathLine->setReadingCoords(x()+ d.x(), y() - d.y());
    pathLine->setVisible(true);

}
