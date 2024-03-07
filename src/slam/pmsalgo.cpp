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
#include <sstream>
#include "pmsalgo.h"
#include "../neuraldis/ndmath.h"
// #include "liblbfgs/lbfgs.h"
#include <lbfgs.h>
#include <math.h>

// #define COUT_DEBUG

using namespace std;

PMSAlgo::PMSAlgo()
    : ScanMatchingAlgo()
{

    fileoutput = NULL;
    fileoutput_evals = NULL;

    return_result = 0;
    tt_start = TimeStamp::nowMicro();
    tt_end = TimeStamp::nowMicro();

    log_level = PMS_LOG_OFF;
    pms_method = PMS_DEFAULT;
    nlopt_method = NLOPT_GN_DIRECT_L;
    nlopt_refinement = false;

    errorMax = 1e10;

    //  PM_FI_MIN = M_PI/2.0 - PM_FOV*PM_D2R/2.0;//[rad] bearing from which laser scans start
    //  PM_FI_MAX = M_PI/2.0 + PM_FOV*PM_D2R/2.0;//[rad] bearing at which laser scans end
    PM_FI_MIN = -PM_FOV*PM_D2R/2.0;//[rad] bearing from which laser scans start
    PM_FI_MAX = PM_FOV*PM_D2R/2.0;//[rad] bearing at which laser scans end

    PM_DFI    = PM_FOV*PM_D2R/ ( PM_L_POINTS -1.0 );//[rad] angular resolution of laser scans

}

PMSAlgo::~PMSAlgo()
{

    if (fileoutput != NULL)
        fclose(fileoutput);

    if (fileoutput_evals != NULL)
        fclose(fileoutput_evals);
}

void PMSAlgo::setPMSVariables(const int method, const nlopt_algorithm nlopt_selection, const bool nlopt_grad_refinement_flag, const nlopt_algorithm nlopt_flag_grad_refinement, const int log_level_input)
{

    // Available methods:
    // 0 - PMS_DEFAULT 	- Default
    // 1 - PMS_INS		- INS proposal
    // 2 - PMS_INS2		- INS proposal #2
    // 3 - PMS_DIOSI	- Diosi
    // 4 - PMS_BRUTE_FORCE	- Brute force
    // 5 - PMS_TOTAL_SEARCH	- Total search
    // 6 - PMS_BFGS 	- BFGS original implementation
    // 7 - PMS_NLOPT	- nlopt library

    // Available methods:
    // 0 -	PMS_LOG_OFF	 	- No logs at all
    // 1 - 	PMS_LOG_NORMAL		- Normal level
    // 2 - 	PMS_LOG_DETAILED	- Detailed XYTH logs
    // 3 - 	PMS_LOG_RESULTS 	- Only results

    pms_method = method;
    nlopt_method = nlopt_selection;
    nlopt_refinement = nlopt_grad_refinement_flag;
    nlopt_refinement_method = nlopt_flag_grad_refinement;
    log_level = log_level_input;

    if (log_level != PMS_LOG_OFF)
    {
        initLogPsmalgo();
        if (log_level != PMS_LOG_RESULTS)
            initLogEvals();
    }

#ifdef COUT_DEBUG
    cout << "PMSAlgo::setPMSVariables(): pms_method: " << pms_method << " nlopt_method: " <<  nlopt_method << " nlopt_refinement: " << nlopt_refinement << " nlopt_refinement_method: " << nlopt_refinement_method << " log_level: " << log_level << endl;
#endif

}

////////////////////////////////////////////////
/// NEW MATCH
////////////////////////////////////////////////

PM_TYPE PMSAlgo::match(PMScan* lsr, PMScan* lsa, int pms_method){
    PM_TYPE   abs_err = 0;
    return(abs_err);
}

PM_TYPE PMSAlgo::match(PMScan* lsr, PMScan* lsa, PMScan* raw_lsr, PMScan* raw_lsa, int pms_method)
{
    PM_TYPE   ax, ay, ath;//robot pos at ref and actual scans
    PM_TYPE   ax_init, ay_init, ath_init;//init robot pos at ref and actual scans
    PM_TYPE   abs_err;

    tt_start = TimeStamp::nowMicro();
    no_evaluations = 0;
    return_result = 0;

    act = *lsa;
    ref = *lsr;
    this->pms_method = pms_method;

    calculateDisplacementsRefFrame(&ax, &ay, &ath);
    //   logHeader(lsr,lsa,raw_lsr,raw_lsa,ax,ay,ath);

    ax_init = ax;
    ay_init = ay;
    ath_init = ath;

    //  pms_method = PMS_ISRAEL;

    switch(pms_method)
    {
    case PMS_INS:
        matchUpdated(&ax, &ay, &ath);
        break;
    case PMS_INS2:
        matchMixedApproach(&ax, &ay, &ath);
        break;
    case PMS_DIOSI:
        matchNewDiosi(&ax, &ay, &ath);
        break;
    case PMS_ISRAEL:
        matchDiosiIsrael(&ax, &ay, &ath);
        break;
    case PMS_KALMAN:
        matchKalman(&ax, &ay, &ath);
        break;
    case PMS_BRUTE_FORCE:
        matchBruteForce(&ax, &ay, &ath);
        break;
    case PMS_TOTAL_SEARCH:
        matchTotalSearch(&ax, &ay, &ath);
        break;
    case PMS_BFGS:
        matchBFGS(&ax, &ay, &ath);
        break;
    case PMS_NLOPT:
        matchNlopt(&ax,&ay,&ath);
        break;
    default:
        matchUpdated(&ax, &ay, &ath);
    }

    abs_err = getErrorOverN(ax, ay, ath);

    if(matchOk) {
        lsa->setRx(ax);
        lsa->setRy(ay);
        lsa->setTh(ath);
    }
    else {
        lsa->setRx(ax_init);
        lsa->setRy(ay_init);
        lsa->setTh(ath_init);
    }

    tt_end = TimeStamp::nowMicro();
    logResult(tt_start, tt_end, abs_err);

    return(abs_err);
}

// matchUpdated is based on Diosi's approach but includes a number of significant improvements

void PMSAlgo::matchUpdated(PM_TYPE* ax, PM_TYPE* ay, PM_TYPE *ath) // match & matchDiosi
{
    int       iter, small_corr_cnt = 0;
    PM_TYPE   dx = 10.0, dy = 10.0, dth = 0.0,lb,ub;//match error, actual scan corrections
    PM_TYPE   initial_abs_err,abs_err;

    iter = -1;
    while (++iter < PM_MAX_ITER && small_corr_cnt < 5) { //has to be 5 small corrections before stop

        if (((fabs(dx/10.0)+fabs(dy/10.0)) < 1.0) && (fabs(dth)< 0.25))
            small_corr_cnt++;
        else
            small_corr_cnt = 0;

        act.setRx(*ax);
        act.setRy(*ay);
        act.setTh(*ath);

        //---------------ORIENTATION SEARCH-----------------------------------

        if (iter % 2 == 0) {

            dx = 0.0;
            dy = 0.0;
            dth = 0.0;

            // While dth = -10 or +10, repeat procedure
            int dth_iters = 0;
            PM_TYPE current_dth;
            PM_TYPE dth1 = 0.0;

            while (dth_iters < 36){

#ifdef COUT_DEBUG
                cout << "PMSAlgo::matchUpdated() before while cut dth (degrees) = " << dth << endl;
#endif

                current_dth = bruteforceOrientationSearch(*ax,*ay,*ath+dth1*M_PI/180.0,-10.0,10.0,0.5,false);
                dth1 += current_dth;
                if (current_dth == 10.0 || current_dth == -10.0)
                {
                    ++dth_iters;
                } else {
                    dth_iters = 40;
                }

#ifdef COUT_DEBUG
                cout << "PMSAlgo::matchUpdated() after while dth (degrees) = " << dth << endl;
#endif

            }

#ifdef COUT_DEBUG
            cout << "PMSAlgo::matchUpdated() first cut dth (degrees) = " << dth << endl;
#endif

            dth = bruteforceOrientationSearch(*ax,*ay,*ath,dth1-0.5,dth1+0.5,0.01,false);

#ifdef COUT_DEBUG
            cout << "PMSAlgo::matchUpdated() final dth (degrees) = " << dth << endl;
#endif

            (*ath) = (*ath) + dth * M_PI / 180.0;

            if (dth1 > 10.0)
            {
                lb = -10.0;
                ub = 10.0 + dth1;
            } else if (dth1 < -10.0) {
                lb = -10.0 + dth1;
                ub = 10.0;
            } else {
                lb = -10.0;
                ub = 10.0;
            }

            logThIteration(*ax,*ay,*ath,dx,dy,dth*M_PI/180.0,lb,ub);

#ifdef COUT_DEBUG
            cout << "PMSAlgo::matchUpdated() ath (degress) and final dth (degrees) AFTER " << (*ath)*180.0/M_PI << " " << dth << endl;
#endif

            continue;

        }

        dx = 0.0;
        dy = 0.0;
        dth = 0.0;

        initial_abs_err = getErrorOverN(*ax, *ay, *ath);
        translationsearch(*ax, *ay, *ath, &dx, &dy);
        abs_err = getErrorOverN(*ax+dx, *ay+dy, *ath);//   if(phi_0 < -M_PI)
        //     phi_0 += M_PI;
        //   else if(phi_0 > M_PI)
        //     phi_0 = M_PI - phi_0;
        //  Use dx, dy only if error is smaller
        if (abs_err < initial_abs_err)
        {
            *ax += dx;
            *ay += dy;
        } else {
            dx = 0.0;
            dy = 0.0;
        }

        logXYIteration(*ax,*ay,*ath,dx,dy,dth*M_PI/180.0);

#ifdef COUT_DEBUG
        cout << "PMSAlgo::matchUpdated() dx dy ax ay = " << dx << " " << dy << " " << *ax << " " << *ay << endl;
#endif

    }

    if(iter >= 30)
        cerr << "Max. num. iter" << endl;

    return;
}

// matchMixedApproach

void PMSAlgo::matchMixedApproach(PM_TYPE* ax, PM_TYPE* ay, PM_TYPE *ath) // match & matchDiosi
{
    int       iter, small_corr_cnt = 0;
    PM_TYPE   dx = 10.0, dy = 10.0, dth = 0.0,lb,ub;//match error, actual scan corrections
    PM_TYPE   initial_abs_err,abs_err;

    iter = -1;
    while (++iter < PM_MAX_ITER && small_corr_cnt < 5) { //has to be 5 small corrections before stop

        if (((fabs(dx/10.0)+fabs(dy/10.0)) < 1.0) && (fabs(dth)< 0.25))
            small_corr_cnt++;
        else
            small_corr_cnt = 0;

        act.setRx(*ax);
        act.setRy(*ay);
        act.setTh(*ath);

        //---------------ORIENTATION SEARCH-----------------------------------

        if (iter % 2 == 0) {

            dx = 0.0;
            dy = 0.0;
            dth = 0.0;

            // While dth = -10 or +10, repeat procedure
            int dth_iters = 0;
            PM_TYPE current_dth;
            PM_TYPE dth1 = 0.0;

            while (dth_iters < 36){

#ifdef COUT_DEBUG
                cout << "PMSAlgo::matchMixedApproach() before while cut dth (degrees) = " << dth << endl;
#endif

                current_dth = bruteforceOrientationSearch(*ax,*ay,*ath+dth1*M_PI/180.0,-10.0,10.0,0.5,false);
                dth1 += current_dth;
                if (current_dth == 10.0 || current_dth == -10.0)
                {
                    ++dth_iters;
                } else {
                    dth_iters = 40;
                }

#ifdef COUT_DEBUG
                cout << "PMSAlgo::matchMixedApproach() after while dth (degrees) = " << dth << endl;
#endif

            }

#ifdef COUT_DEBUG
            cout << "PMSAlgo::matchMixedApproach() first cut dth (degrees) = " << dth << endl;
#endif

            dth = bruteforceOrientationSearch(*ax,*ay,*ath,dth1-0.5,dth1+0.5,0.01,false);

#ifdef COUT_DEBUG
            cout << "PMSAlgo::matchMixedApproach() final dth (degrees) = " << dth << endl;
#endif

            (*ath) = (*ath) + dth * M_PI / 180.0;

            if (dth1 > 10.0)
            {
                lb = -10.0;
                ub = 10.0 + dth1;
            } else if (dth1 < -10.0) {
                lb = -10.0 + dth1;
                ub = 10.0;
            } else {
                lb = -10.0;
                ub = 10.0;
            }

            logThIteration(*ax,*ay,*ath,dx,dy,dth*M_PI/180.0,lb,ub);

#ifdef COUT_DEBUG
            cout << "PMSAlgo::matchMixedApproach() ath (degress) and final dth (degrees) AFTER " << (*ath)*180.0/M_PI << " " << dth << endl;
#endif

            continue;

        }

        dx = 0.0;
        dy = 0.0;
        dth = 0.0;

        initial_abs_err = getErrorOverN(*ax, *ay, *ath);
        //    translationsearch(*ax, *ay, *ath, &dx, &dy);
        nloptBasedSearch_XYonly(*ax, *ay, &dx, &dy);
        abs_err = getErrorOverN(*ax+dx, *ay+dy, *ath);
        //  Use dx, dy only if error is smaller
        if (abs_err < initial_abs_err)
        {
            *ax += dx;
            *ay += dy;
        } else {
            dx = 0.0;
            dy = 0.0;
        }

        logXYIteration(*ax,*ay,*ath,dx,dy,dth*M_PI/180.0);

#ifdef COUT_DEBUG
        cout << "PMSAlgo::matchMixedApproach() dx dy ax ay = " << dx << " " << dy << " " << *ax << " " << *ay << endl;
#endif

    }

    if(iter >= 30)
        cerr << "Max. num. iter" << endl;

    return;
}

void PMSAlgo::matchBruteForce(PM_TYPE* ax, PM_TYPE* ay, PM_TYPE *ath) // match & matchDiosi
{
    int       iter, small_corr_cnt = 0;
    PM_TYPE   dx = 10.0, dy = 10.0, dth = 0.0;//match error, actual scan corrections
    PM_TYPE   ax_best,ay_best,lb,ub;

    iter = -1;
    while (++iter < PM_MAX_ITER && small_corr_cnt < 5) { //has to be 5 small corrections before stop

        if (((fabs(dx/10.0)+fabs(dy/10.0)) < 1.0) && (fabs(dth)< 0.25))
            small_corr_cnt++;
        else
            small_corr_cnt = 0;

        act.setRx(*ax);
        act.setRy(*ay);
        act.setTh(*ath);

        //---------------ORIENTATION SEARCH-----------------------------------

        if (iter % 2 == 0) {

            dx = 0.0;
            dy = 0.0;
            dth = 0.0;

            // While dth = -10 or +10, repeat procedure
            int dth_iters = 0;
            PM_TYPE current_dth;
            PM_TYPE dth1 = 0.0;

            while (dth_iters < 36){
                current_dth = bruteforceOrientationSearch(*ax,*ay,*ath+dth1*M_PI/180.0,-10.0,10.0,0.5,false);
                dth1 += current_dth;
                if (current_dth == 10.0 || current_dth == -10.0)
                {
                    ++dth_iters;
                } else {
                    dth_iters = 40;
                }
            }

#ifdef COUT_DEBUG
            cout << "PMSAlgo::matchBruteForce() first cut dth1 (degrees) = " << dth1 << endl;
#endif

            dth = bruteforceOrientationSearch(*ax,*ay,*ath,dth1-0.5,dth1+0.5,0.01,false);

#ifdef COUT_DEBUG
            cout << "PMSAlgo::matchBruteForce() final dth (degrees) = " << dth << endl;
#endif

            (*ath) = (*ath) + dth * M_PI / 180.0;

#ifdef COUT_DEBUG
            cout << "PMSAlgo::matchBruteForce() ath (degress) and final dth (degrees) AFTER = " << (*ath)*180.0/M_PI << " " << dth << endl;
#endif

            if (dth1 > 10.0)
            {
                lb = -10.0;
                ub = 10.0 + dth1;
            } else if (dth1 < -10.0) {
                lb = -10.0 + dth1;
                ub = 10.0;
            } else {
                lb = -10.0;
                ub = 10.0;
            }

            logThIteration(*ax,*ay,*ath,dx,dy,dth*M_PI/180.0,lb,ub);

            continue;

        }

        dx = 0.0;
        dy = 0.0;
        dth = 0.0;

        bruteforceTranslationSearch(*ax, *ay, *ath, &ax_best, &ay_best,false);

        dx = ax_best - *ax;
        dy = ay_best - *ay;
        *ax = ax_best;
        *ay = ay_best;

#ifdef COUT_DEBUG
        cout << "PMSAlgo::matchBruteForce() dx dy ax ay = " << dx << " " << dy << " " << *ax << " " << *ay << endl;
#endif

        logXYIteration(*ax,*ay,*ath,dx,dy,dth*M_PI/180.0);

    }

    if(iter >= 30)
        cerr << "Max. num. iter" << endl;

}

void PMSAlgo::matchTotalSearch(PM_TYPE* ax, PM_TYPE* ay, PM_TYPE *ath)
{
    int       iter, small_corr_cnt = 0;
    PM_TYPE   dx = 10.0, dy = 10.0, dth = 0.0;
    PM_TYPE   initial_abs_err,abs_err;

    iter = -1;
    while (++iter < PM_MAX_ITER && small_corr_cnt < 5) { //has to be 5 small corrections before stop

        if (((fabs(dx/5.0)+fabs(dy/5.0)) < 1.0) && (fabs(dth)< 0.02))
            small_corr_cnt++;
        else
            small_corr_cnt = 0;

        act.setRx(*ax);
        act.setRy(*ay);
        act.setTh(*ath);

        initial_abs_err = getErrorOverN(*ax, *ay, *ath);
        totalsearch(*ax, *ay, *ath, &dx, &dy, &dth);
        abs_err = getErrorOverN(*ax+dx, *ay+dy, *ath+dth);
        //  Use dx, dy only if error is smaller
        if (abs_err < initial_abs_err)
        {
            *ax += dx;
            *ay += dy;
            *ath += dth;
        } else {
            dx = 0.0;
            dy = 0.0;
            dth = 0.0;
        }
        logIteration(*ax, *ay, *ath, dx, dy, dth);

#ifdef COUT_DEBUG
        cout << "PMSAlgo::matchTotalSearch() - totalsearch - dx dy dth ax ay ath = " << dx << " " << dy << " " << dth*180.0/M_PI << " " << *ax << " " << *ay << " " << *ath << endl;
#endif
    }

    if(iter >= 30)
        cerr << "Max. num. iter" << endl;

    return;
}

void PMSAlgo::matchBFGS(PM_TYPE* ax, PM_TYPE* ay, PM_TYPE *ath)
{
    PM_TYPE dx,dy,dth;
    PM_TYPE   initial_abs_err,abs_err;

    logIteration(*ax, *ay, *ath, dx, dy, dth);
    initial_abs_err = getErrorOverN(*ax, *ay, *ath);
    gradientBasedSearch(*ax, *ay, *ath, &dx, &dy, &dth);
    abs_err = getErrorOverN(*ax+dx, *ay+dy, *ath+dth);
    //  Use dx, dy only if error is smaller
    if (abs_err < initial_abs_err)
    {
        *ax += dx;
        *ay += dy;
        *ath += dth;
    } else {
        dx = 0.0;
        dy = 0.0;
        dth = 0.0;
    }
    logIteration(*ax, *ay, *ath, dx, dy, dth);

#ifdef COUT_DEBUG
    cout << "PMSAlgo::matchBFGS() dx dy dth ax ay ath = " << dx << " " << dy << " " << dth*180.0/M_PI << " " << ax << " " << ay << " " << ath << endl;
#endif

    return;
}

void PMSAlgo::matchNlopt(PM_TYPE* ax, PM_TYPE* ay, PM_TYPE *ath)
{
    PM_TYPE dx,dy,dth;

    logIteration(*ax, *ay, *ath, dx, dy, dth);
    nloptBasedSearch(*ax, *ay, *ath, &dx, &dy, &dth);
    logIteration(*ax, *ay, *ath, dx, dy, dth);
    *ax += dx;
    *ay += dy;
    *ath += dth;
#ifdef COUT_DEBUG
    cout << "PMSAlgo::matchNlopt() dx dy dth ax ay ath = " << dx << " " << dy << " " << dth*180.0/M_PI << " " << *ax << " " << *ay << " " << *ath << endl;
#endif

    return;

}

// ********************************************************************
//------------------------INTERPOLATION------------------------
// Calculate/interpolate the associations to the ref scan points algorithm ignores crosings at 0 and 180 -> to make it faster
// ********************************************************************


void PMSAlgo::interpolation(const PM_TYPE x_c, const PM_TYPE y_c, const PM_TYPE theta_c, PM_TYPE* fi, PM_TYPE* r, PM_TYPE* new_r, int* new_bad, int* index){

    PM_TYPE   x, y, delta;
    PM_TYPE   new_fi[PM_L_POINTS];
    int i;

    // Convert range readings into ref frame
    int kk = 0;

    for (i = 0;i < PM_L_POINTS;i++) {
        delta   = theta_c + pm_fi[i];

        x       = act.r(i) * cos(delta) + x_c;
        y       = act.r(i) * sin(delta) + y_c;
        r[i]    = sqrt(x * x + y * y);
        fi[i]   = atan2(y, x);
        new_r[i]  = 0;//initialize big interpolated r;  a mm.
        new_bad[i] = PM_EMPTY;//for interpolated r;
        index[i] = -1;
    }

    for (i = 0;i < PM_L_POINTS;i++) {
        new_fi[i] = fi[i] + M_PI/2.0;
    }

    for (i = 1;i < PM_L_POINTS;i++) { //i points to the angles in the actual scan
        // i and i-1 has to be in the same segment, both shouldn't be bad and they should be bigger than 0
        if (act.seg(i) != 0 && act.seg(i) == act.seg(i - 1)) {
            //calculation of the "whole" parts of the angles
            int     fi0, fi1;
            PM_TYPE r0, r1, a0, a1;

            if (new_fi[i] > new_fi[i-1])//are the points visible?
            {
                a0  = new_fi[i-1];
                a1  = new_fi[i];
                fi0 = (int) ceil(new_fi[i-1] * PM_R2D);//fi0 is the meas. angle!
                fi1 = (int)(new_fi[i] * PM_R2D);
                r0  = r[i-1];
                r1  = r[i];
            } else {
                continue;
            }

            //interpolate for all the measurement bearings beween fi0 and fi1
            while (fi0 <= fi1) { //if at least one measurement point difference, then ...
                PM_TYPE ri = (r1 - r0) / (a1 - a0) * (((PM_TYPE)fi0 * PM_D2R) - a0) + r0;
                //if fi0 -> falls into the measurement range and ri is shorter
                //than the current range then overwrite it
                if (fi0 >= 0 && fi0 < PM_L_POINTS) {
                    if (new_bad[fi0] == PM_EMPTY || (new_bad[fi0] == 0 && new_r[fi0] > ri)){
                        new_r[fi0]    = ri;//overwrite the previous reading
                        index[fi0]    = i;//store which reading was used at index fi0
                        new_bad[fi0] = 0;//clear the empty flag
                    }
                }
                fi0++;//check the next measurement angle!
            }//while
        }//if act
    }//for i

    for (i = 0;i < PM_L_POINTS;i++) {
        if(new_bad[i] == 0)
        {
            bool visible = visibleFromNewPosition(ref.r(i),pm_fi[i],x_c,y_c,theta_c);
            //       if (visible == visibleFromNewPosition(ref.r(i),pm_fi[i],x_c,y_c,theta_c);
            if (visible == false)
                new_bad[i] = PM_REF_NOT_VISIBLE;
        }
    }

    return;
}

