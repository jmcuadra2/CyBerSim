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
#ifndef PMSALGO_H
#define PMSALGO_H

#include "scanmatchingalgo.h"
#include <QPoint>
#include <QPair>
// #include "liblbfgs/lbfgs.h"
#include <lbfgs.h>
#include <nlopt.h>

#define PMS_DEFAULT 	0
#define PMS_INS	 	1
#define PMS_INS2	2
#define PMS_DIOSI 	3
#define PMS_BRUTE_FORCE 4
#define PMS_TOTAL_SEARCH 5
#define PMS_BFGS	 6
#define PMS_NLOPT	 7
#define PMS_ISRAEL	 8
#define PMS_KALMAN	 9



#define PMS_LOG_OFF 		0
#define PMS_LOG_NORMAL 		1
#define PMS_LOG_DETAILED 	2
#define PMS_LOG_RESULTS 	3

#define PM_REF_NOT_VISIBLE	32

#define PM_MIN_RANGE        100 // [mm] Minimum valid laser range for the reprojected current scan.
#define PM_SEG_MAX_DIST     200.0 // The distance between points to break a segment.
#define PM_WEIGHTING_FACTOR 700*700 // Parameter used for weighting associated points in the position calculation of PSM. Try setting it to 30*30 for laser odometry.
#define PM_CHANGE_WEIGHT_ITER 10 // The number of iterations after which the weighting factor is reduced to weight down outliers.

//#define PM_MAX_ERROR        1000  // [mm] Maximum distance between associated points used in pose estimation. Try setting it to 30 for laser odometry.
#define PM_STOP_COND        0.4  // If the pose change (|dx|+|dy|+|dth|) is smaller than this PSM scan matching stops.
#define PM_MIN_STD_XY          200.0 //[mm] The minimum match result standard deviation in X or Y direction. Used in covariance estimation.
#define PM_MIN_STD_ORIENTATION 4.0  //[degrees] The minimum standard deviation of the orientation match. Used in covariance estimation.
#define PM_MATCH_ERROR_OFFSET  50.0  //<[mm] Offset subtracted from average range residual when scaling the covariance matrix.

/**
    @author Jose Manuel Cuadra Troncoso <jmcuadra@dia.uned.es>
*/
class PMSAlgo : public ScanMatchingAlgo
{

public:
    PMSAlgo();
    ~PMSAlgo();
    void setPMSVariables(const int method, const nlopt_algorithm nlopt_selection, const bool nlopt_grad_refinement_flag, const nlopt_algorithm nlopt_flag_grad_refinement, const int log_level_input);
    PM_TYPE match(PMScan* lsr, PMScan* lsa , int pms_method);
    PM_TYPE match(PMScan* lsr, PMScan* lsa, PMScan* raw_lsr, PMScan* raw_lsa, int pms_method);

protected:
    void matchKalman(PM_TYPE* ax, PM_TYPE* ay, PM_TYPE *ath);
    void matchNewDiosi(PM_TYPE* ax, PM_TYPE* ay, PM_TYPE *ath);
    void matchDiosiIsrael(PM_TYPE* ax, PM_TYPE* ay, PM_TYPE *ath);
    void matchUpdated(PM_TYPE* ax, PM_TYPE* ay, PM_TYPE *ath);
    void matchMixedApproach(PM_TYPE* ax, PM_TYPE* ay, PM_TYPE *ath);
    void matchBruteForce(PM_TYPE* ax, PM_TYPE* ay, PM_TYPE *ath);
    void matchTotalSearch(PM_TYPE* ax, PM_TYPE* ay, PM_TYPE *ath);
    void matchBFGS(PM_TYPE* ax, PM_TYPE* ay, PM_TYPE *ath);
    void matchNlopt(PM_TYPE* ax, PM_TYPE* ay, PM_TYPE *ath);
    void calculateDisplacementsRefFrame(PM_TYPE* ax, PM_TYPE* ay, PM_TYPE *ath);
    void interpolation(const PM_TYPE x_c, const PM_TYPE y_c, const PM_TYPE theta_c, PM_TYPE* fi, PM_TYPE* r, PM_TYPE* new_r, int* new_bad, int* index);

