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

#include <iostream>
#include <vector>
#include <algorithm>
#include <iterator>

#include <boost/math/distributions/normal.hpp> // for normal_distribution
#include <boost/math/special_functions/bessel.hpp> // for normal_distribution

#include "scalespace.h"

using boost::math::normal; // typedef provides default type is double.
using boost::math::detail::bessel_traits;
using namespace std;

// bool sortAreasCriterium(ScaleSpace::areaStruct vec1, ScaleSpace::areaStruct vec2)
// {
//   return vec1.sensorStart < vec2.sensorStart;
// }

short ScaleSpace::maxLevelMax = 700; // max para bessel, máximo calculable 713
const double ScaleSpace::RAD2GRAD = 180.0/M_PI;
const double ScaleSpace::CERO = 1e-9;

ScaleSpace::ScaleSpace(size_t numSensors, double sensorField, double startAngle)
{
  this->numSensors = numSensors;
  full360 = sensorField == 360 ? true : false;
   
  this->sensorField = sensorField/RAD2GRAD;
  
  pad_size = 0;
  
  if(this->full360) {
    sensorAperture = (double)this->sensorField / this->numSensors;
    this->startAngle = -M_PI + sensorAperture; //////////// eh
//     pad_size = int((numSensors + 1)/2);
  }
  else {
    sensorAperture = (double)this->sensorField / (this->numSensors - 1);   
    this->startAngle = startAngle/RAD2GRAD;
//     pad_size = 0;
  }
    
  numLevels = 24;
  maxLevel = maxLevelMax;
//   zeroLevel = 3;
//   subZeroLevel = 8;
  bottomLevel = 24;
  confidenceLevel = 0.99;
//   prevPlanSize = 0;
//   cntPlansRad = -1;
  planSize = 0;

  dOrderEven = 2;
  dOrderOdd = 1;
  currentOrder = 0;

  cellSide = -1;
  cntRealPoint = 0;
  numCurve = -1;
  newLevel = -1;
  newSensor = -1;

  maxSensorVal = 8000;
  dummyVal = maxSensorVal*2;
  maxNoMeasurePoints = round(80/(sensorAperture*RAD2GRAD));

  angle = (double*) fftw_malloc(sizeof(double) * numSensors);
 
  for(uint i = 0; i < numSensors; i++)
    angle[i] = i*sensorAperture + this->startAngle;

  derivative3DMat = new double*[numLevels];
  for(short i = 0; i < numLevels; i++) {
    derivative3DMat[i] = new double[numSensors + 1];
    for(uint j = 0; j < numSensors + 1; j++)
      derivative3DMat[i][j] = 0.0;
  }
  
  initRectStruct(rectTree);
  
}


ScaleSpace::~ScaleSpace()
{

  fftw_free(angle);
  fftw_free(sigmaLevels);

  for(int i = 0; i < numLevels; i++) {
    fftw_free(smoothedRad[i]);
    fftw_free(productRadKer[i]);
    fftw_free(transformsKer[i]);
    fftw_destroy_plan(fftwInversePlans[i]);
    delete[] derivative3DMat[i];
  }
  
  delete[] derivative3DMat;
  fftw_free(transformsRad);
  fftw_free(paddedRadius);
  fftw_destroy_plan(fftwPlan);
  
  deleteRectStruct(rectTree);
  
}

// void ScaleSpace::setSigmaLevels(short numLevels, short maxLevel, short zeroLevel, short subZeroLevel, short bottomLevel, double confidenceLevel)
void ScaleSpace::setSigmaLevels(short numLevels, short maxLevel, double confidenceLevel, double k_semi_amplitude)
{
  if(maxLevel > maxLevelMax) {
    cout << "Maximun sigma2 overflow" << endl;
    return;
  }
  if(numLevels < 2) {
    cout << "Not enough levels" << endl;
    return;
  }  
  
  this->numLevels = numLevels;
//  this->maxLevel = maxLevel;

  this->confidenceLevel = confidenceLevel;
  
  normal gaussian;
  double kern_sigma = confidenceLevel;
  if(kern_sigma < 1)
    kern_sigma = cdf(gaussian, 1 - (1 - confidenceLevel)/2);
  if(kern_sigma < 1)
    kern_sigma = 1;

  if(maxLevel < maxLevelMax) {
    int semi_amp = int(numSensors/k_semi_amplitude) + 1;
    this->maxLevel = (semi_amp/confidenceLevel)*(semi_amp/confidenceLevel);

    if(this->maxLevel > 700)
      this->maxLevel = maxLevel;
  }
  else 
    this->maxLevel = maxLevel;
  levelPoints.resize(numLevels);

  sigmaLevels = (double*) fftw_malloc(sizeof(double) * this->numLevels);
  logSigmaLevels = (double*) fftw_malloc(sizeof(double) * this->numLevels);
      
  if(this->full360) 
     pad_size = round(kern_sigma*sqrt(this->maxLevel)) + 1;
//      pad_size = int(numSensors/2) + 1;
  else
    pad_size = 0;       

  if(full360)
    interpGrid.resize(this->numLevels, numSensors + 1, 4); // un punto extra al final para cerrar el cìrculo
  else
    interpGrid.resize(this->numLevels, numSensors, 4);
  interpGrid.fill(-1000);

  double trans_l = (log(this->maxLevel) + 2*log(kern_sigma)) / (this->numLevels - 1); //  log(1/kern_sigma**2)
                                      // para que primer kernel tenga semilong = 1           
  for(int i = 0; i < numLevels; i++) {
    logSigmaLevels[i] = - 2*log(kern_sigma) + i * trans_l;
    sigmaLevels[i] = exp(logSigmaLevels[i]);
  }

  smoothedRad.resize(this->numLevels);

  double t = sigmaLevels[this->numLevels - 1];
  int semi_long = int(kern_sigma*sqrt(t)+0.5)+1; // 3.5 suaviza casi como 6
  planSize = int(pow(2, int(log(numSensors + 2*pad_size + 2*semi_long)/log(2)) + 1));

  paddedRadius = (double*) fftw_malloc(sizeof(double) * planSize);
  transformsRad = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * planSize);
  fftwPlan = fftw_plan_dft_r2c_1d(planSize, paddedRadius, transformsRad, FFTW_MEASURE);

  for(int i = 0; i < this->numLevels; i++) {
    t = sigmaLevels[i];

    if(abs(int(kern_sigma*sqrt(t)) - kern_sigma*sqrt(t)) < 0.00000001) //entero
      semi_long = round(kern_sigma*sqrt(t));
    else      
      semi_long = int(kern_sigma*sqrt(t)) + 1; // 3.5 suaviza casi como 6,,,, +1

//     semi_long = int(kern_sigma*sqrt(t)+0.5)+1;

    kernSemiSize.push_back(semi_long);
    kernels.push_back((double*) fftw_malloc(sizeof(double) * planSize));
    
    productRadKer.push_back((fftw_complex*) fftw_malloc(sizeof(fftw_complex) * planSize));
    transformsKer.push_back((fftw_complex*) fftw_malloc(sizeof(fftw_complex) * planSize));    
    
    fftw_plan plan = fftw_plan_dft_r2c_1d(planSize, kernels[i], transformsKer[i], FFTW_MEASURE);

    for(int n = 0; n < planSize; n++) {
      if(n < semi_long + 1) {
        double bessel = exp(-t)*boost::math::cyl_bessel_i(n - semi_long,t)/planSize;
        kernels[i][n] = bessel;
        kernels[i][2*semi_long - n ] = bessel;
      }
      else if(n > 2*semi_long)
        kernels[i][n] = 0;
    }
    fftw_execute(plan);

    fftw_destroy_plan(plan);

    smoothedRad[i] = (double*) fftw_malloc(sizeof(double) * planSize);
    fftwInversePlans.push_back(fftw_plan_dft_c2r_1d(planSize, productRadKer[i], smoothedRad[i], FFTW_MEASURE));

  }

}

void ScaleSpace::initRadius(double radius[])
{
  if(full360) {
    for(uint j = numSensors + 2*pad_size; j < planSize; j++)
      paddedRadius[j] = 0;  
  }
  else {
    for(uint j = numSensors; j < planSize; j++)
      paddedRadius[j] = 0;
  }

  setRadius(radius);
  
}

void ScaleSpace::setRadius(double radius[])
{
  QVector <pair <int, int> > noMeasuredAreas;
  int noMeasureCounter = 0, totalNoMeasureCounter = 0;
  double start_ang = 0;
  
  if(full360) {
    for(uint j = 0; j < numSensors; j++) {
      if(round(radius[j]) >= maxSensorVal || radius[j] <= 0) {
        if(!noMeasureCounter) {
          pair<int, int> ends;
          ends.first = j;
          ends.second = j;
          noMeasuredAreas.push_back(ends);
          start_ang = angle[j];
        }
        else {
          if(noMeasureCounter%maxNoMeasurePoints == 0) // no rectas imposibles de largas
            start_ang = angle[j];
          noMeasuredAreas.back().second = j;
        }
        radius[j] = abs(dummyVal/cos(angle[j] - start_ang));
        noMeasureCounter++;
      }
      else {
        if(noMeasureCounter == 1) {
//           noMeasuredAreas.pop_back();
//           if(j > 1 && j < numSensors)
//             radius[j - 1] = (radius[j - 2] + radius[j])/2;
//           else if(j == 1) 
//             radius[0] = max(radius[1] - (radius[2] - radius[1]), 0.0);
          if(j > 1 && j < numSensors) {
            noMeasuredAreas.pop_back();
            radius[j - 1] = (radius[j - 2] + radius[j])/2;
            totalNoMeasureCounter--;
          } 
//           totalNoMeasureCounter--;
        }
        totalNoMeasureCounter += noMeasureCounter;
        noMeasureCounter = 0;
      }      
    }
    totalNoMeasureCounter += noMeasureCounter;
    
    for(uint j = 0; j < pad_size; j++) 
      paddedRadius[j] = radius[numSensors - pad_size + j];
      
    for(uint j = pad_size; j < numSensors + pad_size; j++) {
      paddedRadius[j] = radius[j - pad_size];
    }
    for(uint j = numSensors + pad_size; j < numSensors + 2*pad_size; j++)
      paddedRadius[j] = radius[j - numSensors - pad_size];
  }
  else {
    for(uint j = 0; j < numSensors; j++) {
      paddedRadius[j] = radius[j];
      double rr;
      if(j >= 84)
        rr = radius[j];
      
      if(round(paddedRadius[j]) >= maxSensorVal || paddedRadius[j] <= 0) { // 100 dentro de robot, evitar 0 por problemas en scaleSpace "Celda con num impar de intersecciones"
        if(!noMeasureCounter) {
          pair<int, int> ends;
          ends.first = j;
          ends.second = j;
          noMeasuredAreas.push_back(ends);
          start_ang = angle[j];
        }
        else {
          if(noMeasureCounter%maxNoMeasurePoints == 0) // no rectas imposibles de largas
            start_ang = angle[j];        
          noMeasuredAreas.back().second = j;
        }
        radius[j] = abs(dummyVal/cos(angle[j] - start_ang));
        paddedRadius[j] = radius[j];
        noMeasureCounter++;
      }
      else {
        if(noMeasureCounter == 1) {
//           noMeasuredAreas.pop_back();
//           if(j > 1 && j < numSensors)
//             radius[j - 1] = (radius[j - 2] + radius[j])/2;
//           else if(j == 1) {
//             if(radius[2] >= maxSensorVal || radius[j] < 0)
//               radius[0] = radius[1]*1.001; // para que no sean iguales
//             else
//               radius[0] = max(radius[1] - (radius[2] - radius[1]), 0.0);          
          if(j > 1 && j < numSensors) {
            noMeasuredAreas.pop_back();
            radius[j - 1] = (radius[j - 2] + radius[j])/2;
            paddedRadius[j - 1] = radius[j - 1];
            totalNoMeasureCounter--;
          }         
        }
        totalNoMeasureCounter += noMeasureCounter;
        noMeasureCounter = 0;
      }  
    }
  }
/*
  for(int i = 0;i < pad_size; i++)
    cout << i << " " << paddedRadius[i] << "   ";
  cout << endl;
    for(int i = pad_size;i < numSensors + pad_size; i++)
    cout << i << " " << paddedRadius[i] << "   ";
  cout << endl;
    for(int i = numSensors + pad_size;i < numSensors + 2*pad_size; i++)
    cout << i << " " << paddedRadius[i] << "   ";
  cout << endl;*/
  
//   if(noMeasureCounter == 1) {
//     noMeasuredAreas.pop_back();
//     totalNoMeasureCounter--;
//   }

  if(!totalNoMeasureCounter) // cuando no hay ninguna medida buena sale totalNoMeasureCounter == 0
    totalNoMeasureCounter = noMeasureCounter;

  if(totalNoMeasureCounter == numSensors) 
    noneMeasures = true;
  else 
    noneMeasures = false;
  computeMeasuresAreas(noMeasuredAreas);
  
  fftw_execute(fftwPlan);
  
}

