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

#include <iostream>
#include "icpalgo.h"

using namespace std;

ICPAlgo::ICPAlgo()
 : ScanMatchingAlgo()
{
}

ICPAlgo::~ICPAlgo()
{
}

// minimizes least square error of points through changing lsa->rx, lsa->ry,lsa->th
// by using ICP. Only measurements furher than 32m are ignored. Interpolation is
// not implemented. Only the best 80% of points are used.
// scan projection is done in each step.

PM_TYPE ICPAlgo::match(PMScan* lsr, PMScan* lsa, int pms_method)
{
//   #define GR //comment out if no graphics necessary
  PMScan    act,  ref;//copies of actual and reference scans
  PM_TYPE   lx,ly,lth;//laser position in the ref. frame
  PM_TYPE   rx,ry,rth,ax,ay,ath;//robot pos at ref and actual scans
  PM_TYPE   t13,t23,LASER_Y = PM_LASER_Y,delta;
  QVector<PM_TYPE>   r(PM_L_POINTS),fi(PM_L_POINTS);//actual scan in ref. coord. syst.
  QVector<int>  new_bad(PM_L_POINTS);//bad flags of the projected actual scan range readings
  int       new_r[PM_L_POINTS];//ranges of actual scan projected into ref. frame for occlusion check
  QVector<PM_TYPE>   nx(PM_L_POINTS);//actual scanpoints in ref coord system
  QVector<PM_TYPE>   ny(PM_L_POINTS);//actual scanpoints in ref coord system
  int       index[PM_L_POINTS][2];//match indices actual,refernce
  QVector<PM_TYPE>   dist(PM_L_POINTS);// distance for the matches
  int       n = 0;//number of valid points
  int       iter,i,j,small_corr_cnt=0,k,imax;
  int       window       = 20;//+- width of search for correct orientation
  PM_TYPE   abs_err=0,dx=0,dy=0,dth=0;//match error, actual scan corrections
//   PM_TYPE   MAX_RANGE = 3200,co,si,x,y;
  PM_TYPE   MAX_RANGE = 10000,co,si,x,y;

  #ifdef  PM_GENERATE_RESULTS
     long long int start_tick, dead_tick,end_tick,end_tick2;
     FILE *f;
     f = fopen(PM_TIME_FILE,"w");
     dead_tick = 0;
     start_tick =rdtsc();
  #endif


  act = *lsa;
  ref = *lsr;

  rx = ref.rx(); ry = ref.ry(); rth = ref.th();
  ax =  act.rx(); ay = act.ry(); ath = act.th();

  //transformation of actual scan laser scanner coordinates into reference
  //laser scanner coordinates
  t13 = sin(rth-ath)*LASER_Y+cos(rth)*ax+sin(rth)*ay-sin(rth)*ry-rx*cos(rth);
  t23 = cos(rth-ath)*LASER_Y-sin(rth)*ax+cos(rth)*ay-cos(rth)*ry+rx*sin(rth)-LASER_Y;

  ref.setRx(0);   ref.setRy(0);   ref.setTh(0);
  act.setRx(t13); act.setRy(t23); act.setTh(ath-rth);

  ax = act.rx(); ay = act.ry(); ath = act.th();
  //from now on act.rx,.. express the lasers position in the ref frame
  //intializing x,y of act and ref

//   QVector<PM_TYPE> ref_r = 

  for(i=0;i<PM_L_POINTS;i++)
  {
    ref.setX(i,ref.r(i)*pm_co[i]);
    ref.setY(i,ref.r(i)*pm_si[i]);
    if(ref.r(i)>MAX_RANGE)
      ref.setBad(i, 1);
    else
      ref.setBad(i, 0);

    act.setX(i,act.r(i)*pm_co[i]);
    act.setY(i,act.r(i)*pm_si[i]);
    if(act.r(i)>MAX_RANGE)
      act.setBad(i, 1);
    else
      act.setBad(i, 0);
  }//for i

  iter = -1;
  while(++iter<60 && small_corr_cnt<3) //has to be 5 small corrections before stop
  {

//     if( (fabs(dx)+fabs(dy)+fabs(dth)*PM_R2D)<0.1 )
    if( (fabs(dx/10)+fabs(dy/10)+fabs(dth)*PM_R2D)<0.1 )
      small_corr_cnt++;
    else
      small_corr_cnt=0;

    #ifdef  PM_GENERATE_RESULTS
       end_tick =rdtsc();
       fprintf(f,"%i %lf %lf %lf %lf\n",iter,
          (double)(end_tick-start_tick-dead_tick)*1000.0/CPU_FREQ,ax,ay,ath*PM_R2D);
       end_tick2 =rdtsc();
       dead_tick += end_tick2- end_tick;
    #endif

    #ifdef GR
      dr_erase();
      dr_circle(ax,ay,5.0,"green");
      dr_line(0,-100,200,-100,"black");
      dr_line(0,-200,200,-200,"black");
    #endif
    act.setRx(ax);act.setRy(ay);act.setTh(ath);
    co = cos(ath);
    si = sin(ath);

    //scan projection


    for(i=0;i<PM_L_POINTS;i++)
      new_r[i] = 20000;//initializing "z" buffer for occlusion check

    
    int r0,r1,fi0,fi1;//for occlusion detection purpose
    //transformation of points:
    for(i=0;i<PM_L_POINTS;i++)
    {
      x         = act.x(i)*co - act.y(i)*si + ax;
      y         = act.x(i)*si + act.y(i)*co + ay;
      nx[i]     = x;
      ny[i]     = y;
      new_bad[i]= act.bad(i);
      r[i]      = sqrt(x*x+y*y);
      fi[i]     = atan2(y,x);

      //fill up new_r -> contains the smallest range of actual scan in ref.frame
      // for detecting occlusion of the actual scan by the actual scan
      r1        = (int)r[i];
      if(fi[i]<-M_PI/2.0) //negative numbers around -180 are a problem with indexing
        fi1       = (int)roundf((fi[i]+2.0*M_PI) *PM_R2D);
      else
        fi1       = (int)roundf(fi[i]*PM_R2D);
      if(i!=0)
      {
        if((fi1-fi0)!=0 && act.seg(i)!=0 && act.seg(i)==act.seg(i-1))
        {// did they project into 2 different points?
         //are they part of the same segment? - so a big gap doesn't cover up a lot of points
          int rr,dr,s = (fi1>=fi0)?1:-1;
          dr = (r1-r0)/(fi1-fi0);
          for(int j=fi0; j<=fi1; j+=s)
          {
            rr = r0 + dr*(j-fi0);
            if(j>=0 && j<PM_L_POINTS)
              if(rr<new_r[j])
                new_r[j] = rr;
          }//for j
        }//if
        else //fi0=fi1
          if(fi1>=0 && fi1<PM_L_POINTS)
             new_r[fi1] = (r0>r1)?r0:r1;//set the longer range to not to remove too many points later        
      }
      r0        = r1;
      fi0       = fi1;

    #ifdef GR
      if(ref.bad[i])
        dr_circle(ref.x[i],ref.y[i],4,"yellow");
      else
        dr_circle(ref.x[i],ref.y[i],4,"black");
      if(new_bad[i])
        dr_circle(nx[i],ny[i],4,"green");
      else
        dr_circle(nx[i],ny[i],4,"blue");
    #endif

    }//for i
//    dr_zoom();
    #ifdef GR
      cout <<"interpolated ranges. press enter"<<endl;
      for(i=0;i<PM_L_POINTS;i++)
        dr_circle(new_r[i]*pm_co[i],new_r[i]*pm_si[i],6,"red");
      dr_zoom();
    #endif

    //removing covered points
    for(i=0;i<PM_L_POINTS;i++)
    {
      //is it out of range?
      if(fi[i]<0 || fi[i]>M_PI)
      {
        new_bad[i] = 1;
        continue;
      }
      //was it taken from behind?
      if(i>0 && i<(PM_L_POINTS-1))
      {
        if((fi[i+1]-fi[i-1]) < 0) //wrong order
        {
          new_bad[i] = 1;
          continue;
        }
      }
      //searching if the reference scan is occluding some parts actual scan
      int fi_l = (int)floor(fi[i]*180/M_PI);
      int fi_h = (int)ceil(fi[i]*180/M_PI);

      if(fi_l>=0 && fi_l<=PM_L_POINTS && fi_h>=0 && fi_h<=PM_L_POINTS)
      {
        //interpolate a range reading from ref scan & compare
        //1 deg assumption!
        PM_TYPE rr;
        rr = ref.r(fi_l) + (ref.r(fi_h)-ref.r(fi_l))*(fi[i]-pm_fi[fi_l]);
/*        if((r[i]-rr)>100) // is the reference scan covering out the*/
         if((r[i]-rr)>1000) // is the reference scan covering out thecurrent scan?
        {
          new_bad[i] = 1;
          continue;
        }
        //check if any actual scan point is occluding
        int idx = (int)roundf(fi[i]*PM_R2D);//index into new_r
//         if(idx>=0 && idx<PM_L_POINTS && (r[i]-new_r[i])>100)
        if(idx>=0 && idx<PM_L_POINTS && (r[i]-new_r[i])>1000)
        {
          new_bad[i] = 1;
          continue;
        }
      }
      
      //BUG: remove the scanpoints covered up by the current scan too!
    }//for i

    #ifdef GR
      for(i=0;i<PM_L_POINTS;i++)
      if(new_bad[i])
        dr_circle(nx[i],ny[i],4,"green");
    #endif


    //correspondence search ...
    n=0;
    PM_TYPE d,min_d;
    int min_idx;

    for(i=0;i<PM_L_POINTS;i++)
    {
      min_d = 1000000;
      min_idx = -1;
      if(!new_bad[i])
      {

        int imin,imax;
        imin = (int)(fi[i]*180/M_PI-window);
        if(imin<0)
          imin =0;
        imax = (int)(fi[i]*180/M_PI+window);
        if(imax>PM_L_POINTS)
          imax =PM_L_POINTS;


//        for(j=0;j<PM_L_POINTS;j++)
        for(j=imin;j<imax;j++)
        {
          if(!ref.bad(j))
          {
            d =  SQ(nx[i]-ref.x(j)) + SQ(ny[i]-ref.y(j));//square distance
            if(d<min_d)
            {
              min_d  = d;
              min_idx = j;
            }
          }
        }//for
        if(min_idx>=0 && sqrt(min_d)<PM_MAX_ERROR) // was there any match closer than 1m?
        {
          index[n][0] = i;
          index[n][1] = min_idx;
          dist[n] = sqrt(min_d);
          n++;
          #ifdef GR
              dr_line(nx[i],ny[i],ref.x[min_idx],ref.y[min_idx],"blue");
          #endif

        }
      }//if
    }//for
//    dr_zoom();

    if(n<PM_MIN_VALID_POINTS)
    {
      cerr <<"pm_icp: ERROR not enough points"<<endl;
      #ifdef  PM_GENERATE_RESULTS
          fclose(f);
      #endif
      throw 1;
    }

    //sort the matches with bubble sort
    //put the largest 20 percent to the end
    imax = (int)((double)n*0.2);
    for(i=0;i<imax;i++)
      for(j=1;j<(n-i);j++)
      {
        if(dist[j]<dist[j-1]) //are they in the wrong order?
        { //swap them
          k             = index[j][0];
          index[j][0]   = index[j-1][0];
          index[j-1][0] = k;

          k             = index[j][1];
          index[j][1]   = index[j-1][1];
          index[j-1][1] = k;

          d             = dist[j];
          dist[j]       = dist[j-1];
          dist[j-1]     = d;
        }
      }//for j


    //pose estimation
    //------------------------------------------translation-------------

    // do the weighted linear regression on the linearized ...
    // include angle as well
    PM_TYPE dr;

    //computation of the new dx1,dy1,dtheta1
    PM_TYPE dx1,dy1,dtheta1;
    PM_TYPE sxx=0,sxy=0,syx=0,syy=0;
    PM_TYPE meanpx,meanpy,meanppx,meanppy;
    PM_TYPE apx[PM_L_POINTS], apy[PM_L_POINTS],ppx[PM_L_POINTS], ppy[PM_L_POINTS];
    meanpx = 0;meanpy = 0;
    meanppx= 0;meanppy= 0;

    abs_err=0;
    imax = n-imax;
    for(i=0;i<imax;i++)
    {
      //weight calculation
      // do the cartesian calculations....
      meanpx +=  nx[index[i][0]];
      meanpy +=  ny[index[i][0]];

      meanppx +=  ref.x(index[i][1]);
      meanppy +=  ref.y(index[i][1]);
      #ifdef GR
        dr_line(nx[index[i][0]],ny[index[i][0]],ref.x(index[i][1]),ref.y(index[i][1]),"red");
      #endif
    }//for
    meanpx /= imax;
    meanpy /= imax;

    meanppx /= imax;
    meanppy /= imax;

    for(int i=0;i<imax;i++)
    {
        sxx += (nx[index[i][0]] - meanpx)*(ref.x(index[i][1]) - meanppx);
        sxy += (nx[index[i][0]] - meanpx)*(ref.y(index[i][1]) - meanppy);
        syx += (ny[index[i][0]] - meanpy)*(ref.x(index[i][1]) - meanppx);
        syy += (ny[index[i][0]] - meanpy)*(ref.y(index[i][1]) - meanppy);
    }
      //computation of the resulting translation and rotation
      //for method closest point match
    dth = atan2(sxy-syx,sxx+syy);
    dx  = meanppx - ax - (cos(dth)*(meanpx- ax) - sin(dth)*(meanpy - ay));
    dy  = meanppy - ay - (sin(dth)*(meanpx- ax) + cos(dth)*(meanpy - ay));

    ax += dx;
    ay += dy;
    ath+= dth;
    ath = norm_a(ath);

//    //for SIMULATION iteration results..
//    cout <<iter<<"     "<<ax<<"    "<<ay<<"    "<<ath*PM_R2D<<" ;"<<endl;
    #ifdef GR
      cout <<"iter "<<iter<<" "<<ax<<" "<<ay<<" "<<ath*PM_R2D<<" "<<dx<<" "<<dy<<endl;
//      if(iter==0)
        dr_zoom();
      usleep(10000);

    #endif

  }//for iter
  #ifdef  PM_GENERATE_RESULTS
     end_tick =rdtsc();
     fprintf(f,"%i %lf %lf %lf %lf\n",iter,
        (double)(end_tick-start_tick-dead_tick)*1000.0/CPU_FREQ,ax,ay,ath*PM_R2D);
     fclose(f);
  #endif

  lsa->setRx(ax);lsa->setRy(ay);lsa->setTh(ath);
  return(abs_err/n);
}//pm_icp
