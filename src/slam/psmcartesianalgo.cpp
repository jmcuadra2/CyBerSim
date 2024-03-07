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
#include "psmcartesianalgo.h"
#include <iostream>

using namespace std;

PSMCartesianAlgo::PSMCartesianAlgo()
 : ScanMatchingAlgo()
{
}

PSMCartesianAlgo::~PSMCartesianAlgo()
{
}

//-------------------------------------------------------------------------
// does scan matching using the equations for translation and orietation
//estimation as in Lu & milios, however our matching bearing association rule
//is used together with our association filter.
//have to do an angle search othervise doesn't converge to 0!
//weights implemented!

PM_TYPE PSMCartesianAlgo::match(PMScan* lsr, PMScan* lsa, int pms_method)
{
//   #define GR //comment out if no graphics necessary
  PMScan  act,  ref;//copies of actual and reference scans
  PM_TYPE lx,ly,lth;//laser position in the ref. frame
  PM_TYPE rx,ry,rth,ax,ay,ath;//robot pos at ref and actual scans
  PM_TYPE t13,t23,LASER_Y = PM_LASER_Y,delta;
  PM_TYPE r[PM_L_POINTS],fi[PM_L_POINTS];//actual scan in ref. coord. syst.
  PM_TYPE px[PM_L_POINTS],py[PM_L_POINTS],x,y;//xy coordinates of act. points in ref.
  PM_TYPE new_r[PM_L_POINTS];//interpolated r at measurement bearings
  int     new_bad[PM_L_POINTS];//bad flags of the interpolated range readings
  int     index[PM_L_POINTS];//which new point was used using which old point..
  PM_TYPE C = 70*70;//weighting factor; see dudek00
  int     n = 0;//number of valid points
  int       iter,i,j,small_corr_cnt=0;
  const int window       = 20;//+- width of search for correct orientation
  PM_TYPE   abs_err=0,dx=0,dy=0,dth=0;//match error, actual scan corrections


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

  ref.setRx(0); ref.setRy(0); ref.setTh(0);
  act.setRx(t13); act.setRy(t23); act.setTh(ath-rth);

  ax = act.rx(); ay = act.ry(); ath = act.th();
  //from now on act.rx,.. express the lasers position in the ref frame


  iter = -1;
  while(++iter<PM_MAX_ITER && small_corr_cnt<3) //has to be 5 small corrections before stop
  {
    if( (fabs(dx)+fabs(dy)+fabs(dth))<1 )
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
    // convert range readings into ref frame
    for(i=0;i<PM_L_POINTS;i++)
    {
      delta   = ath+pm_fi[i];
      x       = act.r(i)*cos(delta)+ax;
      y       = act.r(i)*sin(delta)+ay;
      r[i]    = sqrt(x*x+y*y);
      fi[i]   = atan2(y,x);
      px[i]   = x;
      py[i]   = y;
      new_r[i]  = 10000;//initialize big interpolated r;
      new_bad[i]= PM_EMPTY;//for interpolated r;

      //debug
      #ifdef GR
         dr_circle(ref.r[i]*pm_co[i],ref.r[i]*pm_si[i],2.0,"black");
         dr_circle(x,y,2.0,"red");
         dr_circle(pm_fi[i]*PM_R2D,ref.r[i]/10.0-100,1,"black");
         dr_circle(fi[i]*PM_R2D,r[i]/10.0-100,1,"red");
      #endif
    }//for i

    //------------------------INTERPOLATION------------------------
    //calculate/interpolate the associations to the ref scan points
    //algorithm ignores crosings at 0 and 180 -> to make it faster
    for(i=1;i<PM_L_POINTS;i++)
    { //i points to the angles in the actual scan

      // i and i-1 has to be in the same segment, both shouldn't be bad
      // and they should be bigger than 0
      if(act.seg(i)!=0 && act.seg(i)==act.seg(i-1) && !act.bad(i) &&
         !act.bad(i-1) && fi[i]>0 && fi[i-1]>0)
      {
        //calculation of the "whole" parts of the angles
        int     fi0,fi1;
        PM_TYPE r0,r1,a0,a1;
        bool occluded;
        if(fi[i]>fi[i-1])//are the points visible?
        {//visible
          occluded = false;
          a0  = fi[i-1];
          a1  = fi[i];
          fi0 = (int) ceil(fi[i-1]*PM_R2D);//fi0 is the meas. angle!
          fi1 = (int) (fi[i]*PM_R2D);
          r0  = r[i-1];
          r1  = r[i];
        }
        else
        {//invisible - still have to calculate to filter out points which
          occluded = true; //are covered up by these!
          //flip the points-> easier to program
          a0  = fi[i];
          a1  = fi[i-1];
          fi0 = (int) ceil(fi[i]*PM_R2D);
          fi1 = (int) (fi[i-1]*PM_R2D);
          r0  = r[i];
          r1  = r[i-1];
        }
        //here fi0 is always smaller than fi1!

        //interpolate for all the measurement bearings beween fi0 and fi1
        while(fi0<=fi1)//if at least one measurement point difference, then ...
        {
          PM_TYPE ri = (r1-r0)/(a1-a0)*(((PM_TYPE)fi0*PM_D2R)-a0)+r0;

          //if fi0 -> falls into the measurement range and ri is shorter
          //than the current range then overwrite it
          if(fi0>=0 && fi0<PM_L_POINTS && new_r[fi0]>ri)
          {
            new_r[fi0]    = ri;//overwrite the previous reading
            new_bad[fi0] &=~PM_EMPTY;//clear the empty flag
            index[fi0] = i;//store which reading was used at index fi0
            if(occluded) //check if it was occluded
              new_bad[fi0] = new_bad[fi0]|PM_OCCLUDED;//set the occluded flag
             else
              new_bad[fi0] = new_bad[fi0]&~PM_OCCLUDED;
            //the new range reading also it has to inherit the other flags
            new_bad[fi0] |= act.bad(i);
            new_bad[fi0] |= act.bad(i-1);
          }
          fi0++;//check the next measurement angle!
        }//while
      }//if act
    }//for i



    #ifdef GR
//     for(i=0;i<10;i++)
//     {
//       dr_circle(i,fi[i]*PM_R2D,0.5,"blue");
//       dr_circle(fi[i]*PM_R2D,r[i]*10,1,"black");
//       dr_line(fi[i-1]*PM_R2D,r[i-1]*10,fi[i]*PM_R2D,r[i]*10,"black");
//       dr_circle(pm_fi[i]*PM_R2D,new_r[i]*10,1,"red");
//       dr_line(pm_fi[i-1]*PM_R2D,new_r[i-1]*10,pm_fi[i]*PM_R2D,new_r[i]*10,"red");
//     }
//     dr_zoom();

//     cout <<ax<<" "<<ay<<" "<<ath<<endl;
    #endif

    //---------------ORIENTATION SEARCH-----------------------------------
    //search for angle correction using crosscorrelation
    if(iter%4 ==3)
    {

      //pm_fi,ref.r - reference points
      //r,fi        - actual points which are manipulated
      //new_r, new_bad contains the

      PM_TYPE e,err[100];       // the error rating
      PM_TYPE beta[100];// angle for the corresponding error
//      PM_TYPE C = 10000;
      PM_TYPE n;
      int k=0;

      for(int di=-window;di<=window;di++)
      {
        n=0;e=0;

        int min_i,max_i;
        if(di<=0)
          {min_i = -di;max_i=PM_L_POINTS;}
        else
          {min_i = 0;max_i=PM_L_POINTS-di;}

        for(i=min_i;i<max_i;i++)//searching through the actual points
        {
          //if fi[i],r[i] isn't viewed from the back, isn't moving
          // and isn't a solitary point, then try to associate it ..
          //also fi[i] is within the angle range ...
//          if(fi[i-1]<fi[i] && !act.bad[i] && act.seg[i]!=0) //can be speeded up!
//          {
//            PM_TYPE angle = fi[i]*180.0/M_PI+dfi;
//            int     fi_l  = (int)floor(angle+0.5);
//            if(fi_l>=0 ||fi_l<=180)
//            {
//              e += fabs(r[i]-ref.r[fi_l]);
//              n++;
//            }
//          }

            if(!new_bad[i] && !ref.bad(i+di))
            {
              e += fabs(new_r[i]-ref.r(i+di));
              n++;
            }


        }//for i

        if(n>0)
          err[k]  = e/n;//don't forget to correct with n!
        else
          err[k]  = 10000;//don't forget to correct with n!
        beta[k] = di;
        k++;
      }//for dfi

      #ifdef GR
        FILE *fo;
        fo = fopen("angles.txt","w");
        for(i=0;i<k;i++)
        {
         // cout <<beta[i]<<"\t\t"<<err[i]<<endl;
          dr_circle(beta[i],err[i],1.0,"blue");
          fprintf(fo,"%f %f\n",beta[i],err[i]);
        }
        fclose(fo);
//        if(iter>=37)
//        for(i=0;i<k;i++)
//           dr_circle(beta[i],err[i],1.0,"black");
      #endif

      //now search for the global minimum
      //later I can make it more robust
      //assumption: monomodal error function!
      PM_TYPE emin = 1000000;
      int   imin;
      for(i=0;i<k;i++)
        if(err[i]<emin)
        {
          emin = err[i];
          imin = i;
        }
      if(err[imin]>=10000)
      {
        cerr <<"Polar Match: orientation search failed" <<err[imin]<<endl;
        #ifdef  PM_GENERATE_RESULTS
          fclose(f);
        #endif
        throw 1;
      }
      dth = beta[imin];

      //interpolation
      if(imin>=1 && imin<(k-1)) //is it not on the extreme?
      {//lets try interpolation
        PM_TYPE D = err[imin-1]+err[imin+1]-2.0*err[imin];
        PM_TYPE d=1000;
        if(fabs(D)>0.01 && err[imin-1]>err[imin] && err[imin+1]>err[imin])
          d=(err[imin-1]-err[imin+1])/D/2.0;
//        cout <<"ORIENTATION REFINEMENT "<<d<<endl;
        if(fabs(d)<1)
          dth+=d;
      }//if


      ath += dth*PM_D2R;

      #ifdef GR
        cout <<"angle correction "<<dth<<endl;
        //usleep(10000);
        dr_zoom();
      #endif
//      cout  <<" et: orientation " <<(double)(end_tick-start_tick)/896363000.0<<endl;
      continue;
    }
    
    //------------------------------------------translation-------------
    if(iter>10)
       C = 100;

    // do the weighted linear regression on the linearized ...
    // include angle as well
    PM_TYPE dr;

    //computation of the new dx1,dy1,dtheta1
    PM_TYPE dx1,dy1,dtheta1,X=0,Y=0,Xp=0,Yp=0,W=0,w;
    PM_TYPE sxx=0,sxy=0,syx=0, syy=0;
    PM_TYPE meanpx,meanpy,meanppx,meanppy;
    PM_TYPE apx[PM_L_POINTS], apy[PM_L_POINTS],ppx[PM_L_POINTS], ppy[PM_L_POINTS];    
    meanpx = 0;meanpy = 0;
    meanppx= 0;meanppy= 0;

    abs_err=0;
    n=0;
    for(i=0;i<PM_L_POINTS;i++)
    {
      dr = ref.r(i)-new_r[i];
      abs_err += fabs(dr);
      //weight calculation
      if(ref.bad(i)==0 && new_bad[i]==0 && new_r[i]<PM_MAX_RANGE && new_r[i]>10.0 && fabs(dr)<PM_MAX_ERROR)
      {
        // do the cartesian calculations....

//          apx[n] = new_r[i]*pm_co[i];
//          apy[n] = new_r[i]*pm_si[i];
//          ppx[n] = ref.r[i]*pm_co[i]; //could speed up this..
//          ppy[n] = ref.r[i]*pm_si[i];
          PM_TYPE x,y,xp,yp;  
          x = new_r[i]*pm_co[i];//actual
          y = new_r[i]*pm_si[i];
          xp = ref.r(i)*pm_co[i]; //could speed up this..
          yp = ref.r(i)*pm_si[i];//reference

          w = C/(dr*dr+C);

          W += w;
          X += w*x;
          Y += w*y;
          Xp+= w*xp;
          Yp+= w*yp;

          sxx += w*xp*x;
          sxy += w*xp*y;
          syx += w*yp*x;
          syy += w*yp*y;

//          #ifdef GR
//            dr_line(apx[n],apy[n],ppx[n],ppy[n],"black");
//          #endif
//
//
//          meanpx +=  apx[n];
//          meanpy +=  apy[n];
//
//          meanppx +=  ppx[n];
//          meanppy +=  ppy[n];

          n++;          
      }
    }//for
    if(n<PM_MIN_VALID_POINTS || W<0.01) //are there enough points?
    {
      cerr <<"pm_linearized_match: ERROR not enough points "<<n<<" "<<W<<endl;
      #ifdef  PM_GENERATE_RESULTS
          fclose(f);
      #endif      
      throw 1;//not enough points
    }

    dth = atan2(-(X*Yp - Y*Xp + W*(sxy-syx)),-(X*Xp+Y*Yp-W*(sxx+syy)));
    dx  = (Xp-X*cos(dth)+Y*sin(dth))/W;
    dy  = (Yp-X*sin(dth)-Y*cos(dth))/W;
      
//    meanpx /= n;
//    meanpy /= n;
//
//    meanppx /= n;
//    meanppy /= n;

//    for(int i=0;i<n;i++)
//    {
//        sxx += (apx[i] - meanpx)*(ppx[i] - meanppx);
//        sxy += (apx[i] - meanpx)*(ppy[i] - meanppy);
//        syx += (apy[i] - meanpy)*(ppx[i] - meanppx);
//        syy += (apy[i] - meanpy)*(ppy[i] - meanppy);
//    }
      //computation of the resulting translation and rotation
      //for method closest point match
//    dth = atan2(sxy-syx,sxx+syy);
//    dx  = meanppx - ax - (cos(dth)*(meanpx- ax) - sin(dth)*(meanpy - ay));
//    dy  = meanppy - ay - (sin(dth)*(meanpx- ax) + cos(dth)*(meanpy - ay));


    ax += dx;
    ay += dy;
    ath+= dth;

//    //for SIMULATION iteration results..
//    cout <<iter<<"     "<<ax<<"    "<<ay<<"    "<<ath*PM_R2D<<" ;"<<endl;
    #ifdef GR
      cout <<"iter "<<iter<<" "<<ax<<" "<<ay<<" "<<ath*PM_R2D<<" "<<dx<<" "<<dy<<endl;
//      if(iter==0)
//      if(iter==0)
        dr_zoom();
//    usleep(10000);
    #endif
    dth *=PM_R2D;
  }//for iter
//  dr_zoom();
  #ifdef  PM_GENERATE_RESULTS
     end_tick =rdtsc();
     fprintf(f,"%i %lf %lf %lf %lf\n",iter,
        (double)(end_tick-start_tick-dead_tick)*1000.0/CPU_FREQ,ax,ay,ath*PM_R2D);
     fclose(f);
  #endif

  lsa->setRx(ax);lsa->setRy(ay);lsa->setTh(ath);
  return(abs_err/n);
}//pm_cartesian_match
