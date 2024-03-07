//
// C++ Interface: areacentersegmentedcontrol
//
// Description:
//
//
// Author: Jose Manuel Cuadra Troncoso <jmcuadra@dia.uned.es>, (C) 2011
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef AREACENTERSEGMENTEDCONTROL_H
#define AREACENTERSEGMENTEDCONTROL_H

#include "areacenterreactivecontrol.h"
#include "areacenterpolygonsegmented.h"

class ScaleKalmanAlgo;
// class AreaCenterPolygonSegmented;

/**
  @author Jose Manuel Cuadra Troncoso <jmcuadra@dia.uned.es>
*/
class AreaCenterSegmentedControl : public virtual AreaCenterReactiveControl
{
public:
    AreaCenterSegmentedControl();

    virtual ~AreaCenterSegmentedControl();

    void connectIO(void);
    void initElements();

    //    int getType(void);

protected:
    void readGlobalPolygon(void);
    void createAreaCenterPolygon(void);

protected:
    QVector<double> filterData;
    QVector<double> angle;
    QVector<pair<int, int> > segmentsEnds;
    ScaleKalmanAlgo * segmentation;
    AreaCenterPolygonSegmented* segmentsPolygon;
    bool full360;
    int sonar180;

};

#endif