void PMSAlgo::projectSegments(const PM_TYPE x_c, const PM_TYPE y_c, const PM_TYPE theta_c, /*PM_TYPE* fi,*/ PM_TYPE* r)
{
    int numSegments = act.getSegmentsEnds().size();
    PM_TYPE   x, y, delta;
    PM_TYPE fi[PM_L_POINTS];
    double modTrans = sqrt(x_c*x_c + y_c*y_c);
    double alfaTrans = atan2(y_c, -x_c);
    //   if(alfaTrans >= -M_PI/2 && alfaTrans <= M_PI/2)
    //     modTrans = -modTrans;
    //   else
    //     int foo = 0;

    QVector<double> prevRhoList;
    QVector<double> altRhoList;
    QVector<double> phiList;
    //   QVector<bool> comparados;
    for(int i = 0; i < numSegments; i++) {
        //     if(act.getSegmentsEnds().at(i).first == act.getSegmentsEnds().at(i).second)
        //       continue;

        double rho0 = act.getRhoEst().at(i);
        double phi0 = act.getPhiEst().at(i) * PM_R2D;
        double giro = (act.getPhiEst().at(i) - alfaTrans) * PM_R2D;
        double newRho0 = fabs(act.getRhoEst().at(i) - modTrans*cos(act.getPhiEst().at(i) - alfaTrans));
        double newRho01 = fabs(act.getRhoEst().at(i) - modTrans*cos(act.getPhiEst().at(i) + alfaTrans));
        double newRho02 = fabs(act.getRhoEst().at(i) + modTrans*cos(act.getPhiEst().at(i) - alfaTrans));
        double newRho03 = fabs(act.getRhoEst().at(i) + modTrans*cos(act.getPhiEst().at(i) + alfaTrans));

        double newPhi0 = (act.getPhiEst().at(i) + theta_c) * PM_R2D;
        prevRhoList.insert(i, newRho0);
        altRhoList.insert(i, newRho02);
        phiList.insert(i, newPhi0 / PM_R2D);
    }

    for (int i = 0; i < PM_L_POINTS; i++) {
        comparados[i] = false;
    }

    int cnt_comp = 0;
    int prev_seg_ref = 0;
    //   int prev_seg_act = 0;
    //   int prev_act_i = 0;
    for (int i = 0; i < PM_L_POINTS; i++) {
        comparable[i] = false;
        double act_r = act.r(i) >= 0 ? act.r(i) : sensorMaxVal;
        delta   = theta_c + pm_fi[i];
        x       = act_r * cos(delta) + x_c;
        y       = act_r * sin(delta) + y_c;
        r[i]    = sqrt(x * x + y * y);
        fi[i]   = atan2(y, x) + M_PI/2.0;
        int fi0 = (int) ceil(fi[i] * PM_R2D);
        //     int fi0 = (int) (fi[i] * PM_R2D);
        //     int fi0 = round(fi[i] * PM_R2D);

        if(fi0 == 49)
            int foo = 0;
        if(fi0 < 0 || fi0 > PM_L_POINTS - 1) {
            comparable[i] = true;
            ++cnt_comp;
            continue;
        }

        int seg_ref = ref.seg(fi0);
        int seg_act = act.seg(i);

        if(seg_ref && seg_act) {
            double rhoRef = ref.getRhoEst().at(seg_ref - 1);
            double phiRef = ref.getPhiEst().at(seg_ref - 1);
            double rhoAct = prevRhoList.at(seg_act - 1);
            double rhoAltAct = altRhoList.at(seg_act - 1);
            double phiAct = phiList.at(seg_act - 1);

            /*        if(fabs(segment_1->rho_0_est - segment_2->rho_0_est) < 10 && (fabs(segment_1->phi_0_est - segment_2->phi_0_est) < M_PI/180 || fabs(M_PI - fabs(segment_1->phi_0_est - segment_2->phi_0_est)) < M_PI/180) && new_segment->getSigmaEst() < KalmanSegment::MAXSIGMA/2)*/
            if(fi0 == 49)
                int foo = 0;

            if((fabs(rhoRef -rhoAct) < 200 || fabs(rhoRef -rhoAltAct) < 200) && (fabs(phiRef - phiAct) < 5*M_PI/180 || fabs(M_PI - fabs(phiRef - phiAct)) < 5*M_PI/180)) {
                comparable[i] = true;
                comparados[fi0] = true;
                // 	comparables.append(QPair(seg_ref, seg_act));
                ++cnt_comp;
            }
            else  {
                int foo = 0;
                comparados[fi0] = true;
                // 	comparables.appendQPair(seg_ref, seg_act);
            }
        }
    }
    //   int fi0 = 0;
    //   for (int i = 1; i < PM_L_POINTS; i++) {
    //     fi0 = round(fi[i] * PM_R2D);
    //     if(fi0 > 0 && fi0 < PM_L_POINTS) {
    //       if(act.seg(i) == act.seg(i-1) && act.seg(i) && act.seg(i-1) && ref.seg(fi0) == ref.seg(fi0-1) && ref.seg(fi0) && ref.seg(fi0-1)) {
    // 	if(!comparable[i-1])
    // 	  comparable[i] = false;
    //       }
    //     }
    //   }
    //   fi0 = round(fi[0] * PM_R2D);
    //   if(fi0 >= 0 && fi0 < PM_L_POINTS-1) {
    //     if(act.seg(0) == act.seg(1) && act.seg(0) && act.seg(1) &&  ref.seg(fi0) == ref.seg(fi0+1) && ref.seg(fi0) && ref.seg(fi0+1)) {
    //       if(!comparable[1])
    // 	comparable[0] = false;
    //     }
    //   }

}

void PMSAlgo::projectSegments(const PM_TYPE x_c, const PM_TYPE y_c,
                              const PM_TYPE theta_c, PM_TYPE* fi, PM_TYPE* r, PM_TYPE* new_ref,
                              int* new_bad, const PM_TYPE delta_th, int* next_segment)
{
    int numSegments = act.getSegmentsEnds().size();
    PM_TYPE   x, y, delta;
    //   PM_TYPE fi[PM_L_POINTS];
    double modTrans = sqrt(x_c*x_c + y_c*y_c);
    double alfaTrans = atan2(y_c, -x_c);
    double phiAccuracy = 5*M_PI/180;
    double rhoAccuracy = 200;

    QVector<double> prevRhoList(numSegments);
    QVector<double> altRhoList(numSegments);
    phiList.resize(numSegments);
    rhoList.fill(-1, numSegments);
    //   double rho0, phi0, newRho0, newRho01,
    //   QVector<bool> comparados;
    for(int i = 0; i < numSegments; i++) {
        //     if(act.getSegmentsEnds().at(i).first == act.getSegmentsEnds().at(i).second)
        //       continue;

        double rho0 = act.getRhoEst().at(i);
        double phi0 = act.getPhiEst().at(i) * PM_R2D;
        double giro = (act.getPhiEst().at(i) - alfaTrans) * PM_R2D;
        double newRho0 = fabs(act.getRhoEst().at(i) - modTrans*cos(act.getPhiEst().at(i) - alfaTrans));
        double newRho01 = fabs(act.getRhoEst().at(i) - modTrans*cos(act.getPhiEst().at(i) + alfaTrans));
        double newRho02 = fabs(act.getRhoEst().at(i) + modTrans*cos(act.getPhiEst().at(i) - alfaTrans));
        double newRho03 = fabs(act.getRhoEst().at(i) + modTrans*cos(act.getPhiEst().at(i) + alfaTrans));

        double newPhi0 = (act.getPhiEst().at(i) + theta_c) * PM_R2D;
        prevRhoList[i] =  newRho0;
        altRhoList[i] = newRho02;
        phiList[i] = newPhi0 / PM_R2D;
    }

    int cnt_comp = 0;
    //   int cnt_seg = 0;
    for (int i = 0; i < PM_L_POINTS; i++) {

        new_bad[i] = PM_EMPTY;
        new_ref[i] = -1;
        next_segment[i] = 0;
        /*
    if(i > 0) {
      if(act.seg(i) != act.seg(i-1))
    cnt_seg++;
    }*/

        double act_r = act.r(i) >= 0 ? act.r(i) : sensorMaxVal;
        delta   = theta_c + pm_fi[i];
        x       = act_r * cos(delta) + x_c;
        y       = act_r * sin(delta) + y_c;
        r[i]    = sqrt(x * x + y * y);
        fi[i]   = atan2(y, x) + M_PI/2;
        //     int fi0 = (int) floor(fi[i] * PM_R2D);

    }


    double prev_fi = fi[0];
    for (int i = 1;i <= PM_L_POINTS/2;i++) {
        if(fi[i] <= prev_fi)
            new_bad[i] = PM_OCCLUDED;
        else
            prev_fi = fi[i];
    }

    prev_fi = fi[PM_L_POINTS - 1];
    for (int i = PM_L_POINTS - 2; i >= PM_L_POINTS/2; i--) {
        if(fi[i] >= prev_fi)
            new_bad[i] = PM_OCCLUDED;
        else
            prev_fi = fi[i];
    }

    bool endWhile = false;
    int k = 0;
    do {
        for (int i = 0; i < PM_L_POINTS; i++) {
            if(fi[i] < 0 || fi[i] > M_PI || new_bad[i] == PM_OCCLUDED) {
                continue;
            }
            int fi0 = (int) floor(fi[i] * PM_R2D);
            int seg_ref = ref.seg(fi0);

            int seg_act = act.seg(i);

            if(seg_ref && seg_act) {
                double rhoRef = ref.getRhoEst().at(seg_ref - 1);
                double phiRef = ref.getPhiEst().at(seg_ref - 1);
                double rhoAct = prevRhoList.at(seg_act - 1);
                double rhoAltAct = altRhoList.at(seg_act - 1);
                double phiAct = phiList.at(seg_act - 1);

                //       if(rhoList.at(seg_act - 1) < 0) { // No se ha cambiado, actualizamos
                if(fabs(rhoRef - rhoAct) <= fabs(rhoRef - rhoAltAct))
                    rhoList[seg_act - 1] = rhoAct;
                else
                    rhoList[seg_act - 1] = rhoAltAct;
                //       }

                double rl = rhoList.at(seg_act - 1);

                if(fabs(rhoRef - rhoList.at(seg_act - 1)) < rhoAccuracy &&
                        (fabs(phiRef + delta_th - phiAct) < phiAccuracy || fabs(M_PI - fabs(phiRef + delta_th - phiAct)) < phiAccuracy)) {
                    double n_ref = fabs(rhoRef/cos(phiRef - (fi[i] - M_PI/2)));
                    double rr = r[i];
                    if(n_ref < sensorMaxVal && r[i] < sensorMaxVal) {
                        new_ref[i] = n_ref;
                        ++cnt_comp;
                        new_bad[i] = 0;
                    }
                }
                else  {
                    int foo = 0;
                }
            }

            int seg_ref1 = fi0 + 1 < PM_L_POINTS - 1 ? ref.seg(fi0 + 1) : 0;


            if(seg_ref1 && seg_ref1 != seg_ref && seg_act) {
                double rhoRef = ref.getRhoEst().at(seg_ref1 - 1);
                double phiRef = ref.getPhiEst().at(seg_ref1 - 1);
                double rhoAct = prevRhoList.at(seg_act - 1);
                double rhoAltAct = altRhoList.at(seg_act - 1);
                double phiAct = phiList.at(seg_act - 1);

                //       if(rhoList.at(seg_act - 1) < 0) { // No se ha cambiado, actualizamos
                if(fabs(rhoRef - rhoAct) <= fabs(rhoRef - rhoAltAct))
                    rhoList[seg_act - 1] = rhoAct;
                else
                    rhoList[seg_act - 1] = rhoAltAct;
                //       }

                double rr = rhoList.at(seg_act - 1);

                if(fabs(rhoRef - rhoList.at(seg_act - 1)) < rhoAccuracy && (fabs(phiRef + delta_th - phiAct) < phiAccuracy || fabs(M_PI - fabs(phiRef + delta_th - phiAct)) < phiAccuracy)) {

                    double n_ref = fabs(rhoRef/cos(phiRef - (fi[i] - M_PI/2)));
                    double rr = r[i];
                    if(n_ref < sensorMaxVal && r[i] < sensorMaxVal) {
                        if(fabs(n_ref- r[i]) < fabs(new_ref[i]- r[i])) {
                            if(new_ref[i] < 0)
                                ++cnt_comp;
                            new_ref[i] = n_ref;
                            new_bad[i] = 0;
                            next_segment[i] = 1;
                        }
                    }
                }
                else  {
                    int foo = 0;
                }
            }
        }
        if(cnt_comp < PM_MIN_VALID_POINTS) {
            phiAccuracy *= 2;
            rhoAccuracy *= 2;
        }
        else {
            endWhile = true;
        }
        k++;
    } while (!endWhile && k < 2);


}

// void PMSAlgo::interpolation(const PM_TYPE x_c, const PM_TYPE y_c, const PM_TYPE theta_c, PM_TYPE* fi, PM_TYPE* r, PM_TYPE* new_r, PM_TYPE* alt_r, int* new_bad, int* index, QVector< PM_TYPE > ref_r){
//
//   PM_TYPE   x, y, delta;
//   PM_TYPE   new_fi[PM_L_POINTS];
//   int i;
//   int     fi0, fi1;
//   PM_TYPE r0, r1, a0, a1;
//
//   // Convert range readings into ref frame
//   int kk = 0;
//
//   for (i = 0;i < PM_L_POINTS;i++) {
//     double act_r = act.r(i) >= 0 ? act.r(i) : sensorMaxVal;
//
//     delta   = theta_c + pm_fi[i];
//     x       = act_r * cos(delta) + x_c;
//     y       = act_r * sin(delta) + y_c;
//     r[i]    = sqrt(x * x + y * y);
//     fi[i]   = atan2(y, x);
//     new_r[i]  = 0;//initialize big interpolated r;  a mm.
//     alt_r[i] = -1000000;
//     new_bad[i] = PM_EMPTY;//for interpolated r;
//     index[i] = -1;
//   }
//
//   for (i = 0;i < PM_L_POINTS;i++) {
//     new_fi[i] = fi[i] + M_PI/2.0;
//   }
//
//   int segmentEndRef;
//   double prev_fi;
//
//   prev_fi = new_fi[0];
//   new_bad[0] = !comparable[0] ? PM_REF_NOT_VISIBLE : new_bad[0];
//   for (i = 1;i <= PM_L_POINTS/2;i++) {
//     if(new_fi[i] <= prev_fi)
//       new_bad[i] = PM_REF_NOT_VISIBLE;
//     else if(!comparable[i]) {
//       new_bad[i] = PM_REF_NOT_VISIBLE;
//       prev_fi = new_fi[i];
//     }
//     else {
//       prev_fi = new_fi[i];
//     }
//   }
//
//   prev_fi = new_fi[PM_L_POINTS - 1];
//   new_bad[PM_L_POINTS - 1] = !comparable[PM_L_POINTS - 1] ? PM_REF_NOT_VISIBLE : new_bad[PM_L_POINTS - 1];
//   for (int i = PM_L_POINTS - 2; i >= PM_L_POINTS/2; i--) {
//     if(new_fi[i] >= prev_fi)
//       new_bad[i] = PM_REF_NOT_VISIBLE;
//     else if(!comparable[i]) {
//       new_bad[i] = PM_REF_NOT_VISIBLE;
//       prev_fi = new_fi[i];
//     }
//     else {
//       prev_fi = new_fi[i];
//     }
//   }
//
//   for (i = 1;i < PM_L_POINTS;i++) { //i points to the angles in the actual scan
//     // i and i-1 has to be in the same segment, both shouldn't be bad and they should be bigger than 0
//
// /*    if (new_fi[i] > prev_fi)//are the points visible?  */
// //     if (new_bad[i] != PM_REF_NOT_VISIBLE)//are the points visible?
// //     {
// //       a0  = new_fi[i-1];
// //       a1  = new_fi[i];
//       fi0 = (int) floor(new_fi[i] * PM_R2D);//fi0 is the meas. angle!
// //       fi0 = round(new_fi[i-1] * PM_R2D);//fi0 is the meas. angle!
//       fi1 = fi0 + 1;
//
//     if(fi0 = 49)
//       int foo = 0;
//     bool compared = false;
//     int prev_seg_act = 0;
//
//     if (act.seg(i) != 0 &&  ref.seg(i) != 0 && new_bad[i] == PM_EMPTY) {
//
//       //interpolate for all the measurement bearings beween fi0 and fi1
//       while (fi0 <= fi1) { //if at least one measurement point difference, then ...
//         PM_TYPE ri = (r1 - r0) / (a1 - a0) * (((PM_TYPE)fi0 * PM_D2R) - a0) + r0;
//         //if fi0 -> falls into the measurement range and ri is shorter
//         //than the current range then overwrite it
//         if (fi0 >= 0 && fi0 < PM_L_POINTS) {
//
// 	  double rrr = ref.r(fi0);
// 	  if(fi0 == 174)
// 	    int foo = 0;
//           if (new_bad[fi0] == PM_EMPTY || (new_bad[fi0] == 0 && new_r[fi0] > ri) ){
// // 	    if(ri <= ref.r(fi0) + 1) {// + 10
// 	      new_r[fi0]    = ri;//overwrite the previous reading
// 	      if(alt_r[fi0] < 0)
// 		alt_r[fi0]    = new_r[fi0];
// 	      index[fi0]    = i;//store which reading was used at index fi0
// 	      new_bad[fi0] = 0;//clear the empty flag
// // 	    }
// // 	    else
// // 	      new_bad[fi0] = PM_REF_NOT_VISIBLE;
//           }
//         }
//         fi0++;//check the next measurement angle!
//       }//while
//     }//if act
//   }//for i
//
//   for (i = 0;i < PM_L_POINTS;i++) {
//     if(new_bad[i] == 0)
//     {
//       bool visible = visibleFromNewPosition(ref.r(i),pm_fi[i],x_c,y_c,theta_c);
//       if (visible == false)
//         new_bad[i] = PM_REF_NOT_VISIBLE;
//     }
//
// //     cout << new_bad[i] << " " << ref.r(i) << " " << new_r[i] << " " << alt_r[i] << " " << " " << i << " " << new_fi[i]/PM_D2R - 90 << endl;
//   }
// //   cout << endl;
//   for (i = 2;i < PM_L_POINTS;i++) {
//     if(i == PM_L_POINTS - 1) {
//       a0  = (i-2) * PM_D2R;
//       a1  = (i-1) * PM_D2R;
//       r0  = new_r[i-2] > 0 ? new_r[i-2] : alt_r[i-2];
//       r1  = new_r[i-1] > 0 ? new_r[i-1] : alt_r[i-1];
//       double alt_r1 = fabs((r1 - r0) / (a1 - a0) * (((PM_TYPE)(i) * PM_D2R) - a0) + r0);
//
//       if(fabs(new_r[i] - ref_r[i]) > fabs(alt_r1 - ref_r[i]))
// 	alt_r[i] = alt_r1;
//     }
//     else if(((act.seg(i) != 0 && act.seg(i) != act.seg(i + 1)) || ((/*ref.seg(i) != 0 &&*/ ref.seg(i) != ref.seg(i + 1)))) || new_bad[i] != new_bad[i + 1] /*(new_bad[i] == 0 && new_bad[i + 1] != 0) || (new_bad[i] != 0 && new_bad[i + 1] == 0)*/)  {
//       a0  = (i-2) * PM_D2R;
//       a1  = (i-1) * PM_D2R;
//       r0  = new_r[i-2] > 0 ? new_r[i-2] : alt_r[i-2];
//       r1  = new_r[i-1] > 0 ? new_r[i-1] : alt_r[i-1];
//       double alt_r1 = fabs((r1 - r0) / (a1 - a0) * (((PM_TYPE)(i) * PM_D2R) - a0) + r0);
//
//       if(fabs(new_r[i] - ref_r[i]) > fabs(alt_r1 - ref_r[i]))
// 	alt_r[i] = alt_r1;
//     }
//   }
//
//
//   for (i = PM_L_POINTS - 3; i >= 0; i--) {
//
//     if(i == 0) {
//       a0  = (i + 1) * PM_D2R;
//       a1  = (i + 2) * PM_D2R;
//       r0  = new_r[i+1] > 0 ? new_r[i+1] : alt_r[i+1];
//       r1  = new_r[i+2] > 0 ? new_r[i+2] : alt_r[i+2];
//       double alt_r1 = fabs((r0 - r1) / (a1 - a0) * (a1 - (PM_TYPE)(i) * PM_D2R) + r1);
//       if(fabs(alt_r[i] - ref_r[i]) > fabs(alt_r1 - ref_r[i]) && fabs(new_r[i] - ref_r[i]) > fabs(alt_r1 - ref_r[i]))
// 	alt_r[i] = alt_r1;
//     }
//     else if(((act.seg(i) != 0 && act.seg(i) != act.seg(i - 1)) || ((/*ref.seg(i) != 0 &&*/ ref.seg(i) != ref.seg(i - 1)))) || new_bad[i] != new_bad[i - 1] /*(new_bad[i] == 0 && new_bad[i - 1] != 0) || (new_bad[i] != 0 && new_bad[i - 1] == 0)*/)  {
//       a0  = (i + 1) * PM_D2R;
//       a1  = (i + 2) * PM_D2R;
//       r0  = new_r[i+1] > 0 ? new_r[i+1] : alt_r[i+1];
//       r1  = new_r[i+2] > 0 ? new_r[i+2] : alt_r[i+2];
//       double alt_r1 = fabs((r0 - r1) / (a1 - a0) * (a1 - (PM_TYPE)(i) * PM_D2R) + r1);
//       if(fabs(alt_r[i] - ref_r[i]) > fabs(alt_r1 - ref_r[i]) && fabs(new_r[i] - ref_r[i]) > fabs(alt_r1 - ref_r[i]))
// 	alt_r[i] = alt_r1;
//
//     }
//   }
//
//   return;
// }

