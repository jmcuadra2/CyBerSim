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
#include <iterator>
#include <algorithm>
#include <numeric>
#include <boost/math/special_functions/beta.hpp>

#include <QTextStream>

#include "kalmansegment.h"
#include "../neuraldis/timestamp.hpp"

using namespace std;
using boost::math::beta;

const double KalmanSegment::RAD2GRAD = 180.0/M_PI;
const double KalmanSegment::MAXSIGMA = 0.1;

KalmanSegment::KalmanSegment(const int& numSensors, double* radius, double* angle, const bool& full360, const bool& twoStage)
{
    this->numSensors = numSensors;
    this->radius = radius;
    this->angle = angle;
    this->full360 = full360;
    this->classIdx = -1;
    this->twoStage = twoStage;

    if(full360)
        sensorAperture = 2*M_PI/numSensors;
    else
        sensorAperture = abs(angle[numSensors - 1] - angle[0])/(numSensors - 1);

    maxNoMeasurePoints = round(177/(sensorAperture*RAD2GRAD));

    maxSensorsSegment = numSensors;

    sensor_start = 0;
    sensor_end = maxSensorsSegment - 1;
    sensor_start_org = sensor_start;
    sensor_end_org = sensor_end;

    limited = false;
    auto_limited = false;

    valid_idx.resize(maxSensorsSegment);
    idx_r_ang.resize(maxSensorsSegment);
    inverse_idx_r_ang.resize(maxSensorsSegment);

    current_side = notSet;
    current_side_mat.resize(maxSensorsSegment);

    beta_est.resize(maxSensorsSegment);

    weights.resize(maxSensorsSegment);
    //   if(twoStage)
    weights.fill(1);

    weights_factor = 1;
    beta_est_w.resize(maxSensorsSegment);
    for(int i = 0; i < maxSensorsSegment; i++) {
        valid_idx[i] = i;
        QMatrix2x2 m;
        S_w.push_back(m);
        S.push_back(m);
    }
    sum_e2_w.resize(maxSensorsSegment);
    sum_e2_w_out.resize(maxSensorsSegment);

    s2_w.resize(maxSensorsSegment);
    s2_w_out.resize(maxSensorsSegment);

    end_right = false;
    end_left = false;
    normal_end_right = false;
    normal_end_left = false;
    break_point_right = sensor_end;
    break_point_left = sensor_start;
    prev_break_point_right = break_point_right;
    prev_break_point_left = break_point_left;
    break_i = sensor_end;

    next_outlier_cnt_right = 0;
    next_outlier_found_right = false;
    next_outlier_cnt_left = 0;
    next_outlier_found_left = false;

    s2_0 = 0;

    ang_0 = 0;
    ang_n = 0;
    init_ang_0 = sensor_start;
    init_ang_n = sensor_end;
    idx_ord.resize(maxSensorsSegment);

    long_seed = 0;
    isInit = false;
    include_seed = true;
    passed360 = false;
    this->full360 = full360;
    idx_passed360 = -1;

    this->t_vals = t_vals;

}


KalmanSegment::~KalmanSegment()
{
}

QVector<int> KalmanSegment::transform_i(const int& idx_1, const int& idx_2, bool * l_passed360)
{
    QVector<int> idx_t;
    if(l_passed360 != 0)
        *l_passed360 = false;

    if(full360) {
        if(idx_2 < idx_1) {
            for(int i = idx_1; i < numSensors; i++)
                idx_t.push_back(i);
            for(int i = 0; i <= idx_2; i++)
                idx_t.push_back(i);
            if(l_passed360 != 0)
                *l_passed360 = true;
        }
        else
            for(int i = idx_1; i <= idx_2; i++)
                idx_t.push_back(i);
    }
    else {
        for(int i = idx_1; i <= idx_2; i++)
            idx_t.push_back(i);
    }
    return idx_t;
}

int KalmanSegment::transform_i(const int& idx, bool * l_passed360)
{
    int idx_t = idx;
    if(l_passed360 != 0)
        *l_passed360 = false;

    if(full360) {
        if(idx >= maxSensorsSegment) {
            idx_t = idx - maxSensorsSegment;
            if(l_passed360 != 0)
                *l_passed360 = true;
        }
        else if(idx < 0) {
            idx_t = maxSensorsSegment + idx;
            if(l_passed360 != 0)
                *l_passed360 = true;
        }
        else
            idx_t = idx;
    }
    //   else {
    //     if(idx < 0)
    //       idx_t = 0;
    //     else if(idx >= maxSensorsSegment)
    //       idx_t = maxSensorsSegment - 1;
    //     else
    //       idx_t = idx;
    //   }
    return idx_t;
}

void KalmanSegment::estimate2PWKalman(const int& ang_0, const int& ang_n, const bool& break_if_outl)
{

    bool l_passed360 = false;
    this->ang_0 = ang_0;
    this->ang_n = ang_n;
    init_ang_0 = ang_0;
    init_ang_n = ang_n;
    long_seed = ang_n - ang_0 + 1;
    if(long_seed < 0 && !isInit) {
        long_seed = numSensors - 1 - ang_0 + ang_n;
        passed360 = true;
    }

    if(!isInit) {
        initKalmanSeed();
        if(include_seed)
            includeSeed();
    }

    setIdxOrd();

    performKalman(break_if_outl);

}

