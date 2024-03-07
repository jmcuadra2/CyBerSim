//
// C++ Interface: areacenterpolygon
//
// Description:
//
//
// Author: Jose Manuel Cuadra Troncoso <jmcuadra@dia.uned.es>, (C) 2009
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef AREACENTERPOLYGON_H
#define AREACENTERPOLYGON_H

#include <QPolygonF>
#include <QObject>

#include "../neuraldis/ndmath.h"

class AbstractWorld;

/**
  @author Jose Manuel Cuadra Troncoso <jmcuadra@dia.uned.es>
*/

 class AreaCenterPolygon : public QObject, public QPolygonF 
{
  Q_OBJECT
  public:
    enum AdvanceStatus {All, Right, Left, Conservative, Lost};
  public:
    AreaCenterPolygon();
    AreaCenterPolygon(QPointF advanceAngles);
    QPolygonF polygonLeft, polygonRight;

    virtual ~AreaCenterPolygon();

    void setAdvanceAngles(QPointF advanceAngles);
    QPointF getAdvanceAngles() const { return advanceAngles; }
    QPointF getCurrentAdvanceAngles() const { return currentAdvanceAngles; }
    
    virtual bool addSplitPoint(QPointF const& areapoint, bool suggested = false, AdvanceStatus preference = All);
    bool addSplitPointCenter(QPointF const& areapoint,bool suggested,AdvanceStatus preference);
    void calculateAdvancePolygon(void);

    void moveBySplitPoints();
    void setWorld(AbstractWorld* world) { this->world = world; }

    static double polygonArea(QPolygonF const& poly);
    double minimunDistance(QPolygonF const& poly);
    static QPointF calculateAreaCenter(QPolygonF const & polygon, QPointF defaultValue = QPointF());

    static bool polygonContains(QPointF const& point, QPolygonF const & polygon);
    
    bool isAccesible(QPointF const& areaCenter, QPolygonF const& polygon, QPolygonF const& polygonGlobal, double robotRoom = 0.0, AdvanceStatus side = All,bool isPolygonMain=true);
    virtual bool isAdvanceAccesible(QPointF const& areaCenter, double robotRoom = 0.0, AdvanceStatus side = All,bool isPolygonMain=true) { return isAccesible(areaCenter, advancePolygon, *this, robotRoom, side,isPolygonMain); }
        
    QPointF globalAreaCenter(void) const { return calculateAreaCenter(*this) ; };
    QPointF advanceAreaCenter(void);
    AdvanceStatus getAdvanceStatus(void) const { return advanceStatus; }
    void setAdvanceStatus(AdvanceStatus advanceStatus){ this->advanceStatus = advanceStatus; }
    double advanceRate(void);
//     double advanceRate(void) { return (double)advancePolygon.size()/numAdvanceVertex ; }
    void setAdvanceMaxRange(double advanceMaxRange);
    double getAdvanceMaxRange(void) { return advanceMaxRange ; };
    void reset(void);
    void resetLost(void);
    void setRobotDiameter(double robotDiameter) { this->robotDiameter = robotDiameter ; }
    void setSensorMaxRange(double sensorMaxRange);
    void setRobotMovement(double deltaX, double deltaY, double deltaHeading, double lVel, double rVel);

    void showAreaCenterPath(bool on) { showAreaCenter = on ; }
    bool showAreaCenterPath(void) { return showAreaCenter ; }

    double getSensorMaxRange(void) { return sensorMaxRange ; }

    bool addSplitPoints(QPointF const& polygonSplitPointLeft,QPointF const& polygonSplitPointRight);

  protected:
    void defaultValues(void);
    void setLost(void);
    void conservativeAdvancePolygon(void);
    QPolygonF inverseAdvancePolygon(void) const;

  signals:
    void changeSplitPoint(const QPointF& point, int);
    void hideSplitPoint(int);
  
  protected:
    QPointF advanceAngles, currentAdvanceAngles, lostAreaCenter;
    QPolygonF advancePolygon;
    AbstractWorld* world;
    AdvanceStatus advanceStatus;
    
    QPointF polygonSplitPointRight, polygonSplitPointLeft;
    QPointF initPolygonSplitPointRight, initPolygonSplitPointLeft;
        
    double advanceMaxRange;
    double sensorMaxRange;
    double robotDiameter;
    int numAdvanceVertex;

    double deltaX, deltaY, deltaHeading, lVel, rVel;

    bool showAreaCenter;

    bool suggestedSplit;

    bool corridorCollision;
    
};

#endif