void PMSAlgo::interpolation(const PM_TYPE x_c, const PM_TYPE y_c, const PM_TYPE theta_c, PM_TYPE* fi, PM_TYPE* r, PM_TYPE* new_r, PM_TYPE* alt_r, int* new_bad, int* index, QVector< PM_TYPE > ref_r){

    PM_TYPE   x, y, delta;
    PM_TYPE   new_fi[PM_L_POINTS];
    int i;
    int     fi0, fi1;
    PM_TYPE r0, r1, a0, a1;

    // Convert range readings into ref frame
    int kk = 0;

    for (i = 0;i < PM_L_POINTS;i++) {
        double act_r = act.r(i) >= 0 ? act.r(i) : sensorMaxVal;

        delta   = theta_c + pm_fi[i];
        x       = act_r * cos(delta) + x_c;
        y       = act_r * sin(delta) + y_c;
        r[i]    = sqrt(x * x + y * y);
        fi[i]   = atan2(y, x);
        new_r[i]  = 0;//initialize big interpolated r;  a mm.
        alt_r[i] = -1000000;
        new_bad[i] = PM_EMPTY;//for interpolated r;
        index[i] = -1;
    }

    for (i = 0;i < PM_L_POINTS;i++) {
        new_fi[i] = fi[i] + M_PI/2.0;
    }

    int segmentEndRef;
    //   int segmentEndAct;
    double prev_fi;
    //   if(ref.seg(0) > 0) {
    //     segmentEndRef = ref.getSegmentsEnds().at(ref.seg(0) - 1).second;
    //     if(act.seg(0) > 0)
    //       segmentEndAct = act.getSegmentsEnds().at(act.seg(0) - 1).second;
    //     if()
    //     prev_fi = new_fi[0] > segmentEndRef * PM_D2R ? new_fi[0] : segmentEndRef * PM_D2R;
    //   }
    //   else {
    //     prev_fi = new_fi[0];
    //   }
    prev_fi = new_fi[0];
    new_bad[0] = !comparable[0] ? PM_REF_NOT_VISIBLE : new_bad[0];
    for (i = 1;i <= PM_L_POINTS/2;i++) {
        if(new_fi[i] <= prev_fi)
            new_bad[i] = PM_REF_NOT_VISIBLE;
        else if(!comparable[i]) {
            new_bad[i] = PM_REF_NOT_VISIBLE;
            prev_fi = new_fi[i];
        }
        else {
            prev_fi = new_fi[i];
        }
    }

    prev_fi = new_fi[PM_L_POINTS - 1];
    new_bad[PM_L_POINTS - 1] = !comparable[PM_L_POINTS - 1] ? PM_REF_NOT_VISIBLE : new_bad[PM_L_POINTS - 1];
    for (int i = PM_L_POINTS - 2; i >= PM_L_POINTS/2; i--) {
        if(new_fi[i] >= prev_fi)
            new_bad[i] = PM_REF_NOT_VISIBLE;
        else if(!comparable[i]) {
            new_bad[i] = PM_REF_NOT_VISIBLE;
            prev_fi = new_fi[i];
        }
        else {
            prev_fi = new_fi[i];
        }
    }

    //   if(ref.seg(PM_L_POINTS - 1) > 0) {
    //     segmentEndRef = ref.getSegmentsEnds().at(ref.seg(PM_L_POINTS - 1) - 1).first;
    //     prev_fi = new_fi[PM_L_POINTS - 1] < segmentEndRef * PM_D2R ? new_fi[PM_L_POINTS - 1] : segmentEndRef * PM_D2R;
    //   }
    //   else {
    //     prev_fi = new_fi[PM_L_POINTS - 1];
    //   }
    /*  prev_fi = new_fi[PM_L_POINTS - 1];
  for (i = PM_L_POINTS - 2;i >= PM_L_POINTS/2;i--) {
    if(new_fi[i] >= prev_fi)
      new_bad[i] = PM_REF_NOT_VISIBLE;
    else {
      if(ref.seg(i) > 0) {
    segmentEndRef = ref.getSegmentsEnds().at(ref.seg(i) - 1).first;
    prev_fi = new_fi[i] < segmentEndRef * PM_D2R ? new_fi[i] : segmentEndRef * PM_D2R;
      }
      else {
    prev_fi = new_fi[i];
      }
    }
  }*/


    for (i = 1;i < PM_L_POINTS;i++) { //i points to the angles in the actual scan
        // i and i-1 has to be in the same segment, both shouldn't be bad and they should be bigger than 0

        /*    if (new_fi[i] > prev_fi)//are the points visible?  */
        //     if (new_bad[i] != PM_REF_NOT_VISIBLE)//are the points visible?
        //     {
        a0  = new_fi[i-1];
        a1  = new_fi[i];
        fi0 = (int) ceil(new_fi[i-1] * PM_R2D);//fi0 is the meas. angle!
        //       fi0 = round(new_fi[i-1] * PM_R2D);//fi0 is the meas. angle!
        fi1 = (int)(new_fi[i] * PM_R2D);
        r0  = r[i-1];
        r1  = r[i];
        //       prev_fi = new_fi[i];
        //     } else {
        // //       new_bad[i] = PM_REF_NOT_VISIBLE;
        //       continue;
        //     }

        if(fi0 = 49)
            int foo = 0;
        bool compared = false;
        int prev_seg_act = 0;
        /*    int prev_act_i = 0;
    if(act.seg(i) != 0 && act.seg(i-1) != 0 && act.seg(i) != act.seg(i - 1)) {
      for(int j = prev_act_i; j < i; j++) {
    if(new_bad[j] == 0) {
      compared = true;
      break;
    }
      }
      if(compared) {
    for(int j = prev_act_i; j < i; j++) {
      if(new_bad[j] == PM_EMPTY)
        new_bad[j] = PM_REF_NOT_VISIBLE;
    }
      }
    }*/
        if (act.seg(i) != 0 && act.seg(i) == act.seg(i - 1)/* && ref.seg(i) != 0 && ref.seg(i) == ref.seg(i - 1)*/) {
            //calculation of the "whole" parts of the angles

            //       if (new_fi[i] > new_fi[i-1])//are the points visible?
            //       if (new_fi[i] > prev_fi)//are the points visible?
            //       {
            //         a0  = new_fi[i-1];
            //         a1  = new_fi[i];
            //         fi0 = (int) ceil(new_fi[i-1] * PM_R2D);//fi0 is the meas. angle!
            //         fi1 = (int)(new_fi[i] * PM_R2D);
            //         r0  = r[i-1];
            //         r1  = r[i];
            // 	prev_fi = new_fi[i];
            //       } else {
            // 	new_bad[i] = PM_REF_NOT_VISIBLE;
            //  	continue;
            //       }

            //interpolate for all the measurement bearings beween fi0 and fi1
            while (fi0 <= fi1) { //if at least one measurement point difference, then ...
                PM_TYPE ri = (r1 - r0) / (a1 - a0) * (((PM_TYPE)fi0 * PM_D2R) - a0) + r0;
                //if fi0 -> falls into the measurement range and ri is shorter
                //than the current range then overwrite it
                if (fi0 >= 0 && fi0 < PM_L_POINTS) {

                    double rrr = ref.r(fi0);
                    if(fi0 == 174)
                        int foo = 0;
                    if (new_bad[fi0] == PM_EMPTY || (new_bad[fi0] == 0 && new_r[fi0] > ri) ){
                        // 	    if(ri <= ref.r(fi0) + 1) {// + 10
                        new_r[fi0]    = ri;//overwrite the previous reading
                        if(alt_r[fi0] < 0)
                            alt_r[fi0]    = new_r[fi0];
                        index[fi0]    = i;//store which reading was used at index fi0
                        new_bad[fi0] = 0;//clear the empty flag
                        // 	    }
                        // 	    else
                        // 	      new_bad[fi0] = PM_REF_NOT_VISIBLE;
                    }
                }
                fi0++;//check the next measurement angle!
            }//while
        }//if act
        //     else
        //       prev_fi = new_fi[i];

        //     if((i < PM_L_POINTS - 1/* && fi0 < PM_L_POINTS - 1*/)  && ((act.seg(i) != 0 && act.seg(i) != act.seg(i + 1)) || ((ref.seg(i) != 0 && ref.seg(i) != ref.seg(i + 1)))))  {
        // //       a0  = new_fi[i-1];
        // //       a1  = new_fi[i];
        // //       fi0 = (int) ceil(new_fi[i-1] * PM_R2D);//fi0 is the meas. angle!
        // //       fi1 = (int)(new_fi[i] * PM_R2D);
        // //       r0  = r[i-1];
        // //       r1  = r[i];
        // //       double alt_r1 = (r1 - r0) / (a1 - a0) * (((PM_TYPE)(fi0 - 1) * PM_D2R) - a0) + r0;
        // //       if(fi0 > PM_L_POINTS - 2 || fi0 < 0)
        // // 	int foo = 0;
        // //       else
        // // 	alt_r[fi0 + 1] = (r1 - r0) / (a1 - a0) * (((PM_TYPE)(fi0 + 1) * PM_D2R) - a0) + r0;
        //       a0  = (i-1) * PM_D2R;
        //       a1  = i * PM_D2R;
        //       r0  = new_r[i-1];
        //       r1  = new_r[i];
        //       double alt_r1 = (r1 - r0) / (a1 - a0) * (((PM_TYPE)(i + 1) * PM_D2R) - a0) + r0;
        //       if(i > PM_L_POINTS - 2 || i < 0)
        // 	int foo = 0;
        //       else  if(fabs(new_r[i] - ref_r[i]) > fabs(alt_r1 - ref_r[i]))
        // 	alt_r[i + 1] = alt_r1;
        //     }
    }//for i

    for (i = 0;i < PM_L_POINTS;i++) {
        if(new_bad[i] == 0)
        {
            bool visible = visibleFromNewPosition(ref.r(i),pm_fi[i],x_c,y_c,theta_c);
            if (visible == false)
                new_bad[i] = PM_REF_NOT_VISIBLE;

            /*     if(i > 0 && i < PM_L_POINTS - 1) {
    if((new_bad[i - 1] == PM_REF_NOT_VISIBLE && ref.seg(i) == ref.seg(i - 1) && act.seg(i) == act.seg(i - 1) && ref.seg(i) && act.seg(i)) || new_bad[i + 1] == PM_REF_NOT_VISIBLE && ref.seg(i) == ref.seg(i + 1) && act.seg(i) == act.seg(i + 1) && ref.seg(i) && act.seg(i))
      new_bad[i] = PM_REF_NOT_VISIBLE;
      }
      else if(i == 0) {
    if(new_bad[i + 1] != 0 && ref.seg(i) == ref.seg(i + 1))
      new_bad[i] = PM_REF_NOT_VISIBLE;
      }
      else if(i == PM_L_POINTS - 1) {
    if(new_bad[i - 1] != 0 && ref.seg(i) == ref.seg(i - 1))
      new_bad[i] = PM_REF_NOT_VISIBLE;
      } */

            /*     if(i > 0 && i < PM_L_POINTS - 1) {
    if((new_bad[i - 1] != 0 && ref.seg(i) == ref.seg(i - 1) && ref.seg(i) != ref.seg(i + 1)) || new_bad[i + 1] != 0 && ref.seg(i) == ref.seg(i + 1) && ref.seg(i) != ref.seg(i - 1))
      new_bad[i] = PM_REF_NOT_VISIBLE;
      }
      else if(i == 0) {
    if(new_bad[i + 1] != 0 && ref.seg(i) == ref.seg(i + 1))
      new_bad[i] = PM_REF_NOT_VISIBLE;
      }
      else if(i == PM_L_POINTS - 1) {
    if(new_bad[i - 1] != 0 && ref.seg(i) == ref.seg(i - 1))
      new_bad[i] = PM_REF_NOT_VISIBLE;
      }*/
        }

        //     cout << new_bad[i] << " " << ref.r(i) << " " << new_r[i] << " " << alt_r[i] << " " << " " << i << " " << new_fi[i]/PM_D2R - 90 << endl;
    }
    //   cout << endl;
    for (i = 2;i < PM_L_POINTS;i++) {
        if(i == PM_L_POINTS - 1) {
            a0  = (i-2) * PM_D2R;
            a1  = (i-1) * PM_D2R;
            r0  = new_r[i-2] > 0 ? new_r[i-2] : alt_r[i-2];
            r1  = new_r[i-1] > 0 ? new_r[i-1] : alt_r[i-1];
            double alt_r1 = fabs((r1 - r0) / (a1 - a0) * (((PM_TYPE)(i) * PM_D2R) - a0) + r0);
            /*      if(i > PM_L_POINTS - 2 || i < 0)
    int foo = 0;
      else  */
            if(fabs(new_r[i] - ref_r[i]) > fabs(alt_r1 - ref_r[i]))
                alt_r[i] = alt_r1;
        }
        else if(((act.seg(i) != 0 && act.seg(i) != act.seg(i + 1)) || ((/*ref.seg(i) != 0 &&*/ ref.seg(i) != ref.seg(i + 1)))) || new_bad[i] != new_bad[i + 1] /*(new_bad[i] == 0 && new_bad[i + 1] != 0) || (new_bad[i] != 0 && new_bad[i + 1] == 0)*/)  {
            //       a0  = new_fi[i-1];
            //       a1  = new_fi[i];
            //       fi0 = (int) ceil(new_fi[i-1] * PM_R2D);//fi0 is the meas. angle!
            //       fi1 = (int)(new_fi[i] * PM_R2D);
            //       r0  = r[i-1];
            //       r1  = r[i];
            //       double alt_r1 = (r1 - r0) / (a1 - a0) * (((PM_TYPE)(fi0 - 1) * PM_D2R) - a0) + r0;
            //       if(fi0 > PM_L_POINTS - 2 || fi0 < 0)
            // 	int foo = 0;
            //       else
            // 	alt_r[fi0 + 1] = (r1 - r0) / (a1 - a0) * (((PM_TYPE)(fi0 + 1) * PM_D2R) - a0) + r0;
            a0  = (i-2) * PM_D2R;
            a1  = (i-1) * PM_D2R;
            r0  = new_r[i-2] > 0 ? new_r[i-2] : alt_r[i-2];
            r1  = new_r[i-1] > 0 ? new_r[i-1] : alt_r[i-1];
            double alt_r1 = fabs((r1 - r0) / (a1 - a0) * (((PM_TYPE)(i) * PM_D2R) - a0) + r0);
            /*      if(i > PM_L_POINTS - 2 || i < 0)
    int foo = 0;
      else  */
            if(fabs(new_r[i] - ref_r[i]) > fabs(alt_r1 - ref_r[i]))
                alt_r[i] = alt_r1;
        }
    }


    for (i = PM_L_POINTS - 3; i >= 0; i--) {

        if(i == 0) {
            a0  = (i + 1) * PM_D2R;
            a1  = (i + 2) * PM_D2R;
            r0  = new_r[i+1] > 0 ? new_r[i+1] : alt_r[i+1];
            r1  = new_r[i+2] > 0 ? new_r[i+2] : alt_r[i+2];
            double alt_r1 = fabs((r0 - r1) / (a1 - a0) * (a1 - (PM_TYPE)(i) * PM_D2R) + r1);
            if(fabs(alt_r[i] - ref_r[i]) > fabs(alt_r1 - ref_r[i]) && fabs(new_r[i] - ref_r[i]) > fabs(alt_r1 - ref_r[i]))
                alt_r[i] = alt_r1;
        }
        else if(((act.seg(i) != 0 && act.seg(i) != act.seg(i - 1)) || ((/*ref.seg(i) != 0 &&*/ ref.seg(i) != ref.seg(i - 1)))) || new_bad[i] != new_bad[i - 1] /*(new_bad[i] == 0 && new_bad[i - 1] != 0) || (new_bad[i] != 0 && new_bad[i - 1] == 0)*/)  {
            a0  = (i + 1) * PM_D2R;
            a1  = (i + 2) * PM_D2R;
            //       fi0 = (int) ceil(fi[i] * PM_R2D);//fi0 is the meas. angle!
            //       fi1 = (int)(fi[i+1] * PM_R2D);

            //       r0  = r[i+0];
            //       r1  = r[i+1];
            //       double alt_r1 = (r0 - r1) / (a1 - a0) * (a1 - (PM_TYPE)(i) * PM_D2R) + r1;
            // /*      if(fi0 > PM_L_POINTS - 1 || fi0 < 1)
            // 	int foo = 0;
            //       else*/
            //       if(fabs(alt_r[i-1] - ref_r[i-1]) > fabs(alt_r1 - ref_r[i-1]) && fabs(new_r[i-1] - ref_r[i-1]) > fabs(alt_r1 - ref_r[i-1]))
            // 	alt_r[i-1] = alt_r1;

            r0  = new_r[i+1] > 0 ? new_r[i+1] : alt_r[i+1];
            r1  = new_r[i+2] > 0 ? new_r[i+2] : alt_r[i+2];
            double alt_r1 = fabs((r0 - r1) / (a1 - a0) * (a1 - (PM_TYPE)(i) * PM_D2R) + r1);
            if(fabs(alt_r[i] - ref_r[i]) > fabs(alt_r1 - ref_r[i]) && fabs(new_r[i] - ref_r[i]) > fabs(alt_r1 - ref_r[i]))
                alt_r[i] = alt_r1;

        }
    }

    return;
}

void PMSAlgo::interpolation2(const PM_TYPE x_c, const PM_TYPE y_c, const PM_TYPE theta_c, PM_TYPE* fi, PM_TYPE* r, PM_TYPE* new_r, PM_TYPE* alt_r, int* new_bad, int* index){

    PM_TYPE   x, y, delta;
    PM_TYPE   new_fi[PM_L_POINTS];
    int i;
    int     fi0, fi1;
    PM_TYPE r0, r1, a0, a1;

    // Convert range readings into ref frame
    int kk = 0;

    double modTrans = sqrt(x_c * x_c + y_c * y_c);
    double alphaTrans = atan2(y_c, x_c);

    for (i = 0;i < PM_L_POINTS;i++) {
        delta   = theta_c + pm_fi[i];
        x       = act.r(i) * cos(delta) + x_c;
        y       = act.r(i) * sin(delta) + y_c;
        r[i]    = sqrt(x * x + y * y);
        fi[i]   = atan2(y, x);
        new_r[i]  = 0;//initialize big interpolated r;  a mm.
        alt_r[i] = -1000000;
        new_bad[i] = PM_EMPTY;//for interpolated r;
        index[i] = -1;
    }

    for (i = 0;i < PM_L_POINTS;i++) {
        new_fi[i] = fi[i] + M_PI/2.0;
    }

    for (i = 1;i < PM_L_POINTS;i++) { //i points to the angles in the actual scan
        // i and i-1 has to be in the same segment, both shouldn't be bad and they should be bigger than 0
        if (act.seg(i) != 0 && act.seg(i) == act.seg(i - 1)) {
            //calculation of the "whole" parts of the angles

            if(i == 78)
                int foo = 0;

            if (new_fi[i] > new_fi[i-1])//are the points visible?
            {
                a0  = new_fi[i-1];
                a1  = new_fi[i];
                fi0 = (int) ceil(new_fi[i-1] * PM_R2D);//fi0 is the meas. angle!
                fi1 = (int)(new_fi[i] * PM_R2D);
                r0  = r[i-1];
                r1  = r[i];
            } else {
                continue;
            }

            //interpolate for all the measurement bearings beween fi0 and fi1
            while (fi0 <= fi1) { //if at least one measurement point difference, then ...
                PM_TYPE ri = (r1 - r0) / (a1 - a0) * (((PM_TYPE)fi0 * PM_D2R) - a0) + r0;

                // 	double rho0 = act.getRhoEst().at(act.seg(i) - 1);
                // 	double phi0 = act.getPhiEst().at(act.seg(i) - 1);
                //
                // 	double rho1 = fabs(rho0 - modTrans*cos(phi0 - alphaTrans));
                //
                // 	double r_est = fabs(rho1/cos(phi0 - theta_c - ((PM_TYPE)fi0 * PM_D2R + M_PI/2.0 - theta_c)));


                //if fi0 -> falls into the measurement range and ri is shorter
                //than the current range then overwrite it
                if (fi0 >= 0 && fi0 < PM_L_POINTS) {
                    if (new_bad[fi0] == PM_EMPTY || (new_bad[fi0] == 0 && new_r[fi0] > ri)){
                        new_r[fi0]    = ri;//overwrite the previous reading
                        if(alt_r[fi0] < 0)
                            alt_r[fi0]    = new_r[fi0];
                        index[fi0]    = i;//store which reading was used at index fi0
                        new_bad[fi0] = 0;//clear the empty flag
                    }
                }
                fi0++;//check the next measurement angle!
            }//while
        }//if act

        if(i < PM_L_POINTS - 1 && act.seg(i) != 0 && act.seg(i) != act.seg(i + 1) && fi0 < PM_L_POINTS - 1) {
            a0  = new_fi[i-1];
            a1  = new_fi[i];
            fi0 = (int) ceil(new_fi[i-1] * PM_R2D);//fi0 is the meas. angle!
            fi1 = (int)(new_fi[i] * PM_R2D);
            r0  = r[i-1];
            r1  = r[i];
            double alt_r1 = (r1 - r0) / (a1 - a0) * (((PM_TYPE)(fi0 + 1) * PM_D2R) - a0) + r0;
            alt_r[fi0 + 1] = (r1 - r0) / (a1 - a0) * (((PM_TYPE)(fi0 + 1) * PM_D2R) - a0) + r0;
        }
    }//for i

    for (i = 0;i < PM_L_POINTS;i++) {
        if(new_bad[i] == 0)
        {
            bool visible = visibleFromNewPosition(ref.r(i),pm_fi[i],x_c,y_c,theta_c);
            if (visible == false)
                new_bad[i] = PM_REF_NOT_VISIBLE;
        }
    }

    cout << "Refscan " << ref;
    cout << "Actscan " << act << endl;

    bool start = true;
    int initArea = -1;
    overlappedAreas.clear();
    for (i = 1;i < PM_L_POINTS;i++) {
        if(act.seg(i) != 0 && act.seg(i) == act.seg(i - 1) && !new_bad[i] && !ref.bad(i)) {
            if(start) {
                initArea = i - 1;
                start = false;
            }

            if((i < PM_L_POINTS - 1 &&  (act.seg(i) != act.seg(i + 1) || new_bad[i+1] || ref.bad(i+1))) || i == PM_L_POINTS - 1) {//
                overlappedAreas.append(QPoint(initArea, i));
                start = true;
            }
        }
    }

    return;
}



void PMSAlgo::getNumericalH(const PM_TYPE x_c, const PM_TYPE y_c, const PM_TYPE theta_c, PM_TYPE h_matrix[][2]){

    PM_TYPE   delta_xc = 50.0, delta_yc = 50.0;
    int       dummy_index[PM_L_POINTS];//which new point was used using which old point..
    PM_TYPE   dummy_r[PM_L_POINTS], dummy_fi[PM_L_POINTS];//actual scan in ref. coord. syst.
    PM_TYPE   r_plus[PM_L_POINTS],r_minus[PM_L_POINTS];
    int       bad_plus[PM_L_POINTS],bad_minus[PM_L_POINTS];

    // xc + delta AND yc

    interpolation(x_c+delta_xc,y_c,theta_c,dummy_fi,dummy_r,r_plus,bad_plus,dummy_index);

    // xc - delta AND yc

    interpolation(x_c-delta_xc,y_c,theta_c,dummy_fi,dummy_r,r_minus,bad_minus,dummy_index);

    for (int i = 0;i < PM_L_POINTS;i++) {
        if(bad_plus[i] == 0 && bad_minus[i] == 0){
            h_matrix[i][0] = (r_plus[i] - r_minus[i])/(2.0*delta_xc);
        } else {
            h_matrix[i][0] = 0.0;
        }
    }

    // xc AND yc + delta

    interpolation(x_c,y_c+delta_yc,theta_c,dummy_fi,dummy_r,r_plus,bad_plus,dummy_index);

    // xc AND yc - delta

    interpolation(x_c,y_c-delta_yc,theta_c,dummy_fi,dummy_r,r_minus,bad_minus,dummy_index);

    for (int i = 0;i < PM_L_POINTS;i++) {
        if(bad_plus[i] == 0 && bad_minus[i] == 0){
            h_matrix[i][1] = (r_plus[i] - r_minus[i])/(2.0*delta_yc);
        } else {
            h_matrix[i][1] = 0.0;
        }
    }

    return;
}

void PMSAlgo::getCompleteNumericalH(const PM_TYPE x_c, const PM_TYPE y_c, const PM_TYPE theta_c, PM_TYPE h_matrix[][3]){

    PM_TYPE   delta_xc = 2.50, delta_yc = 2.50, delta_thc = 0.01*M_PI/180.0;
    int       dummy_index[PM_L_POINTS];//which new point was used using which old point..
    PM_TYPE   dummy_r[PM_L_POINTS], dummy_fi[PM_L_POINTS];//actual scan in ref. coord. syst.
    PM_TYPE   r_plus[PM_L_POINTS],r_minus[PM_L_POINTS];
    int       bad_plus[PM_L_POINTS],bad_minus[PM_L_POINTS];

    // xc + delta AND yc AND theta_c

    interpolation(x_c+delta_xc,y_c,theta_c,dummy_fi,dummy_r,r_plus,bad_plus,dummy_index);

    // xc - delta AND yc AND theta_c

    interpolation(x_c-delta_xc,y_c,theta_c,dummy_fi,dummy_r,r_minus,bad_minus,dummy_index);

    for (int i = 0;i < PM_L_POINTS;i++) {
        if(bad_plus[i] == 0 && bad_minus[i] == 0){
            h_matrix[i][0] = (r_plus[i] - r_minus[i])/(2.0*delta_xc);
        } else {
            h_matrix[i][0] = 0.0;
        }
    }

    // xc AND yc + delta AND theta_c

    interpolation(x_c,y_c+delta_yc,theta_c,dummy_fi,dummy_r,r_plus,bad_plus,dummy_index);

    // xc AND yc - delta AND theta_c

    interpolation(x_c,y_c-delta_yc,theta_c,dummy_fi,dummy_r,r_minus,bad_minus,dummy_index);

    for (int i = 0;i < PM_L_POINTS;i++) {
        if(bad_plus[i] == 0 && bad_minus[i] == 0){
            h_matrix[i][1] = (r_plus[i] - r_minus[i])/(2.0*delta_yc);
        } else {
            h_matrix[i][1] = 0.0;
        }
    }

    // xc AND yc AND theta_c + delta

    interpolation(x_c,y_c,theta_c+delta_thc,dummy_fi,dummy_r,r_plus,bad_plus,dummy_index);

    // xc AND yc AND theta_c - delta

    interpolation(x_c,y_c,theta_c-delta_thc,dummy_fi,dummy_r,r_minus,bad_minus,dummy_index);

    for (int i = 0;i < PM_L_POINTS;i++) {
        if(bad_plus[i] == 0 && bad_minus[i] == 0){
            h_matrix[i][2] = (r_plus[i] - r_minus[i])/(2.0*delta_thc);
        } else {
            h_matrix[i][2] = 0.0;
        }
    }

    return;
}

