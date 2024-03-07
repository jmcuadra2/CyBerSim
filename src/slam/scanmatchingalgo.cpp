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

#include <cmath>
#include <iostream>
//#include "scanmatchingalgo.h"
#include "pmsalgo.h"
#include "psmcartesianalgo.h"
#include "icpalgo.h"

using namespace std;

PM_TYPE ScanMatchingAlgo::PM_D2R = M_PI / 180.0;
PM_TYPE ScanMatchingAlgo::PM_R2D = 180.0 / M_PI;

ScanMatchingAlgo::ScanMatchingAlgo() : pm_fi(PM_L_POINTS), pm_si(PM_L_POINTS), pm_co(PM_L_POINTS)
{
  sensorMaxVal = PM_MAX_RANGE;
  matchOk = false;
}


ScanMatchingAlgo::~ScanMatchingAlgo()
{
}

//filters the ranges with a median filter. x,y points are not upadted
//ls - laser scan
// seems like the median filter is a good thing!
//if window is 5, then 3 points are needed in a bunch to surrive!
//don't use this function with line fitting!

void ScanMatchingAlgo::medianFilter(PMScan * ls)
{
  const int HALF_WINDOW  = 2;//2 to left 2 to right
  const int WINDOW = 2 * HALF_WINDOW + 1;
  PM_TYPE   r[WINDOW];
  PM_TYPE   w;

  int i, j, k, l;

  for (i = 0;i < PM_L_POINTS;i++) {
    k = 0;
    for (j = i - HALF_WINDOW;j <= i + HALF_WINDOW;j++) {
      l = ((j >= 0) ? j : 0);
      PM_TYPE rk = ls->r(((l < PM_L_POINTS)?l:(PM_L_POINTS-1)));
      r[k] = ls->r(((l < PM_L_POINTS)?l:(PM_L_POINTS-1)));
      k++;
    }
    //bubble sort r
    for (j = (WINDOW - 1);j > 0;j--)
      for (k = 0;k < j;k++)
        if (r[k] > r[k+1]) { // wrong order? - swap them
          w = r[k];
          r[k] = r[k+1];
          r[k+1] = w;
        }
    ls->setR(i, r[HALF_WINDOW]);//choose the middle point
  }
//   cout << "median " << *ls << endl;
}

void ScanMatchingAlgo::segmentScan(PMScan * ls)
{
//   const PM_TYPE   MAX_DIST = 20.0;//max range diff between conseq. points in a seg
  const PM_TYPE   MAX_DIST = 200.0;//max range diff between conseq. points in a seg
  const PM_TYPE   D_PHI    = M_PI / (PM_L_POINTS - 1);//angle increments
  PM_TYPE   r, dr;
  int       seg_cnt = 0;
  int       i, j, k, cnt;
  bool      break_seg;

  seg_cnt = 1;

  //init:
  if (fabs(ls->r(0) - ls->r(1)) < MAX_DIST) { //are they in the same segment?
    ls->setSeg(0, seg_cnt);
    ls->setSeg(1, seg_cnt) ;
    cnt        = 2;    //2 points in the segment
  }
  else {
    ls->setSeg(0, 0); //point is a segment in itself
    ls->setSeg(1, seg_cnt);
    cnt        = 1;
  }

  for (i = 2;i < PM_L_POINTS;i++) { //segment breaking conditions: - bad point;
    break_seg = false;
    if (ls->bad(i)) {
      break_seg = true;
      ls->setSeg(i, 0);
    }
    else {
      dr = ls->r(i) - (2.0 * ls->r(i-1) - ls->r(i-2));//extrapolate & calc difference
      if (fabs(ls->r(i) - ls->r(i-1)) < MAX_DIST || ((ls->seg(i-1) == ls->seg(i-2))
          && fabs(dr) < MAX_DIST)) {//not breaking the segment
        cnt++;
        ls->setSeg(i, seg_cnt);
      }
      else
        break_seg = true;
    }//if ls
    if (break_seg) { // breaking the segment?
      if (cnt == 1) {
        //check first if the last three are not on a line by coincidence
        dr = ls->r(i) - (2.0 * ls->r(i-1) - ls->r(i-2));
        if (ls->seg(i-2) == 0 && ls->bad(i) == 0 && ls->bad(i-1) == 0
            && ls->bad(i-2) == 0 && fabs(dr) < MAX_DIST) {
          ls->setSeg(i, seg_cnt);
          ls->setSeg(i-1, seg_cnt);
          ls->setSeg(i-2, seg_cnt);
          cnt = 3;
        }//if ls->
        else {
          ls->setSeg(i-1, 0);
          //what if ls[i] is a bad point? - it could be the start of a new
          //segment if the next point is a good point and is close enough!
          //in that case it doesn't really matters
          ls->setSeg(i, seg_cnt);//the current point is a new segment
          cnt = 1;
        }
      }//if cnt ==1
      else {
        seg_cnt++;
        ls->setSeg(i, seg_cnt);
        cnt = 1;
      }//else if cnt
    }//if break seg
  }//for
//   cout << "segment " << *ls << endl;
}