void ScaleSpace::setDerivOrders(short firstDOrder)
{
  if(firstDOrder%2 == 1) {
    this->dOrderEven = firstDOrder + 1;
    this->dOrderOdd = firstDOrder;    
  }
  else {
    this->dOrderEven = firstDOrder;
    this->dOrderOdd = firstDOrder + 1;  
  }

}

void ScaleSpace::derivative(void)
{
  int ini, fin;
  double* smoothRad;
    
  for(int idx = 0; idx <  numLevels; idx++) {
    double* deriv = derivative3DMat[idx];
    int semi_long = kernSemiSize[idx];
    smoothRad = smoothedRad[idx];
     
    ini = semi_long + pad_size - 1;
    if(full360)
      fin = numSensors + ini; // fin = numSensors + 1 + ini - 1; 1 que se añade para cerrar el circulo
    else
      fin = numSensors - 1 + ini;
      
    if(currentOrder == 1) {
//       ini = semi_long + pad_size;
//       fin = numSensors + ini; //numSensors+2*semi_long+1-1-semi_long+1
      for(int i = ini; i <= fin; i++) {
        deriv[i - ini] = smoothRad[i + 1] - smoothRad[i];
        deriv[i - ini] = abs(deriv[i - ini]) < CERO ? CERO : deriv[i - ini];
      }
    }
    else if(currentOrder == 2) {
//       ini = semi_long + pad_size - 1;
//       fin = numSensors + ini; //numSensors+2*semi_long+1-1-semi_long+1
      for(int i = ini; i <= fin; i++) {
        deriv[i - ini] = smoothRad[i + 2] - 2*smoothRad[i+1] + smoothRad[i];
        deriv[i - ini] = abs(deriv[i - ini]) < CERO ? CERO : deriv[i - ini];
      }        
    }
    else if(currentOrder == 3) {
//       ini = semi_long + pad_size - 2; 
//       fin = numSensors + ini; //numSensors+2*semi_long+1-1-semi_long+1
      for(int i = ini; i <= fin; i++) {
        deriv[i - ini] = smoothRad[i + 3] - 3*smoothRad[i + 2] + 3*smoothRad[i+1] - smoothRad[i];
        deriv[i - ini] = abs(deriv[i - ini]) < CERO ? CERO : deriv[i - ini];
      }        
    }
    else if(currentOrder == 4) {
//       ini = semi_long + pad_size - 3;
//       fin = numSensors + ini; //numSensors+2*semi_long+1-1-semi_long+1
      for(int i = ini; i <= fin; i++) {
        deriv[i - ini] = smoothRad[i + 4] - 4*smoothRad[i + 3] + 6*smoothRad[i+2] - 4*smoothRad[i+1] + smoothRad[i];
        deriv[i - ini] = abs(deriv[i - ini]) < CERO ? CERO : deriv[i - ini];
      }        
    }
//       fin = padNumSensors + semi_long - 2; //numSensors+2*semi_long+1-1-semi_long+1
//       for(int i = semi_long - 2; i <= fin; i++)
//         deriv[i - semi_long + 2] = smoothRad[i + 4] - 4*smoothRad[i + 3] + 6*smoothRad[i+2] - 4*smoothRad[i+1] + smoothRad[i];
//     }
  }

}

QVector <pair <int, int> > ScaleSpace::perform(double k_filter, double area_lim, bool verbose)
{
  areaSeeds.clear();
  
  if(noneMeasures)
    return areaSeeds;
    
  smooth();
  matRectangles.clear();
  matRectangles.resize(2);
  for(short i = 0; i <= 1; i++) {
    if(i)
      currentOrder = dOrderOdd;
    else
      currentOrder = dOrderEven;
    derivative();
//     cout << "Derivative: " << currentOrder << endl << endl;
    digitalContours();

    statsContours(k_filter, area_lim); // 

//     for(int i = 0; i < levelPoints.size(); i++) {
//       cout << "Level: " << i << "  con puntos: " <<  levelPoints[i].size() << endl;
//       for(int k=0; k < levelPoints[i].size(); k++)
//         cout << " " << levelPoints[i][k] << endl;
//       cout << endl;
//     }

    initRectStruct(rectTree);
    scaleTree(&rectTree, -1, numSensors);

//     vector <vector <int> > matRect = tree2Matrix(&rectTree, -1, false);
    vector <matRectStruct > matRect = tree2Matrix(&rectTree, -1, false);

    int matSize = matRect.size();
    for(int k = 0; k < matSize; k++) {
      if(matRect[k].stable) // estable
        matRectangles[i].push_back(matRect[k].x);
    }

//     cout << "matRect: " <<  matRect.size() << endl;
//     for(uint k = 0; k < matRect.size(); k++) {
//       for(uint j = 0; j < matRect[k].size(); j++)
//           cout << matRect[k][j] << " ";
//       cout << endl;
//     }

//     cout << "matRectangles: " <<  matRectangles[i].size() << endl;
//     for(uint j = 0; j < matRectangles[i].size(); j++)
//         cout << matRectangles[i][j] << " ";
//     cout << endl;

    if(full360)
      sort(matRectangles[i].begin(), matRectangles[i].end());
//     cout << "matRectangles: " <<  matRectangles[i].size() << endl;
//     for(uint j = 0; j < matRectangles[i].size(); j++)
//         cout << matRectangles[i][j] << " ";
//     cout << endl;

    deleteRectStruct(rectTree);
    
  }
  computeSeeds(); // test

   if(verbose) {
     cout << "Seeds: " <<  areaSeeds.size() << endl;
     for(uint j = 0; j < areaSeeds.size(); j++)
       cout << areaSeeds[j].first << ", " << areaSeeds[j].second << endl;
     cout << endl;
   }
  
  return areaSeeds;
}

void ScaleSpace::smooth(void)
{
  fftw_complex* prod;
  fftw_complex* t_rad;
  fftw_complex* t_ker;
  double t_rad_0;
  double t_rad_1;
  double t_ker_0;
  double t_ker_1;
  double prod_0;
  double prod_1;

  for(short i = 0; i < numLevels; i++) {
    prod = productRadKer[i];
    t_ker = transformsKer[i];
    t_rad = transformsRad;

    for(uint j = 0; j < planSize; j++) {
//      *prod = (*t_rad) * (*t_ker);

      t_rad_0 = *t_rad[0];
      t_rad_1 = *t_rad[1];
      t_ker_0 = *t_ker[0];
      t_ker_1 = *t_ker[1];
      prod_0 = t_rad_0 * t_ker_0 - t_rad_1 * t_ker_1;
      prod_1 = t_rad_0 * t_ker_1 + t_rad_1 * t_ker_0;
      *prod[0] = prod_0;
      *prod[1] = prod_1;
      *prod[0] = (*t_rad[0])*(*t_ker[0]) - (*t_rad[1])*(*t_ker[1]);
      *prod[1] = (*t_rad[0])*(*t_ker[1]) + (*t_rad[1])*(*t_ker[0]);
      prod++;
      t_rad++;
      t_ker++;
    }
    
    fftw_execute(fftwInversePlans[i]);

  }

}

void ScaleSpace::digitalContours(void )
{
    
  int n_angs;
  if(full360)
    n_angs = numSensors + 1;
  else
    n_angs = numSensors;

  contourPoints.clear();
  splitContours.clear();
  interpGrid.fill(-1000);
//   for (int i = 0; i < numLevels; i++)
//     levelPoints[i].clear();

  newLevel = -1;
  newSensor = -1;
  numCurve = -1;
  bool split_c = false;
  for(int j = 0; j < n_angs-1; j++) {
    numCurve++;
    QVector< pair<double, int> > digital_points;
    contourPoints.push_back(digital_points);
    
    cntRealPoint = 0;
    split_c = interpolate(0, j);
    if(!cntRealPoint) {
      numCurve--;
      contourPoints.pop_back();
    }
    splitContours.push_back(split_c);
    if(round(interpGrid(0, j, 0)) == -2000) // no inicia curva comprobado
      continue;

    if(cellSide == -1) // ya inició curva comprobado
      continue;
      
    split_c = false;
    while(cellSide > -1) 
      split_c = interpolate(newLevel, newSensor) || split_c;
//     splitContours.back() = split_c;
    if(cntRealPoint) 
      splitContours.back() = split_c;
//       cout << "Curve: " << numCurve << "  con puntos: " <<  cntRealPoint << endl;
//       cout << "CurveD: " << contourPoints.size()-1 << "  con puntos: " <<  contourPoints.back().size() << endl;
//       for(int k=0; k < contourPoints.back().size(); k++)
//         cout << contourPoints.back()[k].first << " " << contourPoints.back()[k].second << endl;
//       cout << endl;
    else {
      numCurve--;
      contourPoints.pop_back();
    }    

  }
}