void KalmanSegment::performKalman(const bool& break_if_outl) {
    bool prev_end_left;
    bool prev_end_right;
    bool continueK = true;
    bool out_l = false;

    pair<double, double> ew_aw_estimate;

    int i = -1;    // E-K-F
    int end_i = valid_idx.size();
    int i_r_ang = -1;
    int i_1;
    int i_valid;

    if(init_ang_0 == 97 && init_ang_n == 97)
        int foo = 0;

    TimeStamp start_tick = TimeStamp::nowMicro();
    while(i < end_i - 1 && continueK) {

        i_valid = valid_idx[++i];
        i_r_ang = idx_r_ang[i_valid];

        prev_end_left = end_left;
        prev_end_right = end_right;
        current_side = current_side_mat[i_valid];

        ew_aw_estimate = estimate(i);
        out_l = false;
        if(i >= 3) {

            double s2 = sqrt(s2_w_out[valid_idx[i]])/weights_factor;
            if(s2 > MAXSIGMA/2 /* && s2 < MAXSIGMA*/)
                out_l = true;
            else {
                int s_out;
                if(end_right || end_left)
                    s_out = outliers.size()/* + 1*/;
                else
                    s_out = outliers.size();
                double t_val = t_vals[i - 3 - s_out]; // t_vals empieza en idx 0, d.f. = idx+1
                i_1 = valid_idx[i - 1];
                double sw = s2_w[i_1];
                double t_exp = ew_aw_estimate.first/sqrt(ew_aw_estimate.second * s2_w[i_1]);

                out_l = abs(t_exp) > t_val;
            }
        }
        else if(long_seed > 2){
            double s2 = sqrt(s2_w_out[valid_idx[i]])/weights_factor;
            if(s2 > MAXSIGMA/2 /* && s2 < MAXSIGMA*/)
                out_l = true;
        }
        if(out_l) {
            if(break_if_outl)
                neglectOutlier(i);
            else {
                if(i_r_ang == idx_passed360 && continueK)
                    passed360 = true;
                //           continue;
            }

        }
        if(break_if_outl) {
            test2Outlier(i, i_r_ang, out_l, end_i, &continueK);
        }

        if(!continueK)
            continue;

        if(i_r_ang == idx_passed360 && continueK)
            passed360 = true;
        if(i <= end_i)  // casa
            look4Limits(i);

        int l_seed = !long_seed ? 2 : long_seed;
        if(auto_limited && ((long_seed && (i >= long_seed || i >= 2)) || (long_seed == 0 && i >= 2))) {
            valid_idx.erase(valid_idx.begin() + i, valid_idx.end());
            end_i = valid_idx.size();
            continueK = false;
        }
        if(!continueK)
            continue;

        if((limited && (i_r_ang == break_point_right || i_r_ang == break_point_left) && (!normal_end_left && !normal_end_right) && (!end_left && !end_right))){
            int min_2 = 2*minim;
            if(i < min_2 - 1) {
//                int size_idx = min(maxSensorsSegment - (min_2 - i - current_side/* - 2*/)/2, valid_idx.size());
//                if(size_idx != valid_idx.size()) {
                int valid_idx_size = valid_idx.size();
                int size_idx = min(maxSensorsSegment - (min_2 - i - current_side/* - 2*/)/2, valid_idx_size);
                if(size_idx != valid_idx_size) {
                    QVector<int> idx_copy(size_idx);
                    QVector<int>::iterator it_idx = idx_copy.begin();
                    int j = 0;
                    int cnt = 0;
                    int i_r_a;
                    //           int start_i = out_l ? i : i + 1;
                    int start_i = i + 1;

                    for(cnt = 0; cnt < start_i && cnt < size_idx; cnt++) {
                        *it_idx = cnt;
                        i_r_a = idx_r_ang[cnt];
                        ++it_idx;
                    }
                    j = start_i;
                    while(j < min_2 && cnt < size_idx) {
                        *it_idx = j;
                        i_r_a = idx_r_ang[j];
                        ++it_idx;
                        j += 2;
                        cnt++;
                    }

                    j = min_2;
                    while(cnt < size_idx) {
                        *it_idx = j;
                        i_r_a = idx_r_ang[j];
                        ++it_idx;
                        j++;
                        cnt++;
                    }

                    extractIdx(valid_idx, idx_copy);
                    end_i = valid_idx.size();
                    int foo = 0;
                    idx_copy.size();
                    foo = 1;
                }
            }
        }

        if(/*current_side == Right && */i_r_ang == break_point_right) {
            normal_end_right = true;
            if(end_left || (limited /*&& !break_if_outl*/ && normal_end_left)) {
                continueK = false;
                //         int start_i = out_l ? i : i + 1;
                int start_i = i + 1;
                valid_idx.erase(valid_idx.begin() + start_i, valid_idx.end());
                end_i = valid_idx.size();
            }
        }
        else if(/*current_side == Left && */i_r_ang == break_point_left) {
            normal_end_left = true;
            if(end_right || (limited /*&& !break_if_outl*/ && normal_end_right)) {
                continueK = false;
                valid_idx.erase(valid_idx.begin() + i + 1, valid_idx.end());
                end_i = valid_idx.size();
            }
        }
    }


    cleanOutliers(false);

    if(!valid_idx.size()) {
        valid_idx.append(0);
        break_point_left = idx_r_ang[valid_idx.front()];
        break_point_left = break_point_right;
    }

    rho_0_est = beta_est_w[valid_idx.back()].first;
    phi_0_est = beta_est_w[valid_idx.back()].second;
    //   sigma_est = sqrt(s2_w[valid_idx.back()])/weights_factor;
    double s2;
    int numElems = getNumElems();
    if(numElems > 2)
        //     s2 = sum_e2_w_out[valid_idx.back()] / (numElems - 2);
        s2 = sum_e2_w[valid_idx.back()] / (numElems - 2);
    else
        s2 = MAXSIGMA/2*MAXSIGMA/2*weights_factor*weights_factor;

    sigma_est = sqrt(s2)/weights_factor;

    var_beta_est_w = S_w[valid_idx.back()]*(float)s2_w[valid_idx.back()];

}

void KalmanSegment::setIdxOrd(void )
{

    int l_ang_0 = ang_0;
    int l_ang_n = ang_n;
    if(ang_n == ang_0) {
        if(ang_0 == 0 && !full360) {
            l_ang_0 = -1;
            l_ang_n = -1;
        }
        else if(ang_n == numSensors - 1  && !full360) {
            l_ang_0 = numSensors;
            l_ang_n = numSensors;
        }
    }

    outliers.clear();
    out_sum_e2.clear();
    last_outliers.clear();

    QVector<int>::iterator pos = idx_ord.begin();
    int new_size = maxSensorsSegment;
    if(valid_idx.size() != new_size)
        valid_idx.resize(new_size);  // ver full360
    QVector<int>::iterator pos_valid = valid_idx.begin();
    QVector<int>::iterator p_idx_r_ang = idx_r_ang.begin();
    QVector<int>::iterator p_inv_idx_r_ang = inverse_idx_r_ang.begin();
    QVector<Side>::iterator p_side = current_side_mat.begin();

    int cnt = 0;
    bool l_passed360 = false, did_l_passed360 = false;
    int idx, maxim, n_left, n_right;
    int end = maxSensorsSegment - 1, start = 0;
    Side lastEndSide;

    if(full360) {

        minim = int(maxSensorsSegment/2);
        maxim = minim;
        lastEndSide = Right; // da igual el valor, no se usa
        if(!limited) {
            int break_l = ang_0 - minim / 2;
            break_l = break_l < 0 ? numSensors + break_l : break_l;
            setBreakPoint(break_l, Left);
            setBreakPoint((ang_n + minim / 2)%numSensors, Right);
        }
        else {
            setBreakPoint(sensor_start, Left);
            setBreakPoint(sensor_end, Right);
        }
    }
    else {
        minim = std::min(end - ang_n, ang_0 - start);
        lastEndSide = minim == ang_0  - start ?  Right : Left;
        maxim = std::max(end - ang_n, ang_0 - start);
    }

    for(int i = 0; i < minim; i++) {
        *pos = i;
        *p_side = Left;
        idx = transform_i(l_ang_0 - *pos - 1, &l_passed360); //
        if(!did_l_passed360 && l_passed360) {
            idx_passed360 = idx;
            did_l_passed360 = l_passed360;
        }
        *p_idx_r_ang = idx;
        *(p_inv_idx_r_ang + idx) = cnt;
        *pos_valid = cnt;
        ++pos;
        ++p_side;
        ++p_idx_r_ang;
        ++pos_valid;
        cnt++;

        *pos = i;
        *p_side = Right;
        idx = transform_i(l_ang_n  + *pos + 1, &l_passed360); // , &l_passed360
        if(!did_l_passed360 && l_passed360) {
            idx_passed360 = idx;
            did_l_passed360 = l_passed360;
        }
        *p_idx_r_ang = idx;
        *(p_inv_idx_r_ang + idx) = cnt;
        *pos_valid = cnt;
        ++pos;
        ++p_side;
        ++p_idx_r_ang;
        ++pos_valid;
        cnt++;
    }
    for(int i = minim; i < maxim; i++) {
        *pos = i;
        *p_side = lastEndSide;
        if(*p_side == Left)
            idx = transform_i(l_ang_0 - *pos - 1, &l_passed360); // , &l_passed360
        else
            idx = transform_i(l_ang_n  + *pos + 1, &l_passed360); // , &l_passed360
        if(!did_l_passed360 && l_passed360 && maxim > 1) {
            idx_passed360 = idx;
            did_l_passed360 = l_passed360;
        }

        *p_idx_r_ang = idx;
        *(p_inv_idx_r_ang + idx) = cnt;
        *pos_valid = cnt;
        ++pos;
        ++p_side;
        ++p_idx_r_ang;
        ++pos_valid;
        cnt++;
    }
}