bool PMSAlgo::visibleFromNewPosition(const PM_TYPE r_original, const PM_TYPE phi_original, const PM_TYPE x_c, const PM_TYPE y_c, const PM_TYPE theta_c)
{

    bool visible; // aquí se supone 180 grados
    PM_TYPE x_new_position = (r_original*cos(phi_original)-x_c)*cos(theta_c)+(r_original*sin(phi_original)-y_c)*sin(theta_c);

    if (x_new_position  >= 0.0)
        visible = true;
    else
        visible=false;

    return(visible);
}

PM_TYPE PMSAlgo::bruteforceOrientationSearch(const PM_TYPE ax, const PM_TYPE ay, const PM_TYPE ath, const PM_TYPE lower_limit, const PM_TYPE upper_limit, const PM_TYPE delta_theta, const bool log)
{

    PM_TYPE   fi[PM_L_POINTS],r[PM_L_POINTS],new_r[PM_L_POINTS];
    int       new_bad[PM_L_POINTS],index[PM_L_POINTS];
    int 	    i, n, k, imin, number_elements;
    PM_TYPE e, new_theta_c, dth, emin = 1000000;
    PM_TYPE *beta, *err;

    number_elements = 1+floor((upper_limit - lower_limit)/delta_theta);
    new_theta_c = lower_limit;

    beta = new PM_TYPE[number_elements];
    err = new PM_TYPE[number_elements];

    for (k = 0; k <  number_elements; ++k) {

        n=0;
        e=0.0;

        beta[k] = new_theta_c;

        interpolation(ax,ay,ath+new_theta_c*M_PI/180.0,fi,r,new_r,new_bad,index);

        for (i = 0;i < PM_L_POINTS;i++) {
            if (!new_bad[i] && !ref.bad(i)) {
                e += fabs(new_r[i] - ref.r(i));
                n++;
            }
        }

        if (n > 0)
            err[k]  = e / n;//don't forget to correct with n!
        else
            err[k]  = 100000;

        new_theta_c = new_theta_c + delta_theta;

    }//for new_theta_c

    for (i = 0;i < k;i++) {
        if (err[i] < emin) {
            emin = err[i];
            imin = i;
        }
    }

    if (err[imin] >= 100000) { // a mm.
        cerr << "Polar Match: orientation search failed" << err[imin] << endl;
        throw 1;
    }

    dth = beta[imin];

    if (log)
    {
        fprintf(fileoutput,"%d ",number_elements);
        for (int j = 0;j < k;j++) {
            fprintf(fileoutput,"%f %f ",beta[j],err[j]);
        }
        fprintf(fileoutput,"%d %f %f ",imin,dth,emin);
    }

    delete[] beta;
    delete[] err;

    return dth;
}

void PMSAlgo::translationsearch(const PM_TYPE ax, const PM_TYPE ay, const PM_TYPE ath, PM_TYPE *dx, PM_TYPE *dy){

    //  if (iter > 10)
    //    C = 10000; // a mm.

    // do the weighted linear regression on the linearized ...
    // include angle as well
    PM_TYPE hi1, hi2, hwi1, hwi2, hw1 = 0, hw2 = 0, hwh11 = 0;
    PM_TYPE hwh12 = 0, hwh22 = 0, w; //  hwh21 = 0,
    PM_TYPE h_matrix[PM_L_POINTS][2], dr;
    PM_TYPE new_r[PM_L_POINTS];//interpolated r at measurement bearings
    int     new_bad[PM_L_POINTS];//bad flags of the interpolated range readings
    PM_TYPE   fi[PM_L_POINTS],r[PM_L_POINTS];
    int       index[PM_L_POINTS];
    PM_TYPE abs_err = 0.0, C = 700 * 700;
    int n = 0;

    interpolation(ax,ay,ath,fi,r,new_r,new_bad,index);
    getNumericalH(ax,ay,ath,h_matrix);

    for (int i = 0;i < PM_L_POINTS;i++) {
        dr = ref.r(i) - new_r[i];

        //weight calculation
        if (ref.bad(i) == 0 && new_bad[i] == 0 && new_r[i] < sensorMaxVal && new_r[i] > 100.0 && fabs(dr) < PM_MAX_ERROR) { // 100 a mm.

            //        abs_err += fabs(dr);

            w = C / (dr * dr + C);

            //        w = 1.0/fabs(dr);if(w>100)  w = 100;
            n++;

            //proper calculations of the jacobian
            // ==ins==        hi1 = pm_co[i];
            // ==ins==        hi2 = pm_si[i];

            hi1 = h_matrix[i][0];
            hi2 = h_matrix[i][1];

            hwi1 = hi1 * w;
            hwi2 = hi2 * w;

            //par = (H^t*W*H)^-1*H^t*W*dr
            hw1 += hwi1 * dr;//H^t*W*dr
            hw2 += hwi2 * dr;

            //H^t*W*H
            hwh11 += hwi1 * hi1;
            hwh12 += hwi1 * hi2;
            //        hwh21 += hwi2*hi1; //should take adv. of symmetricity!!
            hwh22 += hwi2 * hi2;

        }//if
    }//for i
    //    cerr << "num points " << n << endl;

    // ==ins==    if (n < PM_MIN_VALID_POINTS) { //are there enough points?
    if (n < 2) { //are there enough points?
        cerr << "pm_linearized_match: ERROR not enough points " << n << endl;
        throw 1;//not enough points
    }

    //calculation of inverse
    PM_TYPE D;//determinant
    PM_TYPE inv11, inv21, inv12, inv22;//inverse matrix

    D = hwh11 * hwh22 - hwh12 * hwh12;
    if (D < 0.001) {
        cerr << "pm_linearized_match: ERROR determinant to small! " << D << endl;
        //       throw 1;
    }
    inv11 =  hwh22 / D;
    inv12 = -hwh12 / D;
    inv21 = -hwh12 / D;
    inv22 =  hwh11 / D;

    *dx = inv11 * hw1 + inv12 * hw2;
    *dy = inv21 * hw1 + inv22 * hw2;
    // Compute error at new position
    abs_err = getErrorOverN(ax+*dx, ay+*dy, ath);

    return;
}

PM_TYPE PMSAlgo::bruteforceTranslationSearch(const PM_TYPE ax, const PM_TYPE ay, const PM_TYPE ath, PM_TYPE *ax_best, PM_TYPE *ay_best, const bool log){

    PM_TYPE err_dxdy[161], eminxy = 1000000;
    PM_TYPE new_ax, new_ay;
    int y_index, n;
    PM_TYPE new_r[PM_L_POINTS];//interpolated r at measurement bearings
    int     new_bad[PM_L_POINTS];//bad flags of the interpolated range readings
    int     index[PM_L_POINTS];//which new point was used using which old point..
    PM_TYPE r[PM_L_POINTS], fi[PM_L_POINTS];//actual scan in ref. coord. syst.

    //  for (int dx_explore = -800;dx_explore <= 800;dx_explore = dx_explore+10)
    for (PM_TYPE dx_explore = -100.0;dx_explore <= 100.0;dx_explore = dx_explore+1.25)
    {

        new_ax = ax + dx_explore;
        y_index = 0;

        //    for (int dy_explore = -800;dy_explore <= 800;dy_explore = dy_explore+10)
        for (PM_TYPE dy_explore = -100.0;dy_explore <= 100.0;dy_explore = dy_explore+1.25)
        {

            new_ay = ay + dy_explore;
            n = 0;
            err_dxdy[y_index] = 0.0;

            interpolation(new_ax,new_ay,ath,fi,r,new_r,new_bad,index);

            for (int i = 0;i < PM_L_POINTS;i++) {
                if (!new_bad[i] && !ref.bad(i)) {
                    err_dxdy[y_index] += fabs(new_r[i] - ref.r(i));
                    n++;
                }
            }

            if (n > 0)
                err_dxdy[y_index]  = err_dxdy[y_index] / n;//don't forget to correct with n!
            else
                err_dxdy[y_index]  = 100000;

            if(err_dxdy[y_index] < eminxy)
            {
                eminxy = err_dxdy[y_index];
                *ax_best = new_ax;
                *ay_best = new_ay;
            }

            y_index = y_index + 1;

        }

        if (log)
        {
            for (int p_index = 0;p_index <= 160;p_index++)
            {
                fprintf(fileoutput,"%f ",err_dxdy[p_index]);
            }
        }
    }

    return(eminxy);
}

void PMSAlgo::bruteforceXYThSearch(const PM_TYPE ax, const PM_TYPE ay, const PM_TYPE ath, const bool log){

    PM_TYPE err_dxdy[161], eminxy = 1000000;
    PM_TYPE err_dxdth[161], eminxth = 1000000;
    PM_TYPE err_dydth[161], eminyth = 1000000;
    PM_TYPE new_r[PM_L_POINTS];//interpolated r at measurement bearings
    int     new_bad[PM_L_POINTS];//bad flags of the interpolated range readings
    int     index[PM_L_POINTS];//which new point was used using which old point..
    PM_TYPE r[PM_L_POINTS], fi[PM_L_POINTS];//actual scan in ref. coord. syst.
    PM_TYPE ax_best,ay_best,ath_best;
    int new_ax, new_ay, new_ath, y_index, th_index, n;

    // x-y

    for (int dx_explore = -250;dx_explore <= 250;dx_explore = dx_explore+10)
    {

        new_ax = ax + dx_explore;
        y_index = 0;

        for (int dy_explore = -250;dy_explore <= 250;dy_explore = dy_explore+10)
        {

            new_ay = ay + dy_explore;
            n = 0;
            err_dxdy[y_index] = 0.0;

            interpolation(new_ax,new_ay,ath,fi,r,new_r,new_bad,index);

            for (int i = 0;i < PM_L_POINTS;i++) {
                if (!new_bad[i] && !ref.bad(i)) {
                    err_dxdy[y_index] += fabs(new_r[i] - ref.r(i));
                    n++;
                }
            }

            if (n > 0)
                err_dxdy[y_index]  = err_dxdy[y_index] / n;//don't forget to correct with n!
            else
                err_dxdy[y_index]  = 100000;

            if(err_dxdy[y_index] < eminxy)
            {
                eminxy = err_dxdy[y_index];
                ax_best = new_ax;
                ay_best = new_ay;
            }

            y_index = y_index + 1;

        }

        if (log)
        {
            for (int p_index = 0;p_index <= 50;p_index++)
            {
                fprintf(fileoutput,"%f ",err_dxdy[p_index]);
            }
        }
    }

    if (log)
    {
        fprintf(fileoutput,"%f %f %f ",ax_best,ay_best,eminxy);
    }



    // x -th

    for (int dx_explore = -250;dx_explore <= 250;dx_explore = dx_explore+10)
    {

        new_ax = ax + dx_explore;
        th_index = 0;

        for (int dth_explore = -0.25*M_PI;dth_explore <= 0.25*M_PI;dth_explore = dth_explore+0.01*M_PI)
        {

            new_ath = ath + dth_explore;
            n = 0;
            err_dxdth[th_index] = 0.0;

            interpolation(new_ax,ay,new_ath,fi,r,new_r,new_bad,index);

            for (int i = 0;i < PM_L_POINTS;i++) {
                if (!new_bad[i] && !ref.bad(i)) {
                    err_dxdth[th_index] += fabs(new_r[i] - ref.r(i));
                    n++;
                }
            }

            if (n > 0)
                err_dxdth[th_index]  = err_dxdth[th_index] / n;//don't forget to correct with n!
            else
                err_dxdth[th_index]  = 100000;

            if(err_dxdth[th_index] < eminxth)
            {
                eminxth = err_dxdth[th_index];
                ax_best = new_ax;
                ath_best = new_ath;
            }

            th_index = th_index + 1;

        }

        if (log)
        {
            for (int p_index = 0;p_index <= 160;p_index++)
            {
                fprintf(fileoutput,"%f ",err_dxdth[p_index]);
            }
        }
    }

    if (log)
    {
        fprintf(fileoutput,"%f %f %f ",ax_best,ath_best,eminxth);
    }

    // y -th

    for (int dy_explore = -250;dy_explore <= 250;dy_explore = dy_explore+10)
    {

        new_ay = ay + dy_explore;
        th_index = 0;

        for (int dth_explore = -0.25*M_PI;dth_explore <= 0.25*M_PI;dth_explore = dth_explore+0.01*M_PI)
        {

            new_ath = ath + dth_explore;
            n = 0;
            err_dydth[th_index] = 0.0;

            interpolation(ax,new_ay,new_ath,fi,r,new_r,new_bad,index);

            for (int i = 0;i < PM_L_POINTS;i++) {
                if (!new_bad[i] && !ref.bad(i)) {
                    err_dydth[th_index] += fabs(new_r[i] - ref.r(i));
                    n++;
                }
            }

            if (n > 0)
                err_dydth[th_index]  = err_dydth[th_index] / n;//don't forget to correct with n!
            else
                err_dydth[th_index]  = 100000;

            if(err_dydth[th_index] < eminyth)
            {
                eminyth = err_dydth[th_index];
                ay_best = new_ay;
                ath_best = new_ath;
            }

            th_index = th_index + 1;

        }

        if (log)
        {
            for (int p_index = 0;p_index <= 160;p_index++)
            {
                fprintf(fileoutput,"%f ",err_dydth[p_index]);
            }
        }
    }

    if (log)
    {
        fprintf(fileoutput,"%f %f %f ",ay_best,ath_best,eminyth);
    }

    return;
}





// void PMSAlgo::matchDiosi(PM_TYPE* ax, PM_TYPE* ay, PM_TYPE *ath){
//
//   PM_TYPE   delta;
//   PM_TYPE   r[PM_L_POINTS], fi[PM_L_POINTS],new_fi[PM_L_POINTS];//actual scan in ref. coord. syst.
//   PM_TYPE   x, y;
//   PM_TYPE   new_r[PM_L_POINTS];//interpolated r at measurement bearings
//   int       new_bad[PM_L_POINTS];//bad flags of the interpolated range readings
//   int       index[PM_L_POINTS];//which new point was used using which old point..
//   PM_TYPE   C = 700 * 700;//weighting factor; see dudek00 a mm
//   int       n = 0;//number of valid points
//   int       iter, i, small_corr_cnt = 0;
//   int       window       = 20;//+- width of search for correct orientation
//   PM_TYPE   /*abs_err = 0, */dx = 10, dy = 10, dth = 0;//match error, actual scan corrections
//   PM_TYPE   abs_err,initial_abs_err;
//
//   iter = -1;
//
//   while (++iter < PM_MAX_ITER && small_corr_cnt < 5) { //has to be 5 small corrections before stop
//
//     if ((fabs(dx / 10) + fabs(dy / 10) + fabs(dth)) < 1) //  /10 a mm.
//       small_corr_cnt++;
//     else
//       small_corr_cnt = 0;
//
//     act.setRx(*ax);
//     act.setRy(*ay);
//     act.setTh(*ath);
//
//     // convert range readings into ref frame
//     // this can be speeded up, by connecting it with the interpolation
//     for (i = 0;i < PM_L_POINTS;i++) {
//       delta   = *ath + pm_fi[i];
//       x       = act.r(i) * cos(delta) + *ax;
//       y       = act.r(i) * sin(delta) + *ay;
//       r[i]    = sqrt(x * x + y * y);
//       fi[i]   = atan2(y, x);
// //      px[i]   = x;
// //      py[i]   = y;
// //       new_r[i]  = 10000;//initialize big interpolated r;
//       new_r[i]  = 100000;//initialize big interpolated r;  a mm.
//       new_bad[i] = PM_EMPTY;//for interpolated r;
//     }
//
//     for (i = 0;i < PM_L_POINTS;i++) {
//       new_fi[i] = fi[i] + M_PI/2.0;
//     }
//
//     //------------------------INTERPOLATION------------------------
//     //calculate/interpolate the associations to the ref scan points
//     //algorithm ignores crosings at 0 and 180 -> to make it faster
//     for (i = 1;i < PM_L_POINTS;i++) { //i points to the angles in the actual scan
//
//       // i and i-1 has to be in the same segment, both shouldn't be bad
//       // and they should be bigger than 0
//       if (act.seg(i) != 0 && act.seg(i) == act.seg(i - 1) && !act.bad(i) &&
//           !act.bad(i - 1) && new_fi[i] > 0 && new_fi[i-1] > 0) {
//         //calculation of the "whole" parts of the angles
//         int     fi0, fi1;
//         PM_TYPE r0, r1, a0, a1;
//
//         bool occluded;
//         if (new_fi[i] > new_fi[i-1])//are the points visible?
//         {//visible
// // 	  occluded = false;
//           a0  = new_fi[i-1];
//           a1  = new_fi[i];
//           fi0 = (int) ceil(new_fi[i-1] * PM_R2D);//fi0 is the meas. angle!
//           fi1 = (int)(new_fi[i] * PM_R2D);
//           r0  = r[i-1];
//           r1  = r[i];
//         }
//         else {//invisible - still have to calculate to filter out points which
// /*	  if((int) ceil(new_fi[i-1] * PM_R2D) >= PM_L_POINTS / 2)
// 	    occluded = false;
// 	  else
// 	    occluded = true;*/
//
// // (int) ceil(new_fi[i-1] * PM_R2D)
//
//           occluded = true; //are covered up by these!
//           //flip the points-> easier to program
//           a0  = new_fi[i];
//           a1  = new_fi[i-1];
//           fi0 = (int) ceil(new_fi[i] * PM_R2D);
//           fi1 = (int)(new_fi[i-1] * PM_R2D);
//           r0  = r[i];
//           r1  = r[i-1];
//         }
//         //here fi0 is always smaller than fi1!
//
//         //interpolate for all the measurement bearings beween fi0 and fi1
//         while (fi0 <= fi1) { //if at least one measurement point difference, then ...
//           PM_TYPE ri = (r1 - r0) / (a1 - a0) * (((PM_TYPE)fi0 * PM_D2R) - a0) + r0;
// // 	  PM_TYPE ri = fabs(rho_est/cos(phi_est - (fi0 - 90)* PM_D2R));
//
//           //if fi0 -> falls into the measurement range and ri is shorter
//           //than the current range then overwrite it
//           if (fi0 >= 0 && fi0 < PM_L_POINTS && new_r[fi0] > ri) {
//             new_r[fi0]    = ri;//overwrite the previous reading
//             new_bad[fi0] &= ~PM_EMPTY;//clear the empty flag
//             index[fi0]    = i;//store which reading was used at index fi0
//             if (occluded) //check if it was occluded
//               new_bad[fi0] = new_bad[fi0] | PM_OCCLUDED;//set the occluded flag
//             else
//               new_bad[fi0] = new_bad[fi0] & ~PM_OCCLUDED;
//             //the new range reading also it has to inherit the other flags
//             new_bad[fi0] |= act.bad(i);//superfluos - since act.bad[i] was checked for 0
//             new_bad[fi0] |= act.bad(i - 1);//superfluos - since act.bad[i-1] was checked for 0
//             //if(ri>sensorMaxVal)        //uncomment this later
//             //  new_bad[fi0] |= PM_RANGE;
//           }
//           fi0++;//check the next measurement angle!
//         }//while
//       }//if act
//     }//for i
//
// //    for (i = 0;i < PM_L_POINTS;i++) {
// //      fprintf(fileoutput,"%f %f %f %d ",fi[i],r[i],new_r[i],new_bad[i]);
// //    }
//
//     //---------------ORIENTATION SEARCH-----------------------------------
//     //search for angle correction using crosscorrelation
//
//    cout << ref;
//    cout << endl;
//    cout << "i\tnew bad\t new r " << endl;
//    for(int j = 0; j < PM_L_POINTS; j++) {
//      cout << j << "\t   " << new_bad[j] << "\t\t"<< new_r[j] << endl;
//    }
//    cout << endl;
//
//
//    if (iter % 2 == 0) {
//       //pm_fi,ref.r - reference points
//       //r,fi        - actual points which are manipulated
//       //new_r, new_bad contains the
//
//       PM_TYPE e, err[100];      // the error rating
//       PM_TYPE beta[100];// angle for the corresponding error
// //      PM_TYPE C = 10000;
//       PM_TYPE n;
//       int k = 0;
//
//       dx = 0.0;
//       dy = 0.0;
//       dth = 0.0;
//
//       for (int di = -window;di <= window;di++) {
//         if(!di)
//           n = 0;
//         n = 0;
//         e = 0;
//
//         int min_i, max_i;
//         if (di <= 0) {
//           min_i = -di;
//           max_i = PM_L_POINTS;
//         }
//         else {
//           min_i = 0;
//           max_i = PM_L_POINTS - di;
//         }
//
//         for (i = min_i;i < max_i;i++) { //searching through the actual points
//           //if fi[i],r[i] isn't viewed from the back, isn't moving
//           // and isn't a solitary point, then try to associate it ..
//           //also fi[i] is within the angle range ...
//
//           if (!new_bad[i] && !ref.bad(i + di)) {
// 	    double n_r = new_r[i];
// 	    double r_r = ref.r(i + di);
// 	    double d_e = fabs(new_r[i] - ref.r(i + di));
// 	    if(d_e > 300)
// 	      int jj= 0;
//             e += fabs(new_r[i] - ref.r(i + di));
//             n++;
//           }
//
//         }//for i
//
//         if (n > 0)
//           err[k]  = e / n;//don't forget to correct with n!
//         else
// //           err[k]  = 10000;//don't forget to correct with n!
//           err[k]  = 100000;//don't forget to correct with n!   a mm.
//         beta[k] = di;
//         k++;
//       }//for dfi
//
//       //now search for the global minimum
//       //later I can make it more robust
//       //assumption: monomodal error function!
//       PM_TYPE emin = 1000000;
//       int   imin;
//       for (i = 0;i < k;i++) {
//         if (err[i] < emin) {
//           emin = err[i];
//           imin = i;
//         }
//       }
// //       if(err[imin]>=10000)
//       if (err[imin] >= 100000) { // a mm.
//         cerr << "Polar Match: orientation search failed" << err[imin] << endl;
//         throw 1;
//       }
//       dth = beta[imin];
//
//       //interpolation
//       if (imin >= 1 && imin < (k - 1)) //is it not on the extreme?
//       {//lets try interpolation
//         PM_TYPE D = err[imin-1] + err[imin+1] - 2.0 * err[imin];
//         PM_TYPE d = 1000;
//         if (fabs(D) > 0.1 && err[imin-1] > err[imin] && err[imin+1] > err[imin]) { // 0.1 a mm.
//           d = (err[imin-1] - err[imin+1]);
//           d = (err[imin-1] - err[imin+1]) / D / 2.0;
//         }
//         if (fabs(d) < 1)
//           dth += d;
//       }//if
//
//
//       (*ath) = (*ath) + dth * M_PI / 180.0;
//
//       logThIteration(*ax,*ay,*ath,dx,dy,dth*M_PI/180.0,-10.0,10.0);
//
//       continue;
//     }
//
//     dx = 0.0;
//     dy = 0.0;
//     dth = 0.0;
//
//     //------------------------------------------translation-------------
//     if (iter > 10)
// //        C = 100;
//       C = 10000; // a mm.
//     // do the weighted linear regression on the linearized ...
//     // include angle as well
//     PM_TYPE hi1, hi2, hwi1, hwi2, hw1 = 0, hw2 = 0, hwh11 = 0;
//     PM_TYPE hwh12 = 0, hwh22 = 0, w; // hwh21 = 0,
//     PM_TYPE dr;
// //    abs_err = 0;
//     n = 0;
//
//     for (i = 0;i < PM_L_POINTS;i++) {
//
//       dr = ref.r(i) - new_r[i];
//
//       //weight calculation
//       if (ref.bad(i) == 0 && new_bad[i] == 0 && new_r[i] < sensorMaxVal && new_r[i] > 100.0 && fabs(dr) < PM_MAX_ERROR) { // 100 a mm.
//
// //        abs_err += fabs(dr);
//
//         //weighting according to DUDEK00
//         w = C / (dr * dr + C);
// //        w = 1.0/fabs(dr);if(w>100)  w = 100;
//         n++;
//
//         //proper calculations of the jacobian
//         hi1 = pm_co[i];//xx/new_r[i];//this the correct
//         hi2 = pm_si[i];//yy/new_r[i];
//
//         hwi1 = hi1 * w;
//         hwi2 = hi2 * w;
//
//         //par = (H^t*W*H)^-1*H^t*W*dr
//         hw1 += hwi1 * dr;//H^t*W*dr
//         hw2 += hwi2 * dr;
//
//         //H^t*W*H
//         hwh11 += hwi1 * hi1;
//         hwh12 += hwi1 * hi2;
// //        hwh21 += hwi2*hi1; //should take adv. of symmetricity!!
//         hwh22 += hwi2 * hi2;
//
//       }//if
//     }//for i
//
//     cerr << "num points " << n << endl;
//     if (n < 2) { //are there enough points?
// // ==ins==    if (n < PM_MIN_VALID_POINTS) { //are there enough points?
//       cerr << "pm_linearized_match: ERROR not enough points " << n << endl;
//       throw 1;//not enough points
//     }
//
//     //calculation of inverse
//     PM_TYPE D;//determinant
//     PM_TYPE inv11, inv21, inv12, inv22;//inverse matrix
//
//     D = hwh11 * hwh22 - hwh12 * hwh12;
//     if (D < 0.001) {
//       cerr << "pm_linearized_match: ERROR determinant to small! " << D << endl;
//       throw 1;
//     }
//     inv11 =  hwh22 / D;
//     inv12 = -hwh12 / D;
//     inv21 = -hwh12 / D;
//     inv22 =  hwh11 / D;
//
//     dx = inv11 * hw1 + inv12 * hw2;
//     dy = inv21 * hw1 + inv22 * hw2;
//
//     initial_abs_err = getErrorOverN(*ax, *ay, *ath);
//     abs_err = getErrorOverN(*ax+dx, *ay+dy, *ath);
// //  Use dx, dy only if error is smaller
//     if (abs_err < initial_abs_err)
//     {
//       (*ax) = (*ax) + dx;
//       (*ay) = (*ay) + dy;
//     } else {
//       dx = 0.0;
//       dy = 0.0;
//     }
//     logXYIteration(*ax,*ay,*ath,dx,dy,dth*M_PI/180.0);
//
//   }
//   if(iter >= 30)
//     cerr << "Max. num. iter" << endl;
// }