// marks point further than a given distance PM_MAX_RANGE as PM_RANGE

void ScanMatchingAlgo::findFarPoints(PMScan * ls)
{
  for (int i = 0;i < PM_L_POINTS;i++) {
    if (ls->r(i) > sensorMaxVal)
      ls->setBad(i, ls->bad(i) | PM_RANGE);
  }
//   cout << "bad " << *ls << endl;
}

//guesses if the scan was taken of a corridor or not
//by calculating the variance of angles between neighbouring points
//to solve the situation of 180-0 degree transition -> repeats calcultions
//after a 30 degree shift.
//segmentation and median filtering is assumed

bool ScanMatchingAlgo::isCorridor(PMScan * act)
{
  PM_TYPE fi1 = 0, fi2 = 0, fi3 = 0;
  PM_TYPE sxx = 0, sx = 0, std1, std2;
  PM_TYPE n = 0;
//  f=fopen("output.txt","w");
//  pm_plotScan(act,"black");

  for (int i = 0;i < (PM_L_POINTS - 1);i++) {
    if (act->seg(i) == act->seg(i+1) && act->seg(i) != 0 && !act->bad(i)) { //are they in the same segment?
      PM_TYPE x, y, x1, y1, fi;
      x  = act->r(i) * pm_co[i];
      y  = act->r(i) * pm_si[i];
      x1 = act->r(i+1) * pm_co[i+1];
      y1 = act->r(i+1) * pm_si[i+1];
      fi = atan2(y1 - y, x1 - x) * PM_R2D;
//      fprintf(f,"%f\n",fi);

      if (fi < 0)   //want angles from 0 to 180
        fi += 180.0;
      if (i == 0) { //init
        fi1 = fi;
        fi2 = fi;
        fi3 = fi;
      }
      fi1 = fi;
      fi2 = fi1;
      fi3 = fi2;
      // pick out the median value
      if (fi1 <= fi2 && fi2 <= fi3)
        fi = fi2;
      if (fi2 <= fi3 && fi3 <= fi1)
        fi = fi3;
      if (fi3 <= fi1 && fi1 <= fi2)
        fi = fi2;

      sx += fi;
      sxx += fi * fi;
      n += 1.0;
//      cout <<fi<<endl;
    }//if
  }
  if (n > 1)
    std1 = sqrt((sxx - sx * sx / n) / (n - 1.0));
  else {
    cerr << "isCorridor: ERROR n<=1" << endl;
    exit(-1);
  }

  //rotate by 30 deg
  sxx = 0;
  n = 0;
  sx = 0;
  for (int i = 0;i < (PM_L_POINTS - 1);i++) {
    if (act->seg(i) == act->seg(i+1) && act->seg(i) != 0 && !act->bad(i)) { //are they in the same segment?
      PM_TYPE x, y, x1, y1, fi;
      x  = act->r(i) * cos(pm_fi[i] + M_PI / 5);
      y  = act->r(i) * sin(pm_fi[i] + M_PI / 5);
      x1 = act->r(i+1) * cos(pm_fi[i+1] + M_PI / 5);
      y1 = act->r(i+1) * sin(pm_fi[i+1] + M_PI / 5);
      fi = atan2(y1 - y, x1 - x) * PM_R2D;

      if (fi < 0)   //want angles from 0 to 180
        fi += 180.0;
      if (i == 0) { //init
        fi1 = fi;
        fi2 = fi;
        fi3 = fi;
      }
      fi1 = fi;
      fi2 = fi1;
      fi3 = fi2;
      // pick out the median value
      if (fi1 <= fi2 && fi2 <= fi3)
        fi = fi2;
      if (fi2 <= fi3 && fi3 <= fi1)
        fi = fi3;
      if (fi3 <= fi1 && fi1 <= fi2)
        fi = fi2;

      sx += fi;
      sxx += fi * fi;
      n += 1.0;
//      cout <<fi<<endl;
    }//if
  }

  if (n > 1)
    std2 = sqrt((sxx - sx * sx / n) / (n - 1.0));
  else {
    cerr << "pm_is_corridor: ERROR n<=1" << endl;
    exit(-1);
  }

  cout << "std " << std1 << " " << std2 << endl;

//  fclose(f);
//  dr_zoom();

  PM_TYPE st;
  if (std1 < std2)
    st = std1;
  else
    st = std2;
  if (st < 25) {
    cout << "corridor" << endl;
    return true;
  }
  else {
    cout << "room" << endl;
    return false;
  }

}