double KalmanSegment::initKalmanSeed(void)
{
    bool passed = false;

    QVector<int> idx_t;
    QVector <double> r_seed;
    QVector <double> ang_seed;
    int sizeIdx;
    double sum_e2;
    double max_sigma;

    idx_t = transform_i(ang_0, ang_n, &passed);
    sizeIdx = idx_t.size();
    do {
        r_seed.resize(sizeIdx);
        ang_seed.resize(sizeIdx);

        for(int i = 0; i < sizeIdx; i++) {
            r_seed[i] = radius[idx_t[i]];
            ang_seed[i] = angle[idx_t[i]];
        }

        repeatedMedianPol(r_seed, ang_seed);

        if(twoStage) {
            //       weights_factor = 1 + std::max(fabs(tan(ang_seed.front() - phi_0)), fabs(tan(ang_seed.back() - phi_0)));
            weights_factor = std::max(r_seed.front(), r_seed.back());
        }
        else
            weights_factor = 1000;

        int max_i = -1;
        sum_e2 = estimateS2_0(r_seed, ang_seed, &max_i);
        max_sigma = MAXSIGMA/2*MAXSIGMA/2*weights_factor*weights_factor;

        if(s2_0 > max_sigma && sizeIdx >= 2) {
            if(max_i >= sizeIdx/2) {
                idx_t = transform_i(ang_0, ang_n - 1, &passed);
                ang_n = idx_t.back();
            }
            else {
                idx_t = transform_i(ang_0 + 1, ang_n, &passed);
                ang_0 = idx_t.front();
            }
            sizeIdx = idx_t.size();
            if(long_seed > 0)
                long_seed--;
        }
    } while(s2_0 > max_sigma && sizeIdx >= 2);


    Matriz<double> J0(sizeIdx, 2);
    double cos_ang;
    for(int i = 0; i < sizeIdx; i++) {
        double proper_ang = ang_seed[i];
        if(ang_seed[i] - beta_0.second <= M_PI/2)
            proper_ang = M_PI + proper_ang;
        else if(proper_ang - beta_0.second > M_PI/2)
            proper_ang = proper_ang - M_PI;

        cos_ang = cos(proper_ang - beta_0.second);
        J0[i][0] = 1/cos_ang;
        J0[i][1] = -beta_0.first*sin(proper_ang-beta_0.second)/(cos_ang*cos_ang);
    }

    if(ang_0 == ang_n) {
        S0(0, 0) = 1;
        S0(1, 0) = 0;
        S0(0, 1) = 0;
        S0(1, 1) = 1;
    }
    else {
        Matriz<double> SS_0 = J0.transpose()*J0;
        QMatrix2x2 SS0;
        SS0(0, 0) = SS_0[0][0];
        SS0(1, 0) = SS_0[1][0];
        SS0(0, 1) = SS_0[0][1];
        SS0(1, 1) = SS_0[1][1];
        double det = SS0(0, 0)*SS0(1, 1) - SS0(1, 0)*SS0(0, 1);
        if(det) {
            S0(0, 0) = SS0(1, 1)/det;
            S0(1, 0) = -SS0(1, 0)/det;
            S0(0, 1) = -SS0(0, 1)/det;
            S0(1, 1) = SS0(0, 0)/det;
        }
    }
    isInit = true;

    return sum_e2;
}

void KalmanSegment::includeSeed(void )
{
    if(abs(ang_0) < 0.000001 && !full360)
        ang_n = ang_0;
    else if(abs(ang_n - (numSensors - 1)) < 0.000001 && !full360)
        ang_0 = ang_n;
    else {
        QVector<int> idx_t;
        bool l_passed360 = false;
        if(long_seed == 1) { ///nuevo
            idx_t.push_back(transform_i(ang_0, &l_passed360));
            idx_t.push_back(transform_i(ang_n + 1, &l_passed360));
        }
        else
            idx_t = transform_i(ang_0, ang_n, &l_passed360);

        int idx_size = idx_t.size();
        ang_0 = idx_t[idx_size/2];
        if(idx_size > 1)
            ang_n = idx_t[idx_t.size()/2 - 1];
        else
            ang_n = ang_0;
    }
}

pair< double, double > KalmanSegment::estimate(const int& i)
{
    double a, a_w, e_1_w;
    int valid_i = valid_idx[i];
    int i_1 = 0;
    int idx = idx_r_ang[valid_i];
    double rad = radius[idx_r_ang[valid_i]];
    double ang = angle[idx_r_ang[valid_i]];

    if(i == 0) {
        beta_prevest = beta_0;
        S_prev = S0;
    }
    else {
        i_1 = valid_idx[i - 1];
        beta_prevest = beta_est[i_1];
        S_prev = S[i_1];
    }
    double r_est_i = ordinaryKalman(rad, ang, beta_prevest, S_prev, &a);
    beta_est[valid_i] = beta_prevest;
    S[valid_i] = S_prev;

    if(!twoStage) {
        beta_est_w[valid_i] = beta_est[valid_i];
        S_w[valid_i] = S[valid_i];

        e_1_w = rad - r_est_i;
        a_w = a;

        s2_w[valid_i] = estimateS2(i, e_1_w, a_w);
        //   s2_w_out[valid_i] = s2_w[valid_i];
    }
    else {
        weights[valid_i] = weights_factor / r_est_i;
        //     weights[valid_i] = weights_factor / (1 + fabs(tan(ang - beta_est_w[valid_i].second)));

        if(i == 0) {
            beta_prevest_w = beta_est[valid_i]; // la no ponderada
            S_prev_w = S[valid_i]; // la no ponderada
        }
        else {
            beta_prevest_w = beta_est_w[i_1];  // la ponderada
            S_prev_w = S_w[i_1];  // la ponderada
        }

        double r_est_w = weightedKalman(rad, ang, beta_prevest_w, S_prev_w, &a_w, weights[valid_i]);
        beta_est_w[valid_i] = beta_prevest_w;
        S_w[valid_i] = S_prev_w;

        e_1_w = rad - r_est_w;

        s2_w[valid_i] = estimateS2(i, e_1_w, a_w);
        //   s2_w_out[valid_i] = s2_w[valid_i];
    }


    return make_pair(e_1_w, a_w);
}

void KalmanSegment::setTValues(double * t_vals, double confidence)
{
    this->t_vals = t_vals;
    confidenceLevel = confidence;
}

void KalmanSegment::neglectOutlier(const int& i)
{
    int valid_i = valid_idx[i];
    if(i == 0) {
        S[valid_i] = S0; // se desprecia el outlier
        S_w[valid_i] = S0;
        beta_est[valid_i] = beta_0;
        beta_est_w[valid_i] = beta_0;
        weights[valid_i] = 0;
        s2_w[valid_i] = s2_0;
        s2_w_out[valid_i] = s2_0;  // para resolveOverlaps()
        sum_e2_w[valid_i] = 0;
        sum_e2_w_out[valid_i] = 0; // para merge y verdadera estimacion de sigma
    }
    else {
        int i_1 = valid_idx[i - 1];
        if(i_1 < 0) return;

        S[valid_i] = S[i_1]; // se desprecia el outlier
        S_w[valid_i] = S_w[i_1];
        beta_est[valid_i] = beta_est[i_1];
        beta_est_w[valid_i] = beta_est_w[i_1];
        weights[valid_i] = 0;
        s2_w[valid_i] = s2_w[i_1];
        //   s2_w_out[valid_i] = s2_w_out[i_1];  // para resolveOverlaps()
        sum_e2_w[valid_i] = sum_e2_w[i_1];
        //   sum_e2_w_out[valid_i] = sum_e2_w_out[i_1]; // para merge y verdadera estimacion de sigma
    }
}