void PMSAlgo::matchDiosiIsrael(PM_TYPE* ax, PM_TYPE* ay, PM_TYPE *ath){

    PM_TYPE   delta;
    PM_TYPE   r[PM_L_POINTS], fi[PM_L_POINTS],new_fi[PM_L_POINTS];//actual scan in ref. coord. syst.
    PM_TYPE   x, y;
    PM_TYPE   new_r[PM_L_POINTS];//interpolated r at measurement bearings
    int       new_bad[PM_L_POINTS];//bad flags of the interpolated range readings
    int       index[PM_L_POINTS];//which new point was used using which old point..
    PM_TYPE   C = 700 * 700;//weighting factor; see dudek00 a mm
    int       n = 0;//number of valid points
    int       iter, i, small_corr_cnt = 0;
    int       window       = 20;//+- width of search for correct orientation
    PM_TYPE   /*abs_err = 0, */dx = 10, dy = 10, dth = 0;//match error, actual scan corrections
    PM_TYPE   abs_err,initial_abs_err;

    iter = -1;
    matchOk = true;

    while (++iter < PM_MAX_ITER && small_corr_cnt < 5) { //has to be 5 small corrections before stop

        if ((fabs(dx / 10) + fabs(dy / 10) + fabs(dth)) < 1) //  /10 a mm.
            small_corr_cnt++;
        else
            small_corr_cnt = 0;

        act.setRx(*ax);
        act.setRy(*ay);
        act.setTh(*ath);

        // convert range readings into ref frame
        // this can be speeded up, by connecting it with the interpolation
        for (i = 0;i < PM_L_POINTS;i++) {
            delta   = *ath + pm_fi[i];
            x       = act.r(i) * cos(delta) + *ax;
            y       = act.r(i) * sin(delta) + *ay;
            r[i]    = sqrt(x * x + y * y);
            fi[i]   = atan2(y, x);
            //      px[i]   = x;
            //      py[i]   = y;
            //       new_r[i]  = 10000;//initialize big interpolated r;
            new_r[i]  = 100000;//initialize big interpolated r;  a mm.
            new_bad[i] = PM_EMPTY;//for interpolated r;
        }

        for (i = 0;i < PM_L_POINTS;i++) {
            new_fi[i] = fi[i] + M_PI/2.0;
        }

        //------------------------INTERPOLATION------------------------
        //calculate/interpolate the associations to the ref scan points
        //algorithm ignores crosings at 0 and 180 -> to make it faster
        for (i = 1;i < PM_L_POINTS;i++) { //i points to the angles in the actual scan

            // i and i-1 has to be in the same segment, both shouldn't be bad
            // and they should be bigger than 0
            if (act.seg(i) != 0 && act.seg(i) == act.seg(i - 1) && !act.bad(i) &&
                    !act.bad(i - 1) && new_fi[i] > 0 && new_fi[i-1] > 0) {
                //calculation of the "whole" parts of the angles
                int     fi0, fi1;
                PM_TYPE r0, r1, a0, a1;

                bool occluded;
                if (new_fi[i] > new_fi[i-1])//are the points visible?
                {//visible
                    occluded = false;
                    a0  = new_fi[i-1];
                    a1  = new_fi[i];
                    fi0 = (int) ceil(new_fi[i-1] * PM_R2D);//fi0 is the meas. angle!
                    fi1 = (int)(new_fi[i] * PM_R2D);
                    r0  = r[i-1];
                    r1  = r[i];
                }
                else {//invisible - still have to calculate to filter out points which
                    occluded = true; //are covered up by these!
                    //flip the points-> easier to program
                    a0  = new_fi[i];
                    a1  = new_fi[i-1];
                    fi0 = (int) ceil(new_fi[i] * PM_R2D);
                    fi1 = (int)(new_fi[i-1] * PM_R2D);
                    r0  = r[i];
                    r1  = r[i-1];
                }
                //here fi0 is always smaller than fi1!

                //interpolate for all the measurement bearings beween fi0 and fi1
                while (fi0 <= fi1) { //if at least one measurement point difference, then ...
                    PM_TYPE ri = (r1 - r0) / (a1 - a0) * (((PM_TYPE)fi0 * PM_D2R) - a0) + r0;

                    //if fi0 -> falls into the measurement range and ri is shorter
                    //than the current range then overwrite it
                    if (fi0 >= 0 && fi0 < PM_L_POINTS && new_r[fi0] > ri) {
                        new_r[fi0]    = ri;//overwrite the previous reading
                        new_bad[fi0] &= ~PM_EMPTY;//clear the empty flag
                        index[fi0]    = i;//store which reading was used at index fi0
                        if (occluded) //check if it was occluded
                            new_bad[fi0] = new_bad[fi0] | PM_OCCLUDED;//set the occluded flag
                        else
                            new_bad[fi0] = new_bad[fi0] & ~PM_OCCLUDED;
                        //the new range reading also it has to inherit the other flags
                        new_bad[fi0] |= act.bad(i);//superfluos - since act.bad[i] was checked for 0
                        new_bad[fi0] |= act.bad(i - 1);//superfluos - since act.bad[i-1] was checked for 0
                        //if(ri>PM_MAX_RANGE)        //uncomment this later
                        //  new_bad[fi0] |= PM_RANGE;
                    }
                    fi0++;//check the next measurement angle!
                }//while
            }//if act
        }//for i

        //    for (i = 0;i < PM_L_POINTS;i++) {
        //      fprintf(fileoutput,"%f %f %f %d ",fi[i],r[i],new_r[i],new_bad[i]);
        //    }

        //---------------ORIENTATION SEARCH-----------------------------------
        //search for angle correction using crosscorrelation

        if (iter % 2 == 0) {
            //pm_fi,ref.r - reference points
            //r,fi        - actual points which are manipulated
            //new_r, new_bad contains the

            PM_TYPE e, err[100];      // the error rating
            PM_TYPE beta[100];// angle for the corresponding error
            //      PM_TYPE C = 10000;
            PM_TYPE n;
            int k = 0;

            dx = 0.0;
            dy = 0.0;
            dth = 0.0;

            for (int di = -window;di <= window;di++) {
                if(!di)
                    n = 0;
                n = 0;
                e = 0;

                int min_i, max_i;
                if (di <= 0) {
                    min_i = -di;
                    max_i = PM_L_POINTS;
                }
                else {
                    min_i = 0;
                    max_i = PM_L_POINTS - di;
                }

                for (i = min_i;i < max_i;i++) { //searching through the actual points
                    //if fi[i],r[i] isn't viewed from the back, isn't moving
                    // and isn't a solitary point, then try to associate it ..
                    //also fi[i] is within the angle range ...

                    if (!new_bad[i] && !ref.bad(i + di)) {
                        e += fabs(new_r[i] - ref.r(i + di));
                        n++;
                    }

                }//for i

                if (n > 0)
                    err[k]  = e / n;//don't forget to correct with n!
                else
                    //           err[k]  = 10000;//don't forget to correct with n!
                    err[k]  = 100000;//don't forget to correct with n!   a mm.
                beta[k] = di;
                k++;
            }//for dfi

            //now search for the global minimum
            //later I can make it more robust
            //assumption: monomodal error function!
            PM_TYPE emin = 1000000;
            int   imin;
            for (i = 0;i < k;i++) {
                if (err[i] < emin) {
                    emin = err[i];
                    imin = i;
                }
            }
            //       if(err[imin]>=10000)
            if (err[imin] >= 100000) { // a mm.
                cerr << "Polar Match: orientation search failed" << err[imin] << endl;
                throw 1;
            }
            dth = beta[imin];

            //interpolation
            if (imin >= 1 && imin < (k - 1)) //is it not on the extreme?
            {//lets try interpolation
                PM_TYPE D = err[imin-1] + err[imin+1] - 2.0 * err[imin];
                PM_TYPE d = 1000;
                if (fabs(D) > 0.1 && err[imin-1] > err[imin] && err[imin+1] > err[imin]) { // 0.1 a mm.
                    d = (err[imin-1] - err[imin+1]);
                    d = (err[imin-1] - err[imin+1]) / D / 2.0;
                }
                if (fabs(d) < 1)
                    dth += d;
            }//if


            (*ath) = (*ath) + dth * M_PI / 180.0;

            logThIteration(*ax,*ay,*ath,dx,dy,dth*M_PI/180.0,-10.0,10.0);

            continue;
        }

        dx = 0.0;
        dy = 0.0;
        dth = 0.0;

        //------------------------------------------translation-------------
        if (iter > 10)
            //        C = 100;
            C = 10000; // a mm.
        // do the weighted linear regression on the linearized ...
        // include angle as well
        PM_TYPE hi1, hi2, hwi1, hwi2, hw1 = 0, hw2 = 0, hwh11 = 0;
        PM_TYPE hwh12 = 0, hwh22 = 0, w; // hwh21 = 0,
        PM_TYPE dr;
        //    abs_err = 0;
        n = 0;

        for (i = 0;i < PM_L_POINTS;i++) {

            dr = ref.r(i) - new_r[i];

            //weight calculation
            if (ref.bad(i) == 0 && new_bad[i] == 0 && new_r[i] < PM_MAX_RANGE && new_r[i] > 100.0 && fabs(dr) < PM_MAX_ERROR) { // 100 a mm.

                //        abs_err += fabs(dr);

                //weighting according to DUDEK00
                w = C / (dr * dr + C);
                //        w = 1.0/fabs(dr);if(w>100)  w = 100;
                n++;

                //proper calculations of the jacobian
                hi1 = pm_co[i];//xx/new_r[i];//this the correct
                hi2 = pm_si[i];//yy/new_r[i];

                hwi1 = hi1 * w;
                hwi2 = hi2 * w;

                //par = (H^t*W*H)^-1*H^t*W*dr
                hw1 += hwi1 * dr;//H^t*W*dr
                hw2 += hwi2 * dr;

                //H^t*W*H
                hwh11 += hwi1 * hi1;
                hwh12 += hwi1 * hi2;
                //        hwh21 += hwi2*hi1; //should take adv. of symmetricity!!
                hwh22 += hwi2 * hi2;

            }//if
        }//for i

        //    cerr << "num points " << n << endl;

        if (n < 2) { //are there enough points?
            // ==ins==    if (n < PM_MIN_VALID_POINTS) { //are there enough points?
            cerr << "pm_linearized_match: ERROR not enough points " << n << endl;
            throw 1;//not enough points
        }

        //calculation of inverse
        PM_TYPE D;//determinant
        PM_TYPE inv11, inv21, inv12, inv22;//inverse matrix

        D = hwh11 * hwh22 - hwh12 * hwh12;
        if (D < 0.001) {
            cerr << "pm_linearized_match: ERROR determinant to small! " << D << endl;
            throw 1;
        }
        inv11 =  hwh22 / D;
        inv12 = -hwh12 / D;
        inv21 = -hwh12 / D;
        inv22 =  hwh11 / D;

        dx = inv11 * hw1 + inv12 * hw2;
        dy = inv21 * hw1 + inv22 * hw2;

        initial_abs_err = getErrorOverN(*ax, *ay, *ath);
        abs_err = getErrorOverN(*ax+dx, *ay+dy, *ath);
        //  Use dx, dy only if error is smaller
        if (abs_err < initial_abs_err)
        {
            (*ax) = (*ax) + dx;
            (*ay) = (*ay) + dy;
        } else {
            dx = 0.0;
            dy = 0.0;
        }
        logXYIteration(*ax,*ay,*ath,dx,dy,dth*M_PI/180.0);

    }
    if(iter >= 30)
        cerr << "Max. num. iter" << endl;
}

void PMSAlgo::matchKalman(PM_TYPE* ax, PM_TYPE* ay, PM_TYPE *ath){

    PM_TYPE   delta;
    PM_TYPE   r[PM_L_POINTS], fi[PM_L_POINTS],new_fi[PM_L_POINTS];//actual scan in ref. coord. syst.
    PM_TYPE   x, y;
    PM_TYPE   new_r[PM_L_POINTS];//interpolated r at measurement bearings
    PM_TYPE   alt_r[PM_L_POINTS];
    int       new_bad[PM_L_POINTS];//bad flags of the interpolated range readings
    int       index[PM_L_POINTS];//which new point was used using which old point..
    PM_TYPE   ref_r[PM_L_POINTS];
    //   bool comparable[PM_L_POINTS];
    PM_TYPE   C = 700 * 700;//weighting factor; see dudek00 a mm
    int       n = 0;//number of valid points
    int       iter, i, small_corr_cnt = 0;
    int       window       = 20;//+- width of search for correct orientation
    PM_TYPE dx = 10, dy = 10, dth = 0;//match error, actual scan corrections
    PM_TYPE   abs_err,initial_abs_err;
    PM_TYPE new_ref[PM_L_POINTS];
    int next_segment[PM_L_POINTS];

    iter = -1;

    double init_ath = *ath;
    x = *ax;
    y = *ay;
    int new_di = 0;
    bool iter_back = false;

    //   projectSegments(*ax, *ay, *ath, r);
    //   double prev_emin = getError2OverN(*ax, *ay, *ath);
    double prev_emin = getError2OverN(*ax, *ay, *ath, fi, new_r, new_ref, new_bad , 0, next_segment);

    if(prev_emin > 5000) {
        cout << "ERROR prev_emin " << prev_emin << endl;
        //     for (int i = 0;i < PM_L_POINTS;i++) {
        // // 	int ref_seg = round(fi[i]/PM_D2R) + 90;
        // 	int ref_seg = (int) ceil(fi[i]/PM_D2R) + 90;
        //
        //       ref_seg = ref_seg < 0 || ref_seg >= PM_L_POINTS ? 0 : ref.seg(ref_seg);
        //
        //       cout << new_bad[i] << " " << ref.r(i) << " " << act.r(i) << " " << new_r[i] << " " << alt_r[i] << " " << ref.seg(i) << " " << ref_seg << " " << act.seg(i) << " " << (int) ceil(fi[i]/PM_D2R) + 90 << " " << i << endl;
        //     }
        //     cout << endl;
    }

    initial_abs_err = prev_emin + 1;

    /*  if(prev_emin > 500) {
    for (int i = 0;i < PM_L_POINTS;i++) {
      cout << new_bad[i] << " " << ref.r(i) << " " << new_r[i] << " " << alt_r[i] << " " << ref.seg(i) << " " << act.seg(i) << " " << i << endl;
    }
    cout << endl;
  } */

    while (++iter < PM_MAX_ITER && small_corr_cnt < 5) { //has to be 5 small corrections before stop

        if ((fabs(dx / 10) + fabs(dy / 10) + fabs(dth)) < 1) //  /10 a mm.
            small_corr_cnt++;
        else
            small_corr_cnt = 0;

        act.setRx(*ax);
        act.setRy(*ay);
        act.setTh(*ath);
        //     projectSegments(*ax, *ay, *ath, fi, new_r, new_ref, new_bad);

        //     interpolation(*ax, *ay, *ath, fi, r, new_r, alt_r, new_bad, index, ref.r());

        double foo = getError2OverN(*ax, *ay, *ath, fi, new_r, new_ref, new_bad, 0, next_segment);
        //     double foo = getError2OverN(*ax, *ay, *ath);
        if(foo >= 100) {
            cout << "ERROR " << foo << endl;
//            for (int i = 0;i < PM_L_POINTS;i++) {
//                int ref_seg = (int) floor(fi[i]/PM_D2R) + next_segment[i];

//                ref_seg = ref_seg < 0 || ref_seg >= PM_L_POINTS ? 0 : ref.seg(ref_seg);
//                cout << new_bad[i] << " " << new_ref[i] << " " << act.r(i) << " " << new_r[i] << " " << ref_seg << " " << act.seg(i) <<  " " << i << endl;
//            }
//            cout << endl;
        }

        //---------------ORIENTATION SEARCH-----------------------------------
        //search for angle correction using crosscorrelation

        //    cout << ref;
        //    cout << endl;
        //    cout << "i\tnew bad\t new r\t alt r" << endl;
        //    for(int j = 0; j < PM_L_POINTS; j++) {
        //      cout << j << "\t   " << new_bad[j] << "\t\t"<< new_r[j] << "\t\t"<< alt_r[j] << endl;
        //    }
        //    cout << endl;


        if (iter % 2 == 0) {
            //pm_fi,ref.r - reference points
            //r,fi        - actual points which are manipulated
            //new_r, new_bad contains the

            PM_TYPE e, err[100];      // the error rating
            PM_TYPE beta[100];// angle for the corresponding error
            //      PM_TYPE C = 10000;
            PM_TYPE n;
            int k = 0;

            dx = 0.0;
            dy = 0.0;
            dth = 0.0;

            //       initial_abs_err = getErrorOverN(*ax, *ay, *ath);
            for (int di = -window;di <= window;di++) {
                err[k] = getError2OverN(*ax, *ay, (*ath) + di * PM_D2R, fi, new_r, new_ref, new_bad, di*PM_D2R, next_segment);
                // 	err[k] = getError2OverN(*ax, *ay, (*ath) + di * PM_D2R);
                beta[k] = di;
                k++;
            }//for dfi
            //       cout << endl;
            //now search for the global minimum
            //later I can make it more robust
            //assumption: monomodal error function!
            //       PM_TYPE emin = 100000;
            PM_TYPE emin = errorMax;
            int   imin = 0;
            for (i = 0;i < k;i++) {
                //    cout << err[i] << " ";
                if (err[i] < emin) {
                    emin = err[i];
                    imin = i;
                }
            }
            //      cout << endl;

            if(prev_emin > 1000) {
                for (i = 0;i < k;i++)
                    cout << err[i] << " ";
                cout << endl;
            }

            //       if(err[imin]>=10000)
            //       if (err[imin] >= 100000) { // a mm.
            if (err[imin] >= errorMax) { // a mm.
                cerr << "Polar Match: orientation search failed" << err[imin] << endl;
                //        throw 1;
                matchOk = false;
                return;
            }

            //       if(emin > prev_emin)
            // 	continue;
            //       else
            // 	prev_emin = emin;

            dth = beta[imin];
            ////////////////////////
            double d = 0;
            if (imin >= 1 && imin < (k - 1)) { //is it not on the extreme?
                double init_error = err[imin] + 1;
                for (double di = -1;di <= 1.01;di += 0.05) {

                    // 	  double di_error = getErrorOverN(*ax, *ay, *ath + (dth + di) * PM_D2R);
                    double di_error = getError2OverN(*ax, *ay, *ath + (dth + di) * PM_D2R, fi, new_r, new_ref, new_bad, di*PM_D2R, next_segment);
                    if(init_error > di_error) {
                        init_error = di_error;
                        d = di;
                    }
                }
                // 	cout << endl;
            }
            dth += d;
            double new_ath = (*ath) + dth * M_PI / 180.0;
            (*ath) = new_ath;
            ///////////////////////////

            //       // interpolation
            //       if (imin >= 1 && imin < (k - 1)) //is it not on the extreme?
            //       {//lets try interpolation
            //         PM_TYPE D = err[imin-1] + err[imin+1] - 2.0 * err[imin];
            //         PM_TYPE d = 1000;
            //         if (fabs(D) > 0.001 && err[imin-1] > err[imin] && err[imin+1] > err[imin]) { // 0.1 a mm.
            //           d = (err[imin-1] - err[imin+1]);
            //           d = (err[imin-1] - err[imin+1]) / D / 2.0;
            //         }
            //         if (fabs(d) < 1)
            //           dth += d;
            // 	else
            // 	  int foo = 0;
            //
            // 	////////////////// aqui estamos
            // // 	double new_ath = (*ath) + dth * M_PI / 180.0;
            // // 	new_di = 0;
            // // 	if(fabs(new_ath - init_ath) >= M_PI / 180.0 && !iter_back) {
            // // 	  new_di = (new_ath - init_ath) * 180/M_PI;
            // // 	  --iter;
            // // 	  dx = 10.0;
            // // 	  dy = 10.0;
            // // 	  (*ath) = init_ath - (new_ath - init_ath) ;
            // // 	  iter_back = true;
            // // 	}
            // // 	else {
            // // 	  iter_back = false;
            // // 	  (*ath) = new_ath;
            // // 	}
            //       }//if
            //
            //       (*ath) = (*ath) + dth * M_PI / 180.0;
            continue;
        }



        dx = 0.0;
        dy = 0.0;
        //     dth = 0.0;

        //------------------------------------------translation-------------
        if (iter > 10)
            //        C = 100;
            C = 10000; // a mm.
        // do the weighted linear regression on the linearized ...
        // include angle as well
        PM_TYPE hi1, hi2, hwi1, hwi2, hw1 = 0, hw2 = 0, hwh11 = 0;
        PM_TYPE hwh12 = 0, hwh22 = 0, w; // hwh21 = 0,
        PM_TYPE dr;
        //    abs_err = 0;
        n = 0;

        for (i = 0;i < PM_L_POINTS;i++) {
            //       double a_ri = fabs(ref.r(i) - new_r[i]) <= fabs(ref.r(i) - alt_r[i]) ? new_r[i]: alt_r[i];
            //       dr = ref.r(i) - a_ri;
            //
            //       //weight calculation
            //       if (ref.bad(i) == 0 && new_bad[i] == 0 && a_ri < sensorMaxVal && a_ri > 100.0 && fabs(dr) < PM_MAX_ERROR) { // 100 a mm.

            //        abs_err += fabs(dr);
            if (!new_bad[i]) {
                dr = new_ref[i] - new_r[i];

                int fi0 = (int) floor(fi[i] * PM_R2D) + next_segment[i];
                int seg_ref = ref.seg(fi0);
                double weight = act.getSigmaEst().at(act.seg(i) - 1) * 1000;
                weight += ref.getSigmaEst().at(seg_ref - 1) * 1000;

                weight = weight < 0.25 ? 0.25 : weight;

                weight *= sqrt(20.0 / (ref.getSegmentsSizes().at(seg_ref - 1) + act.getSegmentsSizes().at(act.seg(i) - 1)));

                //weighting according to DUDEK00
                w = C / (dr * dr / (weight * weight) + C);
                //        w = 1.0/fabs(dr);if(w>100)  w = 100;
                n++;

                //proper calculations of the jacobian
                hi1 = pm_co[i];//xx/new_r[i];//this the correct
                hi2 = pm_si[i];//yy/new_r[i];

                hwi1 = hi1 * w;
                hwi2 = hi2 * w;

                //par = (H^t*W*H)^-1*H^t*W*dr
                hw1 += hwi1 * dr;//H^t*W*dr
                hw2 += hwi2 * dr;

                //H^t*W*H
                hwh11 += hwi1 * hi1;
                hwh12 += hwi1 * hi2;
                //        hwh21 += hwi2*hi1; //should take adv. of symmetricity!!
                hwh22 += hwi2 * hi2;

            }//if
        }//for i

        //    cerr << "num points " << n << endl;
        //     if (n < 2) { //are there enough points?
        if (n < PM_MIN_VALID_POINTS) { //are there enough points?
            cerr << "pm_linearized_match: ERROR not enough points " << n << endl;
            //       throw 1;//not enough points
            matchOk = false;
            return;
        }

        //calculation of inverse
        PM_TYPE D;//determinant
        PM_TYPE inv11, inv21, inv12, inv22;//inverse matrix

        D = hwh11 * hwh22 - hwh12 * hwh12;
        if (D < 0.001) {
            cerr << "pm_linearized_match: ERROR determinant to small! " << D << endl;
            matchOk = false;
            //       throw 1;
            return;
        }
        inv11 =  hwh22 / D;
        inv12 = -hwh12 / D;
        inv21 = -hwh12 / D;
        inv22 =  hwh11 / D;

        dx = inv11 * hw1 + inv12 * hw2;
        dy = inv21 * hw1 + inv22 * hw2;

        //     initial_abs_err = getErrorOverN(*ax, *ay, *ath);
        //     abs_err = getErrorOverN(*ax+dx, *ay+dy, *ath);
        initial_abs_err = getError2OverN(*ax, *ay, *ath, fi, new_r, new_ref, new_bad, 0, next_segment);
        abs_err = getError2OverN((*ax)+dx, (*ay)+dy, *ath, fi, new_r, new_ref, new_bad, 0, next_segment);

        //    if(initial_abs_err >= 100) {
        //      cout << "ERROR " << foo << endl;
        ////      for (int i = 0;i < PM_L_POINTS;i++) {
        ////	int ref_seg = (int) floor(fi[i]/PM_D2R) + next_segment[i];

        ////	ref_seg = ref_seg < 0 || ref_seg >= PM_L_POINTS ? 0 : ref.seg(ref_seg);
        ////	cout << new_bad[i] << " " << new_ref[i] << " " << act.r(i) << " " << new_r[i] << " " << ref_seg << " " << act.seg(i) <<  " " << i << endl;
        ////      }
        ////      cout << endl;
        //    }

        //  Use dx, dy only if error is smaller
        //    cout << "initial_abs_err " << initial_abs_err << "  abs_err " << abs_err << endl;
        if (abs_err < initial_abs_err)
            //     if (abs_err <= prev_emin)
        {
            (*ax) = (*ax) + dx;
            (*ay) = (*ay) + dy;
            //       prev_emin = abs_err;
        } else {
            //       abs_err = getErrorOverN(*ax-dx/2, *ay-dy/2, *ath);
            abs_err = getError2OverN(*ax-dx/2, *ay-dy/2, *ath, fi, new_r, new_ref, new_bad, 0, next_segment);
            //  Use dx, dy only if error is smaller
            //      cout << "initial_abs_err  " << initial_abs_err << "  abs_err 2 " << abs_err << endl;
            if (abs_err < initial_abs_err) {
                (*ax) = (*ax) - dx/2;
                (*ay) = (*ay) - dy/2;
            }
            else {
                dx = 0.0;
                dy = 0.0;
            }
        }
    }

    //     logXYIteration(*ax,*ay,*ath,dx,dy,dth*M_PI/180.0);


    if(iter >= 30) {
        cerr << "Max. num. iter" << endl;
        //     (*ax) = x;
        //     (*ay) = y;
        //     (*ath) = init_ath;
        matchOk = false;
    }
    else {
        if(initial_abs_err >= errorMax) {
            cerr << "Max. error 1e10" << endl;
            matchOk = false;
        }
        else if(initial_abs_err > prev_emin) {
            (*ax) = x;
            (*ay) = y;
            (*ath) = init_ath;
        }
        matchOk = true;
    }

    /*      (*ax) = x;
      (*ay) = y;
      (*ath) = init_ath;    */
}