bool ScaleSpace::interpolate(int i, int j)
{
  double lastLevel = -1;
//   int lastLevel = 0;
  QVector< pair<double, int> > & digital_points = contourPoints.back();
  bool split_contour = false;

  int newCellSide = -1;
  int currentCellSide = cellSide;
  newLevel = -1;
  newSensor = -1;

  double init_ang = -1000;
  double incre_ang = -1000;
  double init_val = -1000;
  double incre_val = -1000;
  double init_tt = -1000;
  double incre_tt = -1000;
  int add1 = 0;
//   int scan_sensors;


  if(full360) {
    add1 = -1; // porque angle[0] = -pi+ sensorAperture, no solo -pi
//     if(currentOrder <= 1)
//       add1 = 0;
//     else if(currentOrder == 3)
//       add1 = -1;
//     scan_sensors = numSensors + 1;
  }
  else {
    if(currentOrder <= 1)
//       add1 = 1;
      add1 = 0;
    else if(currentOrder == 3)
      add1 = 0;
//     scan_sensors = numSensors;
  }

  if(full360 && j < 0) {
    j = numSensors + j;
    split_contour = true;
  } 
  if(full360 && j >= numSensors) {
    j = j - numSensors;
    split_contour = true;
  }


  int j_1 = j + 1;
  double ang_1;
  double d1, d2;
 // ang_1 = ang(j_1);
  if(full360) {
//     if(j_1 == numSensors) {   ///// nuevo
//       j_1 = j_1 - numSensors;
// //       ang_1 = angle[numSensors - 1] + (j_1 + 1)*sensorField/numSensors;
//       ang_1 = angle[j_1];
//       split_contour = true;
//     }
//     else {
      if(j == numSensors - 1) {
        j_1 = 0;
        ang_1 = angle[numSensors - 1] + sensorField/numSensors;
      }
      else        
        ang_1 = angle[j_1];
//     }
  }    
  else if(j_1 < numSensors)
    ang_1 = angle[j_1];
  
//   pair<double, double> point;
// //   pair<double, int> level_point;
  pair<double, int> point;

  if(currentCellSide == -1) { // inicio línea base
    if(round(interpGrid(i, j, 0)) == -1000) {
      d1 = derivative3DMat[i][j];
      d2 = derivative3DMat[i][j+1];
      
      if((d1 * d2) < 0.0/* && abs(d1) > CERO && abs(d2) > CERO*/) {
        double inter_ang = angle[j] + (ang_1 - angle[j]) * abs(d1 / (d1 - d2));
        interpGrid(i, j, 0) = inter_ang;
        currentCellSide = 0;

        lastLevel = logSigmaLevels[0];
//         lastLevel = 0;
        cntRealPoint++;
//         point.first = int((inter_ang + sensorField/2)*(numSensors - 1)/sensorField + 0.5) + add1;
//         point.first = int((inter_ang + sensorField/2)/sensorAperture + 0.5) + add1;
        point.first = (inter_ang + sensorField/2)/sensorAperture + add1;
//         point.second = lastLevel;
        point.second = 0;
        digital_points.push_back(point);
//         level_point = make_pair(point.first, numCurve);
//         levelPoints[0].push_back(level_point);
      }
      else {
        interpGrid(i, j, 0) = -2000;
        return split_contour;
      }
    }
    else
      return split_contour;
  }

  int ind[4][2] = {{0, 0}, {0, 1}, {1, 1}, {1, 0}};
  vector< pair<int, double> > dd1(4);
  int kk;
  for(int kd = 0; kd < 4; kd++) {
//     kk = (cellSide + kd + 1)%4;
    kk = (currentCellSide + kd + 1)%4;
    
    int j_11 = j + ind[kk][1];
    if(full360 && j_11 >= numSensors)
      j_11 = j_11 - numSensors;
    
    dd1[kd].first = kk;
    double dd = derivative3DMat[i + ind[kk][0]][j_11];
    dd1[kd].second = derivative3DMat[i + ind[kk][0]][j_11];
  }

  int cambios_signo = 1;
  int pos_cambio = -1;
  for(int kd = 0; kd < 3; kd++) {
    if(round(interpGrid(i, j, dd1[kd].first)) == -1000) {
      d1 = dd1[kd].second;
      d2 = dd1[kd+1].second;
      if((d1 * d2) < 0.0) {
//       if(dd1[kd].second*dd1[kd+1].second <= 0) {
        cambios_signo = cambios_signo + 1;
        pos_cambio = kd;
      }
    }      
  }

  if(cambios_signo%2 == 0) {
    if(cambios_signo == 4) {
      double val_center = 0;
      int corners_mas0;
      int corners_mas1;
      int* corners_m = &corners_mas0;
      for(int i = 0; i < 4; i++) {
        if(dd1[i].second > 0) {
          *corners_m = dd1[i].first%2;
          corners_m = &corners_mas1;
        }
        val_center += dd1[i].second;
      }
      val_center /= 4.0;

      if(corners_mas0 == 1 && corners_mas1 == 1) { // 1 y 3
        if(val_center > 0) {
          for(int i = 0; i < 4; i++) {
            if(dd1[i].first == 3 - currentCellSide) {// 0 con 3 y 1 con 2
              pos_cambio = i;
              break;
            }
          }
        }
        else {
          pos_cambio = 5 -  currentCellSide; //0 con 1 y 2 con 3
          if(pos_cambio > 3) {
            pos_cambio = pos_cambio - 4;
          }

          for(int i = 0; i < 4; i++) {
            if(dd1[i].first == pos_cambio) {
              pos_cambio = i;
              break;
            }
          }
        }
      }
      else {
        if(val_center < 0) {
          for(int i = 0; i < 4; i++) {
            if(dd1[i].first == 3 - currentCellSide) { // 0 con 3 y 1 con 2
              pos_cambio = i;
              break;
            }
          }
        }
        else {
          pos_cambio = 5 -  currentCellSide; //0 con 1 y 2 con 3
          if(pos_cambio > 3) {
            pos_cambio = pos_cambio - 4;
          }
          for(int i = 0; i < 4; i++) {
            if(dd1[i].first == pos_cambio) {
              pos_cambio = i;
              break;
            }
          }
        }
      }
    }
//     if(j == 148 || j == 149) {
//       for(int kd = 0; kd < 4; kd++)
//         cout << dd1[kd].second << " ";
//       cout << endl;
//     }
  }
  else {
    // los problemas suelen aparecen con 1 intersenccion por NaN e inf en las derivadas
    // ¡¡¡ Hay que intentar ser que pasa !!!
    // por ahora comento la salida de errores para que no de la lata

//    printf("Celda (%d, %d) con %d intersecciones\n", i, j, cambios_signo);
    cntRealPoint = 0;
    
//    int c_l = digital_points.size();
//    for(int q = 0; q < c_l; q++)
//      cout << digital_points[q].first << " " << digital_points[q].second << " ";
//    cout << endl;
//    for(int kd = 0; kd < 4; kd++)
//      cout << dd1[kd].second << " ";
//    cout << endl;
    
    digital_points.clear();
//     if(c_l == 1) {
//       levelPoints[i].pop_back();
//     }
    cellSide = -1;
    return split_contour;
  }

  if(round(interpGrid(i, j, dd1[pos_cambio].first)) == -1000) {
    int next_pos = (pos_cambio + 1)%4;
    d1 = dd1[pos_cambio].second;
    d2 = dd1[next_pos].second;
    if((d1 * d2) < 0.0) {
//     if(dd1[pos_cambio].second*dd1[next_pos].second <= 0) {
      init_ang = angle[j];
      incre_ang = ang_1 - angle[j];
      init_tt = logSigmaLevels[i];
      incre_tt = logSigmaLevels[i + 1] - logSigmaLevels[i];
      if(dd1[pos_cambio].first%2 == 0) {
        init_val = init_ang;
        incre_val = incre_ang;
      }
      else {
        init_val = init_tt;
        incre_val = incre_tt;
      }

      double val;
      if(dd1[pos_cambio].first <= 1) {
        val = init_val + incre_val * abs(dd1[pos_cambio].second / (dd1[pos_cambio].second - dd1[next_pos].second));
        interpGrid(i, j, dd1[pos_cambio].first) = val;
        newCellSide = dd1[pos_cambio].first + 2;
        if(dd1[pos_cambio].first == 0) {
          cntRealPoint++;
          lastLevel = logSigmaLevels[i];
// //           lastLevel = i;

//           point.first = int((val + sensorField/2)/sensorAperture + 0.5) + add1;
          point.first = (val + sensorField/2)/sensorAperture + add1;
//           point.second = lastLevel;
          point.second = i;
          digital_points.push_back(point);
//           level_point = make_pair(point.first, numCurve);
//           levelPoints[i].push_back(level_point);
// //           levelPoints[i].push_back(make_pair(point.first, numCurve));
        }
        else {
          cntRealPoint++;
          lastLevel = val;
        }
      }
      else {
        val = init_val + incre_val * abs(dd1[next_pos].second/(dd1[next_pos].second - dd1[pos_cambio].second));
        interpGrid(i, j, dd1[pos_cambio].first) = val;
        newCellSide = dd1[pos_cambio].first - 2;

        if(dd1[pos_cambio].first == 2) {
          cntRealPoint++;
          lastLevel = logSigmaLevels[i + 1];
//           lastLevel = i + 1;

//           point.first = int((val + sensorField/2)/sensorAperture + 0.5) + add1;
          point.first = (val + sensorField/2)/sensorAperture + add1;
//           point.second = lastLevel;
          point.second = i + 1;
          digital_points.push_back(point);
//           level_point = make_pair(point.first, numCurve);
//           levelPoints[i+1].push_back(level_point);
// //           levelPoints[i+1].push_back(make_pair(point.first, numCurve));
        }
        else {
          cntRealPoint++;
          lastLevel = val;
        }
      }

      int ind_nuevo[4][2] = {{1, 0}, {0, -1}, {-1, 0}, {0, 1}};
      newLevel = i + ind_nuevo[newCellSide][0];
      newSensor = j + ind_nuevo[newCellSide][1];

      if(full360) {
        if(newSensor < 0)
          newSensor += numSensors;
        else if(newSensor >= numSensors)
          newSensor -= numSensors;
      }
      
      if(newLevel >= 0 && newSensor >= 0)
        interpGrid(newLevel, newSensor, newCellSide) = val;
//       else

    }
    else
      interpGrid(i, j, dd1[pos_cambio].first) = -2000;
  }

  if(abs(lastLevel - logSigmaLevels[0]) < CERO && newCellSide == 2) {
//   if(lastLevel == 0) {
    cellSide = -1;
    return split_contour;
  }

  if(j == 0 && newCellSide == 1) { // salio por la izq.
    double inter_tt = logSigmaLevels[newLevel] + (logSigmaLevels[newLevel+1] - logSigmaLevels[newLevel]) * abs(derivative3DMat[newLevel][j]/(derivative3DMat[newLevel][j] - derivative3DMat[newLevel][j+1]));
    interpGrid(i, j, 1) = inter_tt;
    cntRealPoint++;

    if(!full360) {
      for(int n = newLevel; n >= 0; n--) {
        cntRealPoint++;
        lastLevel = logSigmaLevels[n];
//         lastLevel = n;
        
//         point.first = int((angle[0] +sensorField/2)/sensorAperture + 0.5) + add1;
        point.first = (angle[0] +sensorField/2)/sensorAperture + add1;
//         point.second = lastLevel;
        point.second = n;
        digital_points.push_back(point);
//         level_point = make_pair(point.first, numCurve);
//         levelPoints[n].push_back(level_point);
// //         levelPoints[n].push_back(make_pair(point.first, numCurve));

      }
      cellSide = -1;
      return split_contour;
    }

    cellSide = newCellSide;
    return split_contour;
  }

  if(abs(lastLevel - logSigmaLevels[numLevels - 1]) < CERO) { // points(2, $) == logSigmaLevels[$)
//   if(lastLevel == numLevels - 1) { // points(2, $) == logSigmaLevels[$)
    cellSide = -1;
    return split_contour;
  }

  if(newSensor == numSensors - 1) {
    if(!full360) {
      double inter_tt = logSigmaLevels[newLevel] + (logSigmaLevels[newLevel + 1] - logSigmaLevels[newLevel]) * abs(derivative3DMat[newLevel][newSensor]/(derivative3DMat[newLevel][newSensor] - derivative3DMat[newLevel+1][newSensor]));
      interpGrid(newLevel, newSensor, 1) = inter_tt;
      cntRealPoint++;
      lastLevel = inter_tt;
    
      for(int n = newLevel; n >= 0; n--) {
        cntRealPoint++;
        lastLevel = logSigmaLevels[n];
//         lastLevel = n;

//         point.first = int((angle[numSensors-1] +sensorField/2)/sensorAperture + 0.5) + add1;
        point.first = (angle[numSensors-1] +sensorField/2)/sensorAperture + add1;
//         point.second = lastLevel;
        point.second = n;
        digital_points.push_back(point);
//         level_point = make_pair(point.first, numCurve);
//         levelPoints[n].push_back(level_point);
// //         levelPoints[n].push_back(make_pair(point.first, numCurve));
      }
      cellSide = -1;
      return split_contour;
    }
    
  }
  cellSide = newCellSide;
  return split_contour;
}