void KalmanSegment::test2Outlier(int& i, /*const */int& i_r_ang, const bool& out_l, int& num_points, bool * continueK) // casa
{
    int prev_point, break_point, next_outlier_cnt, last_valid_i;
    bool end_this, end_other, next_outlier_found, normal_end_other;
    int last_out = outliers.size() - 1;
    int i_valid = valid_idx[i];

    int last_i = i;
    int diff_i_out = 0;
    if(current_side == Right) {
        prev_point = -1;
        break_point = break_point_right;
        end_this = end_right;
        end_other = end_left;
        normal_end_other = normal_end_left;
        next_outlier_found = next_outlier_found_right;
        next_outlier_cnt = next_outlier_cnt_right;
    }
    else {
        prev_point = 1;
        break_point = break_point_left;
        end_this = end_left;
        end_other = end_right;
        normal_end_other = normal_end_right;
        next_outlier_found = next_outlier_found_left;
        next_outlier_cnt = next_outlier_cnt_left;
    }

    if(next_outlier_found) {
        if(out_l) {
            if(outliers.back()[1] == current_side)
                last_valid_i = 0;
            else
                last_valid_i = 1;

            break_point = transform_i(outliers[last_out - last_valid_i][0] + prev_point);
            end_this = true;
            if(!end_other || normal_end_other) {
                int min_2 = 2*minim;
                last_i = outliers[last_out - last_valid_i][2] - 1;
                //         last_i = last_i < 0 ? 0 : last_i;
                if(abs(i_r_ang - outliers[last_out - last_valid_i][0]) > numSensors / 2)
                    diff_i_out = numSensors - abs(i_r_ang - outliers[last_out - last_valid_i][0]);
                else
                    diff_i_out = abs(i_r_ang - outliers[last_out - last_valid_i][0]);

                if(last_i < min_2/* - 1*/) {
                    int size_idx;
                    if(normal_end_other)
                        size_idx = i - diff_i_out;
                    else {
                        int valid_idx_size = valid_idx.size();
                        size_idx = min(maxSensorsSegment, valid_idx_size) - (min_2 - last_i/* - 1*/)/2;
                    }
                    QVector<int> idx_copy(size_idx);
                    QVector<int>::iterator it_idx = idx_copy.begin();
                    int j = 0;
                    int cnt = 0;
                    int i_r_a;

                    if(last_i < size_idx) {
                        for(cnt = 0; cnt < last_i; cnt++) {
                            *it_idx = cnt;
                            i_r_a = idx_r_ang[cnt];
                            ++it_idx;
                        }
                    }

                    last_i = last_i == -1 ? 1 : last_i;
                    j = last_i;
                    while(j < min_2 && cnt < size_idx) {
                        *it_idx = j;
                        i_r_a = idx_r_ang[j];
                        ++it_idx;
                        j += 2;
                        cnt++;
                    }

                    j = min_2;
                    while(cnt < size_idx) {
                        *it_idx = j;
                        i_r_a = idx_r_ang[j];
                        ++it_idx;
                        j++;
                        cnt++;
                    }
                    /*          }  */

                    extractIdx(valid_idx, idx_copy);
                    num_points = valid_idx.size();

                    if(i <= valid_idx.size()) { // por si ha llegado a un extremo de forma normal
                        int first_other_out = outliers[last_out - last_valid_i][2];
                        int idx_j = valid_idx[first_other_out];
                        double last_e = out_sum_e2[last_out - last_valid_i];
                        double last_sum = sum_e2_w_out[idx_j];
                        sum_e2_w_out[idx_j] -= out_sum_e2[last_out - last_valid_i];
                        s2_w_out[idx_j] = sum_e2_w_out[idx_j]/(first_other_out - 1);
                    }
                }
                QVector<int> l_o(3);
                l_o[0] = i_r_ang;
                l_o[1] = current_side;
                l_o[2] = i;
                last_outliers.push_back(l_o);
                i -= diff_i_out + 1; // por los dos o tres puntos eliminados
                if(i > -1)
                    i_r_ang = idx_r_ang[valid_idx[i]];
                else
                    *continueK = false;
            }
            else {
                int ss = outliers.size();
                break_i = outliers[last_out - last_valid_i][2] - 1;
                *continueK = false;
                valid_idx.erase(valid_idx.begin() + break_i + 1, valid_idx.end());
                num_points = valid_idx.size();
            }

            int first_other_out = outliers[last_out - last_valid_i][2];
            outliers.erase(outliers.begin() + last_out - last_valid_i);
            out_sum_e2.erase(out_sum_e2.begin() + last_out - last_valid_i);

            int o_size = outliers.size();
            for(int o = 0; o < o_size; o++) {
                int dif_o = outliers[o][2] - first_other_out;
                if(dif_o > 0)
                    outliers[o][2] -= dif_o/2 + 1; // por i -= 2 de unas líneas arriba
            }
        }
        else {
            if(next_outlier_cnt == 1)
                next_outlier_cnt = 2;
            else {
                next_outlier_found = false;
                next_outlier_cnt = 0;
            }
        }
    }
    else {
        if(out_l) {
            next_outlier_found = true;
            next_outlier_cnt = 1;
            QVector<int> l_o(3);
            l_o[0] = i_r_ang;
            l_o[1] = current_side;
            l_o[2] = i;
            outliers.push_back(l_o);
            out_sum_e2.push_back(last_sum_e2);
        }
    }

    if(normal_end_other && out_l) {  // casa
        normal_end_other = false;
        end_other = true;
    }

    if(current_side == Right) {
        setBreakPoint(break_point, Right);
        end_right = end_this;
        end_left = end_other;
        normal_end_left = normal_end_other;
        next_outlier_found_right = next_outlier_found;
        next_outlier_cnt_right = next_outlier_cnt;
    }
    else {
        setBreakPoint(break_point, Left);
        end_left = end_this;
        end_right = end_other;
        normal_end_right = normal_end_other;
        next_outlier_found_left = next_outlier_found;
        next_outlier_cnt_left = next_outlier_cnt;
    }
}

void KalmanSegment::erasePoints(void)
{
    QVector <int> idx_points;
    int size_idx = idx_r_ang.size();

    if(passed360) {
        QVector<int>::iterator it = idx_r_ang.begin();
        int i = 0;
        for(; it < idx_r_ang.end(); ++it, i++) {
            if(*it >= break_point_left || *it <= break_point_right)
                idx_points.push_back(i);
        }
    }
    else {
        QVector<int>::iterator it = idx_r_ang.begin();
        int i = 0;
        for(; it < idx_r_ang.end(); ++it, i++) {
            if(*it >= break_point_left && *it <= break_point_right)
                idx_points.push_back(i);
        }
    }
    break_i = break_i - size_idx;
    extractIdx(S, idx_points);
}