    void interpolation(const PM_TYPE x_c, const PM_TYPE y_c, const PM_TYPE theta_c, PM_TYPE* fi, PM_TYPE* r, PM_TYPE* new_r, PM_TYPE* alt_r, int* new_bad, int* index, QVector< PM_TYPE > ref_r);
    void interpolation2(const PM_TYPE x_c, const PM_TYPE y_c, const PM_TYPE theta_c, PM_TYPE* fi, PM_TYPE* r, PM_TYPE* new_r, PM_TYPE* alt_r, int* new_bad, int* index);

    void getGradient(const PM_TYPE ax, const PM_TYPE ay, const PM_TYPE ath, PM_TYPE* gx, PM_TYPE* gy, PM_TYPE* g_th);
    void getNumericalH(const PM_TYPE x_c, const PM_TYPE y_c, const PM_TYPE theta_c, PM_TYPE h_matrix[][2]);
    void getCompleteNumericalH(const PM_TYPE x_c, const PM_TYPE y_c, const PM_TYPE theta_c, PM_TYPE h_matrix[][3]);
    char* strName() const{ return (char *)"psm"; }
    bool visibleFromNewPosition(const PM_TYPE r_original, const PM_TYPE phi_original, const PM_TYPE x_c, const PM_TYPE y_c, const PM_TYPE theta_c);
    PM_TYPE bruteforceOrientationSearch(const PM_TYPE ax, const PM_TYPE ay, const PM_TYPE ath, const PM_TYPE lower_limit, const PM_TYPE upper_limit, const PM_TYPE delta_theta, const bool log);
    PM_TYPE bruteforceTranslationSearch(const PM_TYPE ax, const PM_TYPE ay, const PM_TYPE ath, PM_TYPE* ax_best, PM_TYPE* ay_best, const bool log);
    void bruteforceXYThSearch(const PM_TYPE ax, const PM_TYPE ay, const PM_TYPE ath, const bool log);
    void translationsearch(const PM_TYPE ax, const PM_TYPE ay, const PM_TYPE ath, PM_TYPE *dx, PM_TYPE *dy);
    void totalsearch(const PM_TYPE ax, const PM_TYPE ay, const PM_TYPE ath, PM_TYPE* dx, PM_TYPE* dy, PM_TYPE* dth);
    void gradientBasedSearch(const PM_TYPE ax, const PM_TYPE ay, const PM_TYPE ath, PM_TYPE* dx, PM_TYPE* dy, PM_TYPE* dth);
    void nloptBasedSearch(const PM_TYPE ax, const PM_TYPE ay, const PM_TYPE ath, PM_TYPE* dx, PM_TYPE* dy, PM_TYPE* dth);
    void nloptBasedSearch_XYonly(const PM_TYPE ax, const PM_TYPE ay, PM_TYPE* dx, PM_TYPE* dy);
    void initLogPsmalgo();
    void initLogEvals();
    void logHeader(const PMScan* lsr, const PMScan* lsa, const PMScan* raw_lsr, const PMScan* raw_lsa, const PM_TYPE ax, const PM_TYPE ay, const PM_TYPE ath);
    void logThIteration(const PM_TYPE ax, const PM_TYPE ay, const PM_TYPE ath, const PM_TYPE dx, const PM_TYPE dy, const PM_TYPE dth, const PM_TYPE lower_bound, const PM_TYPE upper_bound);
    void logXYIteration(const PM_TYPE ax, const PM_TYPE ay, const PM_TYPE ath, const PM_TYPE dx, const PM_TYPE dy, const PM_TYPE dth);
    void logIteration(const PM_TYPE ax, const PM_TYPE ay, const PM_TYPE ath, const PM_TYPE dx, const PM_TYPE dy, const PM_TYPE dth);
    void logResult(TimeStamp tt_start, TimeStamp tt_end, const PM_TYPE abs_err);
    void logEvaluations(const double *x, const double *g, const double fx);
    PM_TYPE getErrorOverN(const PM_TYPE ax, const PM_TYPE ay, const PM_TYPE ath);
    PM_TYPE getErrorOverNIsrael(const PM_TYPE ax, const PM_TYPE ay, const PM_TYPE ath);

