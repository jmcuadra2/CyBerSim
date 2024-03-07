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
#ifndef KALMANSEGMENT_H
#define KALMANSEGMENT_H

#include <cmath>
#include <utility>
#include <QVector>
#include <QString>

#include "../neuraldis/matriz.hpp"
#include <QGenericMatrix>

using namespace std;

/**
  @author Jose Manuel Cuadra Troncoso <jmcuadra@dia.uned.es>
*/
class KalmanSegment
{
    friend class KalmanSet;
public:
    enum Side {Right, Left, notSet = -1};
    
public:
    KalmanSegment(const int& numSensors, double* radius, double* angle,
                  const bool& full360, const bool& twoStage = true);

    ~KalmanSegment();

    void setTValues(double * t_vals, double confidence);

    void setRadius(double* radius) { this->radius = radius; }

    void estimate2PWKalman(const int& ang_0, const int& ang_n, const bool& break_if_outl);
    void reEstimate2PWKalman(int continueStart, int continueEnd);
    void reEstimate2PWKalman(void);
    bool addPoint(Side side, bool realAdd = true);

    void initValidIdx(void);
    template <typename T>
    inline void extractIdx(QVector <T>& org_vect, const QVector<int>& idx)
    {
        QVector<T> new_vect(idx.size());
        typename QVector<T>::iterator pos = org_vect.begin();
        typename QVector<T>::iterator pos_new = new_vect.begin();
        QVector<int>::const_iterator pos_idx = idx.constBegin();

        for(; pos_idx < idx.constEnd(); ++pos_idx, ++pos_new) {
            *pos_new = *(pos + *pos_idx);
        }
        org_vect.clear();
        org_vect = new_vect;

    }

    int getBreakPointRight() const
    {
        return break_point_right;
    }

    int getBreakPointLeft() const
    {
        return break_point_left;
    }

    bool isCollapsed(void) const
    {
        bool ret = false;
        ret = (valid_idx.size() == 0 || break_point_left == -1 || break_point_right == -1);
        return ret;
    }

    void setClassIdx(int classIdx)
    {
        this->classIdx = classIdx;
    }
    

    int getClassIdx() const
    {
        return classIdx;
    }

    int getNumElems() const
    {
        return valid_idx.size();
    }

    double getRho_0() const
    {
        return rho_0;
    }

    double getPhi_0() const
    {
        return phi_0;
    }

    double getRhoEst() const
    {
        return rho_0_est;
    }

    double getPhiEst() const
    {
        return phi_0_est;
    }

    double getSigmaEst() const
    {
        return sigma_est;
    }

    char* print(int *numElements = 0);

protected:
    static const double RAD2GRAD;
    static const double MAXSIGMA;

    int transform_i(const int& idx, bool* l_passed360 = (bool*)0);
    QVector<int> transform_i(const int& idx_1, const int& idx_2,
                             bool* l_passed360 = (bool*)0);
    void setIdxOrd(void);
    
    double initKalmanSeed(void);
    void includeSeed(void);
    
    pair<double, double> estimate(const int& i);
    double ordinaryKalman(const double& rad, double ang, pair<double, double>& beta_prevest,
                          QMatrix2x2& S_prev, double* a);
    double weightedKalman(const double& rad, double ang, pair<double, double>& beta_prevest,
                          QMatrix2x2& S_prev, double* a_w, const double& weight);
    
    void neglectOutlier(const int& i);
    void test2Outlier(int& i,/* const */int& i_r_ang, const bool& out_l, int& num_points,
                      bool * continueK); // casa
    void cleanOutliers(bool alsoEnds);
    
    void erasePoint(const int& idx);
    void erasePoints(void);

    void repeatedMedianPol(QVector<double>& r_seed, QVector<double>& ang_seed);
    double median(QVector<double>& data);
    
    double estimateS2_0(QVector<double>& r_seed, QVector<double>& ang_seed, int *max_i);
    double estimateS2(const int& i, const double& e_w, const double& a_w);
    
    void setBreakPoint(int break_point, const Side&  side);
    void setUserBreakPoint(int break_point, const Side&  side);
    void restoreBreakPoint(const Side&  side);

    void setStartEnd(int sensor_start, int sensor_end);
    void setAutoStartEnd(int sensor_start, int sensor_end);
    void unlimite(void);
    void look4Limits(int i);
    bool limitCondition(int i);
    
    void initKalman(double rho_0, double phi_0, QMatrix2x2 S_0, double s2_0,
                    double weights_factor);

    void clean(bool full = false);
    void performKalman(const bool& break_if_outl);

protected:
    int classIdx;
    double* radius, * angle;
    int sensor_start, sensor_end, sensor_start_org, sensor_end_org, numSensors, maxSensorsSegment;
    double sensorAperture;
    bool limited, auto_limited;
    double rho_0, phi_0, rho_0_est, phi_0_est, sigma_est;
    
    QVector<int> idx_r_ang;
    QVector<int> inverse_idx_r_ang;
    QVector<int> valid_idx;
    Side current_side;
    QVector<Side> current_side_mat;
    int maxNoMeasurePoints;

    QVector< pair <double, double> > beta_est;
    QVector< QMatrix2x2 > S;
    QMatrix2x2 var_beta_est;
    
    QVector <double> weights;
    double weights_factor;
    QVector< pair <double, double> > beta_est_w;
    QVector< QMatrix2x2 > S_w;
    QVector <double> sum_e2_w;
    QVector <double> sum_e2_w_out;
    QMatrix2x2 var_beta_est_w;
    QVector <double> s2_w;
    QVector <double> s2_w_out;

    pair< double, double > beta_prevest;
    QMatrix2x2 S_prev;
    pair< double, double > beta_prevest_w;
    QMatrix2x2 S_prev_w;

    //     QVector <double> t_exp;
    //     QVector <double> t_right;
    //     QVector <double> t_left;
    //     QVector <double> t_vals;
    
    bool end_right, end_left, normal_end_right, normal_end_left;
    int break_point_right, break_point_left, prev_break_point_right,  prev_break_point_left, break_i;

    QVector < QVector <int> > outliers;
    QVector < QVector <int> > last_outliers;
    int next_outlier_cnt_right, next_outlier_cnt_left;
    bool next_outlier_found_right, next_outlier_found_left;
    QVector<double> out_sum_e2;
    double last_sum_e2;
    double confidenceLevel;

    pair <double, double> beta_0;
    QMatrix2x2 S0;
    double s2_0;
    
    int ang_0, ang_n, init_ang_0, init_ang_n;
    QVector <int> idx_ord;
    int minim;
    int long_seed;
    bool isInit, include_seed;
    bool full360, passed360;
    int idx_passed360;

    double* t_vals;

    QString s_print;
    
    bool twoStage;

};

#endif