void KalmanSegment::erasePoint(const int& idx)
{
    QVector<double>::iterator pos_d = weights.begin();
    QVector<QMatrix2x2 >::iterator pos_m = S.begin();
    QVector<int>::iterator pos_i = idx_r_ang.begin();
    QVector< pair <double, double> >::iterator pos_p = beta_est.begin();
    QVector < QVector <int> >::iterator pos_vi = outliers.begin();
    QVector<Side>::iterator pos_s = current_side_mat.begin();
    current_side_mat.erase(pos_s + idx);

    break_i = break_i - 1; //  - size_idx

    S.erase(pos_m + idx);
    beta_est.erase(pos_p + idx);
    weights.erase(pos_d + idx);
    outliers.erase(pos_vi + idx);//   no vale hay que buscarlo
    idx_r_ang.erase(pos_i + idx);

    pos_d = s2_w.begin();
    pos_m = S_w.begin();
    pos_p = beta_est_w.begin();
    pos_vi = outliers.begin();

    S_w.erase(pos_m + idx);
    beta_est_w.erase(pos_p + idx);
    s2_w.erase(pos_d + idx);

    pos_d = sum_e2_w.begin();
    sum_e2_w.erase(pos_d + idx);
    pos_d = sum_e2_w_out.begin();
    sum_e2_w_out.erase(pos_d + idx);
    last_outliers.clear();

    pos_d = s2_w_out.begin();
    s2_w_out.erase(pos_d + idx);

    if(passed360) {
        if(break_point_right < break_point_left)
            passed360 = true;
        else
            passed360 = false;
    }
}

void KalmanSegment::repeatedMedianPol(QVector< double > & r_seed, QVector< double > & ang_seed)
{
    int size_seed = ang_seed.size();
    QVector< QVector<double> > phi(size_seed);

    if(size_seed == 1) {
        rho_0 = r_seed[0];
        phi_0 = ang_seed[0];
        beta_0.first = rho_0;
        beta_0.second = phi_0;
        return;
    }

    double r_i, r_j, ang_i;
    QVector<double> distances(size_seed);
    QVector<double>::iterator pos;
    QVector<double> ang_seg(size_seed);

    for(int i = 0; i < size_seed; i++) {
        if(ang_seed[0] > ang_seed.back()) {
            if(ang_seed[i] > M_PI/2)
                ang_seed[i] -= M_PI;
            else
                ang_seed[i] += M_PI;
        }
    }

    for(int i = 0; i < size_seed; i++) {
        r_i = r_seed[i];
        ang_i = ang_seed[i];
        pos = distances.begin();
        for(int j = 0; j < size_seed; j++) {
            if(i < j) {
                r_j = r_seed[j];
                *pos = sqrt(r_j*r_j + r_i*r_i - 2*r_j*r_i*cos(ang_seed[j] - ang_i));
            }
            ++pos;
        }

        for(int j = 0; j < size_seed; j++) {

            if(i < j) {
                double a_sin = r_seed[j]* sin(ang_seed[j] - ang_i)/distances[j];
                if(a_sin > 1)
                    a_sin = 1;
                else if(a_sin < -1)
                    a_sin = -1;

                if(r_seed[j] > r_seed[i]/cos(ang_seed[j] - ang_seed[i]))
                    ang_seg[j] = ang_i - (M_PI - asin(a_sin));
                else
                    ang_seg[j] = ang_i - asin(a_sin);
            }
            else if(i > j)
                ang_seg[j] = phi[j][i];
        }
        phi[i] = ang_seg;
    }

    QVector<double> median_row(size_seed);
    QVector<double> phi_no_ii(size_seed - 1);
    for(int i = 0; i < size_seed; i++) {
        int jj = -1;
        for(int j = 0; j < size_seed;j++) {
            if(j != i) {
                ++jj;
                phi_no_ii[jj] = phi[i][j];
            }
        }
        median_row[i] = median(phi_no_ii);
    }
    phi_0 = median(median_row) - M_PI/2;
    if(phi_0 < -M_PI)
        phi_0 += M_PI;
    else if(phi_0 > M_PI)
        phi_0 = M_PI - phi_0;


    QVector<double> rho(size_seed);
    for(int i = 0; i < size_seed; i++)
        rho[i] = r_seed[i] * abs(cos(ang_seed[i] - phi_0));
    rho_0 = median(rho);

    //   if(ang_seed[size_seed - 1] < 0 && phi_0 > M_PI/2)
    //     phi_0 -= M_PI;
    //   else if(ang_seed[0] > 0 && phi_0 < -M_PI/2)
    //     phi_0 += M_PI;

    if(phi_0 < -M_PI/2)
        phi_0 += M_PI;
    else if(phi_0 > M_PI/2)
        phi_0 -= M_PI;

    beta_0.first = rho_0;
    beta_0.second = phi_0;

}

double KalmanSegment::estimateS2_0(QVector< double > & r_seed, QVector< double > & ang_seed, int *max_i)
{
    double sum_e2 = 0;
    double e_new;
    int sampleSize = ang_seed.size();

    *max_i = -1;
    double max_err = 0;

    for(int i = 0; i < sampleSize; i++) {
        e_new = r_seed[i] - abs(beta_0.first/cos(ang_seed[i] - beta_0.second));
        sum_e2 += e_new*e_new;
        if(max_err < abs(e_new)) {
            max_err = abs(e_new);
            *max_i = i;
        }
    }
    if(sampleSize > 2)
        s2_0 = sum_e2 / (sampleSize - 2);
    else
        s2_0 = MAXSIGMA/2*MAXSIGMA/2*weights_factor*weights_factor;  // casa

    s2_w[0] = s2_0;
    s2_w_out[0] = s2_0;

    return sum_e2;
}

double KalmanSegment::estimateS2(const int& i, const double& e_w, const double& a_w)
{
    double s2 = 0;
    int valid_i = valid_idx[i];
    double val;
    if(i == 0) {
        val = (e_w*weights[valid_i])*(e_w*weights[valid_i]);
        sum_e2_w[valid_i] = val;
        sum_e2_w_out[valid_i] = val;
        last_sum_e2 = val;
    }
    else {
        val = e_w*e_w/a_w;
        sum_e2_w[valid_i] = sum_e2_w[valid_idx[i - 1]] + val;
        sum_e2_w_out[valid_i] = sum_e2_w_out[valid_idx[i - 1]] + val;
        last_sum_e2 = val;
    }
    
    int s_out = outliers.size();
    if((i >= 2 + s_out) && (long_seed >= 2)) {
        s2 = sum_e2_w[valid_i] / (i - 1 - s_out);
        s2_w_out[valid_i] = sum_e2_w_out[valid_i] / (i - 1);
    }
    else {
        s2 = s2_0;
        s2_w_out[valid_i] = max(s2_0, val);
    }

    return s2;
}

double KalmanSegment::ordinaryKalman(const double& rad, double ang, pair< double, double >& beta_prevest, QMatrix2x2& S_prev, double* a)
{
    double r_est = 0;

    if(ang - beta_prevest.second <= -M_PI/2)
        ang = M_PI + ang;

    if(ang - beta_prevest.second > M_PI/2)
        ang = ang - M_PI;
    double l_cos = cos(ang - beta_prevest.second);
    double J_r = 1 / cos(ang - beta_prevest.second);
    double J_phi = -beta_prevest.first * sin(ang - beta_prevest.second) / (l_cos*l_cos);

    *a = 1 + J_r*J_r*S_prev(0, 0) + J_phi*J_phi*S_prev(1, 1) + J_r*J_phi*(S_prev(1, 0) + S_prev(0, 1));
    double K_0 = (S_prev(0, 0)*J_r + S_prev(0, 1)*J_phi)/(*a);
    double K_1 = (S_prev(1, 0)*J_r + S_prev(1, 1)*J_phi)/(*a);
    r_est = beta_prevest.first/l_cos;
    double error = (rad - r_est);

    beta_prevest.first += K_0*error;
    beta_prevest.second += K_1*error;

    double s0, s1;
    s0 = S_prev(0, 0);
    s1 = S_prev(1, 0);
    S_prev(0, 0) = (1 - K_0*J_r)*s0 - K_0*J_phi*s1;
    S_prev(1, 0) = -K_1*J_r*s0 + (1 - K_1*J_phi)*s1;

    s0 = S_prev(0, 1);
    s1 = S_prev(1, 1);
    S_prev(0, 1) = (1 - K_0*J_r)*s0 - K_0*J_phi*s1;
    S_prev(1, 1) = -K_1*J_r*s0 + (1 - K_1*J_phi)*s1;
    //   cout << S_prev <<endl;
    return r_est;
}

