//
// C++ Interface: areacenter
//
// Description: 
//
//
// Author: Jose Manuel Cuadra Troncoso <jmcuadra@dia.uned.es>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef AREACENTER_H
#define AREACENTER_H


#include <QColor>
#include <QGraphicsItem>
#include <QObject>

#include "../world/drawings.h"

#include "../neuraldis/ndmath.h"

class DrawingPath;
class RobotPosition;
class AbstractWorld;

/**
  @author Jose Manuel Cuadra Troncoso <jmcuadra@dia.uned.es>
*/
class AreaCenter : public QObject
{
    Q_OBJECT
public:
    AreaCenter(const double& x = 0.0, const double& y = 0.0);
    void setPoint(QPointF const& point)
    {
        centerPoint.setX(point.x());
        centerPoint.setY(point.y());
    }

    virtual ~AreaCenter();
    void updatePath();
    void setWorld(AbstractWorld* world);
    void setWorld(AbstractWorld* world,QString& color1, QString& color2);
    void setRobotPosition(RobotPosition* robotPosition);
    RobotPosition* getRobotPosition( ) {return robotPosition;}
    void resetPath(void);
    void setColor(QColor const& color);
    int getXWorld(void) const { return xWorld ; }
    int getYWorld(void) const { return yWorld ; }
    void setVisible(bool on);
    double x(void) { return centerPoint.x() ; }
    double y(void) { return centerPoint.y() ; }

public slots:
    void setSplitPoint(const QPointF& splitPoint, int side);
    void hideSplitPoint(int side);


protected:
    DrawingPath* pathDrawing;
    RobotPosition* robotPosition;
    AbstractWorld* world;
    int xWorld, yWorld;
    QPointF centerPoint, splitPointLeft, splitPointRight;
    DrawingItem* splitPointLeftDraw, * splitPointRightDraw;
    QColor prevColor;
    
};

#endif