//calculates an error index expressing the quality of the match
//of the actual scan to the reference scan
//has to be called after scan matching so the actual scan in expressed
//in the reference scan coordinate system
//return the average minimum Euclidean distance; MAXIMUM RANGE points
//are not considered; number of non maximum range points have to be
//smaller than a threshold
//actual scan is compared to reference scan and vice versa, maximum is
//taken
//I could implement it in polar frame as well, would be O(n)

PM_TYPE ScanMatchingAlgo::errorIndex(PMScan * lsr, PMScan * lsa)
{
  int     i, j;
  PM_TYPE rx[PM_L_POINTS], ry[PM_L_POINTS], ax[PM_L_POINTS], ay[PM_L_POINTS];
  PM_TYPE x, y, xx, yy;
  PM_TYPE d, dmin, dsum, dx, dy;
  PM_TYPE dsum1;
  int     n, n1, rn = 0, an = 0;
  const   PM_TYPE HUGE_ERROR       = 1000000;
  const   int     MIN_POINTS = 100;

  lsa->setTh(norm_a(lsa->th()));
  PM_TYPE co = cos(lsa->th()), si = sin(lsa->th());
  PM_TYPE c, sig;

  //x axis equation si*x-co*y+c=0
  c = -(lsa->rx() * si - lsa->ry() * co);//calc for the x axis of the actual frame
  //"signum" of a point from the lasers view substituted into the equation
  sig = si * (lsa->rx() + cos(lsa->th() + 0.1)) - co * (lsa->ry() + sin(lsa->th() + 0.1)) + c;
//  cout <<lsa->rx*si-co*lsa->ry+c<<endl;
//  cout <<lsa->rx*si-co*(lsa->ry+10)+c<<endl;
//  cout <<lsa->rx*si-co*(lsa->ry-10)+c<<endl;



  for (i = 0;i < PM_L_POINTS;i++) {
    x = lsr->r(i) * pm_co[i];
    y = lsr->r(i) * pm_si[i];
    if (!lsr->bad(i) && sig*(si*x - co*y + c) > 0) {
      rx[rn] = x;
      ry[rn++] = y;
//      dr_circle(x,y,5,"blue");
    }//if
    if (!lsa->bad(i)) {
      x = lsa->r(i) * pm_co[i];
      y = lsa->r(i) * pm_si[i];
      ax[an] = x * co - y * si + lsa->rx();
      ay[an] = x * si + y * co + lsa->ry();
      if (ay[an] > 0) {
//        dr_circle(ax[an],ay[an],5,"brown");
        an++;
      }
    }//if
  }//for i



  dsum = 0;
  n = 0;
  for (i = 0;i < an;i++) {
    dmin = 100000;
    for (j = 0;j < rn;j++) {
      dx = rx[j] - ax[i];
      dy = ry[j] - ay[i];
      d = sqrt(dx * dx + dy * dy);
      if (d < dmin)
        dmin = d;
    }//for j
    if (dmin < 100000) {
      n++;
      dsum += dmin;
    }
  }//for i

  if (n > 0) {
    dsum1 = dsum / (PM_TYPE)n;
    n1    = n;
  }
  else
    return     HUGE_ERROR;

  //now checking the reference scan agains the actual
  dsum = 0;
  n = 0;
  for (i = 0;i < rn;i++) {
    dmin = 100000;
    for (j = 0;j < an;j++) {
      dx = rx[i] - ax[j];
      dy = ry[i] - ay[j];
      d = sqrt(dx * dx + dy * dy);
      if (d < dmin)
        dmin = d;
    }//for j
    if (dmin < 100000) {
      n++;
      dsum += dmin;
    }
  }//for i

  if (n > 0) {
    dsum = dsum / (PM_TYPE)n;
  }
  else
    return     HUGE_ERROR;

  cout << "pm_error_index: " << n1 << " " << dsum1 << " " << n << " " << dsum << endl;

  if (n1 > MIN_POINTS && n > MIN_POINTS) {
    if (dsum1 > dsum)
      return dsum1; //return the larger one
    else
      return dsum;
  }
  return     HUGE_ERROR;

}

//assuming the scan was taken of a corridor, determines the
//orientation of the corridor by finding the maximum of a
// 180 deg angle histogram