double KalmanSegment::weightedKalman(const double& rad, double ang, pair< double, double >& beta_prevest, QMatrix2x2& S_prev, double* a_w, const double& weight)
{
    double r_est = 0;

    if(ang - beta_prevest.second <= -M_PI/2)
        ang = M_PI + ang;

    if(ang - beta_prevest.second > M_PI/2)
        ang = ang - M_PI;

    double J_r = 1 / cos(ang - beta_prevest.second);
    double J_phi = -beta_prevest.first * sin(ang - beta_prevest.second) * J_r * J_r;

    *a_w = 1/(weight*weight) + J_r*J_r*S_prev(0, 0) + J_phi*J_phi*S_prev(1, 1) + J_r*J_phi*(S_prev(1, 0) + S_prev(0, 1));
    double K_0 = (S_prev(0, 0)*J_r + S_prev(0, 1)*J_phi)/(*a_w);
    double K_1 = (S_prev(1, 0)*J_r + S_prev(1, 1)*J_phi)/(*a_w);
    //   r_est = beta_prevest.first/l_cos;
    r_est = beta_prevest.first * J_r;
    double error = (rad - r_est);

    beta_prevest.first += K_0*error;
    beta_prevest.second += K_1*error;

    double s0, s1;
    s0 = S_prev(0, 0);
    s1 = S_prev(1, 0);
    S_prev(0, 0) = (1 - K_0*J_r)*s0 - K_0*J_phi*s1;
    S_prev(1, 0) = -K_1*J_r*s0 + (1 - K_1*J_phi)*s1;

    s0 = S_prev(0, 1);
    s1 = S_prev(1, 1);
    S_prev(0, 1) = (1 - K_0*J_r)*s0 - K_0*J_phi*s1;
    S_prev(1, 1) = -K_1*J_r*s0 + (1 - K_1*J_phi)*s1;

    return r_est;
}

void KalmanSegment::setBreakPoint(int break_point, const Side& side)
{

    if(side == KalmanSegment::Right) {
        prev_break_point_right = break_point;
        break_point_right = break_point;
    }
    else {
        prev_break_point_left = break_point;
        break_point_left = break_point;
    }
    if(passed360) {
        if(side == Right) {
            if(break_point >= break_point_left)
                passed360 = false;
        }
        else {
            if(break_point <= break_point_right)
                passed360 = false;
        }
    }
}

void KalmanSegment::setUserBreakPoint(int break_point, const Side&  side)
{
    int prev_break_point;
    bool collapsed = false;

    if(side == KalmanSegment::Right)
        prev_break_point = prev_break_point_right;
    else
        prev_break_point = prev_break_point_left;

    if(prev_break_point == break_point)
        return;

    if(full360) {
        if(break_point == idx_passed360)
            passed360 = true;
        if(!(passed360 && limited) && !auto_limited) {
            double l_start = 0;
            double l_end = numSensors - 1;
            if(break_point > l_end)
                break_point = break_point - l_end + l_start - 1;
            else if(break_point < l_start)
                break_point = break_point + l_end - l_start + 1;
        }
    }
    
    collapsed = (((side == KalmanSegment::Right && break_point < break_point_left) || (side == KalmanSegment::Left && break_point > break_point_right)) && !passed360) || (((side == KalmanSegment::Right && break_point < break_point_left) && (side == KalmanSegment::Left && break_point > break_point_right)) && passed360);

    if(auto_limited) {
        if(passed360) {
            if(break_point > break_point_left || break_point < break_point_right || collapsed)
                unlimite();
        }
        else { // casa
            if((break_point > break_point_left && break_point <= break_point_right) || (break_point >= break_point_left && break_point < break_point_right) || collapsed)
                unlimite();
        }
    }
    if(auto_limited)
        return;

    if(collapsed)
        valid_idx.clear();
    else {
        int prev_i = inverse_idx_r_ang[prev_break_point];
        int current_i = inverse_idx_r_ang[break_point];
        QVector<int>::iterator it;

        if((side == current_side_mat[current_i] && prev_i > current_i) || (side != current_side_mat[current_i]/* && current_side_mat[current_i] == Right*/)) { // casa

            it = valid_idx.begin();

            QVector <int> ii_dx;
            int cnt = 0;
            int i_r_ang;
            for(; it < valid_idx.end(); ++it) {
                i_r_ang = idx_r_ang[*it];
                if(side == Right) {
                    if(passed360) {
                        if(break_point > numSensors / 2) {
                            if(i_r_ang <= break_point && i_r_ang > numSensors / 2)
                                ii_dx.push_back(cnt);
                        }
                        else {
                            if(i_r_ang <= break_point || i_r_ang > numSensors / 2)
                                ii_dx.push_back(cnt);
                        }
                    }
                    else {
                        if(i_r_ang <= break_point)
                            ii_dx.push_back(cnt);
                    }
                }
                else {
                    if(passed360) {
                        if(break_point < numSensors / 2) {
                            if(i_r_ang >= break_point &&  i_r_ang < numSensors / 2)
                                ii_dx.push_back(cnt);
                        }
                        else {
                            if(i_r_ang >= break_point || i_r_ang < numSensors / 2)
                                ii_dx.push_back(cnt);
                        }
                    }
                    else {
                        if(i_r_ang >= break_point)
                            ii_dx.push_back(cnt);
                    }
                }
                cnt++;
            }
            extractIdx(valid_idx, ii_dx);
        }
        else if(side == current_side_mat[current_i] && prev_i <= current_i) {
            int last_valid = valid_idx.back();
            while(current_i > last_valid) {
                valid_idx.push_back(inverse_idx_r_ang[2*last_valid - current_i]);
                current_i--;
            }

            int itr = valid_idx.size() - 1;
            while(current_i >= prev_i && itr >= 0) {
                if(valid_idx[itr] <= current_i && side == current_side_mat[current_i]) {
                    valid_idx.insert(itr, current_i);  // casa
                    current_i--;
                }
                --itr;
            }
        }
        else {
            int itr = valid_idx.size() - 1;
            while(itr >= 0) {
                if(valid_idx[itr] <= current_i && current_i > prev_i && side == current_side_mat[current_i]) {
                    valid_idx.insert(itr, current_i);  // casa
                    current_i--;
                }
                else if(valid_idx[itr] <= current_i && current_i < prev_i) {
                    valid_idx.insert(itr, current_i);  // casa
                    current_i--;
                }
                --itr;
            }
        }
    }
    if(side == Right) {
        prev_break_point_right = break_point_right;
        break_point_right = break_point;
    }
    else {
        prev_break_point_left = break_point_left;
        break_point_left = break_point;
    }

    if(passed360) {
        if(side == Right) {
            if(break_point >= break_point_left)
                passed360 = false;
        }
        else {
            if(break_point <= break_point_right)
                passed360 = false;
        }
        //     if(break_point_right - break_point_left == -1)  // colapsado
        if(valid_idx.size() == 0)  // colapsado
            passed360 = false;
    }
}