PM_TYPE PMSAlgo::getErrorOverNIsrael(const PM_TYPE ax, const PM_TYPE ay, const PM_TYPE ath)
{
    PM_TYPE new_r[PM_L_POINTS];//interpolated r at measurement bearings
    int     new_bad[PM_L_POINTS];//bad flags of the interpolated range readings
    int     index[PM_L_POINTS];//which new point was used using which old point..
    PM_TYPE r[PM_L_POINTS], fi[PM_L_POINTS];//actual scan in ref. coord. syst.

    int n=0;
    PM_TYPE e=0.0;
    no_evaluations = no_evaluations + 1;

#ifdef COUT_DEBUG
    cout << "PMSAlgo::getErrorOverN:: ax = " << ax << " ay = " << ay << " ath = " << ath << endl;
#endif

    interpolation(ax,ay,ath,fi,r,new_r,new_bad,index);

    for (int i = 0;i < PM_L_POINTS;i++) {
        if (!new_bad[i] && !ref.bad(i)) {
            e += fabs(new_r[i] - ref.r(i));
            n++;
        }
    }
    if (n > 0)
        e  = e / n;//don't forget to correct with n!
    else
        e  = 100000;

#ifdef COUT_DEBUG
    cout << "PMSAlgo::getErrorOverN:: e = " << e << endl;
#endif

    return(e);
}

PM_TYPE PMSAlgo::getErrorOverN(const PM_TYPE ax, const PM_TYPE ay, const PM_TYPE ath)
{
    PM_TYPE new_r[PM_L_POINTS];//interpolated r at measurement bearings
    int     new_bad[PM_L_POINTS];//bad flags of the interpolated range readings
    int     index[PM_L_POINTS];//which new point was used using which old point..
    PM_TYPE r[PM_L_POINTS], fi[PM_L_POINTS];//actual scan in ref. coord. syst.

    PM_TYPE   alt_r[PM_L_POINTS];

    int n=0;
    PM_TYPE e=0.0;
    no_evaluations = no_evaluations + 1;

#ifdef COUT_DEBUG
    cout << "PMSAlgo::getErrorOverN:: ax = " << ax << " ay = " << ay << " ath = " << ath << endl;
#endif

    //   interpolation(ax,ay,ath,fi,r,new_r,alt_r,new_bad,index);

    for (int i = 0;i < PM_L_POINTS;i++) {
        if (!new_bad[i] && !ref.bad(i)) {

            double n_r = new_r[i];
            double a_r = alt_r[i];
            double weight = ref.r(i) < 1 ? 1 : ref.r(i);
            double weight_d = new_r[i] < 1 ? 1 : new_r[i];
            double weight_a = alt_r[i] < 1 ? 1 : alt_r[i];
            double d_e = fabs(new_r[i] - ref.r(i)); /* / weight_d*/
            double a_e = fabs(alt_r[i] - ref.r(i)); /* / weight_a*/
            e += fmin(d_e, a_e);

            //       e += fabs(new_r[i] - ref.r(i));
            n++;
        }
    }
    if (n > 0)
        e  = e / n;//don't forget to correct with n!
    else
        e  = 100000;

#ifdef COUT_DEBUG
    cout << "PMSAlgo::getErrorOverN:: e = " << e << endl;
#endif

    return(e);
}

PM_TYPE PMSAlgo::getError2OverN(const PM_TYPE ax, const PM_TYPE ay, const PM_TYPE ath, PM_TYPE* fi, PM_TYPE* new_r, PM_TYPE* new_ref, int* new_bad, const PM_TYPE delta_th, int* next_segment)
{

    int n=0;
    PM_TYPE e=0.0;
    no_evaluations = no_evaluations + 1;

#ifdef COUT_DEBUG
    cout << "PMSAlgo::getError2OverN:: ax = " << ax << " ay = " << ay << " ath = " << ath << endl;
#endif

    projectSegments(ax, ay, ath, fi, new_r, new_ref, new_bad, delta_th, next_segment);

    for (int i = 0;i < PM_L_POINTS;i++) {
        if (!new_bad[i]) {
            if(i == 65)
                int foo = 0;
            double n_r = new_r[i];
            double n_ref = new_ref[i];
            int fi0 = (int) floor(fi[i] * PM_R2D) + next_segment[i];
            int seg_ref = ref.seg(fi0);
            double weight = act.getSigmaEst().at(act.seg(i) - 1) * 1000;
            weight += ref.getSigmaEst().at(seg_ref - 1) * 1000;

            weight = weight < 0.25 ? 0.25 : weight;

            weight *= sqrt(20.0 / (ref.getSegmentsSizes().at(seg_ref - 1) + act.getSegmentsSizes().at(act.seg(i) - 1)));

            double d_e = (new_r[i] - new_ref[i])*(new_r[i] - new_ref[i])/ (weight * weight);
            if(d_e > 100000)
                int foo = 0;
            e += d_e;

            //       e += fabs(new_r[i] - ref.r(i));
            n++;
        }
    }
    if (n > 0)
        e  = e / n;//don't forget to correct with n!

    else {
        //     e  = 100000;
        e  = errorMax;
        cout << "ERROR no points " << e << endl;
        //     for (int i = 0;i < PM_L_POINTS;i++) {
        // 	cout << new_bad[i] << " " << ref.r(i) << " " << act.r(i) << " " << new_r[i] << " " << alt_r[i] << " " << ref.seg(i) << " " << act.seg(i) << " " << fi[i]/PM_D2R<< " " << i << endl;
        //     }
        cout << endl;
    }

#ifdef COUT_DEBUG
    cout << "PMSAlgo::getError2OverN:: e2 = " << e << endl;
#endif

    return(e);
}

PM_TYPE PMSAlgo::getError2OverN(const PM_TYPE ax, const PM_TYPE ay, const PM_TYPE ath)
{
    PM_TYPE new_r[PM_L_POINTS];//interpolated r at measurement bearings
    int     new_bad[PM_L_POINTS];//bad flags of the interpolated range readings
    int     index[PM_L_POINTS];//which new point was used using which old point..
    PM_TYPE r[PM_L_POINTS], fi[PM_L_POINTS];//actual scan in ref. coord. syst.

    PM_TYPE   alt_r[PM_L_POINTS];
    //   bool comparable[PM_L_POINTS];

    int n=0;
    PM_TYPE e=0.0;
    no_evaluations = no_evaluations + 1;

#ifdef COUT_DEBUG
    cout << "PMSAlgo::getError2OverN:: ax = " << ax << " ay = " << ay << " ath = " << ath << endl;
#endif

    interpolation(ax, ay, ath, fi, r, new_r, alt_r, new_bad, index, ref.r());
    //   for (int i = 0;i < PM_L_POINTS;i++) {
    //     cout << new_bad[i] << " " << ref.r(i) << " " << new_r[i] << " " << alt_r[i] << " " << " " << i << endl;
    //   }
    //   cout << endl;

    for (int i = 0;i < PM_L_POINTS;i++) {
        if (!new_bad[i] && !ref.bad(i) && ref.seg(i) && act.seg(i)) {
            if(i > 0) {
                if(ref.seg(i) != ref.seg(i-1) || act.seg(i) != act.seg(i-1))
                    continue;
            }
            if(i < PM_L_POINTS -1) {
                if(ref.seg(i) != ref.seg(i+1) || act.seg(i) != act.seg(i+1))
                    continue;
            }

            double n_r = new_r[i];
            double a_r = alt_r[i];
            //       double weight = ref.r(i) < 1 ? 1 : ref.r(i);
            double weight = ref.getSigmaEst().at(ref.seg(i) - 1) * 1000;
            //       weight *= weight;
            /*      double weight = 1;*/
            double weight_d = new_r[i] < 1 ? 1 : new_r[i];
            double weight_a = alt_r[i] < 1 ? 1 : alt_r[i];
            double d_e = (new_r[i] - ref.r(i))*(new_r[i] - ref.r(i))/ weight;/* / weight_d*/
            double a_e = (alt_r[i] - ref.r(i))*(alt_r[i] - ref.r(i))/ weight;/* / weight_a*/
            e += fmin(d_e, a_e);

            //       e += fabs(new_r[i] - ref.r(i));
            n++;
        }
    }
    if (n > 0) {
        e  = e / n;//don't forget to correct with n!
    }
    else {
        //     e  = 100000;
        e  = errorMax;
        cout << "ERROR no points " << e << endl;
        //     for (int i = 0;i < PM_L_POINTS;i++) {
        // 	cout << new_bad[i] << " " << ref.r(i) << " " << act.r(i) << " " << new_r[i] << " " << alt_r[i] << " " << ref.seg(i) << " " << act.seg(i) << " " << fi[i]/PM_D2R<< " " << i << endl;
        //     }
        //    cout << endl;
    }

#ifdef COUT_DEBUG
    cout << "PMSAlgo::getError2OverN:: e2 = " << e << endl;
#endif

    return(e);
}

void PMSAlgo::totalsearch(const PM_TYPE ax, const PM_TYPE ay, const PM_TYPE ath, PM_TYPE *dx, PM_TYPE *dy, PM_TYPE *dth){

    // do the weighted linear regression on the linearized ...
    // include angle as well
    PM_TYPE w, hi1, hi2, hi3, hwi1, hwi2, hwi3, hw1 = 0, hw2 = 0, hw3 = 0;
    PM_TYPE  hwh11 = 0.0, hwh12 = 0.0, hwh13 = 0.0, hwh22 = 0.0, hwh23 = 0.0, hwh33 = 0.0;
    PM_TYPE hwh_matrix[3][3], inverse_hwh_matrix[3][3];
    PM_TYPE h_matrix[PM_L_POINTS][3], dr;
    PM_TYPE new_r[PM_L_POINTS];//interpolated r at measurement bearings
    int     new_bad[PM_L_POINTS];//bad flags of the interpolated range readings
    PM_TYPE C = 30 * 30;
    int n = 0;

    getCompleteNumericalH(ax,ay,ath,h_matrix);

    for (int i = 0;i < PM_L_POINTS;i++) {
        dr = ref.r(i) - new_r[i];

        //weight calculation
        if (ref.bad(i) == 0 && new_bad[i] == 0 && new_r[i] < sensorMaxVal && new_r[i] > 100.0 && fabs(dr) < PM_MAX_ERROR) { // 100 a mm.

            w = C / (dr * dr + C);
            // w = 1.0;

            n++;

            //proper calculations of the jacobian
            hi1 = h_matrix[i][0];
            hi2 = h_matrix[i][1];
            hi3 = h_matrix[i][2];

            hwi1 = hi1 * w;
            hwi2 = hi2 * w;
            hwi3 = hi3 * w;

            //par = (H^t*W*H)^-1*H^t*W*dr
            hw1 += hwi1 * dr;//H^t*W*dr
            hw2 += hwi2 * dr;
            hw3 += hwi3 * dr;

            //H^t*W*H
            hwh11 += hwi1 * hi1;
            hwh12 += hwi1 * hi2;
            hwh13 += hwi1 * hi3;
            //        hwh21 ->  Take adv. of symmetry!!
            hwh22 += hwi2 * hi2;
            hwh23 += hwi2 * hi3;
            //        hwh31  ->  Take adv. of symmetry!
            //        hwh32  ->  Take adv. of symmetry!
            hwh33 += hwi3 * hi3;

        }//if
    }//for i

    cerr << "num points " << n << endl;

    // ==ins==    if (n < PM_MIN_VALID_POINTS) { //are there enough points?
    if (n < 2) { //are there enough points?
        cerr << "PMSAlgo::totalsearch: ERROR not enough points " << n << endl;
        throw 1;//not enough points
    }

    //calculation of inverse
    hwh_matrix[0][0] = hwh11;
    hwh_matrix[0][1] = hwh12;
    hwh_matrix[0][2] = hwh13;
    hwh_matrix[1][0] = hwh12;
    hwh_matrix[1][1] = hwh22;
    hwh_matrix[1][2] = hwh23;
    hwh_matrix[2][0] = hwh13;
    hwh_matrix[2][1] = hwh23;
    hwh_matrix[2][2] = hwh33;

    getInverseMatrix3by3(hwh_matrix, inverse_hwh_matrix);

    *dx  = inverse_hwh_matrix[0][0] * hw1 + inverse_hwh_matrix[0][1] * hw2 + inverse_hwh_matrix[0][2] * hw3;
    *dy  = inverse_hwh_matrix[1][0] * hw1 + inverse_hwh_matrix[1][1] * hw2 + inverse_hwh_matrix[1][2] * hw3;
    *dth = inverse_hwh_matrix[2][0] * hw1 + inverse_hwh_matrix[2][1] * hw2 + inverse_hwh_matrix[2][2] * hw3;

    return;
}

void PMSAlgo::gradientBasedSearch(const PM_TYPE ax, const PM_TYPE ay, const PM_TYPE ath, PM_TYPE *dx, PM_TYPE *dy, PM_TYPE *dth){

    PM_TYPE abs_err = 0.0; // to be deleted

    // Get dx, dy and dth using external library
    lbfgsfloatval_t fx;
    lbfgsfloatval_t *m_x = lbfgs_malloc(3);

    if (m_x == NULL) {
        cerr << "PMSAlgo::gradientBasedSearch: lbfgs_malloc failed to allocate a memory block for variables " << endl;
        throw;
    }

    // Initialize the variables
    m_x[0] = ax;
    m_x[1] = ay;
    m_x[2] = ath;

    // Start the L-BFGS optimization; this will invoke the callback functions
    // evaluate() and progress() when necessary.

    return_result = lbfgs(3, m_x, &fx, _evaluate, _progress, this, NULL);

    // Report the result

#ifdef COUT_DEBUG
    cout << "PMSAlgo::gradientBasedSearch: L-BFGS optimization terminated with status code =  " << return_result << endl;
    cout << "PMSAlgo::gradientBasedSearch: fx = " << fx << " m_x[0] = " << m_x[0] << " m_x[1] = " << m_x[1] << " m_x[2] = " << m_x[2] << endl;
#endif

    // Save the results
    abs_err = getErrorOverN(m_x[0], m_x[1], m_x[2]); // to be deleted

#ifdef COUT_DEBUG
    cout << "PMSAlgo::gradientBasedSearch: abs_err = " << abs_err << endl;
#endif

    *dx = m_x[0] - ax;
    *dy = m_x[1] - ay;
    *dth = m_x[2] - ath;

#ifdef COUT_DEBUG
    cout << "PMSAlgo::gradientBasedSearch: dx = " <<  *dx << " dy = " <<  *dy << " dth = " <<  *dth << endl;
#endif

    return;
}

