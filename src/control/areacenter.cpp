//
// C++ Implementation: areacenter
//
// Description: 
//
//
// Author: Jose Manuel Cuadra Troncoso <jmcuadra@dia.uned.es>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//

#include "areacenter.h"

#include "../robot/robotposition.h"
#include "../world/abstractworld.h"
#include "../world/drawings.h"

AreaCenter::AreaCenter(const double& x, const double& y)
    : QObject()
{
    pathDrawing = 0;
    robotPosition = 0;
    world = 0;
    xWorld = 0;
    yWorld = 0;
    splitPointLeftDraw = 0;
    splitPointRightDraw = 0;
    centerPoint.setX(x);
    centerPoint.setY(y);
}

AreaCenter::~AreaCenter()
{
    if(pathDrawing) {
        delete pathDrawing;
    }

    if(splitPointLeftDraw) {
        delete splitPointLeftDraw;
    }

    if(splitPointRightDraw) {
        delete splitPointRightDraw;
    }
}

void AreaCenter::updatePath()
{
    if(pathDrawing && robotPosition) {
        double xt = centerPoint.x()/world->getWorldScale();
        double yt = centerPoint.y()/world->getWorldScale() *
                world->headingTransform(1);  // esto es un apaño para tener en cuenta el cambio de signo de las y's
        QPointF p_r = robotPosition->rotateWithRobot(xt, yt);
        xWorld = NDMath::roundInt(p_r.x());
        yWorld = NDMath::roundInt(p_r.y());

        pathDrawing->addPoint(QPoint(xWorld, yWorld));
    }
}

void AreaCenter::setWorld(AbstractWorld* world)
{
    this->world = world;
    QGraphicsScene* canvas = world->getCanvas();
    pathDrawing  = DrawingsFactory::factoryDrawingPath(canvas, "magenta", 50);
    pathDrawing->setShowColor(true);

    splitPointLeftDraw = DrawingsFactory::factoryDrawingItem(DrawingItem::Ellipse, canvas,
                                                             "black", "grey");
    splitPointLeftDraw->canvasItem()->setData(1, "splitPointLeft");
    splitPointLeftDraw->canvasItem()->setVisible(false);
    splitPointLeftDraw->setShowColor(false);
    splitPointLeftDraw->setValidReading(true);
    splitPointLeftDraw->setSize(20, 20);
    splitPointRightDraw = DrawingsFactory::factoryDrawingItem(DrawingItem::Ellipse, canvas,
                                                              "black", "grey");
    splitPointRightDraw->canvasItem()->setData(1, "splitPointRight");
    splitPointRightDraw->canvasItem()->setVisible(false);
    splitPointRightDraw->setShowColor(false);
    splitPointRightDraw->setValidReading(true);
    splitPointRightDraw->setSize(20, 20);

    splitPointLeftDraw->canvasItem()->setZValue(1500);
    splitPointRightDraw->canvasItem()->setZValue(1500);
}

void AreaCenter::setWorld(AbstractWorld* world, QString& color1, QString& color2)
{
    this->world = world;
    QGraphicsScene* canvas = world->getCanvas();
    pathDrawing  = DrawingsFactory::factoryDrawingPath(canvas, "magenta", 50);
    pathDrawing->setShowColor(true);
    splitPointLeftDraw = DrawingsFactory::factoryDrawingItem(DrawingItem::Ellipse, canvas,
                                                             color1, color2);
    splitPointLeftDraw->canvasItem()->setData(1, "splitPointLeft");
    splitPointLeftDraw->canvasItem()->setVisible(false);
    splitPointLeftDraw->setShowColor(false);
    splitPointLeftDraw->setValidReading(true);
    splitPointLeftDraw->setSize(20, 20);
    splitPointRightDraw = DrawingsFactory::factoryDrawingItem(DrawingItem::Ellipse, canvas,
                                                              color1, color2);
    splitPointRightDraw->canvasItem()->setData(1, "splitPointRight");
    splitPointRightDraw->canvasItem()->setVisible(false);
    splitPointRightDraw->setShowColor(false);
    splitPointRightDraw->setValidReading(true);
    splitPointRightDraw->setSize(20, 20);
    splitPointLeftDraw->canvasItem()->setZValue(1500);
    splitPointRightDraw->canvasItem()->setZValue(1500);
}

void AreaCenter::setRobotPosition(RobotPosition* robotPosition)
{
    this->robotPosition = robotPosition;
    if(pathDrawing) {
        double xt = centerPoint.x()/world->getWorldScale();
        double yt = centerPoint.y()/world->getWorldScale() *
                world->headingTransform(1);  // esto es un apaño para tener en cuenta el cambio de signo de las y's
        QPointF p_r = robotPosition->rotateWithRobot(xt, yt);
    }
}

void AreaCenter::resetPath(void)
{
    if(pathDrawing && robotPosition) {
        double xt = centerPoint.x()/world->getWorldScale();
        double yt = centerPoint.y()/world->getWorldScale() *
                world->headingTransform(1);  // esto es un apaño para tener en cuenta el cambio de signo de las y's
        QPointF p_r = robotPosition->rotateWithRobot(xt, yt);
        xWorld = NDMath::roundInt(p_r.x());
        yWorld = NDMath::roundInt(p_r.y());

        pathDrawing->resetPath(xWorld, yWorld);
        hideSplitPoint(1);
        hideSplitPoint(0);
    }
}

void AreaCenter::setColor(QColor const& color)
{
    if(pathDrawing) {
        if(prevColor != color) {
            pathDrawing->setColor(color);
            prevColor = color;
        }
    }
}

void AreaCenter::setVisible(bool on)
{
    if(pathDrawing)
        pathDrawing->setShowColor(on);
}


void AreaCenter::setSplitPoint(const QPointF& splitPoint, int side)
{

    double xt = (splitPoint.x() - 2)/world->getWorldScale();
    double yt = (splitPoint.y() - 2)/world->getWorldScale() *
            world->headingTransform(1);  // esto es un apaño para tener en cuenta el cambio de signo de las y's
    QPointF p_r = robotPosition->rotateWithRobot(xt, yt);
    int xW = NDMath::roundInt(p_r.x());
    int yW = NDMath::roundInt(p_r.y());

    if(side == 0) { // right
        this->splitPointRight = splitPointRight;
        if(splitPointRightDraw) {
            splitPointRightDraw->setReadingCoords(xW, yW);
            if(!splitPointRightDraw->canvasItem()->isVisible())
                splitPointRightDraw->setShowColor(true);
        }
    }
    else {
        this->splitPointLeft = splitPointLeft;
        if(splitPointLeftDraw) {
            splitPointLeftDraw->setReadingCoords(xW, yW);
            if(!splitPointLeftDraw->canvasItem()->isVisible())
                splitPointLeftDraw->setShowColor(true);
        }
    }
}

void AreaCenter::hideSplitPoint(int side)
{
    if(side == 0) {
        if(splitPointRightDraw) {
            if(splitPointRightDraw->canvasItem()->isVisible())
                splitPointRightDraw->setShowColor(false);
        }
    }
    else {
        if(splitPointLeftDraw) {
            if(splitPointLeftDraw->canvasItem()->isVisible())
                splitPointLeftDraw->setShowColor(false);
        }
    }
}