    PM_TYPE getError2OverN(const PM_TYPE ax, const PM_TYPE ay, const PM_TYPE ath);

    PM_TYPE getError2OverN(const PM_TYPE ax, const PM_TYPE ay, const PM_TYPE ath, PM_TYPE* fi, PM_TYPE* new_r, PM_TYPE* new_ref, int* new_bad, const PM_TYPE delta_th, int* next_segment);

    int getInverseMatrix3by3(const PM_TYPE a_matrix[][3], PM_TYPE a_matrix_inverse[][3]);
    static lbfgsfloatval_t _evaluate(void *instance, const lbfgsfloatval_t *x, lbfgsfloatval_t *g, const int n, const lbfgsfloatval_t step)
    {
        return reinterpret_cast<PMSAlgo*>(instance)->evaluate(x, g, n, step);
    }
    lbfgsfloatval_t evaluate(const lbfgsfloatval_t *x, lbfgsfloatval_t *g, const int n, const lbfgsfloatval_t step);
    static int _progress(void *instance, const lbfgsfloatval_t *x, const lbfgsfloatval_t *g, const lbfgsfloatval_t fx, const lbfgsfloatval_t xnorm,
                         const lbfgsfloatval_t gnorm, const lbfgsfloatval_t step, int n, int k, int ls)
    {
        return reinterpret_cast<PMSAlgo*>(instance)->progress(x, g, fx, xnorm, gnorm, step, n, k, ls);
    }
    int progress(const lbfgsfloatval_t *x, const lbfgsfloatval_t *g, const lbfgsfloatval_t fx, const lbfgsfloatval_t xnorm, const lbfgsfloatval_t gnorm,
                 const lbfgsfloatval_t step, int n, int k, int ls);
    static double _nlopt_eval(unsigned n, const double *x, double *grad, void *instance)
    {
        return reinterpret_cast<PMSAlgo*>(instance)->nlopt_eval(n, x, grad);
    }
    double nlopt_eval(unsigned n, const double *x, double *grad);

    void projectSegments(const PM_TYPE x_c, const PM_TYPE y_c, const PM_TYPE theta_c, PM_TYPE* r);

    void projectSegments(const PM_TYPE x_c, const PM_TYPE y_c, const PM_TYPE theta_c, PM_TYPE* fi, PM_TYPE* r, PM_TYPE* new_ref, int* new_bad, const PM_TYPE delta_th, int* next_segment);

    void pm_scan_project(PM_TYPE *new_r, int *new_bad);
    PM_TYPE pm_orientation_search (const PM_TYPE *new_r, const int *new_bad);
    PM_TYPE pm_translation_estimation(const PM_TYPE *new_r, const int *new_bad, PM_TYPE C, PM_TYPE *dx, PM_TYPE *dy);
//    void pm_preprocessScan(PMScan* scan);
//    void pm_median_filter (PMScan *scan);
//    void pm_find_far_points (PMScan *scan);
//    void pm_segment_scan (PMScan *scan);

protected:
    FILE *fileoutput;
    FILE *fileoutput_evals;
    PMScan act, ref;//copies of actual and reference scans
    int no_evaluations, return_result;
    TimeStamp tt_start, tt_end;
    int pms_method;
    nlopt_algorithm nlopt_method;
    bool nlopt_refinement;
    nlopt_algorithm nlopt_refinement_method;
    int log_level;

    QVector<QPoint> overlappedAreas;
    QVector<QPair <int, int> > comparables;
    bool comparable[PM_L_POINTS];
    bool comparados[PM_L_POINTS];

    QVector<double> rhoList;
    QVector<double> phiList;

    double errorMax;

    PM_TYPE   PM_FI_MIN;
    PM_TYPE   PM_FI_MAX;
    PM_TYPE   PM_DFI;
};

#endif