void PMSAlgo::nloptBasedSearch(const PM_TYPE ax, const PM_TYPE ay, const PM_TYPE ath, PM_TYPE *dx, PM_TYPE *dy, PM_TYPE *dth){

    nlopt_opt opt;

    //  PM_TYPE lb[3] = {-HUGE_VAL,-HUGE_VAL,-HUGE_VAL};
    //  PM_TYPE ub[3] = {HUGE_VAL,HUGE_VAL,HUGE_VAL};
    //  PM_TYPE lb[3] = {-1000.0,-1000.0,-M_PI};
    //  PM_TYPE ub[3] = {1000.0,1000.0,M_PI};
    PM_TYPE lb[3] = {ax-100.0,ay-100.0,ath-30.0*M_PI/180.0}; // Previosly 250.0 & 0.25*M_PI
    PM_TYPE ub[3] = {ax+100.0,ay+100.0,ath+30.0*M_PI/180.0}; // Previosly 250.0 & 0.25*M_PI
    PM_TYPE minf,x[3]= {ax,ay,ath};

    // Init opt structure
    // Algotihm types:
    // *Global optimization
    //   Direct-L: NLOPT_GN_DIRECT_L (ok)
    //             NLOPT_GN_DIRECT_NOSCAL (ko)
    //             NLOPT_GN_DIRECT_L_NOSCAL (ko)
    //   NLOPT_GN_CRS2_LM (ok)
    //   NLOPT_GN_MLSL (ok - tbd)
    //   NLOPT_GD_MLSL (ok - tbd)
    //   NLOPT_GN_MLSL_LDS (ok - tbd)
    //   NLOPT_GD_MLSL_LDS (ok)
    //   NLOPT_GD_STOGO (ko need c++)
    //   NLOPT_GD_STOGO_RAND (ko need c++)
    //   NLOPT_GN_ISRES (ok)
    // *Local - No Gradient
    //   NLOPT_LN_COBYLA (ko)
    //   NLOPT_LN_BOBYQA (ko - ok)
    //   NLOPT_LN_SBPLX (ko)
    // *Local - Gradient Based
    //   NLOPT_LD_MMA (ko)
    //   NLOPT_LD_SLSQP (ko - ok)
    //   Low-storage BFGS: NLOPT_LD_LBFGS (ko)
    //   NLOPT_LD_TNEWTON (ko)
    //   NLOPT_LD_TNEWTON_RESTART (ko)
    //   NLOPT_LD_VAR2 (ko - ok)

    opt = nlopt_create(nlopt_method,3);
    if (opt == NULL){
        cerr << "PMSAlgo::nloptBasedSearch: nlopt_create FAILED! " << endl;
    }

    // Set lower & upper bounds for design variables
    nlopt_set_lower_bounds(opt,lb);
    nlopt_set_upper_bounds(opt,ub);

    // Set objective function
    nlopt_set_min_objective(opt,_nlopt_eval,this);

    // Set stopping criteria
    nlopt_set_xtol_rel(opt,1.0e-2); // To be revised
    nlopt_set_stopval(opt,1.0);
    nlopt_set_maxeval(opt,100000);

    if (nlopt_method == NLOPT_GN_MLSL || nlopt_method == NLOPT_GD_MLSL || nlopt_method == NLOPT_GN_MLSL_LDS || nlopt_method == NLOPT_GD_MLSL_LDS )
    {

        nlopt_opt opt_local = nlopt_create(NLOPT_LD_SLSQP,3);
        if (opt_local == NULL){
            cerr << "PMSAlgo::nloptBasedSearch=local=: nlopt_create FAILED! " << endl;
        }

        // Set stopping criteria
        nlopt_set_xtol_rel(opt_local,1.0e-2); // To be revised
        nlopt_set_stopval(opt_local,1.0);
        nlopt_set_maxeval(opt_local,1000);

        // MLSL
        nlopt_set_local_optimizer(opt,opt_local);
        nlopt_destroy(opt_local);
    }

    // Set initial step size (optional)
    //nlopt_set_initial_step(opt,{100.0,100.0,03})

    // Apply optimization algorithm
    //
    // Successful return values:
    // NLOPT_SUCCESS = 1 Generic success return value.
    // NLOPT_STOPVAL_REACHED = 2 Optimization stopped because stopval (above) was reached.
    // NLOPT_FTOL_REACHED = 3 Optimization stopped because ftol_rel or ftol_abs (above) was reached.
    // NLOPT_XTOL_REACHED = 4 Optimization stopped because xtol_rel or xtol_abs (above) was reached.
    // NLOPT_MAXEVAL_REACHED = 5 Optimization stopped because maxeval (above) was reached.
    // NLOPT_MAXTIME_REACHED = 6 Optimization stopped because maxtime (above) was reached.
    // Error codes (negative return values)
    // NLOPT_FAILURE = -1 Generic failure code.
    // GN_D>IERENLOPT_INVALID_ARGS = -2 Invalid arguments (e.g. lower bounds are bigger than upper bounds, an unknown algorithm was specified, etcetera).
    // NLOPT_OUT_OF_MEMORY = -3 Ran out of memory.
    // NLOPT_ROUNDOFF_LIMITED = -4 Halted because roundoff errors limited progress. (In this case, the optimization still typically returns a useful result.)
    // NLOPT_FORCED_STOP = -5 Halted because of a forced termination

    return_result = nlopt_optimize(opt,x,&minf);
    if (return_result < 0){
        // Report the failed result

#ifdef COUT_DEBUG
        cout << "PMSAlgo::nloptBasedSearch: optimization failed with status code =  " << return_result << endl;
        cout << "PMSAlgo::nloptBasedSearch: (KO?) minf = " << minf << " x[0] = " << x[0] << " x[1] = " << x[1] << " x[2] = " << x[2] << endl;
#endif

    } else {

#ifdef COUT_DEBUG
        cout << "PMSAlgo::nloptBasedSearch: succeeded with status code =  " << return_result << endl;
        cout << "PMSAlgo::nloptBasedSearch: (OK) minf = " << minf << " x[0] = " << x[0] << " x[1] = " << x[1] << " x[2] = " << x[2] << endl;
#endif

    }

    *dx = x[0] - ax;
    *dy = x[1] - ay;
    *dth = x[2] - ath;

#ifdef COUT_DEBUG
    cout << "PMSAlgo::nloptBasedSearch: dx = " <<  *dx << " dy = " <<  *dy << " dth (rad) = " <<  *dth << endl;
#endif

    nlopt_destroy(opt);


    if (nlopt_refinement)
    {

        nlopt_opt opt2;
        int ret2;
        PM_TYPE ax2 = ax+*dx;
        PM_TYPE ay2 = ay+*dy;
        PM_TYPE ath2 = ath+*dth;
        PM_TYPE lb2[3] = {ax2-100.0,ay2-100.0,ath2-0.05*M_PI};
        PM_TYPE ub2[3] = {ax2+100.0,ay2+100.0,ath2+0.05*M_PI};
        PM_TYPE minf2,x2[3]= {ax2,ay2,ath2};

        opt2 = nlopt_create(nlopt_refinement_method,3);
        if (opt2 == NULL){
            cerr << "PMSAlgo::nloptBasedSearch2: nlopt_create FAILED! " << endl;
        }

        // Set lower & upper bounds for design variables
        nlopt_set_lower_bounds(opt2,lb2);
        nlopt_set_upper_bounds(opt2,ub2);

        // Set objective function
        nlopt_set_min_objective(opt2,_nlopt_eval,this);

        // Set stopping criteria
        nlopt_set_xtol_rel(opt2,1.0e-3); // To be revised
        nlopt_set_stopval(opt2,1.0);
        nlopt_set_maxeval(opt2,500);

        // Set initial step size (optional)
        //nlopt_set_initial_step(opt2,{100.0,100.0,03})

        ret2 = nlopt_optimize(opt2,x2,&minf2);
        if (ret2 < 0){
            // Report the failed result
#ifdef COUT_DEBUG
            cout << "PMSAlgo::nloptBasedSearch2: optimization failed with status code =  " << ret2 << endl;
            cout << "PMSAlgo::nloptBasedSearch2: (KO?) minf2 = " << minf2 << " x2[0] = " << x2[0] << " x2[1] = " << x2[1] << " x2[2] = " << x2[2] << endl;
#endif
        }else{
#ifdef COUT_DEBUG
            cout << "PMSAlgo::nloptBasedSearch2: succeeded with status code =  " << ret2 << endl;
            cout << "PMSAlgo::nloptBasedSearch2: (OK) minf2 = " << minf2 << " x2[0] = " << x2[0] << " x2[1] = " << x2[1] << " x2[2] = " << x2[2] << endl;
#endif
        }

        *dx = *dx + (x2[0] - ax2);
        *dy = *dy + (x2[1] - ay2);
        *dth = *dth * (x2[2] - ath2);

#ifdef COUT_DEBUG
        cout << "PMSAlgo::nloptBasedSearch2: dx = " <<  *dx << " dy = " <<  *dy << " dth(rad) = " <<  *dth << endl;
#endif

        nlopt_destroy(opt2);

    }

    return;

}

void PMSAlgo::nloptBasedSearch_XYonly(const PM_TYPE ax, const PM_TYPE ay, PM_TYPE *dx, PM_TYPE *dy){

    nlopt_opt opt;

    PM_TYPE lb[2] = {ax-100.0,ay-100.0}; // Previously 250.0 & 0.25*M_PI
    PM_TYPE ub[2] = {ax+100.0,ay+100.0}; // Previously 250.0 & 0.25*M_PI
    PM_TYPE minf,x[2]= {ax,ay};


    opt = nlopt_create(nlopt_method,2);
    if (opt == NULL){
        cerr << "PMSAlgo::nloptBasedSearch_XYonly: nlopt_create FAILED! " << endl;
    }

    // Set lower & upper bounds for design variables
    nlopt_set_lower_bounds(opt,lb);
    nlopt_set_upper_bounds(opt,ub);

    // Set objective function
    nlopt_set_min_objective(opt,_nlopt_eval,this);

    // Set stopping criteria
    nlopt_set_xtol_rel(opt,1.0e-2); // To be revised
    nlopt_set_stopval(opt,1.0);
    nlopt_set_maxeval(opt,100000);

    if (nlopt_method == NLOPT_GN_MLSL || nlopt_method == NLOPT_GD_MLSL || nlopt_method == NLOPT_GN_MLSL_LDS || nlopt_method == NLOPT_GD_MLSL_LDS )
    {

        nlopt_opt opt_local = nlopt_create(NLOPT_LD_SLSQP,3);
        if (opt_local == NULL){
            cerr << "PMSAlgo::nloptBasedSearch=local=: nlopt_create FAILED! " << endl;
        }

        // Set stopping criteria
        nlopt_set_xtol_rel(opt_local,1.0e-2); // To be revised
        nlopt_set_stopval(opt_local,1.0);
        nlopt_set_maxeval(opt_local,1000);

        // MLSL
        nlopt_set_local_optimizer(opt,opt_local);
        nlopt_destroy(opt_local);
    }

    // Apply optimization algorithm
    return_result = nlopt_optimize(opt,x,&minf);
    if (return_result < 0){
        // Report the failed result

#ifdef COUT_DEBUG
        cout << "PMSAlgo::nloptBasedSearch_XYonly: optimization failed with status code =  " << return_result << endl;
        cout << "PMSAlgo::nloptBasedSearch_XYonly: (KO?) minf = " << minf << " x[0] = " << x[0] << " x[1] = " << x[1] << endl;
#endif

    } else {

#ifdef COUT_DEBUG
        cout << "PMSAlgo::nloptBasedSearch_XYonly: succeeded with status code =  " << return_result << endl;
        cout << "PMSAlgo::nloptBasedSearch_XYonly: (OK) minf = " << minf << " x[0] = " << x[0] << " x[1] = " << x[1] << endl;
#endif

    }

    *dx = x[0] - ax;
    *dy = x[1] - ay;

#ifdef COUT_DEBUG
    cout << "PMSAlgo::nloptBasedSearch_XYonly: dx = " <<  *dx << " dy = " <<  *dy << endl;
#endif

    nlopt_destroy(opt);

    return;

}

lbfgsfloatval_t PMSAlgo::evaluate(const lbfgsfloatval_t *x, lbfgsfloatval_t *g, const int n, const lbfgsfloatval_t step)
{
    lbfgsfloatval_t fx = 0.0;
    PM_TYPE gx, gy, gth;

    // x[0] --> ax
    // x[1] --> ay
    // x[2] --> ath

    act.setRx(x[0]);
    act.setRy(x[1]);
    act.setTh(x[2]);

    getGradient(x[0],x[1],x[2],&gx,&gy,&gth);

#ifdef COUT_DEBUG
    cout << "PMSAlgo::evaluate: gx = " << gx << " gy = " << gy  << " gth = " << gth  << endl;
#endif

    g[0] = gx;
    g[1] = gy;
    g[2] = gth;

    fx = getErrorOverN(x[0], x[1], x[2]);

#ifdef COUT_DEBUG
    cout << "PMSAlgo::evaluate: fx = " << fx << " x[0] =  " << x[0] << " x[1] = " << x[1]  << " x[2] = " << x[2]  << endl;
    cout << "PMSAlgo::evaluate: g[0] = " << g[0] << " g[1] = " << g[1]  << " g[2] = " << g[2]  << endl;
#endif

    logEvaluations(x,g,fx);

    return fx;
}

int PMSAlgo::progress(const lbfgsfloatval_t *x, const lbfgsfloatval_t *g, const lbfgsfloatval_t fx, const lbfgsfloatval_t xnorm,
                      const lbfgsfloatval_t gnorm, const lbfgsfloatval_t step, int n, int k, int ls)
{

#ifdef COUT_DEBUG
    cout << "PMSAlgo::progress: L-BFGS optimization -- iteration =  " << k << " fx = " << fx << " x[0] =  " << x[0] << " x[1] = " << x[1]  << " x[2] = " << x[2]  << endl;
    cout << "PMSAlgo::progress: L-BFGS optimization -- iteration =  " << k << " xnorm  = " << xnorm << " gnorm =  " << gnorm << " step = " << step  << endl;
#endif

    logIteration(x[0], x[1], x[2], 0.0, 0.0, 0.0);

    return 0;
}

double PMSAlgo::nlopt_eval(unsigned n, const double *x, double *grad)
{
    PM_TYPE feval;
    // x[0] --> ax
    // x[1] --> ay
    // x[2] --> ath

    act.setRx(x[0]);
    act.setRy(x[1]);
    act.setTh(x[2]);

    if (grad) {
        PM_TYPE gx, gy, gth;
        getGradient(x[0],x[1],x[2],&gx,&gy,&gth);

#ifdef COUT_DEBUG
        cout << "PMSAlgo::nlopt_eval: gx = " << gx << " gy = " << gy  << " gth = " << gth  << endl;
#endif

        grad[0] = gx;
        grad[1] = gy;
        grad[2] = gth;
    }

    feval = getErrorOverN(x[0], x[1], x[2]);

#ifdef COUT_DEBUG
    cout << "PMSAlgo::nlopt_eval: feval = " << feval << " n = " << n << " x[0] =  " << x[0] << " x[1] = " << x[1]  << " x[2] = " << x[2]  << endl;
    if (grad) {
        cout << "PMSAlgo::nlopt_eval: grad[0] = " << grad[0] << " grad[1] = " << grad[1]  << " grad[2] = " << grad[2]  << endl;
    }
#endif

    logEvaluations(x,grad,feval);

    return feval;
}

void PMSAlgo::getGradient(const PM_TYPE ax, const PM_TYPE ay, const PM_TYPE ath, PM_TYPE *gx, PM_TYPE *gy, PM_TYPE *gth)
{

    PM_TYPE delta_xc = 1.0, delta_yc = 1.0, delta_thc = 0.01*M_PI/180.0;
    PM_TYPE abs_err_plus, abs_err_minus;

#ifdef COUT_DEBUG
    cout << "PMSAlgo::getGradient: ax = " << ax << " ay = " << ay  << " ath = " << ath  << endl;
#endif

    // xc + delta AND yc AND theta_c
    abs_err_plus = getErrorOverN(ax + delta_xc, ay, ath);

    // xc - delta AND yc AND theta_c
    abs_err_minus = getErrorOverN(ax - delta_xc, ay, ath);

    *gx = (abs_err_plus - abs_err_minus) / (2.0*delta_xc);

#ifdef COUT_DEBUG
    cout << "PMSAlgo::getGradient: *gx = " << *gx << " abs_err_plus = " << abs_err_plus  << " abs_err_minus = " << abs_err_minus  << endl;
#endif

    // xc AND yc + delta AND theta_c
    abs_err_plus = getErrorOverN(ax, ay + delta_yc, ath);

    // xc AND yc - delta AND theta_c
    abs_err_minus = getErrorOverN(ax, ay - delta_yc, ath);

    *gy = (abs_err_plus - abs_err_minus) / (2.0*delta_yc);

#ifdef COUT_DEBUG
    cout << "PMSAlgo::getGradient: *gy = " << *gy << " abs_err_plus = " << abs_err_plus  << " abs_err_minus = " << abs_err_minus  << endl;
#endif

    // xc AND yc AND theta_c + delta
    abs_err_plus = getErrorOverN(ax, ay, ath + delta_thc);

    // xc AND yc AND theta_c - delta
    abs_err_minus = getErrorOverN(ax, ay, ath - delta_thc);

    *gth = (abs_err_plus - abs_err_minus) / (2.0*delta_thc);

#ifdef COUT_DEBUG
    cout << "PMSAlgo::getGradient: *gth = " << *gth << " abs_err_plus = " << abs_err_plus  << " abs_err_minus = " << abs_err_minus  << endl;
#endif

    return;

}

int PMSAlgo::getInverseMatrix3by3(const PM_TYPE a_matrix[][3], PM_TYPE a_matrix_inverse[][3])
{

    PM_TYPE determinant = a_matrix[0][0]*(a_matrix[2][2]*a_matrix[1][1]-a_matrix[2][1]*a_matrix[1][2])
            -a_matrix[1][0]*(a_matrix[2][2]*a_matrix[0][1]-a_matrix[2][1]*a_matrix[0][2])
            +a_matrix[2][0]*(a_matrix[1][2]*a_matrix[0][1]-a_matrix[1][1]*a_matrix[0][2]);

    if (determinant < 0.001) {
        cerr << "PMSAlgo::getInverseMatrix3by3(): ERROR determinant to small! " << determinant << endl;
        //      return(-1);
    }

    a_matrix_inverse[0][0] = (1.0/determinant)*(a_matrix[2][2]*a_matrix[1][1]-a_matrix[2][1]*a_matrix[1][2]);
    a_matrix_inverse[0][1] = -(1.0/determinant)*(a_matrix[2][2]*a_matrix[0][1]-a_matrix[2][1]*a_matrix[0][2]);
    a_matrix_inverse[0][2] = (1.0/determinant)*(a_matrix[1][2]*a_matrix[0][1]-a_matrix[1][1]*a_matrix[0][2]);
    a_matrix_inverse[1][0] = -(1.0/determinant)*(a_matrix[2][2]*a_matrix[1][0]-a_matrix[2][0]*a_matrix[1][2]);
    a_matrix_inverse[1][1] = (1.0/determinant)*(a_matrix[2][2]*a_matrix[0][0]-a_matrix[2][0]*a_matrix[0][2]);
    a_matrix_inverse[1][2] = -(1.0/determinant)*(a_matrix[1][2]*a_matrix[0][0]-a_matrix[1][0]*a_matrix[0][2]);
    a_matrix_inverse[2][0] = (1.0/determinant)*(a_matrix[2][1]*a_matrix[1][0]-a_matrix[2][0]*a_matrix[1][1]);
    a_matrix_inverse[2][1] = -(1.0/determinant)*(a_matrix[2][1]*a_matrix[0][0]-a_matrix[2][0]*a_matrix[0][1]);
    a_matrix_inverse[2][2] = (1.0/determinant)*(a_matrix[1][1]*a_matrix[0][0]-a_matrix[1][0]*a_matrix[0][1]);

    return(0);
}

void PMSAlgo::calculateDisplacementsRefFrame(PM_TYPE* ax, PM_TYPE* ay, PM_TYPE *ath)
{
    PM_TYPE rx, ry, rth, t13, t23, LASER_Y = PM_LASER_Y;

    rx = ref.rx();
    ry = ref.ry();
    rth = ref.th();
    *ax =  act.rx();
    *ay = act.ry();
    *ath = act.th();

    //transformation of actual scan laser scanner coordinates into reference
    //laser scanner coordinates
    //  t13 = sin(rth - *ath) * LASER_Y + cos(rth) * (*ax - rx) + sin(rth) * (*ay - ry);
    //  t23 = cos(rth - *ath) * LASER_Y - sin(rth) * (*ax - rx) + cos(rth) * (*ay - ry) - LASER_Y;
    t13 = cos(rth - *ath) * LASER_Y + cos(rth) * (*ax - rx) + sin(rth) * (*ay - ry)- LASER_Y;
    t23 = -sin(rth - *ath) * LASER_Y - sin(rth) * (*ax - rx) + cos(rth) * (*ay - ry);

    ref.setRx(0);
    ref.setRy(0);
    ref.setTh(0);
    act.setRx(t13);
    act.setRy(t23);

    double ang = *ath - rth;
    if(ang > M_PI)
        ang -= 2*M_PI;
    else if(ang <= -M_PI)
        ang += 2*M_PI;
    act.setTh(ang);

    *ax = act.rx();
    *ay = act.ry();
    *ath = act.th();

}

void PMSAlgo::initLogPsmalgo(){
    fileoutput = fopen("./pmsalgo.txt","w");
    if(fileoutput == NULL)
    {
        cerr << "PMSAlgo::initLogPsmalgo(): Cannot open file: " << "./pmsalgo.txt" << endl;
        exit(8);
    };
}

void PMSAlgo::initLogEvals(){
    fileoutput_evals = fopen("./pmsalgo_evals.txt","w");
    if(fileoutput_evals == NULL)
    {
        cerr << "PMSAlgo::initLogEvals(): Cannot open file: " << "./pmsalgo_evals.txt" << endl;
        exit(8);
    };
}

void PMSAlgo::logHeader(const PMScan* lsr, const PMScan* lsa, const PMScan* raw_lsr, const PMScan* raw_lsa, const PM_TYPE ax, const PM_TYPE ay, const PM_TYPE ath)
{
    int i, new_bad[PM_L_POINTS], index[PM_L_POINTS];
    PM_TYPE new_r[PM_L_POINTS], fi[PM_L_POINTS], r[PM_L_POINTS], abs_err;

    if (fileoutput && (log_level != PMS_LOG_RESULTS))
    {

        fprintf(fileoutput,"HEADER____ ");

        fprintf(fileoutput,"%s %f %f %f ",(*lsr).t().toChar(),(*lsr).rx(),(*lsr).ry(),(*lsr).th());
        for (i = 0;i < PM_L_POINTS;i++) {
            fprintf(fileoutput,"%f %d %d ",(*lsr).r(i),(*lsr).seg(i),(*lsr).bad(i));
        }

        fprintf(fileoutput,"%s %f %f %f ",(*lsa).t().toChar(),(*lsa).rx(),(*lsa).ry(),(*lsa).th());
        for (i = 0;i < PM_L_POINTS;i++) {
            fprintf(fileoutput,"%f %d %d ",(*lsa).r(i),(*lsa).seg(i),(*lsa).bad(i));
        }

        fprintf(fileoutput,"%s %f %f %f ",(*raw_lsr).t().toChar(),(*raw_lsr).rx(),(*raw_lsr).ry(),(*raw_lsr).th());
        for (i = 0;i < PM_L_POINTS;i++) {
            fprintf(fileoutput,"%f %d %d ",(*raw_lsr).r(i),(*raw_lsr).seg(i),(*raw_lsr).bad(i));
        }

        fprintf(fileoutput,"%s %f %f %f ",(*raw_lsa).t().toChar(),(*raw_lsa).rx(),(*raw_lsa).ry(),(*raw_lsa).th());
        for (i = 0;i < PM_L_POINTS;i++) {
            fprintf(fileoutput,"%f %d %d ",(*raw_lsa).r(i),(*raw_lsa).seg(i),(*raw_lsa).bad(i));
        }

        interpolation(ax,ay,ath,fi,r,new_r,new_bad,index);

        for (i = 0;i < PM_L_POINTS;i++) {
            fprintf(fileoutput,"%f %f %f %d ",fi[i],r[i],new_r[i],new_bad[i]);
        }

        // Initial error
        abs_err = getErrorOverN(ax, ay, ath);
        fprintf(fileoutput,"%f %f %f %f\n",ax,ay,ath,abs_err);
    }

    return;
}

void PMSAlgo::logThIteration(const PM_TYPE ax, const PM_TYPE ay, const PM_TYPE ath, const PM_TYPE dx, const PM_TYPE dy, const PM_TYPE dth, const PM_TYPE lower_bound, const PM_TYPE upper_bound)
{

    PM_TYPE new_r[PM_L_POINTS], fi[PM_L_POINTS], r[PM_L_POINTS], err;
    int new_bad[PM_L_POINTS], index[PM_L_POINTS];

    if (fileoutput && (log_level != PMS_LOG_RESULTS))
    {

        if (log_level == PMS_LOG_DETAILED)
        {
            fprintf(fileoutput,"ITER_TH_FS ");
        } else {
            fprintf(fileoutput,"ITER_TH_NM ");
        }

        interpolation(ax,ay,ath,fi,r,new_r,new_bad,index);

        for (int i = 0;i < PM_L_POINTS;i++) {
            fprintf(fileoutput,"%f %f %f %d ",fi[i],r[i],new_r[i],new_bad[i]);
        }

        err = getErrorOverN(ax, ay, ath);

        fprintf(fileoutput,"%f %f %f %f %f %f %f ",err,dx,dy,dth*180.0/M_PI,ax,ay,ath); // print dth in degrees

        if (log_level == PMS_LOG_DETAILED)
        {
            bruteforceOrientationSearch(ax,ay,ath-dth,lower_bound,upper_bound,0.5,true);
            bruteforceOrientationSearch(ax,ay,ath,-0.5,0.5,0.01,true);
        }

        fprintf(fileoutput,"\n");
    }

    return;
}

void PMSAlgo::logXYIteration(const PM_TYPE ax, const PM_TYPE ay, const PM_TYPE ath, const PM_TYPE dx, const PM_TYPE dy, const PM_TYPE dth)
{

    PM_TYPE new_r[PM_L_POINTS], fi[PM_L_POINTS], r[PM_L_POINTS], err;
    int new_bad[PM_L_POINTS], index[PM_L_POINTS];

    if (fileoutput && (log_level != PMS_LOG_RESULTS))
    {

        if (log_level == PMS_LOG_DETAILED)
        {
            fprintf(fileoutput,"ITER_XY_FS ");
        } else {
            fprintf(fileoutput,"ITER_XY_NM ");
        }

        interpolation(ax,ay,ath,fi,r,new_r,new_bad,index);

        for (int i = 0;i < PM_L_POINTS;i++) {
            fprintf(fileoutput,"%f %f %f %d ",fi[i],r[i],new_r[i],new_bad[i]);
        }

        err = getErrorOverN(ax, ay, ath);

        fprintf(fileoutput,"%f %f %f %f %f %f %f ",err,dx,dy,dth,ax,ay,ath); // print dth in degrees

        if (log_level == PMS_LOG_DETAILED)
        {
            PM_TYPE ax_best, ay_best, emin;
            emin = bruteforceTranslationSearch(ax-dx,ay-dy,ath,&ax_best,&ay_best,true);
            fprintf(fileoutput,"%f %f %f ",ax_best,ay_best,emin);
        }

        fprintf(fileoutput,"\n");

    }

    return;
}

