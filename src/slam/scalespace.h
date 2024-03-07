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
#ifndef SCALESPACE_H
#define SCALESPACE_H

//#define FFTW_NO_Complex
#include <iostream>
#include <cmath>
#include <QVector>
#include <utility>
#include <complex.h>
#include <fftw3.h>

#include "../neuraldis/matriz.hpp"
#include "../neuraldis/matriz3d.hpp"

/**
	@author Jose Manuel Cuadra Troncoso <jmcuadra@dia.uned.es>
*/

class ScaleSpace{
  protected:
    class rectStruct {
      public:
        rectStruct():isInit(false) {};
        void setValues(int initL, int endL, double senS, double senE) {
          initLevel = initL;
          endLevel = endL;
          sensorStart = senS;
          sensorEnd = senE;
          isInit = true;
        };
        ~rectStruct() {
//            cout << "removing " << initLevel << " " << endLevel<< " " << sensorStart << " " << sensorEnd << endl;
        };

        friend ostream&
        operator <<(ostream& os, rectStruct rect) {
           os << rect.initLevel << " " << rect.endLevel << " " << rect.sensorStart << " " << rect.sensorEnd << endl;
           return os;
        }
      public:
        int initLevel;
        int endLevel;
        double sensorStart;
        double sensorEnd;
        vector<rectStruct> children;
        bool isInit;
    };

    class areaStruct {
      public:
        void setValues(int lev, double senS, double senE) {
          level = lev;
          sensorStart = senS;
          sensorEnd = senE;
        };
      public:
        int level;
        double sensorStart;
        double sensorEnd;
    };
       
    class matRectStruct {
      public:
          matRectStruct():x(0), h(0), stable(false) {}
          void setValues(double x, int h, bool stable) {
          this->x = x;
          this->h = h;
          this->stable = stable;
        };
      public:
        double x;
        int h;
        double stable;
    };       
      
  public:
    ScaleSpace(size_t numSensors, double sensorField, double startAngle = -90);

    ~ScaleSpace();

//     void setSigmaLevels(short numLevels, short maxLevel, short zeroLevel, short subZeroLevel, short bottomLevel, double confidenceLevel);
    void setSigmaLevels(short numLevels, short maxLevel, double confidenceLevel, double k_semi_amplitude);
    void setDerivOrders(short firstDOrder);
    void initRadius(double radius[]);
    void setRadius(double radius[]);
    QVector <pair <int, int> > perform(double k_filter = 2.5, double area_lim = 50, bool verbose = false);

    void printResults(void);

    QVector< pair < int , int > > getMeasuredAreas() const { return measuredAreas; }
    
  void setSensorMaxVal(int sensorMaxVal) {maxSensorVal = sensorMaxVal; }    

  bool getFull360() const
  {
    return full360;
  }
  
  

  protected:
    void derivative(void);
    void smooth(void);
    void digitalContours(void);
    bool interpolate(int i, int j);
//     void statsContours(double width = 2);
    void statsContours(double k_filter, double area_lim = 50);
//     void scaleTree(rectStruct* subtree, int sensor_start, int sensor_end);
    void scaleTree(rectStruct* subtree, double sensor_start, double sensor_end);
//     static bool sortAreasCriterium(areaStruct vec1, ScaleSpace::areaStruct vec2)
//       {
//         return vec1.sensorStart < vec2.sensorStart;
//       }
    void deleteRectStruct(rectStruct& rect);
    void initRectStruct(rectStruct& rect_str);
//     vector <vector <int> > tree2Matrix(rectStruct* subtree, double parentHeight, bool stable_ancestor);
    vector<matRectStruct> tree2Matrix(rectStruct* subtree, double parentHeight, bool stable_ancestor);
    void computeSeeds(void);
    void computeMeasuresAreas(QVector <pair <int, int> > noMeasuredAreas);
    
  public:
    static const double RAD2GRAD;
    static short maxLevelMax;
    
  protected:
    static const double CERO;   
    bool full360;
    int pad_size;
    size_t numSensors;
    double sensorField;
    double sensorAperture;
    double startAngle;

//     double* radius;
    double* paddedRadius;
    double* angle;
    
    short numLevels;
    short maxLevel;
//     short zeroLevel;
//     short subZeroLevel;
    short bottomLevel;
    double confidenceLevel;

    double* sigmaLevels;
    double* logSigmaLevels;
    double** derivative3DMat;
    vector<double*> smoothedRad;
    Matriz3D <int> interpGrid;
    
    vector<double*> kernels;
    fftw_complex* transformsRad;
    vector<fftw_complex*> transformsKer;
    vector<fftw_complex*> productRadKer;

    fftw_plan fftwPlan;
    vector<fftw_plan> fftwInversePlans;
    int planSize;
//     int prevPlanSize;
//     short cntPlansRad;
//     vector<uint> sizePlans;
//     vector<short> idxPlans;
    vector<int> kernSemiSize;

    short dOrderEven, dOrderOdd, currentOrder;

    QVector< QVector< pair<double, int> > > contourPoints;
    vector<bool> splitContours;
//     vector< vector< pair<double, int> > > levelPoints;
    vector< vector< int > > levelPoints;
    int newLevel, newSensor, cellSide; //cellSide: 0-->abajo, 1-->dcha, 2-->arriba, 3-->izq
    int cntRealPoint, numCurve;

    rectStruct rectTree;
    vector <vector <double> > matRectangles;
    QVector <pair <int, int> > areaSeeds;
    QVector <pair <int, int> > measuredAreas;
    int maxSensorVal, dummyVal;
    int maxNoMeasurePoints;
    bool noneMeasures;

};



#endif
