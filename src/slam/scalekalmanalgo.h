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
#ifndef SCALEKALMANALGO_H
#define SCALEKALMANALGO_H

#include "algoritmo.h"

#include <QVector>
#include <utility>

class ScaleSpace;
class KalmanSet;

/**
  @author Jose Manuel Cuadra Troncoso <jmcuadra@dia.uned.es>
*/
class ScaleKalmanAlgo : public Algoritmo
{
  public:
    ScaleKalmanAlgo();

    ~ScaleKalmanAlgo();

    double msr(void);
    void initMeasures(double* coord1, double* coord2 = 0);
    void setMeasures(double* coord1, double* coord2 = 0);
    void setParams(QList< QVariant > params);    
    void init(void);
    void printResults(int minNumber = 0);
    QVector< pair < int , int > > getEndPoints() const;
    QVector< double > getParam1Est() const;
    QVector< double > getParam2Est() const;
    QVector< double > getSigmaEst() const;
    QVector< double > getFilterData(void);
    QVector<pair < int, int> > getConnectedAreas(void);
    QVector< QMatrix2x2 > getSw() const;
    AlgoCoordinates getAlgoCoordinates(void) {return Polar ; };
    
    QVector< double > getRhoEst() const;
    QVector< double > getPhiEst() const;

    int minusDF(void) { return 2 ; } // num derivadas en mcg
    
    void execute(void);
    
    void setSensorMaxVal(int sensorMaxVal);
    bool getTooMuchMeasures() const;

  protected:
    void adaptResults(void) {}
    void mergerFirstLast(void) {}
//     void execute(void);

  protected:
    int numLevels, maxLevel;
    double k_semi_amplitude, area_lim, k_filter, confidenceLevel1, confidenceLevel2;
    ScaleSpace* scaleSpace;
    KalmanSet* set;
    QVector <std::pair <int, int> > areaSeeds;
};

#endif