void ScaleSpace::scaleTree(rectStruct* subtree, double sensor_start, double sensor_end)
{

//   int levels = levelPoints.size();
// 

  int subtree_ini = subtree->initLevel;
  int& subtree_end = subtree->endLevel;
  double x_0 = subtree->sensorStart;
  double x_1 = subtree->sensorEnd;
 
  vector<rectStruct>* rect_children = &(subtree->children);
  
  int ini = numLevels - 1 - subtree_ini;

  vector<areaStruct> areas;
  bool split = false;
  int break_i = -1;

  int n_curva1, n_curva2;
  double xx_00, xx_11, yy_00, yy_11;  
  
  if(subtree_ini == -1) {
    int j = 0;
    int end_j = levelPoints[numLevels - 1].size();
    while(j < end_j){
      n_curva1 = levelPoints[numLevels - 1][j];
      yy_00 = contourPoints[n_curva1][0].second;
      yy_11 = contourPoints[n_curva1].back().second;
      if(yy_00 <= yy_11) 
        xx_00 = contourPoints[n_curva1][0].first;
      else
        xx_00 = contourPoints[n_curva1].back().first;
      
      xx_11 = -1000;
      areaStruct ar;
      ar.setValues(0, xx_00, xx_11);
      areas.push_back(ar);
      
      split = true;
      j = j + 1;
    }
    ini -= 2; // porque subtree_ini = -1 --> ini = numLevels
  }
  if(split) {
    subtree_end = 0;
    break_i = numLevels - 1;
  }
  else {
    for(int i = ini; i >= bottomLevel + 1; i--) {
      int j = 0;
      int end_j = levelPoints[i].size() - 1;
      while(j < end_j){
        n_curva1 = levelPoints[i][j];
        n_curva2 = levelPoints[i][j+1];
        if(n_curva1 == n_curva2) {
          xx_00 = contourPoints[n_curva1][0].first;
          xx_11 = contourPoints[n_curva1].back().first;


          if(x_0 > x_1) {
            if(((xx_00 > x_0) && (round(xx_00) <= sensor_end) && (xx_11 > x_0) && (round(xx_11) <= sensor_end)) || ((round(xx_00) >= sensor_start) && (xx_00 < x_1) && (round(xx_11) >= sensor_start) && (xx_11 < x_1))) {
              if(xx_00 > xx_11 ) {
                double ss = xx_11;
                xx_11 = xx_00;
                xx_00 = ss;
              }
                       
              areaStruct ar;
              ar.setValues(numLevels - 1 - i, xx_00, xx_11);
              areas.push_back(ar);
              split = true;
            }
            else if(((xx_00 > x_0) && (xx_11 < x_1)) || ((xx_00 < x_1) && (xx_11 > x_0))) {
              if(xx_00 < xx_11 ) {
                double ss = xx_11;
                xx_11 = xx_00;
                xx_00 = ss;
              }           
              areaStruct ar;
              ar.setValues(numLevels - 1 - i, xx_00, xx_11);
              areas.push_back(ar);
              split = true;              
            }
          }            
          else {
            if(xx_00 > xx_11 ) {
              double ss = xx_11;
              xx_11 = xx_00;
              xx_00 = ss;
            }           
            if((xx_00 > x_0) && (xx_11 < x_1)) {
              areaStruct ar;
              ar.setValues(numLevels - 1 - i, xx_00, xx_11);
              areas.push_back(ar);
              split = true;   
            } 
          }


          
//           if(xx_00 > xx_11) {
//             double ss = xx_11;
//             xx_11 = xx_00;
//             xx_00 = ss;
//           }
//           if((xx_00 > x_0) && (xx_11 < x_1)) {
//             areaStruct ar;
//             ar.setValues(levels - i + 1, xx_00, xx_11);
//             areas.push_back(ar);
//             split = true;
//           }
          j = j + 2;
        }
        else
          j = j + 1;
      }
      if(split == true) {
        subtree_end = numLevels - 1 - i;
        break_i = i;
        break;
      }

    }
  }

  if(split == false)
    subtree_end = numLevels - 1 - bottomLevel - 1;

  int end_k = areas.size();
  if(end_k > 0) { // el i del break de split = %t

//     sort(areas.begin(), areas.end(), sortAreasCriterium);
//     for(int i = 0; i < end_k; i++)
//       cout << "areas30 " << areas[i].level<< " " << areas[i].sensorStart<< " " << areas[i].sensorEnd << endl;
//     cout << endl;
   
    for(int k = 0; k < end_k; k++) {
      if(round(areas[k].sensorEnd) == -1000) {
        if(k < end_k - 1)
          areas[k].sensorEnd = areas[k+1].sensorStart;
        else {       
          if(!full360)
            areas[k].sensorEnd = x_1;
          else  {
            if(x_0 > x_1 || subtree_ini == -1)  {// split contour
              areas[k].sensorEnd = areas[0].sensorStart;
            }
            else {
              areas[k].sensorEnd = x_1;
            }
          }
        }
      }
      else if(k < end_k - 1){
        if(areas[k + 1].sensorStart < areas[k].sensorEnd) {
          if(areas[k + 1].sensorStart > 0 || areas[k].sensorEnd < numSensors - 1) {
            areaStruct ar;
            ar.setValues(numLevels - 1 - break_i, areas[k].sensorEnd, areas[k + 1].sensorStart);
            areas.insert(areas.begin() + k + 1, ar);
          }
        }
        else if(round(areas[k + 1].sensorStart) - round(areas[k].sensorEnd) > 0) {
          areaStruct ar;
          ar.setValues(numLevels - 1 - break_i, areas[k].sensorEnd, areas[k + 1].sensorStart);
          areas.insert(areas.begin() + k + 1, ar);
        }
      }
    }


    if(x_0 > x_1 && subtree_ini != -1) {
      if(((areas[0].sensorStart > x_0) && (round(areas[0].sensorStart) <= numSensors)) || ((areas[0].sensorStart < x_0) && (round(areas[0].sensorStart) >= 0) && (areas[0].sensorStart < x_1))) {
        areaStruct ar;
        ar.setValues(numLevels - 1 - break_i, x_0, areas[0].sensorStart);
        areas.insert(areas.begin(), ar);
      }

      
      if(((areas.back().sensorEnd < x_1) && (round(areas.back().sensorEnd) >= sensor_start)) || ((areas.back().sensorEnd > x_1) && (round(areas.back().sensorEnd) <= sensor_end) && (areas.back().sensorEnd > x_0))) {
        areaStruct ar;
        ar.setValues(numLevels - 1 - break_i, areas.back().sensorEnd, x_1);
        areas.insert(areas.begin(), ar);
      }
    }    
    else if(x_0 < x_1 && (subtree_ini != -1 || !full360)) {
      if(areas[0].sensorStart > x_0 ) {
        areaStruct ar;
        ar.setValues(numLevels - 1 - break_i, x_0, areas[0].sensorStart);
        areas.insert(areas.begin(), ar);
      }  
      if(areas.back().sensorEnd < x_1) {
        if(round(areas.back().sensorEnd) == -1000) {
          areaStruct ar;
          ar.setValues(numLevels - 1 - break_i, areas.back().sensorStart, x_1);
          areas.push_back(ar);
        }
        else {
          areaStruct ar;
          ar.setValues(numLevels - 1 - break_i, areas.back().sensorEnd, x_1);
          areas.push_back(ar);
        }      
      } 
    }
  }
  int end_i = areas.size();

//   for(int i = 0; i < end_i; i++)
//     cout << "areas40 " << areas[i].level<< " " << areas[i].sensorStart<< " " << areas[i].sensorEnd << endl;
//   cout << endl;

  for(int i = 0; i < end_i; i++) {
    areaStruct ar = areas[i];
    if(round(areas[i].sensorStart) == round(areas[i].sensorEnd)) // rectángulos vacíos
      continue;
    if(round(areas[i].sensorStart) == sensor_end/* + 1*/  && !full360) // rectángulo final artificial
      continue;
    if(round(areas[i].sensorEnd) == sensor_start/* - 1*/  && !full360) // rectángulo inicial artificial
      continue;
      
    rectStruct childRect;
    childRect.setValues(areas[i].level, areas[i].level, areas[i].sensorStart, areas[i].sensorEnd);

    scaleTree(&childRect, sensor_start, sensor_end);
    
    if(round(childRect.sensorStart) == sensor_start/* - 1*/)// inicio artificial
      childRect.sensorStart = sensor_start + 1;
    if(round(childRect.sensorEnd) == sensor_end/* + 1*/) // final artificial
      childRect.sensorEnd = sensor_end - 1;
    rect_children->push_back(childRect);

//     cout << childRect;
    
    childRect.isInit = false;
    if(i < end_i - 1) {
//       if(areas[i].sensorEnd < areas[i+1].sensorStart) {
//         childRect.setValues(areas[i].level, areas[i].level, areas[i].sensorEnd, areas[i+1].sensorStart);
//         scaleTree(&childRect, sensor_start, sensor_end);
//         rect_children->push_back(childRect);
//         // varias curvas aparecen al mismo nivel


      if(x_0 > x_1) {
        if((areas[i].sensorEnd < areas[i+1].sensorStart && areas[i].sensorEnd < x_1 && areas[i+1].sensorStart < x_1)) 
          childRect.setValues(areas[i].level, areas[i].level, areas[i].sensorEnd, areas[i+1].sensorStart);
        else if((areas[i].sensorEnd  < areas[i+1].sensorStart && areas[i].sensorEnd  > x_0 && areas[i+1].sensorStart > x_0))
          childRect.setValues(areas[i].level, areas[i].level, areas[i].sensorEnd, areas[i+1].sensorStart);
      }    
      else if(areas[i].sensorEnd < areas[i+1].sensorStart)
        childRect.setValues(areas[i].level, areas[i].level, areas[i].sensorEnd, areas[i+1].sensorStart);

      if(childRect.isInit) {
        scaleTree(&childRect, sensor_start, sensor_end);
        rect_children->push_back(childRect);
//         cout << childRect;
        // varias curvas aparecen al mismo nivel
      }  
    }
  }  
}



