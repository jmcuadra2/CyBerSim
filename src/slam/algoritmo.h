/***************************************************************************
 *   Copyright (C) 2010 by Jose Manuel Cuadra Troncoso   *
 *   jmcuadra@dia.uned.es   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
#ifndef ALGORITMO_H
#define ALGORITMO_H
#include <QList>
#include <QVariant>
#include <utility>
#include <QVector>
#include <cmath>
#include <QFile>
#include <QDataStream>
#include <QGenericMatrix>

using namespace std;

/**
  @author Jose Manuel Cuadra Troncoso <jmcuadra@dia.uned.es>
*/
class Algoritmo
{
  public:
    Algoritmo();

    virtual ~Algoritmo();

    virtual void setParams(QList<QVariant> params) = 0;
    virtual void init(void) {};
    virtual void setMeasures(double *coord1, double * coord2 = 0);
    virtual void initMeasures(double *coord1, double * coord2 = 0) { setMeasures(coord1, coord2); };
    virtual void execute(void) = 0;
    virtual double msr(void) = 0;
    void setBeamFeatures(int numSensors, int sensorsField, int startAngle);
    virtual void printResults(int minNumber = 0) = 0;
    
    void setVerbose(bool verbose) { this->verbose = verbose;}
    
    virtual void setSensorMaxVal(int sensorMaxVal) {this->sensorMaxVal = sensorMaxVal ;}    

    virtual QVector< pair < int , int > > getEndPoints() const = 0;
    virtual QVector< double > getRhoEst() const = 0;
    virtual QVector< double > getPhiEst() const = 0;
    virtual QVector< double > getSigmaEst() const = 0;
    virtual QVector< double > getFilterData(void) = 0;
    virtual QVector<pair < int, int> > getConnectedAreas(void) = 0;

    enum AlgoType {ScaleKalman, Ransac, SplitMerge, Batch};
	enum AlgoCoordinates {Polar, Rectangular};
    static const double RAD2GRAD;

  protected:
    double *coord1, *coord2;
    int numSensors, sensorsField, startAngle;
    bool verbose;
    QFile fileDebug;
    QDataStream outDebug;    

//     QVector<pair<int, int> > endPoints;
//     QVector<double> rhoEst;
//     QVector<double> phiEst;
//     QVector<double> sigmaEst;
	
    int numSegments;
    int sensorMaxVal;

};

#endif
