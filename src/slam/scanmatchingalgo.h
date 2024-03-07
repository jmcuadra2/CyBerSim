/***************************************************************************
        Adapted from project for scan matching in polar coordinates
                           -------------------
   begin                : mon nov 8 2004
   version              : 0.1
   copyright            : (C) 2005 by Albert Diosi and Lindsay Kleeman
   email                : albert.diosi@gmail.com
***************************************************************************/
/***************************************************************************
 *   Copyright (C) 2009 by Jose Manuel Cuadra Troncoso   *
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
#ifndef SCANMATCHINGALGO_H
#define SCANMATCHINGALGO_H

#include <cmath>
#include "pmscan.h"

#define PM_LASER_Y  0 // 125//31.3 // y coord of the laser on the robot
//for ground thruth & simulation, has to be set to 0

#define PM_MAX_ERROR  1000 //[mm] max distance between associated points used in pose est.
// #define PM_MAX_ERROR  100 //[cm] max distance between associated points used in pose est.

//#define PM_MIN_VALID_POINTS 30 //minimum number of valid points for scanmatching

#define SQ(x) (x)*(x)

//#define PM_MAX_RANGE  12000 // max valid laser range

#define PM_MAX_ITER   30 // maximum number of iterations for polar scan matching

#define PM_RANGE     1  // range reading is longer than PM_MAX_RANGE
#define PM_MOVING    2  // range reading corresponds to a moving point
#define PM_MIXED     4  // range reading is a mixed pixel
#define PM_OCCLUDED  8  // range reading is occluded
#define PM_EMPTY     16 // at that bearing there is no measurment (between 2
//    segments there is no interpolation!)

#define PM_TIME_DELAY 0.00 //time delay in the laser measurements, por ahora se usa 0.01, en SICK es 0.02

/**
  @author Jose Manuel Cuadra Troncoso <jmcuadra@dia.uned.es>
*/
class ScanMatchingAlgo
{
  public:

    enum Tipes {PM_PSM = 1, PM_PSM_C = 2, PM_ICP = 3 }; // polar scanmatching - matching bearing
    // polar scanmatching - using cartesian equations
    // scan matchign with iterative closest point

    ScanMatchingAlgo();

    virtual ~ScanMatchingAlgo();

    virtual PM_TYPE match(PMScan* lsr, PMScan* lsa, int pms_method) = 0;
    void medianFilter(PMScan* ls);
    int translate(PMScan *ls, PM_TYPE dx, PM_TYPE dy, int di, PMScan *act);
    void findFarPoints(PMScan* ls);
    void segmentScan(PMScan* ls);
    bool isCorridor(PMScan* ls);
    void angleHistogramMatch(PMScan *lsr, PMScan *lsa);
    PM_TYPE errorIndex(PMScan *lsr, PMScan *lsa);
    PM_TYPE corridorAngle(PMScan *act);
    void covEstimation(PM_TYPE err, double *c11, double *c12, double *c22, double *c33, bool corridor = false, PM_TYPE corr_angle = 0);

    virtual void pm_preprocessScan(PMScan* scan) {}

    static PM_TYPE norm_a(PM_TYPE a) {
      int m;
      m = (int)(a / (2.0 * M_PI));
      if(a < 0)
          m -= 1;
      a = a - (PM_TYPE)m * 2*M_PI;
      if (a < (-M_PI))
        a += 2.0 * M_PI;
      if (a >= M_PI)
        a -= 2.0 * M_PI;
      return(a);
    }

    static ScanMatchingAlgo* factory(int algorithm);

    virtual char* strName() const = 0;

    QVector< PM_TYPE > fi() {
      return pm_fi;
    }

    QVector< PM_TYPE > sinFi() {
      return pm_si;
    }

    QVector< PM_TYPE > cosFi() {
      return pm_co;
    }

    void setFi(const QVector< PM_TYPE >& fi); /*{
      pm_fi = fi;
    }*/

    void setSinFi(const QVector< PM_TYPE >& sinfi) {
      pm_si = sinfi;
    }

    void setCosFi(const QVector< PM_TYPE >& cosfi) {
      pm_co = cosfi;
    }

    __inline__ unsigned long long int rdtsc()
    {
      unsigned long long int x;
      __asm__ volatile (".byte 0x0f, 0x31" : "=A" (x));
      return x;
    }

    void setSensorMaxVal(int sensorMaxVal) {this->sensorMaxVal = sensorMaxVal ;}
    bool getMatchOk(void) {return matchOk;}

  public:
    static PM_TYPE   PM_D2R; // degrees to rad
    static PM_TYPE   PM_R2D; // rad to degrees

  protected:
    QVector<PM_TYPE>   pm_fi;//contains precomputed angles (0-180)
    QVector<PM_TYPE>   pm_si;//contains sinus of angles
    QVector<PM_TYPE>   pm_co;//contains cos of angles

    int sensorMaxVal;
    bool matchOk;

};

#endif