// void ScaleSpace::scaleTree(rectStruct* subtree, double sensor_start, double sensor_end)
// {
// 
//   int levels = levelPoints.size();
// 
// 
//   int subtree_ini = subtree->initLevel;
//   int& subtree_end = subtree->endLevel;
//   double x_0 = subtree->sensorStart;
//   double x_1 = subtree->sensorEnd;
//  
//   vector<rectStruct>* rect_children = &(subtree->children);
//   
//   int ini = levels - subtree_ini - 1;
// 
//   vector<areaStruct> areas;
//   bool split = false;
//   int break_i = -1;
// 
//   int n_curva1, n_curva2;
//   double xx_00, xx_11, yy_00, yy_11;  
//   
//   if(subtree_ini == -1) {
//     int j = 0;
//     int end_j = levelPoints[levels - 1].size();
//     while(j < end_j){
//       n_curva1 = levelPoints[levels - 1][j];
//       yy_00 = contourPoints[n_curva1][0].second;
//       yy_11 = contourPoints[n_curva1].back().second;
//       if(yy_00 <= yy_11) 
//         xx_00 = contourPoints[n_curva1][0].first;
//       else
//         xx_00 = contourPoints[n_curva1].back().first;
//       
//       xx_11 = -1000;
//       areaStruct ar;
//       ar.setValues(0, xx_00, xx_11);
//       areas.push_back(ar);
//       
//       split = true;
//       j = j + 1;
//     }
//     ini--;
//   }
//   if(split) {
//     subtree_end = 0;
//     break_i = levels;
//   }
//   else {
//     for(int i = ini; i >= levels - bottomLevel; i--) {
//       int j = 0;
//       int end_j = levelPoints[i].size() - 1;
//       while(j < end_j){
//         n_curva1 = levelPoints[i][j];
//         n_curva2 = levelPoints[i][j+1];
//         if(n_curva1 == n_curva2) {
//           xx_00 = contourPoints[n_curva1][0].first;
//           xx_11 = contourPoints[n_curva1].back().first;
//           if(xx_00 > xx_11) {
//             double ss = xx_11;
//             xx_11 = xx_00;
//             xx_00 = ss;
//           }
//           if((xx_00 > x_0) && (xx_11 < x_1)) {
//             areaStruct ar;
//             ar.setValues(levels - i == 0 ? 0 : levels - i - 1, xx_00, xx_11);
//             areas.push_back(ar);
//             split = true;
//           }
//           j = j + 2;
//         }
//         else
//           j = j + 1;
//       }
//       if(split == true) {
//         subtree_end = levels - i - 1;
//         break_i = i;
//         break;
//       }
// 
//     }
//   }
// 
//   if(split == false)
//     subtree_end = bottomLevel - 1;
// 
//   int end_k = areas.size();
//   if(end_k > 0) { // el i del break de split = %t
// 
//     sort(areas.begin(), areas.begin(), sortAreasCriterium);
//    
//     for(int k = 0; k < end_k; k++) {
//       if(round(areas[k].sensorEnd) == -1000) {
//         if(k < end_k - 1)
//           areas[k].sensorEnd = areas[k+1].sensorStart;
//         else {       
//           if(!full360)
//             areas[k].sensorEnd = x_1;
//           else  {
//             if(x_0 > x_1 || subtree_ini == 0) , // split contour
//               areas[k].sensorEnd = areas[1].sensorStart;
//             else
//               areas[k].sensorEnd = x_1;
//           }
//         }
//       }
//     }
// 
//     if(areas[0].sensorStart > x_0 && !full360) {
//         areaStruct ar;
//         ar.setValues(levels - break_i == 0 ? 0 : levels - break_i - 1, x_0, areas[0].sensorStart);
//         areas.insert(areas.begin(), ar);
//     }
//     if(areas.back().sensorEnd < x_1 && !full360) {
//       if(round(areas.back().sensorEnd) == -1000) {
//         vector <int> new_area(3);
//         areaStruct ar;
//         ar.setValues(levels - break_i == 0 ? 0 : levels - break_i - 1, areas.back().sensorStart, x_1);
//         areas.push_back(ar);
//       }
//       else {
//         areaStruct ar;
//         ar.setValues(levels - break_i == 0 ? 0 : levels - break_i - 1, areas.back().sensorEnd, x_1);
//         areas.push_back(ar);      }
//     }
//   }
// 
//   int end_i = areas.size();
// 
// //   for(int i = 0; i < end_i; i++)
// //     cout << "areas33 " << areas[i][0]<< " " << areas[i][1]<< " " << areas[i][2] << endl;
// //   cout << endl;
// 
//   for(int i = 0; i < end_i; i++) {
//     if(round(areas[i].sensorStart) == round(areas[i].sensorEnd)) // rectángulos vacíos
//       continue;
//     if(round(areas[i].sensorStart) == sensor_end + 1) // rectángulo final artificial
//       continue;
//     if(round(areas[i].sensorEnd) == sensor_start - 1) // rectángulo inicial artificial
//       continue;
//       
//     rectStruct childRect;
//     childRect.setValues(areas[i].level, areas[i].level, areas[i].sensorStart, areas[i].sensorEnd);
// 
//     scaleTree(&childRect, sensor_start, sensor_end);
//     
//     if(round(childRect.sensorStart) == sensor_start - 1)// inicio artificial
//       childRect.sensorStart = sensor_start;
//     if(round(childRect.sensorEnd) == sensor_end + 1) // final artificial
//       childRect.sensorEnd = sensor_end;
//     rect_children->push_back(childRect);
//     if(i < end_i - 1) {
//       if(areas[i].sensorEnd < areas[i+1].sensorStart) {
//         rectStruct childRect;
//         childRect.setValues(areas[i].level, areas[i].level, areas[i].sensorEnd, areas[i+1].sensorStart);
//         scaleTree(&childRect, sensor_start, sensor_end);
//         rect_children->push_back(childRect);
//         // varias curvas aparecen al mismo nivel
//       }
//     }
//   }  
// }


// void ScaleSpace::scaleTree(rectStruct* subtree, double sensor_start, double sensor_end)
// {
// 
//   int levels = levelPoints.size();
// //   if(bottom > levels) 
// //     bottom = levels;
// 
//   int subtree_ini = subtree->initLevel;
//   int& subtree_end = subtree->endLevel;
// //   int x_0 = subtree->sensorStart;
// //   int x_1 = subtree->sensorEnd;
//   double x_0 = subtree->sensorStart;
//   double x_1 = subtree->sensorEnd;
//  
//   vector<rectStruct>* rect_children = &(subtree->children);
//   
//   int ini = levels - subtree_ini - 1;
// //   vector< vector< int > > areas;
//   vector<areaStruct> areas;
//   bool split = false;
//   int break_i = -1;
// //   int xx_00, xx_11, n_curva1, n_curva2;
// //   double yy_00, yy_11;
//   int n_curva1, n_curva2;
//   double xx_00, xx_11, yy_00, yy_11;  
//   
//   if(subtree_ini == -1) {
//     int j = 0;
//     int end_j = levelPoints[levels - 1].size();
//     while(j < end_j){
//       n_curva1 = levelPoints[levels - 1][j].second;
//       yy_00 = contourPoints[n_curva1][0].second;
//       yy_11 = contourPoints[n_curva1].back().second;
//       if(yy_00 <= yy_11) 
//         xx_00 = contourPoints[n_curva1][0].first;
//       else
//         xx_00 = contourPoints[n_curva1].back().first;
//       
//       xx_11 = -1000;
// //       vector <int> new_area(3);
// //       new_area[0] = 0;
// //       new_area[1] = xx_00;
// //       new_area[2] = xx_11;
// 
// //       areas.push_back(new_area);
//       areaStruct ar;
//       ar.setValues(0, xx_00, xx_11);
//       areas.push_back(ar);
//       
//       split = true;
//       j = j + 1;
//     }
//     ini--;
//   }
//   if(split) {
//     subtree_end = 0;
//     break_i = levels;
//   }
//   else {
//     for(int i = ini; i >= levels - bottomLevel; i--) {
//       int j = 0;
//       int end_j = levelPoints[i].size() - 1;
//       while(j < end_j){
//         n_curva1 = levelPoints[i][j].second;
//         n_curva2 = levelPoints[i][j+1].second;
//         if(n_curva1 == n_curva2) {
//           xx_00 = contourPoints[n_curva1][0].first;
//           xx_11 = contourPoints[n_curva1].back().first;
//           if(xx_00 > xx_11) {
// //             int ss = xx_11;
//             double ss = xx_11;
//             xx_11 = xx_00;
//             xx_00 = ss;
//           }
//           if((xx_00 > x_0) && (xx_11 < x_1)) {
// //             vector <int> new_area(3);
// //             new_area[0] = levels - i == 0 ? 0 : levels - i - 1;
// //             new_area[1] = xx_00;
// //             new_area[2] = xx_11;            
// //             
// //             areas.push_back(new_area);
//             areaStruct ar;
//             ar.setValues(levels - i == 0 ? 0 : levels - i - 1, xx_00, xx_11);
//             areas.push_back(ar);
//             split = true;
//           }
//           j = j + 2;
//         }
//         else
//           j = j + 1;
//       }
//       if(split == true) {
//         subtree_end = levels - i - 1;
//         break_i = i;
//         break;
//       }
// 
//     }
//   }
// 
//   if(split == false)
//     subtree_end = bottomLevel - 1;
// 
//   int end_k = areas.size();
//   if(end_k > 0) { // el i del break de split = %t
// 
//     sort(areas.begin(), areas.begin(), sortAreasCriterium);
//    
//     for(int k = 0; k < end_k; k++) {
//       if(round(areas[k].sensorEnd) == -1000) {
//         if(k < end_k - 1)
//           areas[k].sensorEnd = areas[k+1].sensorStart;
//         else {       
//           if(!full360)
//             areas[k].sensorEnd = x_1;
//         }
//       }
//     }
// 
//     if(areas[0].sensorStart > x_0 && !full360) {
// //       vector <int> new_area(3);
// //       new_area[0] = levels - break_i == 0 ? 0 : levels - break_i - 1;
// //       new_area[1] = x_0;
// //       new_area[2] = areas[0][1];
// // 
// //       areas.insert(areas.begin(), new_area);
// 
//         areaStruct ar;
//         ar.setValues(levels - break_i == 0 ? 0 : levels - break_i - 1, x_0, areas[0].sensorStart);
//         areas.insert(areas.begin(), ar);
//     }
//     if(areas.back().sensorEnd < x_1 && !full360) {
//       if(round(areas.back().sensorEnd) == -1000) {
//         vector <int> new_area(3);
// //         new_area[0] = levels - break_i == 0 ? 0 : levels - break_i - 1;
// //         new_area[1] = areas.back()[1];
// //         new_area[2] = x_1;        
// // 
// //         areas.push_back(new_area);
//         areaStruct ar;
//         ar.setValues(levels - break_i == 0 ? 0 : levels - break_i - 1, areas.back().sensorStart, x_1);
//         areas.push_back(ar);
//       }
//       else {
// //         vector <int> new_area(3);
// //         new_area[0] = levels - break_i == 0 ? 0 : levels - break_i - 1;
// //         new_area[1] = areas.back()[2];
// //         new_area[2] = x_1;         
// // 
// //         areas.push_back(new_area);
//         areaStruct ar;
//         ar.setValues(levels - break_i == 0 ? 0 : levels - break_i - 1, areas.back().sensorEnd, x_1);
//         areas.push_back(ar);      }
//     }
//   }
// 
//   int end_i = areas.size();
// 
// //   for(int i = 0; i < end_i; i++)
// //     cout << "areas33 " << areas[i][0]<< " " << areas[i][1]<< " " << areas[i][2] << endl;
// //   cout << endl;
// 
//   for(int i = 0; i < end_i; i++) {
// //     if(areas[i][1] == areas[i][2]) // rectángulos vacíos
// //       continue;
// //     if(areas[i][1] == sensor_end + 1) // rectángulo final artificial
// //       continue;
// //     if(areas[i][2] == sensor_start - 1) // rectángulo inicial artificial
// //       continue;
//     if(round(areas[i].sensorStart) == round(areas[i].sensorEnd)) // rectángulos vacíos
//       continue;
//     if(round(areas[i].sensorStart) == sensor_end + 1) // rectángulo final artificial
//       continue;
//     if(round(areas[i].sensorEnd) == sensor_start - 1) // rectángulo inicial artificial
//       continue;
//       
//     rectStruct childRect;
//     childRect.setValues(areas[i].level, areas[i].level, areas[i].sensorStart, areas[i].sensorEnd);
// 
//     scaleTree(&childRect, sensor_start, sensor_end);
//     
//     if(round(childRect.sensorStart) == sensor_start - 1)// inicio artificial
//       childRect.sensorStart = sensor_start;
//     if(round(childRect.sensorEnd) == sensor_end + 1) // final artificial
//       childRect.sensorEnd = sensor_end;
//     rect_children->push_back(childRect);
//     if(i < end_i - 1) {
//       if(areas[i].sensorEnd < areas[i+1].sensorStart) {
//         rectStruct childRect;
//         childRect.setValues(areas[i].level, areas[i].level, areas[i].sensorEnd, areas[i+1].sensorStart);
//         scaleTree(&childRect, sensor_start, sensor_end);
//         rect_children->push_back(childRect);
//         // varias curvas aparecen al mismo nivel
//       }
//     }
//   }  
// }

