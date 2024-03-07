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
#include "scalekalmanalgo.h"
#include "scalespace.h"
#include "kalmanset.h"
// #include "kalmansegment.h"

ScaleKalmanAlgo::ScaleKalmanAlgo(): Algoritmo()
{
  scaleSpace = 0;
  set = 0;
}


ScaleKalmanAlgo::~ScaleKalmanAlgo()
{
  delete scaleSpace;
  delete set;
}


double ScaleKalmanAlgo::msr(void)
{
    return 0;
}


void ScaleKalmanAlgo::setParams(QList< QVariant > params)
{
  numLevels = params[0].toInt();
  maxLevel = params[1].toInt();
  k_semi_amplitude = params[2].toDouble();
  area_lim = params[3].toDouble();
  k_filter = params[4].toDouble();
  confidenceLevel1 = params[5].toDouble();
  confidenceLevel2 = params[6].toDouble();
  verbose = params[7].toBool();
}

void ScaleKalmanAlgo::execute(void)
{
  
  areaSeeds = scaleSpace->perform(k_filter, area_lim, verbose);
  set->clean();
  set->doEstimation(scaleSpace->getMeasuredAreas(), areaSeeds, verbose);
  numSegments = set->getEndPoints().size();
//   set->printResults();
}

void ScaleKalmanAlgo::init(void)
{
  if(!scaleSpace) {
    scaleSpace = new ScaleSpace(numSensors, sensorsField, -sensorsField/2);

    scaleSpace->setSigmaLevels(numLevels, maxLevel, confidenceLevel1, k_semi_amplitude);
    scaleSpace->setDerivOrders(1);
  }

  if(!set) {
    set = new KalmanSet(numSensors, sensorsField, confidenceLevel2, -sensorsField/2, true);
    set->t_values();
    set->createSegments();
  }
  Algoritmo::init();
}

void ScaleKalmanAlgo::setMeasures(double * coord1, double * coord2)
{
    Algoritmo::setMeasures(coord1, coord2);
    scaleSpace->setRadius(coord1);
    set->setRadius(coord1);

//     if(verbose) { // para Cybersim debug
//     for(int i = 0; i < numSensors; i++)
//       outDebug << coord1[i];
//     }
//     fileDebug.flush();
//     fileDebug.seek(0);
}

void ScaleKalmanAlgo::initMeasures(double * coord1, double * coord2)
{
  Algoritmo::setMeasures(coord1, coord2);
  scaleSpace->initRadius(coord1);
  set->setRadius(coord1);
}

void ScaleKalmanAlgo::printResults(int minNumber)
{
  cout << "ScaleKalman ";
  scaleSpace->printResults();
  set->printResults(minNumber);
}

QVector< double > ScaleKalmanAlgo::getParam1Est() const
{
  return set->getRhoEst();
}

QVector< double > ScaleKalmanAlgo::getParam2Est() const
{
  return set->getPhiEst();
}

QVector< double > ScaleKalmanAlgo::getSigmaEst() const
{
  return set->getSigmaEst();
}

QVector< pair < int , int > > ScaleKalmanAlgo::getEndPoints() const
{
  return set->getEndPoints();
}

QVector< double > ScaleKalmanAlgo::getFilterData(void)
{
  return set->getFilterData();
}

QVector<pair < int, int> > ScaleKalmanAlgo::getConnectedAreas(void )
{
  return set->getConnectedAreas();
}

QVector< QMatrix2x2 > ScaleKalmanAlgo::getSw() const
{
  return set->getSw();
}

QVector< double > ScaleKalmanAlgo::getRhoEst() const
{
  return set->getRhoEst();
}

QVector< double > ScaleKalmanAlgo::getPhiEst() const
{
  return set->getPhiEst();
}

void ScaleKalmanAlgo::setSensorMaxVal(int sensorMaxVal) 
{
  this->sensorMaxVal = sensorMaxVal;
  scaleSpace->setSensorMaxVal(sensorMaxVal);
  
}

bool ScaleKalmanAlgo::getTooMuchMeasures() const
{
    return set->getTooMuchMeasures();
}