void PMSAlgo::logIteration(const PM_TYPE ax, const PM_TYPE ay, const PM_TYPE ath, const PM_TYPE dx, const PM_TYPE dy, const PM_TYPE dth)
{

    PM_TYPE new_r[PM_L_POINTS], fi[PM_L_POINTS], r[PM_L_POINTS], err;
    int new_bad[PM_L_POINTS], index[PM_L_POINTS];

    if (fileoutput && (log_level != PMS_LOG_RESULTS))
    {

        if (log_level == PMS_LOG_DETAILED)
        {
            fprintf(fileoutput,"IT_XYTH_FS ");
        } else {
            fprintf(fileoutput,"IT_XYTH_NM ");
        }

        interpolation(ax,ay,ath,fi,r,new_r,new_bad,index);

        for (int i = 0;i < PM_L_POINTS;i++) {
            fprintf(fileoutput,"%f %f %f %d ",fi[i],r[i],new_r[i],new_bad[i]);
        }

        err = getErrorOverN(ax, ay, ath);

        fprintf(fileoutput,"%f %f %f %f %f %f %f ",err,dx,dy,dth,ax,ay,ath); // print dth in degrees

        if (log_level == PMS_LOG_DETAILED)
        {
            bruteforceXYThSearch(ax,ay,ath,true);
        }

        fprintf(fileoutput,"\n");
    }

    return;
}

void PMSAlgo::logResult(TimeStamp tt_start, TimeStamp tt_end, const PM_TYPE abs_err)
{
    TimeStamp delta_t = tt_end-tt_start;

    if (fileoutput)
    {
        fprintf(fileoutput,"RESULTS___ ");
        ostringstream outs;
        outs << delta_t;
        string time_str = outs.str();
        fprintf(fileoutput,"%s %f %d\n",time_str.c_str(),abs_err,no_evaluations);
    }

#ifdef COUT_DEBUG
    cout << "---insxxxPMSAlgo::logResult() -- ttstart: " << tt_start << " tt_end: " << tt_end << " delta_t: " << delta_t << " abs_err: " << abs_err << " no_evaluations: " << no_evaluations << endl;
#endif

    return;
}

void PMSAlgo::logEvaluations(const double *x, const double *grad, const double feval)
{
    if (fileoutput_evals)
    {
        if (grad) {
            fprintf(fileoutput_evals,"%d %f %f %f %f %f %f %f\n",no_evaluations,x[0],x[1],x[2],grad[0],grad[1],grad[2],feval); // print dth in degrees
        } else {
            fprintf(fileoutput_evals,"%d %f %f %f %f %f %f %f\n",no_evaluations,x[0],x[1],x[2],0.0,0.0,0.0,feval); // print dth in degrees
        }
    }
    return;
}


///////////////////// New Diosi

/** @brief Prepares a scan for scan matching.

//Filters the scan using median filter, finds far away points and segments the scan->
//@param ls The scan to be preprocessed.
//*/
//void PMSAlgo::pm_preprocessScan(PMScan *scan)
//{
//  pm_median_filter(scan);
//  pm_find_far_points(scan);
//  pm_segment_scan(scan);
//}


///** @brief Filters the laser ranges with a median filter.

//The job of this median filter is to remove chair and table
//legs which are likely to change position with time.

//The median filter helps to get rid of spurious data.
//If the median filter's window is 5, then 3 points need be
//close to each other to surrive the filtering. Chair legs taking
//1 or 2 range readings will be removed.

//Do not use this function when fitting lines to laser scans!

//Median filter will round up corners.

//x,y coordinates of points are not upadted.
//@param ls Laser scan to be filtered.
//*/
//void PMSAlgo::pm_median_filter (PMScan *scan)
//{
//  const int HALF_WINDOW  = 2;//2 to left 2 to right
//  const int WINDOW = 2*HALF_WINDOW+1;
//  PM_TYPE   r[WINDOW];
//  PM_TYPE   w;

//  int i,j,k,l;

//  for ( i=0;i<PM_L_POINTS;i++ )
//  {
//    k=0;
//    for ( j=i-HALF_WINDOW;j<=i+HALF_WINDOW;j++ )
//    {
//      l = ( ( j>=0 ) ?j:0 );
//      r[k]=act.r( ( l < PM_L_POINTS ) ?l: ( PM_L_POINTS-1 ) );
//      k++;
//    }
//    //bubble sort r
//    for ( j= ( WINDOW-1 );j>0;j-- )
//      for ( k=0;k<j;k++ )
//        if ( r[k]>r[k+1] ) // wrong order? - swap them
//        {
//          w=r[k];
//          r[k]=r[k+1];
//          r[k+1] = w;
//        }
//    act.setR(i,r[HALF_WINDOW]);//choose the middle point
//  }
//}

///** @brief Tags point further than a given distance PM_MAX_RANGE.

//Far away points get tagged as @a PM_RANGE.
//@param ls The scan searched for far points.
//*/
//void PMSAlgo::pm_find_far_points (PMScan *scan)
//{
//  for ( int i=0;i<PM_L_POINTS;i++ )
//  {
//    if ( act.r(i)>=PM_MAX_RANGE )
//      act.setBad(i,PM_RANGE);
//  }
//}

///** @brief Segments scanpoints into groups based on range discontinuities.

//By segmenting scans into groups of disconnected sets of points, one can
//prevent falsely interpolating points into the free space between disconnected
//objects during scan projection.

//Segment number 0 is reserved to segments containing only 1 point.

//Far away points (r > PM_MAX_RANGE), gaps between groups of
//points - divide segments. The gap between extrapolated point and
//current point has to be large as well to prevent corridor walls to
//be segmented into separate points.
//*/
//void PMSAlgo::pm_segment_scan (PMScan *scan)
//{
//  const PM_TYPE   MAX_DIST = PM_SEG_MAX_DIST;//max range diff between conseq. points in a seg
//  PM_TYPE   dr;
//  int       seg_cnt = 0;
//  int       i,cnt;
//  bool      break_seg;

//  seg_cnt = 1;

//  //init:
//  if ( fabs ( scan->r(0)-scan->r(1) ) < MAX_DIST ) //are they in the same segment?
//  {
//    scan->setSeg(0,seg_cnt);
//    scan->setSeg(1,seg_cnt);
//    cnt        = 2;    //2 points in the segment
//  }
//  else
//  {
//    scan->setSeg(0,0); //point is a segment in itself
//    scan->setSeg(1,seg_cnt);
//    cnt        = 1;
//  }

//  for ( i=2;i<PM_L_POINTS;i++ )
//  {
//    //segment breaking conditions: - bad point;
//    break_seg = false;
//    if ( scan->bad(i) )
//    {
//      break_seg = true;
//      scan->setSeg(i,0);
//    }
//    else
//    {
//      dr = scan->r(i)- ( 2.0*scan->r(i-1) - scan->r(i-2) );//extrapolate & calc difference
//      //Don't break a segment if the distance between points is small
//      //or the distance beween the extrapolated point and current point is small.
//      if ( fabs ( scan->r(i)-scan->r(i-1) ) < MAX_DIST ||
//         ( ( scan->seg(i-1)==scan->seg(i-2) ) && fabs ( dr ) <MAX_DIST ) )
//      {
//        //not breaking the segment
//        cnt++;
//        scan->setSeg(i,seg_cnt);
//      }
//      else
//        break_seg = true;
//    }//if scan->bad

//    if ( break_seg ) // breaking the segment?
//    {
//      if ( cnt==1 )
//      {
//        //check first if the last three are not on a line by coincidence
//        dr = scan->r(i)- ( 2.0*scan->r(i-1)-scan->r(i-2) );
//        if ( scan->seg(i-2) == 0 && scan->bad(i) == 0 && scan->bad(i-1) == 0
//                && scan->bad(i-2) == 0 && fabs ( dr ) <MAX_DIST )
//        {
//          scan->setSeg(i,seg_cnt);
//          scan->setSeg(i-1,seg_cnt);
//          scan->setSeg(i-2,seg_cnt);
//          cnt = 3;
//        }//if scan->
//        else
//        {
//          scan->setSeg(i-1,0);
//          //what if ls[i] is a bad point? - it could be the start of a new
//          //segment if the next point is a good point and is close enough!
//          //in that case it doesn't really matters
//          scan->setSeg(i,seg_cnt);//the current point is a new segment
//          cnt = 1;
//        }
//      }//if cnt ==1
//      else
//      {
//        seg_cnt++;
//        scan->setSeg(i,seg_cnt);
//        cnt = 1;
//      }//else if cnt
//    }//if break seg
//  }//for
//}//pm_segment_scan

///** @brief Match two laser scans using polar scan matching.

//Minimizes the sum of square range residuals through changing lsa->rx, lsa->ry, lsa->th.
//The error is minimized by iterating a translation estimation step followed by an
//orientation search step.

//PSM was not explicitly designed for laser scan matching based odometry where scans with small
//pose difference are matched with each other without any prior pose information. However when
//using PSM for this purpose, reduce the values of PM_MAX_ERROR, PM_WEIGHTING_FACTOR to
//reflect the small inter-scan motion. Also by reducing the value of PM_STOP_COND,
//larger matching accuracy can be achieved. The currently implemented error estimation
//functions are not useful for laser odometry error estimation.

//Limitations: due to the nature of the association rule divergence in a slow rate
//may be experienced in rooms where there are not many features to constrain
//the solution in all directions. This can occur for examples in corridor-like environments
//including rooms where the room directly in front of the laser is outside of
//the range of the laser range finder.

//@param lsr The reference scan->
//@param lra The current scan->
//*/
void PMSAlgo::matchNewDiosi(PM_TYPE* ax, PM_TYPE* ay, PM_TYPE *ath)
{
    PM_TYPE   new_r[PM_L_POINTS];//interpolated r at measurement bearings
    int       new_bad[PM_L_POINTS];//bad flags of the interpolated range readings
    PM_TYPE   C = PM_WEIGHTING_FACTOR;//weighting factor; see dudek00
    int       iter,small_corr_cnt=0;
    PM_TYPE   dx=0,dy=0,dth=0;//match error, current scan corrections
    PM_TYPE   avg_err = 100000000.0;

    iter = -1;
    matchOk = true;
    while ( ++iter < PM_MAX_ITER && small_corr_cnt < 3 ) //Has to be a few small corrections before stopping.
    {
        if ( ( fabs ( dx/10) +fabs ( dy/10) +fabs ( dth ) ) < PM_STOP_COND )
            small_corr_cnt++;
        else
            small_corr_cnt=0;

        act.setRx(*ax);act.setRy(*ay);act.setTh(*ath);
        pm_scan_project(new_r, new_bad);

        //---------------ORIENTATION SEARCH-----------------------------------
        //search for angle correction using crosscorrelation, perform it every second step
        if ( iter%2 == 0 )
        {
            dth = pm_orientation_search(new_r, new_bad);
            *ath += dth;
            continue;
        }

        //------------------------------------------translation-------------
        //reduce C with time to consider only the best matches
        if ( iter == PM_CHANGE_WEIGHT_ITER )
            C = C/5000.0; // weigh far points even less.
        avg_err = pm_translation_estimation(new_r, new_bad, C, &dx, &dy);
        *ax += dx;
        *ay += dy;
    }//while iter
    act.setRx(*ax);
    act.setRy(*ay);
    act.setTh(*ath);
    //  return ( avg_err);
}//pm_psm

///** @brief Performs scan projection.

//This function enables the comparisson of two scans.
//It projects the current (active) scan @a act into the reference scans @a ref
//coordinate frame,  using the current scan's pose. As the reference scan
//is assumed to be at the origin, its coordinates don't need to be passed along.
//Returns in new_r the interpolated range readinds r at the reference scan's
//measurement bearings. Returns in new_bad bad flags of the interpolated range
//readings, where occluded readings are tagged.

//@param act The current scan.
//@param new_r Array of the projected range readings (has to have the correct size).
//@param new_bad Information about the validity of the interpolated range readings is returned here.
//*/
void PMSAlgo::pm_scan_project(PM_TYPE *new_r, int *new_bad)
{
    PM_TYPE   r[PM_L_POINTS],fi[PM_L_POINTS];//current scan in ref. coord. syst.
    PM_TYPE   x,y;
    int       i;
    PM_TYPE   delta;

    // convert range readings into the reference frame
    // this can be speeded up, by connecting it with the interpolation
    for ( i=0;i<PM_L_POINTS;i++ )
    {
        delta   = act.th() + pm_fi[i];
        x       = act.r(i)*cosf ( delta ) + act.rx();
        y       = act.r(i)*sinf ( delta ) + act.ry();
        r[i]    = sqrtf ( x*x+y*y );
        fi[i]   = atan2 ( y,x );
        //handle discontinuity at pi (Angle goes from -pi/1 to 3pi/2 for 360deg. scans)
        if(x<0 && y<0)
            fi[i] += 2.0*M_PI;
        new_r[i]  = 100000;//initialize big interpolated r;
        new_bad[i]= PM_EMPTY;//for interpolated r;

    }//for i

    //------------------------INTERPOLATION------------------------
    //calculate/interpolate the associations to the ref scan points
    //algorithm ignores crosings at the beginning and end points to make it faster
    for ( i=1;i<PM_L_POINTS;i++ )
    {
        //i points to the angles in the current scan

        // i and i-1 has to be in the same segment, both shouldn't be bad
        // and they should be larger than the minimum angle
        if ( act.seg(i) != 0 && act.seg(i) == act.seg(i-1) && !act.bad(i) && !act.bad(i-1) ) /* && fi[i]>PM_FI_MIN && fi[i-1]>PM_FI_MIN*/
        {
            //calculation of the "whole" parts of the angles
            int j0,j1;
            PM_TYPE r0,r1,a0,a1;
            bool occluded;
            //This is a crude hack to fix a serious bug here!!!!
            //At the -pi pi boundary it failed by interpolating throught the whole scan->
            //The affected 360 scans, or Hokuyo scans where the matched scans had
            //more than 60degree orientation difference.
            if( fabs(fi[i]-fi[i-1]) >= M_PI ) ///TODO: replace this hack with proper fix where we don't loose points.
                continue;

            if ( fi[i]>fi[i-1] ) //are the points visible?
            {
                //visible
                occluded = false;
                a0  = fi[i-1];
                a1  = fi[i];
                j0  =  (int) ceil ( ( fi[i-1] - PM_FI_MIN ) /PM_DFI );
                j1  =  (int) floor ( ( fi[i] - PM_FI_MIN ) /PM_DFI );
                r0  = r[i-1];
                r1  = r[i];
            }
            else
            {
                //invisible - still have to calculate to filter out points which
                occluded = true; //are covered up by these!
                //flip the points-> easier to program
                a0  = fi[i];
                a1  = fi[i-1];
                j0  =  (int) ceil ( ( fi[i] - PM_FI_MIN ) /PM_DFI );
                j1  =  (int) floor ( ( fi[i-1] - PM_FI_MIN ) /PM_DFI );
                r0  = r[i];
                r1  = r[i-1];
            }
            //here j0 is always smaller than j1!

            //interpolate for all the measurement bearings beween j0 and j1
            while ( j0<=j1 ) //if at least one measurement point difference, then ...
            {
                PM_TYPE ri = ( r1-r0 ) / ( a1-a0 ) * ( ( ( PM_TYPE ) j0*PM_DFI+PM_FI_MIN )-a0 ) +r0;

                //if j0 -> falls into the measurement range and ri is shorter
                //than the current range then overwrite it
                if ( j0>=0 && j0<PM_L_POINTS && new_r[j0]>ri )
                {
                    new_r[j0]    = ri;//overwrite the previous reading
                    new_bad[j0] &=~PM_EMPTY;//clear the empty flag
                    if ( occluded ) //check if it was occluded
                        new_bad[j0] = new_bad[j0]|PM_OCCLUDED;//set the occluded flag
                    else
                        new_bad[j0] = new_bad[j0]&~PM_OCCLUDED;
                    //the new range reading also has to inherit the other flags
                    new_bad[j0] |= act.bad(i);//superfluos - since act.bad(i) was checked for 0
                    new_bad[j0] |= act.bad(i-1);//superfluos - since act.bad(i-1) was checked for 0
                    ///TODO: Uncomment this? (or leave it as it is a local scan matching approach anyway)
                    //if(ri>PM_MAX_RANGE)        //uncomment this later
                    //  new_bad[fi0] |= PM_RANGE;
                }
                j0++;//check the next measurement angle!
            }//while
        }//if act
    }//for i

}//pm_scan_project


/** @brief Performs one iteration of orientation alignment of current scan->

Function estimating the orientation of the current scan represented with range readings
@a new_r tagged with flags @a new_bad with respect to the reference scan @a ref.

This function exploits that if the current and reference scan are taken at the same
position, an orientation change of the current scan results in a left or right shift
of the scan ranges.

This function estimates the orientation by finding that shift which minimizes the
difference between the current and ref. scan-> The orientation estimate is then
refined using interpolation by fitting a parabole to the maximum and its
neighbours and finding the maximum.

@param ref The reference scan->
@param new_r The interpolated ranges of the current scan->
@param new_bad The tags corresponding to the new_r.
@return Returns the rotation of @new_bad in radians which minimize the sum of absolute range residuals.
 */
PM_TYPE PMSAlgo::pm_orientation_search(const PM_TYPE *new_r, const int *new_bad)
{
    int       i;
    int       window = PM_SEARCH_WINDOW;//20;//+- width of search for correct orientation
    PM_TYPE   dth = 0.0;//current scan corrections
    //pm_fi,ref.r - reference points
    PM_TYPE e, err[PM_L_POINTS]; // the error rating
    PM_TYPE beta[PM_L_POINTS];// angle corresponding to err
    const PM_TYPE LARGE_NUMBER = 100000;
    PM_TYPE n;
    int k=0;

    for ( int di=-window;di<=window;di++ )
    {
        n=0;e=0;

        int min_i,max_i;
        if ( di<=0 )
        {min_i = -di;max_i=PM_L_POINTS;}
        else
        {min_i = 0;max_i=PM_L_POINTS-di;}

        ///TODO: speed up by unrolling the loop, replace if with multiplication with 0 or 1/
        /// use sse2 instructions...
        for ( i=min_i;i<max_i;i++ ) //searching through the current points
        {
            PM_TYPE delta = fabs ( new_r[i]-ref.r(i+di) );
#if PM_LASER == PM_HOKUYO_UTM_30LX
            //checking delta < PM_MAX_ERROR helps to choose the correct local minimum for the UTM.
            //Without it the solution may be pulled in the wrong direction. Don't remove it.
            ///TODO: Find out when is it useful to check if delta < PM_MAX_ERROR - why only for the UTM...
            if ( !new_bad[i] && !ref.bad(i+di)  && delta < PM_MAX_ERROR)
#else
            if ( !new_bad[i] && !ref.bad(i+di) )
#endif
            {
                e += delta;
                n++;
            }
        }//for i

        if ( n > 0 )
            err[k]  = e/n;//don't forget to correct with n!
        else
            err[k]  = LARGE_NUMBER;
        beta[k] = di;
        k++;
    }//for dfi

    //now search for the global minimum
    //later I can make it more robust
    //assumption: monomodal error function!
    PM_TYPE emin = LARGE_NUMBER*10.0;
    int   imin=-1;
    for ( i = 0; i < k; i++ )
    {
        if ( err[i] < emin )
        {
            emin = err[i];
            imin = i;
        }
    }

    //      if(emin > 0) {
    //          for (i = 0;i < k;i++)
    //            cout << err[i] << " ";
    //          cout << endl;
    //      }

    if ( err[imin]>=LARGE_NUMBER )
    {
        cerr <<"Polar Match: orientation search failed" <<err[imin]<<endl;
        throw 1;
    }
    dth = beta[imin]*PM_DFI;

    //interpolation
    if ( imin >= 1 && imin < ( k-1 ) ) //is it not on the extreme?
    {
        //lets try interpolation
        PM_TYPE D = err[imin-1]+err[imin+1]-2.0*err[imin];
        PM_TYPE d = LARGE_NUMBER;
        if ( fabs ( D ) >0.01 && err[imin-1]>err[imin] && err[imin+1]>err[imin] )
            d= ( err[imin-1]-err[imin+1] ) /D/2.0;
        //        cout <<"ORIENTATION REFINEMENT "<<d<<endl;
        if ( fabs ( d ) < 1.0 )
            dth+=d*PM_DFI;
    }//if

    return(dth);
}//pm_orientation_search

/** @brief Estimate the postion of the current scan with respect to a reference scan->

@param ref The reference scan->
@param new_r The interpolated ranges of the current scan->
@param new_bad The tags corresponding to the new_r.
@param C Weighting factor for range residuals.
@param dx Estimated position increment X coordinate is returned here.
@param dy Estimated position increment Y coordinate is returned here.
@return Returns the average range residual.
*/
PM_TYPE PMSAlgo::pm_translation_estimation(const PM_TYPE *new_r, const int *new_bad, PM_TYPE C, PM_TYPE *dx, PM_TYPE *dy)
{
    // do the weighted linear regression on the linearized ...
    // include angle as well
    int i;
    PM_TYPE hi1, hi2,hwi1,hwi2, hw1=0,hw2=0,hwh11=0;
    PM_TYPE hwh12=0,hwh21=0,hwh22=0,w;
    PM_TYPE dr;
    PM_TYPE abs_err = 0;
    int     n = 0;
    for ( i=0;i<PM_L_POINTS;i++ )
    {
        dr = ref.r(i)-new_r[i];
        abs_err += fabs ( dr );
        //weight calculation
        if ( ref.bad(i)==0 && new_bad[i]==0 && new_r[i]<PM_MAX_RANGE && new_r[i]>PM_MIN_RANGE && fabs ( dr ) <PM_MAX_ERROR )
        {
            //        cout <<i<<" "<<dr<<";"<<endl;
            //weighting according to DUDEK00
            w = C/ ( dr*dr+C );
            n++;

            //proper calculations of the jacobian
            hi1 = pm_co[i];//xx/new_r[i];//this the correct
            hi2 = pm_si[i];//yy/new_r[i];

            hwi1 = hi1*w;
            hwi2 = hi2*w;

            //par = (H^t*W*H)^-1*H^t*W*dr
            hw1 += hwi1*dr;//H^t*W*dr
            hw2 += hwi2*dr;

            //H^t*W*H
            hwh11 += hwi1*hi1;
            hwh12 += hwi1*hi2;
            //        hwh21 += hwi2*hi1; //should take adv. of symmetricity!!
            hwh22 += hwi2*hi2;

        }//if
    }//for i
    if ( n<PM_MIN_VALID_POINTS ) //are there enough points?
    {
        cerr <<"pm_translation_estimation: ERROR not enough points ("<<n<<")"<<endl;
        throw 1;//not enough points
    }

    //calculation of inverse
    PM_TYPE D;//determinant
    PM_TYPE inv11,inv21,inv12,inv22;//inverse matrix

    D = hwh11*hwh22-hwh12*hwh21;
    if ( D<0.001 )
    {
        cerr <<"pm_linearized_match: ERROR determinant to small! "<<D<<endl;
        throw 1;
    }
    inv11 =  hwh22/D;
    inv12 = -hwh12/D;
    inv21 = -hwh12/D;
    inv22 =  hwh11/D;

    *dx = inv11*hw1+inv12*hw2;
    *dy = inv21*hw1+inv22*hw2;
    return(abs_err/n);
}//pm_translation_estimation