void ScaleSpace::deleteRectStruct(rectStruct& rect)
{
  
  uint childrenSize = rect.children.size();
  vector<rectStruct>& child_ren = rect.children;
  for(uint i = 0; i < childrenSize; i++) {
    rectStruct& rest = child_ren[i];
    deleteRectStruct(rest);
  }
//   child_ren.clear();
}

void ScaleSpace::initRectStruct(rectStruct& rect_str)
{
  rect_str.initLevel = -1;
  rect_str.endLevel = -1;
  rect_str.sensorStart = -1;
  rect_str.sensorEnd = numSensors;
  rect_str.children.clear();
  rect_str.isInit = false;
}

// vector <vector <int> > ScaleSpace::tree2Matrix(rectStruct* subtree, double parentHeight, bool stable_ancestor)
vector<ScaleSpace::matRectStruct> ScaleSpace::tree2Matrix(rectStruct* subtree, double parentHeight, bool stable_ancestor)
{

  double x_rect = subtree->sensorStart;
  int h_rect = subtree->endLevel - subtree->initLevel;
  vector<rectStruct>& children = subtree->children;
  bool is_stable = !stable_ancestor;
   
//   vector <vector <int> > matRect;
//   vector < vector <int> > matChild;
  vector <matRectStruct> matRect;
  vector <matRectStruct> matChild;
  
  if(parentHeight > h_rect)
    is_stable =false;

  int end_i = children.size();
  if(is_stable) {
    for(int i = 0; i < end_i; i++) {
      rectStruct& child = children[i];
      if(child.endLevel - child.initLevel >= h_rect)
        is_stable = false;  
    }
  }
  
  for(int i = 0; i < end_i; i++) {
    rectStruct* child = &(children[i]);
    matChild = tree2Matrix(child, h_rect, is_stable || stable_ancestor);
    int end_j = matChild.size();
    for(int j = 0; j < end_j; j++)
      matRect.push_back(matChild[j]);
  }
    
//   vector <int> new_mat(3);
//   new_mat[0] = x_rect;
//   new_mat[1] = h_rect;
//   new_mat[2] = is_stable;
//   matRect.insert(matRect.begin(), new_mat);
    matRectStruct mr;
    mr.setValues(x_rect, h_rect, is_stable);
    matRect.insert(matRect.begin(), mr);
  
//   cout << "matRectangles: " <<  matRect.size() << endl;
//   for(uint i = 0; i < matRect.size(); i++) {
//       cout << matRect[i].x << " " << matRect[i].h << " " << matRect[i].stable;
//     cout << endl;
//   }
//   cout << endl;

  return matRect;
}

