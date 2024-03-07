//
// C++ Interface: areacenterpolygonsegmented
//
// Description:
//
//
// Author: Jose Manuel Cuadra Troncoso <jmcuadra@dia.uned.es>, (C) 2011
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef AREACENTERPOLYGONSEGMENTED_H
#define AREACENTERPOLYGONSEGMENTED_H

#include "areacenterpolygon.h"

/**
  @author Jose Manuel Cuadra Troncoso <jmcuadra@dia.uned.es>
*/
class AreaCenterPolygonSegmented : public AreaCenterPolygon
{
  public:
    AreaCenterPolygonSegmented();

    AreaCenterPolygonSegmented(QPointF advanceAngles);

    ~AreaCenterPolygonSegmented();

    bool addSplitPoint(QPointF const& areapoint, bool suggested, AdvanceStatus preference);
    bool isAdvanceAccesible(QPointF const& areaCenter, double robotRoom = 0.0, AdvanceStatus side = All);

    void setEndPoints(const QVector< pair < int , int > > & endPoints)
    {
      this->endPoints = endPoints;
    }
    void setConnectedAreas(const QVector< pair < int , int > >& connectedAreas)
    {
      this->connectedAreas = connectedAreas;
    }
    void setRhoEst(const QVector< double >& rhoEst)
    {
      this->rhoEst = rhoEst;
    }
    void setPhiEst(const QVector< double >& rhoEst)
    {
      this->phiEst = rhoEst;
    }

  protected:
    QVector< pair < int , int > > endPoints;
    QVector<double> rhoEst;
    QVector<double> phiEst;    
    QVector< pair < int , int > > connectedAreas;

};

#endif
