//
// C++ Interface: areacenterreactivecontrolrightleft
//
// Description:
//
//
// Author: Javier Fernandez Perez
//
// Copyright: See COPYING file that comes with this distribution
//
//

#ifndef AREACENTERREACTIVECONTROLRIGHTLEFT_H
#define AREACENTERREACTIVECONTROLRIGHTLEFT_H

#include "areacentersegmentedcontrol.h"
#include "../slam/scalekalmanalgo.h"

//class ScaleKalmanAlgo;

/**
  @author Javier Fernandez Perez
*/
class AreaCenterReactiveControlRightLeft : /*public QObject, */public AreaCenterSegmentedControl
{

  Q_OBJECT
  public:
    enum Behaviour {SearchCrossing, AvoidObstacle, DetectCrossing, GoCrossing};

  public:
    AreaCenterReactiveControlRightLeft();

    ~AreaCenterReactiveControlRightLeft();

    int getType(void);

    void setWorld(AbstractWorld* world);

    void connectIO(void);

    void initElements(void);

    void propagate(void);

    void doMonitor(void);

  protected:
    void setWorldsAdvanceAreaCenter(void);

    void createAreaCenterPolygons(void);

    void connectIO(AreaCenter *advanceAreaCent,AreaCenterPolygon *areaCenterPoly);

    void initElementsReactiveControlBeforePropagate(void);

    void initElements(AreaCenterPolygon *areaCenterPoly, double angleStart,double angleEnd);

    void readGlobalPolygon(double angleStart,double angleEnd, AreaCenterPolygon *areaCenterPol);

    void initElementsReactiveControlAfterPropagate(void);

    void searchCrossing(void);

    void avoidObstacle(void);

    void detectCrossing(void);

    QList<double> createSensorsDistances(void);

    QList<double> calculateSecondDerivateSensorsDistances(QList<double> sensorsDistances);

    QList<QString> calculateZoneTags(QList<double> secondDerivateSensorsDistances);

    QList<QPair<int,int> > calculateZonesLimits(QList<QString> zoneTags);

    QPair<int,int> calculateBestZoneLimits(QList<QPair<int,int> > zonesLimits);

    double calculateZoneDepth(QPair<int,int> zoneLimits);

//    void calculateSplitPoints(QPair<int,int> bestZoneLimits);

    void goCrossing(void);

    QVector< pair < int , int > > getFreeZones(QVector< pair < int , int > > endPointsZones);

    void deleteSmallZones(QVector< pair < int , int > > zones);

    pair<int,int> getRandomFreeZone(QVector< pair < int , int > > freeZones);

    void calculateSplitPoints(pair<int,int> zone);

    pair<int,int> getBestZone(QVector< pair < int , int > > endPointsZones);

    //void calculateAreaCentersRightLeft(void);

  protected:
    double centralSector, middleAngle;
    double startAngleLeft, endAngleLeft, startAngleRight, endAngleRight;
    AreaCenter *advanceAreaCenterLeft, *advanceAreaCenterRight;
    AreaCenterPolygon *areaCenterPolygonLeft, *areaCenterPolygonRight;
    Behaviour behaviour;
};

#endif