void ScaleSpace::computeSeeds(void)
{
  double min_tam = 3.5;
  vector <double> areas_fin;
  int sizeRects = matRectangles.size();
  uint j;
  int cntMeasure = 0;
  int numMeasure = measuredAreas.size();
  int measure_start = 0;
  int measure_end = numSensors - 1;
  vector <int> limits_rects;

//     for (int i = 0; i < sizeRects; i++) {
//       vector <double> & rects = matRectangles[i];
//       int sz = matRectangles[i].size();
//       for (int j = 0; j < sz; j++)
//         cout << rects[j] << " ";
//       cout << endl;
//     }

  for (int i = 0; i < sizeRects; i++) {
    vector <double> & rects = matRectangles[i];
    vector <double> new_rects;
    int sz = matRectangles[i].size();
    int j = 0;
    cntMeasure = 0;
    bool last_seg_inserted = false;
    while(j < sz && cntMeasure < numMeasure) {
      double r_start = rects[j];
      bool is_in = false;
      bool area_change = false;
      pair<int, int> p = measuredAreas[cntMeasure];
      pair<int, int> pb = measuredAreas.back();
      bool pre_pend = false;

      if(abs(r_start - p.first) < 0.01 || abs(r_start - p.second) < 0.01) {
        rects.erase(rects.begin() + j);
        sz--;
        continue;
      }
      if(cntMeasure == 0) {
        if(p.first <= p.second) {
          if(j == 0 /*&& abs(r_start - p.first) > 0.0000001*/)
            new_rects.push_back(p.first);
            
          if(r_start > p.first && r_start < p.second) {
            is_in = true;
          }
          else if(/*numMeasure > 1 && */pb.first > pb.second && r_start < pb.second) {
            is_in = true;
            pre_pend = true;
          }
          else if(r_start >= p.second)
            area_change = true;
        }
        else { // solo hay entonces 1 area
          if(r_start > p.first || r_start < p.second)
            is_in = true;
          else if(r_start > p.second && new_rects.back() != p.first)
            area_change = true;
        }
      }
      else {
        if(p.first <= p.second) {
          if(r_start > p.first && r_start < p.second) {
            is_in = true;
          }
          else if(r_start >= p.second)
            area_change = true;
        }
        else {
          if(r_start > p.first || r_start < p.second) {
            is_in = true;
          }
        }
      }

      if(is_in) {
        if(pre_pend) {
          new_rects.insert(new_rects.begin() + j, r_start);
          pre_pend = false;
        }
        else
          new_rects.push_back(r_start);
        j++;
      }
      if(area_change) {
        cntMeasure++;
        if(p.first <= p.second){
          new_rects.push_back(p.second);
          if(cntMeasure < numMeasure)
            new_rects.push_back(measuredAreas[cntMeasure].first);
        }
        else {// es la ultima
          if(numMeasure > 1)
            new_rects.push_back(p.first);
          else {
            new_rects.push_back(measuredAreas[0].second);
            new_rects.push_back(measuredAreas[0].first);
            cntMeasure--;
            j++;
          }
                 
        }
        area_change = false;
      }
      else if(!is_in)
        j++;
      
    }

//     sz = new_rects.size();
//     if(j < sz)
//       rects.erase(rects.begin() + j, rects.end());
    if(j == sz) {
      while(cntMeasure < numMeasure) {
        pair<int, int> p = measuredAreas[cntMeasure];
        if(new_rects.back() < p.first) {
          new_rects.push_back(p.first);
          new_rects.push_back(p.second);
          last_seg_inserted = true;
        }
        else if(abs(new_rects.back() - p.first) < 0.0000001) {
          new_rects.push_back(p.second);
          last_seg_inserted = true;
        }
        else if(new_rects.back() < p.second && !full360 && !last_seg_inserted /*cntMeasure == numMeasure - 1*/) {
          new_rects.push_back(p.second);
          last_seg_inserted = true;
        }
          
        cntMeasure++;
      }
    }
    
    rects = new_rects;
//     sz = rects.size();
//     j = 0;
//     while(j < sz) {
//       if(numMeasure > 1 && measuredAreas.back().first > measuredAreas.back().second) {
//         if(rects[j] < measuredAreas.back().second) {
//           rects.push_back(rects[j]);
//           rects.erase(rects.begin());
//           j++;
//         }
//         else {
//           rects.push_back(measuredAreas.back().second);
//           break;
//         }
//       }
//     }
  }


  
//   if(numMeasure > 0) { //////////////////////////////////
//     for (int i = 0; i < sizeRects; i++) {
//       vector <double> & rects = matRectangles[i];
//       int sz = matRectangles[i].size();
//       int j = 0;
//       cntMeasure = 0;
//       while(j < sz && cntMeasure < numMeasure) {
//         int r_start = round(rects[j]);
//         bool del = false;
//         pair<int, int> p = measuredAreas[cntMeasure];
//         if(measuredAreas[cntMeasure].first > measuredAreas[cntMeasure].second) {
//           if(r_start >= measuredAreas[cntMeasure].second && r_start <= measuredAreas[cntMeasure].first)
//             del = true;
//         }
//         else if(r_start <= measuredAreas[cntMeasure].first)
//           del = true;
//           
//         if(del) {
//           if(j > 0) {
//             double rr = rects[j-1];
//             if(measuredAreas[cntMeasure].first <= measuredAreas[cntMeasure].second) {
//               if(round(rects[j-1]) != measuredAreas[cntMeasure].first) {
//                 rects[j] = measuredAreas[cntMeasure].first;
//                 del = false;
//               }
//             }
//             else {
// 
//                 if(round(rects[j-1]) <= measuredAreas[cntMeasure].second) {
//                   rects[j] = measuredAreas[cntMeasure].second;
//                   del = false;
//                 }
// 
//                 else if(round(rects[j-1]) != measuredAreas[cntMeasure].second) {
//                   rects[j] = measuredAreas[cntMeasure].second; ////
//                   del = false;
//                   cntMeasure++;
//                 }            
// //               if(numMeasure == 1) {
// //                 if(round(rects[j-1]) != measuredAreas[cntMeasure].second) {
// //                   rects[j] = measuredAreas[cntMeasure].second;
// //                   del = false;
// //                 }
// //               }
// //               else {
// //                 if(round(rects[j-1]) != measuredAreas[cntMeasure].second) {
// //                   rects[j] = measuredAreas[cntMeasure].second; ////
// //                   del = false;
// //                   cntMeasure++;
// //                 }
//               }
// //               else if(round(rects[j-1]) != measuredAreas[cntMeasure].first) {
// //                   rects[j] = measuredAreas[cntMeasure].first;
// //                   del = false;
// //               }
//             }
// //           }
//           else {
//             if(measuredAreas[cntMeasure].first <= measuredAreas[cntMeasure].second) {
//               rects[j] = measuredAreas[cntMeasure].first;
//               del = false;
//             }
//             else {
//               rects[j] = measuredAreas[cntMeasure].second;
//               del = false;
//             }
//           }
//           if(del) {
//             rects.erase(rects.begin() + j);
//             sz--;
//           }
//           else
//             j++;
//         }
//         else {
//           if(j == 0 && !full360) {
//             rects.insert(rects.begin(), measuredAreas[0]. first);
//             sz++;
//           }
//           if(measuredAreas[cntMeasure].first > measuredAreas[cntMeasure].second) {
//             if(rects[j] > measuredAreas[cntMeasure].first/* && numMeasure > 1*/) {
//               if(j > 0) {
//                 double rr = rects[j-1];
//                 if(round(rects[j-1]) < measuredAreas[cntMeasure].first) {
//                   rects.insert(rects.begin() + j,measuredAreas[cntMeasure].first);
//                   j += 2;
//                   sz++;
//                 }
//                 else
//                   j++;
//               }
// //               if(numMeasure > 1)           
// //                 cntMeasure++;
//             }
//             else if(rects[j] < measuredAreas[cntMeasure].second) {
//               j++;
// //               if(j > 0) {
// //                 if(round(rects[j-1]) != measuredAreas[cntMeasure].second) {
// //                   rects.insert(rects.begin() + j,measuredAreas[cntMeasure].second);
// //                   j += 2;
// //                 }
// //                 else
// //                   j++;                
// //               }
// //               else
// //                 j++;
// //               if(numMeasure > 1)
// //                 cntMeasure++;
//             }
//           }
//           else {
//             if(rects[j] > measuredAreas[cntMeasure].second) {
//               if(j > 0) {
//                 double rr = rects[j-1];
//                 if(round(rects[j-1]) != measuredAreas[cntMeasure].second) {
//                   rects.insert(rects.begin() + j,measuredAreas[cntMeasure].second);
//                   j++;
//                   sz++;
//                 }
//               }
//               else {
//                 rects[0] = measuredAreas[cntMeasure].second;
//                 rects.insert(rects.begin() + j,measuredAreas[cntMeasure].first);              
//                 j += 2;
//                 sz++;
//               }
//               cntMeasure++;
//             }
//             else
//               j++;
//            }
//         }
//       }
//       if(j < sz)
//         rects.erase(rects.begin() + j, rects.end());
// //       if(!full360 && rects.back() != measuredAreas.back().second)
// //         rects.push_back(measuredAreas.back().second);
//       double rr = rects.back();
//       pair<int, int> p = measuredAreas.back();
//       if(rects.back() < measuredAreas.back().first){
//         rects.push_back(measuredAreas.back().first);
//         rects.push_back(measuredAreas.back().second);
//       }
//       else if(rects.back() < measuredAreas.back().second)
//         rects.push_back(measuredAreas.back().second);
// //       else if(numMeasure == 1 && measuredAreas.back().first > measuredAreas.back().second)
// //         rects.push_back(rects[0]);
//     }
//   }

//     for (int i = 0; i < sizeRects; i++) {
//       vector <double> & rects = matRectangles[i];
//       int sz = matRectangles[i].size();
//       for (int j = 0; j < sz; j++)
//         cout << rects[j] << " ";
//       cout << endl;
//     }
      
//   for (int i = 0; i < sizeRects; i++) {
//     vector <double> & rects = matRectangles[i];
// //     int s_m = rects.size();
//     int j = 0;
//     for(cntMeasure = 0; cntMeasure < numMeasure; cntMeasure++) {
//       measure_start = measuredAreas[cntMeasure].first;
//       measure_end = measuredAreas[cntMeasure].second;
//       if(measure_start <= measure_end) {
//         if(round(rects[0]) - measure_start > min_tam - 1)
//           rects.insert(rects.begin(), measure_start);
//         else
//           rects[0] = measure_start;
//         if(round(rects.back()) < measure_end + 1 - (min_tam - 1))
//           rects.push_back(measure_end);
//         else
//           rects.back() = measure_end;
//       }
//       else {
// 
//       }
//     }
//   }

  if(sizeRects == 2) {
    while(true) {
      if(matRectangles[0].empty()) {
        int end_k = matRectangles[1].size();
        for(int k = 0; k < end_k; k++)
          areas_fin.push_back(matRectangles[1][k]);
        break;
      }
      if(matRectangles[1].empty()) {
        int end_k = matRectangles[0].size();
        for(int k = 0; k < end_k; k++)
          areas_fin.push_back(matRectangles[0][k]);
        break;
      }
      
      if(abs(matRectangles[0][0] - matRectangles[1][0]) < 0.000001) {
        areas_fin.push_back(matRectangles[0][0]);
        matRectangles[0].erase(matRectangles[0].begin());
        matRectangles[1].erase(matRectangles[1].begin());
      }
      else if(matRectangles[0][0] <= matRectangles[1][0]) {
        if(matRectangles[0][0] >= areas_fin.back()) {
          areas_fin.push_back(matRectangles[0][0]);
          matRectangles[0].erase(matRectangles[0].begin());
        }
        else {
          areas_fin.push_back(matRectangles[1][0]);
          matRectangles[1].erase(matRectangles[1].begin());
        }
      }
      else {
      
        if(matRectangles[1][0] >= areas_fin.back()) {
          areas_fin.push_back(matRectangles[1][0]);
          matRectangles[1].erase(matRectangles[1].begin());
        }
        else {
          areas_fin.push_back(matRectangles[0][0]);
          matRectangles[0].erase(matRectangles[0].begin());
        }
/*
      
        areas_fin.push_back(matRectangles[1][0]);
        matRectangles[1].erase(matRectangles[1].begin());*/
      }
    }
  }
  else
    areas_fin = matRectangles[0];

  int sz = areas_fin.size();
  
//   for (int i = 0; i < sz; i++)
//     cout << areas_fin[i] << " ";
//   cout << endl;

//   int sz = areas_fin.size();
  j = 0;
  while(j < sz) {
  double a = areas_fin[j];
    if(numMeasure > 1 && measuredAreas.back().first > measuredAreas.back().second) {
      if(areas_fin[j] < measuredAreas.back().second) {
        areas_fin.push_back(areas_fin[j]);
        areas_fin.erase(areas_fin.begin());
//         j++;
      }
      else {
        areas_fin.push_back(measuredAreas.back().second);
        break;
      }
    }
    else {
      if(numMeasure == 1 && measuredAreas.back().first > measuredAreas.back().second)
        areas_fin.push_back(areas_fin[0]);
      break;
    }
  }



  int s_a = areas_fin.size();
  
//   for (int i = 0; i < s_a; i++)
//     cout << areas_fin[i] << " ";
//   cout << endl;
  j = 1;
  int i = 0;

  while(i < numMeasure && j < s_a) {
    bool passed360 = measuredAreas[i].first > measuredAreas[i].second;
    if(!passed360) {
      measure_start = measuredAreas[i].first;
      measure_end = measuredAreas[i].second;
    }
    else {
      measure_start = measuredAreas[i].second;
      measure_end = measuredAreas[i].first;
    }
    while (j < s_a) {
      double & area_1 = areas_fin[j-1];
      double & area = areas_fin[j];
      bool small = false;
      if(area >= area_1)
        small = area - area_1 < min_tam;
      else
        small = numSensors - area + area_1 < min_tam;
      
      if(small) {
        if(abs(area_1 - measure_start) < 0.00000001 && abs(area - measure_end) < 0.00000001) {
            i++;
//           if(measure_start == measure_end)
            j += 2;
//           else
//             j++;
          break;
        }
        else if(abs(area_1 - measure_start) < 0.00000001) {
          areas_fin.erase(areas_fin.begin() + j);
          s_a--;
        }
        else if(abs(area - measure_start) < 0.00000001) {
          areas_fin.erase(areas_fin.begin() + j - 1);
          s_a--;
        }        
        else if(abs(area_1 - measure_end) < 0.00000001) {
          areas_fin.erase(areas_fin.begin() + j);
          if(numMeasure > 1)
            i++;
//           j += 2;
          j++;
          s_a--;
          break;          
        }
        else if(abs(area - measure_end) < 0.00000001) {
          areas_fin.erase(areas_fin.begin() + j - 1);
          if(numMeasure > 1)
            i++;
//           j += 2;
          j++;
          s_a--;
          break;
        }
        else {
          area_1 = (area_1 + area)/2.0;
          areas_fin.erase(areas_fin.begin() + j);
          s_a--;
        }
      }    
      else {
        j++;
        if(abs(area_1 - measure_start) < 0.00000001 && abs(area - measure_end) < 0.00000001) {
          i++;
          j++;
          break;
        }
        if(!passed360) {
          if(abs(area - measure_end) < 0.00000001) {
            i++;
            j++;
            break;
          }       
        }
        else {
          if(abs(area - measure_start) < 0.00000001 && (numMeasure > 1)) {
            i++;
//             j++;
            break;
          }
        }
      }
    }
  }

//   if(full360 && measuredAreas.back().first > measuredAreas.back().second && numMeasure == 1) {
//     if((round(areas_fin.back()) < numSensors - 1) && (round(areas_fin[0]) > 0) && (numSensors - 1 - areas_fin.back() + areas_fin[0] > min_tam))
//       areas_fin.push_back(areas_fin[0]/* - 1*/);
// //     else if(areas_fin.back() < numSensors - 1 - (min_tam - 1))
// //       areas_fin.push_back(numSensors - 1);
// //     else if(areas_fin.back() >= numSensors - 1 - (min_tam - 1))
// //       areas_fin.back() = numSensors - 1;
// //     else if(areas_fin[0] > (min_tam - 1))
// //       areas_fin.insert(areas_fin.begin(), 0);
// //     else if(areas_fin[0] <= (min_tam - 1))
// //       areas_fin[0] = 0;
//   }
  
//   if(full360) {
//     if(measuredAreas.back().first <= measuredAreas.back().second) {
//       measure_start = measuredAreas.back().first;
//       measure_end = measuredAreas.back().second;
//     }
//     else {
//       measure_start = measuredAreas.back().second;
//       measure_end = measuredAreas.back().first;
//     }
//     if((round(areas_fin.back()) < measure_end) && (round(areas_fin[1]) > measure_start) && (measure_end - areas_fin.back() + areas_fin[0] > min_tam))
//       areas_fin.push_back(areas_fin[0]);
//   }
  
  int end_i = areas_fin.size();

//   for (int i = 0; i < end_i; i++)
//     cout << areas_fin[i] << " ";
//   cout << endl;
  
  bool passed360;
  j = 0;
  for (int i = 1; i < end_i; i++) {
    double fin = areas_fin[i];
    double ini = areas_fin[i-1];
    if(fin < ini)
      passed360 = true;
    else  
      passed360 = false;

    if(passed360) {
      if(numSensors - 1 - ini + fin < min_tam) {}
      else if(numSensors - ini + fin - 1 < 8.5) { // menos de 9 puntos
        if(int(ini) + 1 >= int(fin))
          areaSeeds.push_back(make_pair(int(ini) + 1 , int(fin)));
      }
      else {
        double semilong = 4.5; // semillas de 9 puntos
        double pos = (ini - (numSensors - 1) + fin)/2;
        double pos_l, pos_r;
        
        if(pos < 0.5)
          pos += numSensors;
          
        if(pos + semilong < 0)
          pos_r = pos + semilong + numSensors;
        else if(pos + semilong >= numSensors)
          pos_r = pos + semilong - numSensors;
        else
          pos_r = pos + semilong;

        if(pos - semilong < -1)
          pos_l = pos - semilong + numSensors;
        else
          pos_l = pos - semilong;

        areaSeeds.push_back(make_pair(int(pos_l) + 1, int(pos_r)));
      }

      if(j < numMeasure) {
        if(round(fin) == measuredAreas[j].first) {
          j++;
          i++;
        }
      }
    }
    else {
      if(numSensors - 1 - ini + fin < min_tam) {
        int v = 0;
      }
      else if(fin - ini < 8.5) { // menos de 9 puntos
        if(int(fin) - int(ini) >= 2)
          areaSeeds.push_back(make_pair(int(ini) + 1, int(fin)));
        else
          areaSeeds.push_back(make_pair(int(ini), int(fin)));
      }
      else {
        double semilong = 4.5; // semillas de 9 puntos
        double pos = (ini + fin)/2;
        areaSeeds.push_back(make_pair(int(pos - semilong) + 1 , int(pos + semilong)));
      }
      if(j < numMeasure) {
        if(fin >= measuredAreas[j].second) {
          j++;
          i++;
        }
      }
      
    }

  }
    
//   printResults();
}