double KalmanSegment::median(QVector< double > & data)
{
    double ret = 0;
    int size_data = data.size();
    sort(data.begin(), data.end());
    QVector< double >::const_iterator it = data.begin();
    if(size_data%2)
        ret = *(it + size_data/2);
    else
        ret = (*(it + size_data/2 - 1) + *(it + size_data/2))/2;
    return ret;
}

void KalmanSegment::restoreBreakPoint(const Side&  side)
{
    //   int break_point;
    if(side == Right) {
        setUserBreakPoint(prev_break_point_right, side);
        prev_break_point_right = break_point_right;
    }
    else {
        setUserBreakPoint(prev_break_point_left, side);
        prev_break_point_left = break_point_left; // casa
    }
    if(full360 && break_point_right < break_point_left)
        passed360 = true;

}

void KalmanSegment::reEstimate2PWKalman(void)
{

    clean();
    int min_r_ang = numSensors;
    int max_r_ang = -1;
    QVector<int>::iterator it = valid_idx.begin();
    for(; it < valid_idx.end(); ++it) {
        if(idx_r_ang[*it] <= min_r_ang)
            min_r_ang = idx_r_ang[*it];
        if(idx_r_ang[*it] >= max_r_ang)
            max_r_ang = idx_r_ang[*it];
    }

    if(classIdx == 3)
        int foo = 0;

    setStartEnd(min_r_ang, max_r_ang);
    bool break_if_outl = false;
    performKalman(break_if_outl);
}

void KalmanSegment::reEstimate2PWKalman(int continueStart, int continueEnd)
{

    int i;
    int i_valid;
    cleanOutliers(true);
    if(continueEnd < 0)
        continueEnd = valid_idx.size() - 1;

    if(continueStart == continueEnd - 1)
        i = continueEnd;

    if(!i)
        i--;
    else
        i = continueStart;

    int min_r_ang = numSensors;
    int max_r_ang = -1;
    QVector<int>::iterator it = valid_idx.begin();
    for(; it < valid_idx.end(); ++it) {
        if(idx_r_ang[*it] <= min_r_ang)
            min_r_ang = idx_r_ang[*it];
        if(idx_r_ang[*it] >= max_r_ang)
            max_r_ang = idx_r_ang[*it];
    }

    if((min_r_ang >  init_ang_0 || max_r_ang <  init_ang_n) && min_r_ang <= init_ang_n && max_r_ang >= init_ang_0 && (min_r_ang != 0 || max_r_ang != numSensors - 1)) { // mirar full360
        ang_0 = min_r_ang;
        ang_n = max_r_ang;

        init_ang_0 = ang_0;
        init_ang_n = ang_n;
        long_seed = ang_n - ang_0 + 1;
        if(long_seed < 0 && !isInit) {
            long_seed = numSensors - 1 - ang_0 + ang_n;
            passed360 = true;
        }

        initKalmanSeed();
        if(include_seed) {
            includeSeed();
            i = 1;
        }
        else
            i = long_seed;


        //     sum_e2_w[valid_idx.back()] = initKalmanSeed();
        //     sum_e2_w_out[valid_idx.back()] = sum_e2_w[valid_idx.back()];
        //
        //     for(int j = 0; j < abs(ang_0 - ang_n) + 1; j++) {
        //       beta_est_w[valid_idx[j]].first = beta_0.first;
        //       beta_est_w[valid_idx[j]].second = beta_0.second;
        //       s2_w[valid_idx[j]] = s2_0;
        //       s2_w_out[valid_idx[j]] = s2_0;
        //     }
        //     weights_factor = 1;
    }
    //   else {
    while(i < continueEnd) {
        i_valid = valid_idx[++i];
        if(weights[i_valid] == 0) { // fue outlier
            neglectOutlier(i);
            continue;
        }
        pair<double, double> ew_aw_estimate = estimate(i);
    }
    //   }

    i_valid = valid_idx[continueEnd];
    rho_0_est = beta_est_w[i_valid].first;
    phi_0_est = beta_est_w[i_valid].second;
    /*  sigma_est = sqrt(s2_w_out[i_valid])/weights_factor;*/

    double s2;
    if(continueEnd > 2)
        //     s2 = sum_e2_w_out[i_valid] / (continueEnd - 2);
        s2 = sum_e2_w[i_valid] / (continueEnd - 2);
    else
        s2 = s2_0;
    sigma_est = sqrt(s2)/weights_factor;

    var_beta_est_w = S_w[valid_idx[continueEnd]]*(float)s2_w[valid_idx[continueEnd]];
}

void KalmanSegment::cleanOutliers(bool alsoEnds)
{
    if( outliers.size()) {
        if(alsoEnds)
            outliers.clear();
        else {
            QVector < QVector <int> >::iterator it_o = outliers.begin();
            for(it_o = outliers.end() - 1; it_o >= outliers.begin(); --it_o) {
                //       cout << (*it_o).front() << endl;
                if(passed360) {
                    if((*it_o).front() < break_point_left && (*it_o).front() > break_point_right)
                        outliers.erase(it_o);
                }
                else {
                    if((*it_o).front() < break_point_left || (*it_o).front() > break_point_right)
                        outliers.erase(it_o);
                }
            }
        }
    }
}