PM_TYPE ScanMatchingAlgo::corridorAngle(PMScan * act)
{
  PM_TYPE fi;
  int   n = 0, j, i;
  PM_TYPE ang[PM_L_POINTS];
  int hist[180];//180 deg angle hist. at 2 deg; 0,2,4....
//  f=fopen("output.txt","w");
//  pm_plotScan(act,"black");


  for (i = 0;i < 180;i++)
    hist[i] = 0;

  for (i = 0;i < (PM_L_POINTS - 1);i++) {
    if (act->seg(i) == act->seg(i+1) && act->seg(i) != 0 && !act->bad(i)) { //are they in the same segment?
      PM_TYPE x, y, x1, y1, fi;
      x  = act->r(i) * pm_co[i];
      y  = act->r(i) * pm_si[i];
//      cout <<x<<"    "<<y<<endl;
      x1 = act->r(i+1) * pm_co[i+1];
      y1 = act->r(i+1) * pm_si[i+1];
      fi = atan2(y1 - y, x1 - x) * PM_R2D;
//      fprintf(f,"%f\n",fi);

      if (fi < 0)   //want angles from 0 to 180
        fi += 180.0;
      ang[n] = fi;
      n++;

      j = (2 * (int)floor(fi / 2.0 + 0.5)) % 180;//index into the angle hist.
      hist[j]++;
    }//if
  }
//  for(i=0;i<180;i+=2)
//    cout <<i<<" "<<hist[i]<<endl;

  //find the maximum
  int imax, max = 0;
  for (i = 0;i < 180;i += 2)
    if (hist[i] > max) {
      max   = hist[i];
      imax  = i;
    }//if
  if (max == 0) {
    cerr << "pm_corridor_angle: ERROR no maximum" << endl;
    throw 1;
  }

  PM_TYPE m = 0;
  int cnt = 0, d;

  for (i = 0;i < n;i++) {
    fi = ang[i];
    j = ((int)floor(fi + 0.5)) % 180;//index into the angle hist.
    d = abs(j - imax);
    const int TRESHOLD = 5;
    if (d < 90 && d % 90 < TRESHOLD || d >= 90 && (90 - d % 90) < TRESHOLD) { //watch out average is tricky with angles! around 180 and 0
      cnt++;
      if (imax < 10 && j > 170)
        m += 180.0 - fi;
      else
        if (imax > 170 && j < 10)
          m += 180.0 + fi;
        else
          m += fi;
    }//i
  }
  m = m / (PM_TYPE)cnt;
  return m*PM_D2R;

}

//estimates the covariance matrix(c11,c12,c22,c33) (x,y,th) of
//a scan match based on an error index (err-depends on how good the
//match is), and the angle of the corridor if it is a +

// for non corridors cov matrix is diagonal

void ScanMatchingAlgo::covEstimation(PM_TYPE err, double * c11, double * c12, double * c22, double * c33, bool corridor, PM_TYPE corr_angle)
{
#define SQ(x) (x)*(x)
//   const double cov_x  = SQ(10);         // cm
//   const double cov_y  = SQ(10);         // cm
//   const double cov_th = SQ(2 * M_PI / 180); // 2 deg
//   //for corridors
//   const double cov_along   = SQ(400);   // cm
//   const double cov_across  = SQ(10);    // cm

  const double cov_x  = SQ(100);         // cm
  const double cov_y  = SQ(100);         // cm
  const double cov_th = SQ(2 * M_PI / 180); // 2 deg
  //for corridors
  const double cov_along   = SQ(4000);   // cm
  const double cov_across  = SQ(100);    // cm  

  err = err - 5;
  if (err < 1)
    err = 1;
  if (corridor) { //was the actual scan taken of a corridor?
    double co = cos(corr_angle);
    double si = sin(corr_angle);
    *c11 = err * (SQ(co) * cov_along + SQ(si) * cov_across);
    *c12 = err * (-co * si * (-cov_along + cov_across));
    *c22 = err * (SQ(si) * cov_along + SQ(co) * cov_across);
    *c33 = err * cov_th;
  }//if
  else {
    *c12 = 0;
    *c11 = err * cov_x;
    *c22 = err * cov_y;
    *c33 = err * cov_th;
  }
}

ScanMatchingAlgo * ScanMatchingAlgo::factory(int algorithm)
{
  ScanMatchingAlgo* algo = nullptr;
  switch (algorithm) {
    case PM_PSM:    //polar scanmatching - matching bearing
      algo = (ScanMatchingAlgo*)new PMSAlgo();
      break;
    case PM_PSM_C: //polar scanmatching - using cartesian equations
      algo = (ScanMatchingAlgo*)new PSMCartesianAlgo();
      break;
    case PM_ICP: //scanmatchign with iterative closest point
      algo = (ScanMatchingAlgo*)new ICPAlgo();
      break;
  }
  return algo;
}

void ScanMatchingAlgo::setFi(const QVector< PM_TYPE >& fi) {
  pm_fi = fi;
//   for(int i = 0; i < fi.size();i++)
//     cout << pm_fi[i] << " ";
//   cout << endl;
}