// void ScaleSpace::statsContours(double width)
void ScaleSpace::statsContours(double k_filter, double area_lim)
{
  int n_curves = contourPoints.size();
  QVector< pair<double, int> >::iterator pos_ini,pos_end;
  double max_level = -1;
  double mean_h = 0;
  double sum_h = 0;
  double sum_h2 = 0;
  double sd_h = 0;
  double w, h;
  int cnt = 0;
  vector<int> h_curves;
  
  for (int i = 0; i < numLevels; i++)
    levelPoints[i].clear();
    
/*  cout << "h " << currentOrder << endl; */ 
  for(int j = 0; j< n_curves; j ++) {
    pos_ini = contourPoints[j].begin();
    pos_end = contourPoints[j].end();

    if(splitContours.at(j)) {
      if(contourPoints[j][0].first > numSensors || contourPoints[j].back().first > numSensors) // pequeña posible curva final que termina entre numSensors - 1 y numSensors
        w = std::min(numSensors - abs(contourPoints[j][0].first - contourPoints[j].back().first), abs(contourPoints[j][0].first - contourPoints[j].back().first));
      else
        w = numSensors - abs(contourPoints[j][0].first - contourPoints[j].back().first);
    }
    else
      w = abs(contourPoints[j][0].first - contourPoints[j].back().first);


    max_level = -1;
    for(;pos_ini < pos_end; ++pos_ini) {
      if((*pos_ini).second > max_level)
        max_level = (*pos_ini).second;
    }
//     if(max_level - logSigmaLevels[numLevels - 1] < 0.000001) {
    if(max_level == numLevels - 1) {
      h_curves.push_back(max_level);
      continue;
    }
      
    h = max_level;
    h_curves.push_back(h);
    
    if((w * h) <= area_lim) {
      sum_h += h;
      sum_h2 += h*h;
      cnt++;
//       cout  << sum_h << " ";

    }
    
  }
//   cout << endl;
  if(cnt) {
    mean_h = sum_h/cnt;
    sd_h = sqrt(sum_h2/cnt - mean_h*mean_h);
  }
  else {
    mean_h = 1; // ver siguiente if
    sd_h = 0;
  }
  
//   for(int i = 0; i < numLevels; i++) {
//     if(logSigmaLevels[i] >= mean_h)  // sin sd
//       bottomLevel = numLevels - i;
//       break;
// //   }
  if(abs(mean_h - int(mean_h)) < 0.000001)
    bottomLevel = int(mean_h) - 1;
  else
    bottomLevel = int(mean_h);
    
  int size_contour = contourPoints.size();
  QVector< pair<double, int> >::iterator pos;
  double limit = mean_h + k_filter*sd_h;
  for(int j = 0; j < size_contour; j++) {
    if(h_curves[j] >= limit) {
//       cout << "CurveD: " << j << "  con puntos: " <<  contourPoints[j].size() << endl;
      pos = contourPoints[j].begin();
      for(; pos < contourPoints[j].end(); ++pos) {
        levelPoints[(*pos).second].push_back(j);
//         cout << (*pos).first << " " << (*pos).second << endl;
      }
    }
  }  
//   for(int i = 0; i < levelPoints.size(); i++) {
//     cout << "level " << i << endl;
//     for(int j = 0; j < levelPoints[i].size(); j++) 
//       cout << levelPoints[i][j] << " ";
//     cout << endl << endl;
//   }
}

void ScaleSpace::printResults(void)
{
  cout << "Areas: " <<  measuredAreas.size() << endl;
  for(int i = 0; i < measuredAreas.size(); i++)
    cout << "(" << measuredAreas[i].first << "," << measuredAreas[i].second << ")  ";
  cout << endl;
  cout << "Seeds: " <<  areaSeeds.size() << endl;
  for(uint j = 0; j < areaSeeds.size(); j++)
    cout << areaSeeds[j].first << ", " << areaSeeds[j].second << endl;
  cout << endl;
}

void ScaleSpace::computeMeasuresAreas(QVector< pair < int , int > > noMeasuredAreas)
{

  int noMeasuredAreasSize = noMeasuredAreas.size();

  if(noMeasuredAreasSize > 1) {
    if(full360 && noMeasuredAreas[0].first == 0 && noMeasuredAreas.back().second == numSensors - 1) {
      noMeasuredAreas[0].first = noMeasuredAreas.back().first;
      noMeasuredAreas.pop_back();
      noMeasuredAreasSize--;
    }
  }
//   if(full360 && noMeasuredAreasSize > 1 && (noMeasuredAreas.back().first > noMeasuredAreas.back().second)) {
//     noMeasuredAreas.insert(noMeasuredAreas.begin(), noMeasuredAreas.back());
//     measuredAreas.pop_back();
//   }
  
//   for(int i = 0; i < noMeasuredAreasSize; i++)
//     cout << "(" << noMeasuredAreas[i].first << "," << noMeasuredAreas[i].second << ")  ";
//   cout << endl;
  
  measuredAreas.clear();
  if(noMeasuredAreasSize == 0) {
    pair<int, int> p(0, numSensors - 1);
    measuredAreas.push_back(p);
  }
  else if(noneMeasures) // noneMeasures
    return;
  else {
    int start, end;
    for(int j = 0; j < noMeasuredAreasSize; j++) {
      
      if(j == 0) {
        if(full360) {
          start = noMeasuredAreas[0].second + 1;          
          if(noMeasuredAreasSize > 1)
            end = noMeasuredAreas[1].first - 1;
          else
            end = noMeasuredAreas[0].first - 1;
          start = start == numSensors ? 0 : start;
          end = end == -1 ? numSensors - 1 : end;
        }
        else {
          if(noMeasuredAreas[0].first > 0) {
            start = 0;
            end = noMeasuredAreas[0].first - 1;
            pair<int, int> p(start, end);
            measuredAreas.push_back(p);            
          }
//           else {
            start = noMeasuredAreas[0].second + 1;
            if(noMeasuredAreasSize > 1)
              end = noMeasuredAreas[1].first - 1;
            else
              end = numSensors - 1;
//           }
        }

        if(full360 || (!full360 && start <= end)) {
          pair<int, int> p(start, end);
          measuredAreas.push_back(p);
        }
             
//         if(noMeasuredAreas[0].first > 0 && noMeasuredAreasSize > 1) {
//           start = noMeasuredAreas[0].second + 1;
//           end = noMeasuredAreas[1].first - 1;
//           pair<int, int> p(start, end);
//           measuredAreas.push_back(p);
//         }
      }
      
      else if(j < noMeasuredAreasSize - 1) {
        start = noMeasuredAreas[j].second + 1;
        end = noMeasuredAreas[j + 1].first - 1;
        if(full360 || (!full360 && start <= end)) {
          pair<int, int> p(start, end);
          measuredAreas.push_back(p);
        }
      }
//       else {
//         if(full360) {
//           start = noMeasuredAreas[noMeasuredAreasSize - 1].second + 1;
//           end = noMeasuredAreas[0].first - 1;
//           start = start == numSensors ? 0 : start;
//           end = end == -1 ? numSensors - 1 : end;
//           pair<int, int> p(start, end);
//           measuredAreas.push_back(p);
//         }
//         else if(noMeasuredAreas[noMeasuredAreasSize - 1].second < numSensors - 1) {
//           start = noMeasuredAreas[noMeasuredAreasSize - 1].second + 1;
//           end = numSensors - 1;
//           pair<int, int> p(start, end);
//           measuredAreas.push_back(p);
//         }
//       }
    }

    if(noMeasuredAreasSize > 0) {
      if(full360) {
        start = noMeasuredAreas[noMeasuredAreasSize - 1].second + 1;
        end = noMeasuredAreas[0].first - 1;
        start = start == numSensors ? 0 : start;
        end = end == -1 ? numSensors - 1 : end;
      }
      else if(noMeasuredAreas[noMeasuredAreasSize - 1].second < numSensors - 1) {
        start = noMeasuredAreas[noMeasuredAreasSize - 1].second + 1;
        end = numSensors - 1;
      }
      if(measuredAreas.size()) {
        if(start != measuredAreas.back().first && end != measuredAreas.back().second) {
          if(full360 || (!full360 && start <= end)) {
            pair<int, int> p(start, end);
            measuredAreas.push_back(p);
          }
        }
      }
    }
    
  }
  if(measuredAreas.size()) {
    while(measuredAreas.back().first < measuredAreas[0].first) {
      measuredAreas.insert(measuredAreas.begin(), measuredAreas.back());
      measuredAreas.pop_back();
    }
  }

//   for(int i = 0; i < measuredAreas.size(); i++)
//     cout << "(" << measuredAreas[i].first << "," << measuredAreas[i].second << ")  ";
//   cout << endl;
}