bool KalmanSegment::addPoint(Side side, bool realAdd)
{
    bool added = false;
    int i = valid_idx.size();
    //   valid_idx.append(i);  // casa
    int i_valid;

    int i_break; // casa

    double a, a_w, e_1_w;

    if(side == KalmanSegment::Left)  // casa
        i_break = inverse_idx_r_ang[break_point_left];
    else
        i_break = inverse_idx_r_ang[break_point_right];

    if(current_side_mat[i_break + 1] == side)
        i_valid = i_break + 1;
    else
        i_valid = i_break + 2;

    if(i_valid >= idx_r_ang.size())
        return added;

    int i_r_ang = idx_r_ang[i_valid];
    double rad = radius[i_r_ang];
    double ang = angle[i_r_ang];

    valid_idx.append(i_valid);  // casa
    int i_1 = valid_idx[i - 1];
    current_side = side;

    pair< double, double > beta_prevest;
    QMatrix2x2 S_prev;
    pair< double, double > beta_prevest_w;
    QMatrix2x2 S_prev_w;

    beta_prevest = beta_est[i_1];
    S_prev = S[i_1];

    double r_est_i = ordinaryKalman(rad, ang, beta_prevest, S_prev, &a);
    beta_est[i_valid] = beta_prevest;
    S[i_valid] = S_prev;


    if(!twoStage) {
        beta_est_w[i_valid] = beta_est[i_valid];
        S_w[i_valid] = S[i_valid];

        e_1_w = rad - r_est_i;
        a_w = a;

        s2_w[i_valid] = estimateS2(i, e_1_w, a_w);
        //   s2_w_out[valid_i] = s2_w[valid_i];
    }
    else {

        weights[i_valid] = weights_factor / r_est_i;
        //     weights[i_valid] = weights_factor / (1 + fabs(tan(ang - 			beta_est_w[i_valid].second)));
        beta_prevest_w = beta_est_w[i_1];
        S_prev_w = S_w[i_1];

        double r_est_w = weightedKalman(rad, ang, beta_prevest_w, S_prev_w, &a_w, weights[i_valid]);
        beta_est_w[i_valid] = beta_prevest_w;
        S_w[i_valid] = S_prev_w;

        e_1_w = rad - r_est_w;
        s2_w[i_valid] = estimateS2(i, e_1_w, a_w);
        //   s2_w_out[i_valid] = s2_w[i_valid];
    }

    double s2 = sqrt(s2_w[i_valid])/weights_factor;
    bool out_l_1, out_l_2;

    if(s2 >= MAXSIGMA)
        out_l_2 = true;
    else {

        int s_out = outliers.size();
        //     double sigLevel = (1 - confidenceLevel)/10;
        double sigLevel = (1 - confidenceLevel);

        double t_val_1 = t_vals[i - 3 - s_out];
        int df = i - 3 - s_out > 0 ? i - 3 - s_out : 1; // casa
        double t_val_2 = boost::math::ibeta_inv(df/2.0, 0.5, sigLevel);
        t_val_2 = sqrt(df*(1 - t_val_2)/t_val_2);

        double t_exp_i = e_1_w/sqrt(a_w * s2_w[i_1]);
        bool out_l_1 = abs(t_exp_i) > t_val_1;
        out_l_2 = abs(t_exp_i) > t_val_2;
    }
    if(out_l_2) { // casa
        valid_idx.pop_back();
        return added;
    }

    added = true;

    if(!realAdd) {
        valid_idx.pop_back();
        return added;
    }

    if(out_l_1) {
        neglectOutlier(i);
        QVector<int> l_o(3);
        l_o[0] = 0;
        l_o[1] = side;
        l_o[0] = 1;
        outliers.push_back(l_o); // 0 por ahora
    }

    if(current_side == Right) {
        if(!passed360)
            setBreakPoint(transform_i(break_point_right + 1, &passed360), Right);
        else
            setBreakPoint(transform_i(break_point_right + 1), Right);
    }
    else {
        if(!passed360)
            setBreakPoint(transform_i(break_point_left - 1, &passed360), Left);
        else
            setBreakPoint(transform_i(break_point_left - 1), Left);
    }
    
    current_side_mat[i_valid] = current_side;


    return added;
}

void KalmanSegment::setStartEnd(int sensor_start, int sensor_end)
{
    sensor_start_org = transform_i(sensor_start);
    sensor_end_org = transform_i(sensor_end);
    this->sensor_start = sensor_start_org;
    this->sensor_end = sensor_end_org;
    this->end_right = false;
    this->end_left = false;
    this->normal_end_right = false;
    this->normal_end_left = false;

    setBreakPoint(sensor_start_org, Left);
    setBreakPoint(sensor_end_org, Right);
    idx_passed360 = -1;

    limited = true; // limitan a pesar de full360, para chow  // nota no se usa limited en el archivo
    outliers.clear();
    last_outliers.clear();
}

void KalmanSegment::setAutoStartEnd(int sensor_start, int sensor_end)
{
    this->sensor_start = transform_i(sensor_start);
    this->sensor_end = transform_i(sensor_end);
    setBreakPoint(this->sensor_start, Left);
    setBreakPoint(this->sensor_end, Right);
    auto_limited = true;
}


void KalmanSegment::initKalman(double rho_0, double phi_0, QMatrix2x2 S_0, double s2_0, double weights_factor)
{
    this->rho_0 = rho_0;
    this->phi_0 = phi_0;
    beta_0.first = rho_0;
    beta_0.second = phi_0;
    this->s2_0 = s2_0;
    S0 = S_0;
    this->weights_factor = weights_factor;
    isInit = true;
    end_right = false;
    end_left = false;
    normal_end_right = false;
    normal_end_left = false;
}

void KalmanSegment::initValidIdx(void )
{
    valid_idx.resize(maxSensorsSegment);
    for(int i = 0; i < maxSensorsSegment; i++) {
        valid_idx[i] = i;
    }
}

void KalmanSegment::unlimite(void) {

    auto_limited = false;
    sensor_start = sensor_start_org;
    sensor_end = sensor_end_org;

}

void KalmanSegment::look4Limits(int i)
{

    bool limit = limitCondition(i);

    if(limit) {  ///nuevo
        QVector<int>::iterator it = valid_idx.begin();
        i = i >= 2 ? i - 1 : i;
        QVector<int>::iterator ite = it + i;
        int max_ang = -1;
        int min_ang = numSensors;
        if(passed360) { /// segmentos menos de 180 grados
            for(; it <= ite; ++it) {
                if(idx_r_ang[*it] > numSensors / 2 && idx_r_ang[*it] <= min_ang)
                    min_ang = idx_r_ang[*it];
                if(idx_r_ang[*it] < numSensors / 2 && idx_r_ang[*it] >= max_ang)
                    max_ang = idx_r_ang[*it];
            }

        }
        else {
            for(; it <= ite; ++it) {
                if(idx_r_ang[*it] <= min_ang)
                    min_ang = idx_r_ang[*it];
                if(idx_r_ang[*it] >= max_ang)
                    max_ang = idx_r_ang[*it];
            }
        }
        setAutoStartEnd(min_ang, max_ang);
        end_left = true;
        end_right = true;
    }

}

bool KalmanSegment::limitCondition(int i)
{
    bool limit = false;
    double s2 = sqrt(s2_w_out[valid_idx[i]])/weights_factor;
    if(long_seed <= 2 && i == long_seed - 1)
        limit = true;
    else if((i >= maxNoMeasurePoints) || (s2 >= MAXSIGMA/* && i > long_seed - 1*/)) //////// ehhhh
        limit = true;
    
    if(s2 >= MAXSIGMA/* && i > long_seed - 1*/)
        neglectOutlier(i);
    return limit;
}

void KalmanSegment::clean(bool full)
{

    //   valid_idx.clear();
    auto_limited = false;
    limited = false;
    isInit = false;
    next_outlier_cnt_right = 0;
    next_outlier_found_right = false;
    next_outlier_cnt_left = 0;
    next_outlier_found_left = false;
    end_right = false;
    end_left = false;
    normal_end_right = false;
    normal_end_left = false;

    //   if(!full)
    //     return;

    //   valid_idx.resize(numSensors);
    outliers.clear();
    last_outliers.clear();
    passed360 = false;

    sensor_start = 0;
    sensor_end = numSensors - 1;
    sensor_start_org = sensor_start;
    sensor_end_org = sensor_end;
    break_point_right = sensor_end;
    break_point_left = sensor_start;
    prev_break_point_right = break_point_right;
    prev_break_point_left = break_point_left;
    break_i = sensor_end;

}

char* KalmanSegment::print(int *numElements) {

    s_print = "";
    s_print += "Idx " + QString::number(getClassIdx()) + "\n";
    s_print += "left " + QString::number(getBreakPointLeft()) + "   right " + QString::number(getBreakPointRight()) + "   num " +  QString::number(getNumElems()) + "\n";
    s_print += "rho_0 " + QString::number(getRho_0()) + "   phi_0 " + QString::number(getPhi_0()*180/M_PI) + "\n";
    s_print += "rho_est " + QString::number(getRhoEst()) + "   phi_est " + QString::number(getPhiEst()*180/M_PI) + "\n";
    s_print += "sigma_est " + QString::number(getSigmaEst()*1000) + "\n";

    char* ch = s_print.toLatin1().data();
    if(numElements)
        *numElements = getNumElems();
    return ch;
}
