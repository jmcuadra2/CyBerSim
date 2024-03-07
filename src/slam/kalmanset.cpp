/***************************************************************************
 *   Copyright (C) 2010 by Jose Manuel Cuadra Troncoso   *
 *   jmcuadra@dia.uned.es   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 2 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program; if not, write to the
 *   Free Software Foundation, Inc.,
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 ***************************************************************************/

#include <iostream>
#include <boost/math/special_functions/beta.hpp>
#include <boost/math/distributions/fisher_f.hpp>
#include <boost/math/distributions/chi_squared.hpp>

using boost::math::chi_squared;
using boost::math::quantile;

#include "kalmanset.h"
#include "../neuraldis/timestamp.hpp"

using boost::math::beta;
using boost::math::fisher_f;

const double KalmanSet::RAD2GRAD = 180.0/M_PI;

KalmanSet::KalmanSet(size_t numSensors, double sensorField, double probabil, double startAngle, const bool& twoStage)
{
    semilong = 4; // semillas de 9 puntos
    this->numSensors = numSensors;
    if(sensorField == 360)
        full360 = true;
    else
        full360 = false;
    
    this->sensorField = sensorField / RAD2GRAD;
    if(full360) {
        sensorAperture = (double)this->sensorField / this->numSensors;
        this->startAngle = -M_PI + sensorAperture;
    }
    else {
        sensorAperture = (double)this->sensorField / (this->numSensors - 1);
        this->startAngle = startAngle/RAD2GRAD;
    }
    
    angle = new double[numSensors];
    for(uint i = 0; i < numSensors; i++)
        angle[i] = i*sensorAperture + this->startAngle;
    
    radius = new double[numSensors];

    confidenceLevel = (1 + probabil)/2;
    t_values();

    verbose = false;
    numMeasures = 0;

    this->twoStage = twoStage;

}

KalmanSet::~KalmanSet()
{
    delete[] angle;
    delete[] radius;
    int s_n = segmentsStore.size();
    for(int i = 0; i < s_n; i++)
        delete segmentsStore[i];
    segmentsStore.clear();
    segments.clear();

    //   QVector<KalmanSegment>::iterator it = segmentsStore.begin();
    //   int cnt = 0;
    //   while(cnt < segmentsStore.size()) {
    //     if(segmentsStore[cnt].classIdx <= 100) {
    //       segmentsStore.erase(it + cnt);
    //     }
    //     else
    //       cnt++;
    //  }
}

void KalmanSet::createSegments(void)
{
    maxNumSegments = numSensors/6;
    currentSegments.resize(maxNumSegments);
    usedSegments.resize(maxNumSegments);
    for(int i = 0; i < maxNumSegments; i++) {
        KalmanSegment* segment = new KalmanSegment(numSensors, radius, angle, full360, twoStage);
        segment->setClassIdx(i);
        segment->setTValues(t_vals.data(), confidenceLevel);
        segmentsStore.push_back(segment);
    }
}

void KalmanSet::calculateLimits(int i, int j, int* left, int* right)
{
    tooMuchMeasures = false;
    uint end_i = seeds.size();
    pair<int, int> p = measuredAreas[j];
    if(full360) {
        if(i == 0) {
            *left = max(this->seeds.back().first, p.first);
            *right = end_i > 1 ? min(this->seeds[1].second, p.second) : seeds.back().second;
        }
        else if(i == end_i - 1) {
            *left = end_i > 1 ? max(this->seeds[end_i - 2].first, p.first) : max(seeds.back().first, p.first);
            *right = min(this->seeds[0].second, p.second);
        }
        else{
            *left = max(this->seeds[i - 1].first, p.first);
            *right = min(this->seeds[i + 1].second, p.second);
        }
    }
    else {
        if(i <= 1) {
            *left = max(0, p.first);
            //       *right = end_i > 1 ? min(this->seeds[i + 1].second, p.second) : min(seeds.back().second, p.second);
            *right = i + 1 < end_i ? min(this->seeds[i + 1].second, p.second) : min(seeds.back().second, p.second);
            if(*left == 97)
                int foo = 0;
        }
        else if(i >= end_i - 2) {
            //       *left = end_i > 1 ? max(this->seeds[i - 1].first, p.first) : max(seeds.back().first, p.first);
            *left = i > 0 ? max(this->seeds[i - 1].first, p.first) : max(seeds.back().first, p.first);
            *right = min((int)numSensors - 1, p.second);
            if(*left == 97)
                int foo = 0;
        }
        else{
            *left = max(this->seeds[i - 1].first, p.first);
            *right = min(this->seeds[i + 1].second, p.second);
            int l = this->seeds[i].first;
            int r = this->seeds[i].second;
            if(*left == 97)
                int foo = 0;
        }
    }
}

void KalmanSet::doEstimation(QVector< pair < int , int > > measuredAreas, QVector< pair < int , int > > seeds, bool verbose)
{
    uint end_i = seeds.size();
    int end_j = measuredAreas.size();
    if(!end_i || !end_j)
        return;

    this->measuredAreas = measuredAreas;
    this->seeds = seeds;
    this->verbose = verbose;
    bool break_if_outl = true;
    int left = 0, right = 0;
    int j = 0;
    numMeasures = 0;
    int start, end;
    for(; j < end_j; j++) {
        start = measuredAreas[j].first;
        end = measuredAreas[j].second;
        if(start <= end)
            numMeasures += end - start + 1;
        else
            numMeasures += numSensors - start + end + 1;
    }

    j = 0;
    for(uint i = 0; i < end_i; i++) {
        if(i == 11)
            int f = 0;
        segments.push_back(getNewSegment());
        pair<int, int> p = measuredAreas[j];
        if(seeds[i].first > p.second && j < end_j - 1) // && ->por las passed360
            j++;
        calculateLimits(i, j, &left, &right);
        //     segments[i]->setClassIdx(i); // casa
        segments[i]->setStartEnd(left, right);
        segments[i]->estimate2PWKalman(seeds[i].first, seeds[i].second, break_if_outl);

        //     if(!segments[i]->valid_idx.size()) {
        //       removeSimpleOverlap(&i, KalmanSegment::Left);
        //     }
    }

    if(verbose) {
        printResults();
    }

    if(full360) {
        QVector<KalmanSegment*>::iterator it = segments.begin();
        QVector< pair < int , int > >::iterator its = this->seeds.begin();
        KalmanSegment* seg;
        pair < int , int > seed;
        int cnt = 0;
        int cnt_num2 = 0;
        if(end_i > 1) {
            for(uint i = 0; i < end_i; i++) {
                seg = *(it + i);
                seed = *(its + i);
                if(seg->break_point_left >= numSensors/2) {
                    segments.erase(it + i);
                    this->seeds.erase(its + i);
                    segments.push_back(seg);
                    this->seeds.push_back(seed);
                    cnt_num2++;
                    if(cnt_num2 == end_i - 1) // el último sale
                        break;
                    i--;
                }
                else {
                    cnt++;
                    if(cnt > end_i/10 + 1) // para no pasarse y no terminar nunca el bucle, repensar condicion
                        break;
                }
            }
        }
    }

    if(verbose) {
        printResults();
    }

    resolveSimpleOverlaps();
    look4Corners();  //casa
    look4Holes();

    //   orderSegments();
    if(verbose) {
        printResults();
    }
    
    resolveOverlaps();
    if(verbose) {
        cout << endl << "Tras resolveOverlaps" << endl;
        printResults();
    }


    KalmanSegment* seg;
    int total_lenght = 0;
    int segment_lenght = 0;
    end_i = segments.size();
    for(int i = 0; i < end_i; i++) {
        seg = segments[i];
        if(!seg->passed360)
            segment_lenght = seg->break_point_right - seg->break_point_left + 1;
        else
            segment_lenght = numSensors - seg->break_point_left + seg->break_point_right + 1;
        total_lenght += segment_lenght;
    }

    if(total_lenght > numMeasures) {
        tooMuchMeasures = true;
        cout << "Error total_lenght "<< total_lenght << " > " << "numMeasures "<< numMeasures << endl;
        //    printResults();
    }

    reEstimate();

    end_i = segments.size();
    int cnt = 0;
    do {

        total_lenght = 0;
        end_i = segments.size();
        for(int i = 0; i < end_i; i++) {
            seg = segments[i];
            if(!seg->passed360)
                segment_lenght = seg->break_point_right - seg->break_point_left + 1;
            else
                segment_lenght = numSensors - seg->break_point_left + seg->break_point_right + 1;
            total_lenght += segment_lenght;
        }
        if(total_lenght < numMeasures && end_i > 0)
            look4HolesStrict();
        cnt++;
    } while(total_lenght < numMeasures && cnt < 3);

    if(total_lenght != numMeasures && verbose) {
        cout << "Error total_lenght "<< total_lenght << " != " << "numMeasures "<< numMeasures << endl;
        //        tooMuchMeasures = true;
        //     for(int i = 0; i < end_i; i++) {
        //       seg = segments[i];
        //       endPoints[i].first = seg->break_point_left;
        //       endPoints[i].second = seg->break_point_right;
        //       rhoEst[i] = seg->getRhoEst();
        //       phiEst[i] = seg->getPhiEst();
        //       sigmaEst[i] = seg->getSigmaEst();
        //       Sw[i] = seg->var_beta_est_w;
        //
        //       if(!seg->passed360)
        // 	segment_lenght = seg->break_point_right - seg->break_point_left + 1;
        //       else
        // 	segment_lenght = numSensors - seg->break_point_left + seg->break_point_right + 1;
        //       if(segment_lenght != seg->getNumElems()) {
        // 	cout << "Error segment_lenght "<< segment_lenght << " != " << "num_points "<< seg->getNumElems() << endl;
        // 	cout << seg->print();
        //       }
        //     }

    }

    breakShortSegments();
    equalityTest();


    //   end_i = possible_corners.size();
    //   for(int i = 0; i < end_i; i++) {
    //     if(!possible_corners[i]) {
    //       int i_1 = full360 && i == end_i - 1 ? 0 : i + 1;
    //       if(segments[i]->addPoint(KalmanSegment::Right, false))
    //         possible_corners[i] = true;
    //       if(segments[i_1]->addPoint(KalmanSegment::Left, false))
    //         possible_corners[i] = true;
    //     }
    //   }

    if(verbose) {
        cout << endl << "Tras merging y breakShortSegments" << endl;
        printResults();
        cout << endl << endl;
    }

    end_i = segments.size();
    endPoints.resize(end_i);
    rhoEst.resize(end_i);
    phiEst.resize(end_i);
    sigmaEst.resize(end_i);
    Sw.resize(end_i);

    total_lenght = 0;
    for(int i = 0; i < end_i; i++) {
        seg = segments[i];
        endPoints[i].first = seg->break_point_left;
        endPoints[i].second = seg->break_point_right;
        rhoEst[i] = seg->getRhoEst();
        phiEst[i] = seg->getPhiEst();
        sigmaEst[i] = seg->getSigmaEst();
        Sw[i] = seg->var_beta_est_w;

        if(!seg->passed360)
            segment_lenght = seg->break_point_right - seg->break_point_left + 1;
        else
            segment_lenght = numSensors - seg->break_point_left + seg->break_point_right + 1;
        if(segment_lenght != seg->getNumElems() && verbose) {
            cout << "Error segment_lenght "<< segment_lenght << " != " << "num_points "<< seg->getNumElems() << endl;
            cout << seg->print();
        }

        total_lenght += segment_lenght;
    }

    if(total_lenght != numMeasures) {
        cout << "FINAL ERROR total_lenght "<< total_lenght << " != " << "numMeasures "<< numMeasures << endl;

    }
    //   printResults();

}

void KalmanSet::setRadius(double radius[])
{
    for(uint i = 0; i < numSensors; i++)
        this->radius[i] = radius[i];
}

double* KalmanSet::t_values(void)
{
    t_vals.clear();
    for(uint i = KalmanSegment::Left ; i <= numSensors; i++) {
        double x = boost::math::ibeta_inv(i/2.0, 0.5, 1 - confidenceLevel); // bilateral, no 2*(1-confidenceLevel)
        t_vals.push_back(sqrt(i*(1 - x)/x));
    }
    return t_vals.data();
}

void KalmanSet::look4Corners(void)
{

    //   QVector<int> idx; // casa
    int i_idx;
    int num_overlaps;
    if(full360) {
        num_overlaps = segments.size();
        num_overlaps = num_overlaps == 1 ? 0 : num_overlaps;
    }
    else
        num_overlaps = segments.size() - 1;

    idx_1.resize(num_overlaps);
    idx_2.resize(num_overlaps);
    idx_10.resize(num_overlaps);
    idx_20.resize(num_overlaps);
    idx_11.resize(num_overlaps);
    idx_21.resize(num_overlaps);
    possible_corners.resize(num_overlaps);

    for(int i = 0; i < num_overlaps; i++) {

        if(i == num_overlaps - 1 && full360)
            i_idx = 0;
        else
            i_idx = i + 1;

        bool overlap;
        //     idx.clear();
        //     overlap =  overlapped(segments[i], segments[i_idx]);
        //     overlap =  idxOverlap(segments[i], segments[i_idx], i, idx, idx_1[i], idx_10[i], idx_11[i], idx_2[i], idx_20[i], idx_21[i]);
        int end_left = 0, end_right = 0;
        overlap =  idxOverlap(segments[i], segments[i_idx], i, idx_1[i], idx_2[i], &end_left, &end_right);

        if(!overlap) {  // casa

            if(verbose)
                printf("Segmento %d (%d) separado de segmento %d (%d)\n", segments[i]->getClassIdx(), segments[i]->break_point_right, segments[i_idx]->getClassIdx(), segments[i_idx]->break_point_left);
            possible_corners[i] = false;
        }
        else {
            if(verbose)
                printf("Corner entre segmento %d (%d) y segmento %d (%d)\n", segments[i]->getClassIdx(), end_right, segments[i_idx]->getClassIdx(), end_left);
            possible_corners[i] = true;
        }
    }

}

bool KalmanSet::overlapped(KalmanSegment* segment_1, KalmanSegment*  segment_2, int* max_left, int* min_right)
{
    bool overlap = false;
    //   if(full360 && ((segment_1->break_point_right > numSensors/2 && segment_1->break_point_left > numSensors/2 && segment_2->break_point_left < numSensors/2 && segment_2->break_point_right < numSensors/2))) {
    if(full360 && ((segment_1->break_point_right > numSensors/2 && segment_1->break_point_left > numSensors/2 && segment_2->break_point_left < segment_1->break_point_left - numSensors/2))) {
        *max_left = segment_2->break_point_left;
        *min_right = segment_1->break_point_right;
    }
    else if(full360 && ((segment_2->break_point_right < numSensors && segment_2->break_point_left > numSensors/2 && segment_1->break_point_left < segment_2->break_point_left - numSensors/2))) {
        *max_left = segment_1->break_point_left;
        *min_right = segment_2->break_point_right;
    }
    else if(segment_1->passed360 && segment_2->passed360) {
        *max_left = max(segment_1->break_point_left, segment_2->break_point_left);
        *min_right = min(segment_1->break_point_right, segment_2->break_point_right);
        overlap = true;
    }
    else if(segment_1->passed360) {
        int limit = segment_1->break_point_left - numSensors / 2;
        if(segment_2->break_point_left > limit) {
            *max_left = max(segment_1->break_point_left, segment_2->break_point_left);
            *min_right = segment_2->break_point_right;
        }
        else {
            *max_left = segment_2->break_point_left;
            *min_right = min(segment_1->break_point_right, segment_2->break_point_right);
        }
        overlap = *max_left <= *min_right;
    }
    else if(segment_2->passed360) {
        int limit = segment_2->break_point_left - numSensors / 2;
        if(segment_1->break_point_left > limit) {
            *max_left = max(segment_1->break_point_left, segment_2->break_point_left);
            *min_right = segment_1->break_point_right;
        }
        else {
            *max_left = segment_1->break_point_left;
            *min_right = min(segment_1->break_point_right, segment_2->break_point_right);
        }
        overlap = *max_left <= *min_right;
    }
    else {
        *max_left = max(segment_1->break_point_left, segment_2->break_point_left);
        *min_right = min(segment_1->break_point_right, segment_2->break_point_right);
        overlap = *max_left <= *min_right;
    }
    return overlap;
}


bool KalmanSet::overlapped(int numSensors, bool full360, pair<int, int> segment_1, pair<int, int> segment_2, int* max_left, int* min_right)
{
    bool overlap = false;
    bool s1_passed360 = segment_1.first > segment_1.second;
    bool s2_passed360 = segment_2.first > segment_2.second;
    //   if(full360 && ((segment_1.second > numSensors/2 && segment_1.first > numSensors/2 && segment_2.first < numSensors/2 && segment_2.second < numSensors/2))) {
    if(full360 && ((segment_1.second > numSensors/2 && segment_1.first > numSensors/2 && segment_2.first < segment_1.first - numSensors/2))) {
        *max_left = segment_2.first;
        *min_right = segment_1.second;
    }
    else if(full360 && ((segment_2.second < numSensors && segment_2.first > numSensors/2 && segment_1.first < segment_2.first - numSensors/2))) {
        *max_left = segment_1.first;
        *min_right = segment_2.second;
    }
    else if(s1_passed360 && s2_passed360) {
        *max_left = max(segment_1.first, segment_2.first);
        *min_right = min(segment_1.second, segment_2.second);
        overlap = true;
    }
    else if(s1_passed360) {
        int limit = segment_1.first - numSensors / 2;
        if(segment_2.first > limit) {
            *max_left = max(segment_1.first, segment_2.first);
            *min_right = segment_2.second;
        }
        else {
            *max_left = segment_2.first;
            *min_right = min(segment_1.second, segment_2.second);
        }
        overlap = *max_left <= *min_right;
    }
    else if(s2_passed360) {
        int limit = segment_2.first - numSensors / 2;
        if(segment_1.first > limit) {
            *max_left = max(segment_1.first, segment_2.first);
            *min_right = segment_1.second;
        }
        else {
            *max_left = segment_1.first;
            *min_right = min(segment_1.second, segment_2.second);
        }
        overlap = *max_left <= *min_right;
    }
    else {
        *max_left = max(segment_1.first, segment_2.first);
        *min_right = min(segment_1.second, segment_2.second);
        overlap = *max_left <= *min_right;
    }
    return overlap;
}

bool KalmanSet::idxOverlap(KalmanSegment * segment_1, KalmanSegment * segment_2, const int & cnt_seg, QVector< int > & idx_i1, QVector< int > & idx_i2, int* end_left, int* end_right) // end_left, end_right solo para mostrar mensaje en pantalla para depuracion en printf("Corners ...)
{
    int max_left = 0, min_right = 0;
    int size_1 = segment_1->valid_idx.size();
    int size_2 = segment_2->valid_idx.size();
    int total_size_1 = size_1;
    int total_size_2 = size_2;

    if(!size_1)
        int h = 0;

    idx_i1.clear();
    idx_i2.clear();

    QVector<int> idx_i10;
    QVector<int> idx_i11;
    QVector<int> idx_i20;
    QVector<int> idx_i21;

    bool overlap = overlapped(segment_1, segment_2, &max_left, &min_right);

    //   if(segment_1->passed360 && segment_2->passed360) {
    // //     if(((segment_1->break_point_left >= segment_2->break_point_left && segment_1->break_point_right < segment_2->break_point_right) || (segment_1->break_point_left > segment_2->break_point_left && segment_1->break_point_right <= segment_2->break_point_right)) || ((segment_1->break_point_left <= segment_2->break_point_left && segment_1->break_point_right > segment_2->break_point_right) || (segment_1->break_point_left < segment_2->break_point_left && segment_1->break_point_right >= segment_2->break_point_right)))
    // //       overlap = false;
    // //     else {
    //       max_left = max(segment_1->break_point_left, segment_2->break_point_left);
    //       min_right = min(segment_1->break_point_right, segment_2->break_point_right);
    //       overlap = true;
    // //     }
    //   }
    //   else if(segment_1->passed360) {
    //     int limit = segment_1->break_point_left - numSensors / 2;
    //     if(segment_2->break_point_left > limit) {
    //       max_left = max(segment_1->break_point_left, segment_2->break_point_left);
    //       min_right = segment_2->break_point_right;
    //     }
    //     else {
    //       max_left = segment_2->break_point_left;
    //       min_right = min(segment_1->break_point_right, segment_2->break_point_right);
    //     }
    //     overlap = max_left <= min_right;
    //   }
    //   else if(segment_2->passed360) {
    //     int limit = segment_2->break_point_left - numSensors / 2;
    //     if(segment_1->break_point_left > limit) {
    //       max_left = max(segment_1->break_point_left, segment_2->break_point_left);
    //       min_right = segment_1->break_point_right;
    //     }
    //     else {
    //       max_left = segment_1->break_point_left;
    //       min_right = min(segment_1->break_point_right, segment_2->break_point_right);
    //     }
    //     overlap = max_left <= min_right;
    //   }
    //   else {
    //     max_left = max(segment_1->break_point_left, segment_2->break_point_left);
    //     min_right = min(segment_1->break_point_right, segment_2->break_point_right);
    //     overlap = max_left <= min_right;
    //   }

    if(end_left != 0)
        *end_left = max_left;
    if(end_right != 0)
        *end_right = min_right;

    if(overlap) {
        int i_r_ang;
        if(segment_1->passed360 && segment_2->passed360) {
            for(int i = 0; i < size_1; i++) {
                i_r_ang = segment_1->idx_r_ang[segment_1->valid_idx[i]];
                if(i_r_ang >= max_left || i_r_ang  <= min_right) {
                    if(segment_1->current_side_mat[segment_1->valid_idx[i]] == KalmanSegment::Right)
                        idx_i10.push_back(segment_1->valid_idx[i]);
                    else
                        idx_i11.push_back(segment_1->valid_idx[i]);
                }
            }

            for(int i = 0; i < size_2; i++) {
                i_r_ang = segment_2->idx_r_ang[segment_2->valid_idx[i]];
                if(i_r_ang >= max_left || i_r_ang  <= min_right) {
                    if(segment_2->current_side_mat[segment_2->valid_idx[i]] == KalmanSegment::Right)
                        idx_i20.push_back(segment_2->valid_idx[i]);
                    else
                        idx_i21.push_back(segment_2->valid_idx[i]);
                }
            }

        }
        else {
            //       for(int i = 0; i < size_1; i++)  // casa
            //         cout << segment_1->idx_r_ang[segment_1->valid_idx[i]] << " ";
            //       cout << endl;
            //       for(int i = 0; i < size_2; i++)
            //         cout << segment_2->idx_r_ang[segment_2->valid_idx[i]] << " ";
            //       cout << endl;

            for(int i = 0; i < size_1; i++) {
                i_r_ang = segment_1->idx_r_ang[segment_1->valid_idx[i]];
                if(i_r_ang >= max_left && i_r_ang  <= min_right) {
                    if(segment_1->current_side_mat[segment_1->valid_idx[i]] == KalmanSegment::Right)
                        idx_i10.push_back(segment_1->valid_idx[i]);
                    else
                        idx_i11.push_back(segment_1->valid_idx[i]);
                }
            }

            for(int i = 0; i < size_2; i++) {
                i_r_ang = segment_2->idx_r_ang[segment_2->valid_idx[i]];
                if(i_r_ang >= max_left && i_r_ang  <= min_right) {
                    if(segment_2->current_side_mat[segment_2->valid_idx[i]] == KalmanSegment::Right)
                        idx_i20.push_back(segment_2->valid_idx[i]);
                    else
                        idx_i21.push_back(segment_2->valid_idx[i]);
                }
            }
        }

        int change1 = idx_i11.size();
        int change2 = idx_i21.size();
        size_1 = change1 + idx_i10.size();
        size_2 = change2 + idx_i20.size();

        if(size_1 != size_2) {
            printf("Tamaños distintos %d, %d\n", cnt_seg , cnt_seg + 1);

            cout << "Segment " << segment_1->classIdx << "-> ";  // casa
            for(int j = 0; j < segment_1->valid_idx.size(); j++)
                cout << segment_1->idx_r_ang[segment_1->valid_idx[j]] << " ";
            cout << endl;
            cout << "Segment " << segment_2->classIdx << "-> ";
            for(int j = 0; j < segment_2->valid_idx.size(); j++)
                cout << segment_2->idx_r_ang[segment_2->valid_idx[j]] << " ";

            if(total_size_1 == 1) {
                cout << endl << "Removed " << segment_1->getClassIdx();
                removeSegment(cnt_seg, cnt_seg);
                *end_left = -1;
            }
            if(total_size_2 == 1) {
                cout << endl << "Removed " << segment_2->getClassIdx();
                removeSegment(cnt_seg + 1, cnt_seg + 1);
                *end_right = -1;
            }
            cout << endl << endl;

            return false;
        }
        
        for(int i = change1 - 1; i >= 0; i--) {
            idx_i1.push_back(idx_i11[i]);
        }
        for(int i = change1; i < size_1; i++) {
            idx_i1.push_back(idx_i10[i - change1]);
        }

        for(int i = change2 - 1; i >= 0; i--) {
            idx_i2.push_back(idx_i21[i]);
        }
        for(int i = change2; i < size_2; i++) {
            idx_i2.push_back(idx_i20[i - change2]);
        }
    }
    //   size_1 = idx_i1.size();
    //   size_2 = idx_i2.size();
    //   for(j = 0; j < size_1; j++)
    //     cout << idx_i1[j] << " ";
    //   cout << endl;
    //   for(j = 0; j < size_1; j++)
    //     cout << segment_1->idx_r_ang[idx_i1[j]] << " ";
    //   cout << endl;
    //   for(j = 0; j < size_2; j++)
    //     cout << idx_i2[j] << " ";
    //   cout << endl;
    //   for(j = 0; j < size_1; j++)
    //     cout << segment_2->idx_r_ang[idx_i2[j]] << " ";
    //   cout << endl;
    //   cout << endl;
    return overlap;
}

// bool KalmanSet::idxOverlap(KalmanSegment* segment_1 , KalmanSegment* segment_2 , const int& cnt_seg, QVector<int>& idx, QVector<int>& idx_i1, QVector<int>& idx_i10, QVector<int>& idx_i11, QVector<int>& idx_i2, QVector<int>& idx_i20, QVector<int>& idx_i21)
// {
// 
//   bool overlap = false;
//   bool passed_1 = false;
//   bool passed_2 = false;
//   int max_left, min_right;
//   int size_1 = segment_1->valid_idx.size();
//   int size_2 = segment_2->valid_idx.size();
//   int j;
// 
//   idx_i1.clear();
//   idx_i2.clear();
//   idx_i10.clear();
//   idx_i11.clear();
//   idx_i20.clear();  
//   idx_i21.clear();
//   
//   if(segment_1->passed360 && segment_2->passed360) {
//     max_left = max(segment_1->break_point_left, segment_2->break_point_left);
//     min_right = min(segment_1->break_point_right, segment_2->break_point_right);
//     
//     j = 0;
//     while(j < size_1) {
//       if(segment_1->idx_r_ang[segment_1->valid_idx[j]] > min_right)
//         passed_1 = true;
//       j++;
//     }
// 
//     j = 0;
//     while(j < size_2) {
//       if(segment_2->idx_r_ang[segment_2->valid_idx[j]] < max_left)
//         passed_2 = true;
//       j++;        
//     }    
// 
//     for(int j = max_left; j <= segment_2->sensor_end; j++) 
//       idx.push_back(j);
//     for(int j = segment_1->sensor_start; j <= min_right; j++) 
//       idx.push_back(j);      
//   }
//   else if(segment_1->passed360) {
//     max_left = segment_2->break_point_left;
//     min_right = min(segment_1->break_point_right, segment_2->break_point_right);
// 
//     j = 0;
//     while(j < size_1) {
//       if(segment_1->idx_r_ang[segment_1->valid_idx[j]] > min_right) {
//         passed_1 = true;
//         break;
//       }
//       j++;        
//     }
// 
//     for(int j = max_left; j <= min_right; j++)
//       idx.push_back(j);
// 
//   } 
//   else if(segment_2->passed360) {
//     max_left = max(segment_1->break_point_left, segment_2->break_point_left);
//     min_right = segment_1->break_point_right;
// 
//     j = 0;
//     while(j < size_2) {
//       if(segment_2->idx_r_ang[segment_2->valid_idx[j]] < max_left) {
//         passed_2 = true;
//         break;
//       }        
//       j++;
//     }
// 
//     for(int j = max_left; j <= min_right; j++)
//       idx.push_back(j);
// 
//   }
//   else {
//     max_left = max(segment_1->break_point_left, segment_2->break_point_left);
//     min_right = min(segment_1->break_point_right, segment_2->break_point_right);
// 
// //     for(int i = 0; i < size_1; i++)
// //       cout << segment_1->idx_r_ang[segment_1->valid_idx[i]] << " ";
// //     cout << endl;
// //     for(int i = 0; i < size_2; i++)
// //       cout << segment_2->idx_r_ang[segment_2->valid_idx[i]] << " ";
// //     cout << endl;
// 
//     j = 0;
//     while(j < size_1) {
//       if(segment_1->idx_r_ang[segment_1->valid_idx[j]] > min_right) {
//         passed_1 = true;
//         break;
//       }         
//       j++;        
//     }
// 
//     j = 0;
//     while(j < size_2) {
//       if(segment_2->idx_r_ang[segment_2->valid_idx[j]] < max_left) {
//         passed_2 = true;
//         break;
//       }         
//       j++;        
//     }    
// 
//     for(int j = max_left; j <= min_right; j++)
//       idx.push_back(j);
// 
//   }
// 
// /*  if(passed_1) {
//     segment_1->setUserBreakPoint(min_right, KalmanSegment::Right);
//     segment_2->setUserBreakPoint(min_right, KalmanSegment::Right);
//     size_1 = segment_1->valid_idx.size();
//     size_2 = segment_2->valid_idx.size();
//   }
//   if(passed_2) {
//     segment_1->setUserBreakPoint(max_left, KalmanSegment::Left);
//     segment_2->setUserBreakPoint(max_left, KalmanSegment::Left);
//     size_1 = segment_1->valid_idx.size();
//     size_2 = segment_2->valid_idx.size();    
//   }*/ 
// 
//   if(idx.size()) {
//     overlap = true;
// 
//     if(segment_1->passed360 && segment_2->passed360) {
//       for(int i = 0; i < size_1; i++) {      
//         if(segment_1->idx_r_ang[segment_1->valid_idx[i]] >= idx.front() || segment_1->idx_r_ang[segment_1->valid_idx[i]]  <= idx.back()) {
//           if(segment_1->current_side_mat[segment_1->valid_idx[i]] == KalmanSegment::Right)
//             idx_i10.push_back(segment_1->valid_idx[i]);
//           else
//             idx_i11.push_back(segment_1->valid_idx[i]);
//         }
//       }
// 
//       for(int i = 0; i < size_2; i++) {
//         if(segment_2->idx_r_ang[segment_2->valid_idx[i]] >= idx.front() || segment_2->idx_r_ang[segment_2->valid_idx[i]]  <= idx.back()) {
//           if(segment_2->current_side_mat[segment_2->valid_idx[i]] == KalmanSegment::Right)
//             idx_i20.push_back(segment_2->valid_idx[i]);
//           else
//             idx_i21.push_back(segment_2->valid_idx[i]);
//         }
//       }
//       
//     }
//     else {
// //       for(int i = 0; i < size_1; i++)
// //         cout << segment_1->idx_r_ang[segment_1->valid_idx[i]] << " ";
// //       cout << endl;
// //       for(int i = 0; i < size_2; i++)
// //         cout << segment_2->idx_r_ang[segment_2->valid_idx[i]] << " ";
// //       cout << endl;
//       
//       for(int i = 0; i < size_1; i++) {      
//         if(segment_1->idx_r_ang[segment_1->valid_idx[i]] >= idx.front() && segment_1->idx_r_ang[segment_1->valid_idx[i]]  <= idx.back()) {
//           if(segment_1->current_side_mat[segment_1->valid_idx[i]] == KalmanSegment::Right) 
//             idx_i10.push_back(segment_1->valid_idx[i]);
//           else
//             idx_i11.push_back(segment_1->valid_idx[i]);
//         }
//       }
// 
//       for(int i = 0; i < size_2; i++) {
//         if(segment_2->idx_r_ang[segment_2->valid_idx[i]] >= idx.front() && segment_2->idx_r_ang[segment_2->valid_idx[i]]  <= idx.back()) {
//           if(segment_2->current_side_mat[segment_2->valid_idx[i]] == KalmanSegment::Right)
//             idx_i20.push_back(segment_2->valid_idx[i]);
//           else
//             idx_i21.push_back(segment_2->valid_idx[i]);
//         }
//       }
//     }
// 
//     int change1 = idx_i11.size();
//     int change2 = idx_i21.size();
//     size_1 = change1 + idx_i10.size();
//     size_2 = change2 + idx_i20.size();
// 
//     if(size_1 != size_2)
//       printf("Tamaños distintos %d, %d\n", cnt_seg , cnt_seg + 1);    
//                    
//     for(int i = change1 - 1; i >= 0; i--) {
//       idx_i1.push_back(idx_i11[i]);
//     }
//     for(int i = change1; i < size_1; i++) {
//       idx_i1.push_back(idx_i10[i - change1]);
//     }
// 
//     for(int i = change2 - 1; i >= 0; i--) {
//       idx_i2.push_back(idx_i21[i]);
//     }
//     for(int i = change2; i < size_2; i++) {
//       idx_i2.push_back(idx_i20[i - change2]);
//     }            
//   }
//   else {
//     idx_i1.clear();
//     idx_i2.clear();      
//   }
// //   size_1 = idx_i1.size();
// //   size_2 = idx_i2.size();
// //   for(j = 0; j < size_1; j++)
// //     cout << idx_i1[j] << " ";
// //   cout << endl;
// //   for(j = 0; j < size_1; j++)
// //     cout << segment_1->idx_r_ang[idx_i1[j]] << " ";
// //   cout << endl;
// //   for(j = 0; j < size_2; j++)
// //     cout << idx_i2[j] << " ";
// //   cout << endl;    
// //   for(j = 0; j < size_1; j++)
// //     cout << segment_2->idx_r_ang[idx_i2[j]] << " ";
// //   cout << endl;    
// //   cout << endl;
//   return overlap;
// }

pair < int , int > KalmanSet::computeSeed(int start, int last)
{
    if(start == numSensors && full360)
        start = 0;
    if(last == -1 && full360)
        last = numSensors - 1;
    int lenght = last - start;
    int pos;
    int left, right;
    pair < int , int > seed;
    if(lenght < 0) {
        lenght = numSensors - 1 + lenght;
        pos = start + lenght/2;
        pos = pos >= numSensors ? pos - numSensors :pos;
        seed.first = pos - semilong < 0 ? numSensors + (pos - semilong) : pos - semilong;
        seed.second = pos + semilong >= numSensors ? numSensors - (pos + semilong) : pos + semilong;
    }
    else if(lenght < 8) {  // menos de 9 puntos
        seed.first = start;
        seed.second = last;
    }
    else {
        pos = int((start + last)/2);
        seed.first = pos - semilong;
        seed.second = pos + semilong;
    }
    return seed;
}


void KalmanSet::newSeed(int start, int last, QVector< pair < int , int > >& seeds_new, bool prepend)
{
    pair < int , int > seed = computeSeed(start, last);

    if(prepend)
        seeds_new.insert(seeds_new.begin(),make_pair(seed.first, seed.second));
    else
        seeds_new.push_back(make_pair(seed.first, seed.second));

    idxSeeds.push_back(-1); // -1 new segment
}

void KalmanSet::look4Holes(void)
{
    if(verbose) {
        printf("\n");
        printf("Buscando huecos\n");
    }

    QVector< pair < int , int > > seeds_new;
    pair < int , int > new_seed;
    int i_idx;
    int max_left = 0, min_right = 0;
    int end_i = possible_corners.size();
    int numMeasures = measuredAreas.size();
    int j = 0, i = 0;
    KalmanSegment* s0 = 0;
    KalmanSegment* sb = 0;
    int cntMeasures = 0;

    while(j < numMeasures && (i < end_i || end_i == 0)) {
        cntMeasures++;
        pair<int, int> p = measuredAreas[j];
        bool a_passed = (p.first > p.second) ||
                (full360 && p.first == 0 && p.second == numSensors - 1);
        s0 = segments[i];
        bool breakArea = false;

        if(s0->break_point_left > measuredAreas[j].first && !a_passed)   // hueco al inicio
            newSeed(measuredAreas[j].first, s0->break_point_left - 1, seeds_new);

        while(i < end_i){ // buscando huecos
            pair<int, int> p_s = seeds[i];
            seeds_new.push_back(seeds[i]);
            idxSeeds.push_back(i); // i > -1 old segment

            if(!possible_corners[i]) {
                if(i == end_i - 1 && full360)
                    i_idx = 0;
                else
                    i_idx = i+1;

                KalmanSegment* s1 = segments[i];
                KalmanSegment* s2 = segments[i_idx];

                //         pair<int, int> p = measuredAreas[j];
                bool sameArea = false;
                if(p.first == 0 && p.second == numSensors - 1)
                    sameArea = true;
                else if(p.first > p.second) {
                    if((s1->break_point_right < p.second && s2->break_point_right <= p.second) ||
                            (s1->break_point_left >= p.first && s2->break_point_left > p.first))
                        sameArea = true;
                }
                else if(s1->break_point_left >= p.first && s2->break_point_left >= p.first &&
                        s1->break_point_right < p.second && s2->break_point_right <= p.second &&
                        !s2->passed360)
                    sameArea = true;

                if(sameArea){
                    if(!overlapped(s1, s2, &max_left, &min_right)) {
                        if(abs(max_left - min_right) > 1) {
                            newSeed(min_right + 1, max_left - 1, seeds_new);
                            if(min_right + 1 == 101 && max_left - 1 == 109)
                                int foo = 0;
                        }
                    }
                    i++;
                }
                else /*if(j < numMeasures - 1)*/ {
                    j++;
                    cntMeasures = 0;
                    breakArea = true;
                    break;
                }
            }
            else {
                i++;
                //         if(
                continue;
            }
        }

        if(!breakArea && i < end_i - 1)
            i--;
        else
            breakArea = false;

        if(i < end_i || end_i == 0) {
            sb = segments[i];
            // hueco al final
            if(p.second - p.first < numSensors - 1) {
                if(sb->break_point_right < p.second)
                    newSeed(sb->break_point_right + 1, p.second, seeds_new);
                //           if()
                //             move_last++;
            }
            i++;
            if(end_i == 0)
                end_i++; // para salir cuando solo hay un segmento en todo el set
        }

    }

    if(!cntMeasures){ //ultima area con 1 segment
        if(j < numMeasures) {
            sb = segments.back();
            if(sb->break_point_left > measuredAreas[j].first)
                newSeed(measuredAreas[j].first, sb->break_point_left - 1, seeds_new);
        }
        //     int prev_numSegments = seeds.size();
        //     if(!full360) {
        //       seeds_new.push_back(seeds.back());
        //       idxSeeds.push_back(prev_numSegments - 1); // i > -1 old segment
        //     }
    }
    /*  KalmanSegment* s0 = segments[0];

  if(s0->break_point_left > measuredAreas[0].first && !full360)   // hueco al inicio
    newSeed(measuredAreas[0].first, s0->break_point_left - 1, seeds_new);

  int end_i = possible_corners.size();
  int numMeasures = measuredAreas.size();
  int j = 0;
  for(int i = 0; i < end_i; i++) {// buscando huecos
    seeds_new.push_back(seeds[i]);
    idxSeeds.push_back(i); // i > -1 old segment

    if(!possible_corners[i]) {
      if(i == end_i - 1 && full360)
        i_idx = 0;
      else
        i_idx = i+1;

      KalmanSegment* s1 = segments[i];
      KalmanSegment* s2 = segments[i_idx];
      
      pair<int, int> p = measuredAreas[j];
      bool sameArea = false;
      if(p.first > p.second) {
//         if((s1->break_point_right >= p.first || s1->break_point_left < p.second) && (s2->break_point_left >= p.first || s2->break_point_right <= p.second))
        if((s1->break_point_right < p.second && s2->break_point_right <= p.second) || (s1->break_point_left >= p.first && s2->break_point_left > p.first))
        sameArea = true;
      }
      else if(s1->break_point_left >= p.first && s2->break_point_left >= p.first && s1->break_point_right < p.second && s2->break_point_right <= p.second && !s2->passed360)
        sameArea = true;

      if(sameArea){
        if(!overlapped(s1, s2, &max_left, &min_right)) {
          if(abs(max_left - min_right) > 1)
            newSeed(min_right + 1, max_left - 1, seeds_new);
        }
      }
      else if(j < numMeasures - 1)
        j++;

//       if((s1->passed360 && s2->passed360) || (!s1->passed360 && !s2->passed360)) {
//         if(s1->break_point_right < s2->break_point_left - 1)
//           newSeed(s1->break_point_right + 1, s2->break_point_left - 1, seeds_new);
//       }
    }
  }*/

    if(!full360) {
        int prev_numSegments = seeds.size();
        if(seeds_new.size()) {
            if(seeds.back().first > seeds_new.back().second) {
                seeds_new.push_back(seeds.back());
                idxSeeds.push_back(prev_numSegments - 1); // i > -1 old segment
            }
        }
        else {
            seeds_new.push_back(seeds.back());
            idxSeeds.push_back(prev_numSegments - 1); // i > -1 old segment
        }
    }
    //
    sb = segments.back();
    // hueco al final remirarcon full360 = true
    if(!full360) {
        //        if(sb->break_point_right < measuredAreas.back().second &&
        //                sb->break_point_left >= seeds_new.back().first)
        if(sb->break_point_right < measuredAreas.back().second &&
                sb->break_point_right >= seeds_new.back().second)
            newSeed(sb->break_point_right + 1, measuredAreas.back().second, seeds_new);
    }
    //   else if((sb->break_point_right < numSensors - 1) || (s0->break_point_left > 0)) {
    // //   if((sb->passed360 && !s0->passed360) || (!sb->passed360 && s0->passed360)) {
    // //     if(sb->break_point_right < s0->break_point_left - 1)
    //       newSeed(sb->break_point_right + 1, s0->break_point_left - 1, seeds_new, true);
    //   }

    seeds = seeds_new;
    insertSegments();

}

void KalmanSet::insertSegments(void) { 
    numSegments = seeds.size();

    QVector<KalmanSegment*> new_segments(numSegments);

    int cnt_olds = 0;
    int num_overlaps;
    //   QVector < QVector<int> >::iterator it_1 = idx_1.begin();  // casa
    //   QVector < QVector<int> >::iterator it_2 = idx_2.begin();
    //   QVector < QVector<int> >::iterator it_10 = idx_10.begin();
    //   QVector < QVector<int> >::iterator it_11 = idx_11.begin();
    //   QVector < QVector<int> >::iterator it_20 = idx_20.begin();
    //   QVector < QVector<int> >::iterator it_21 = idx_21.begin();

    //   QVector<int> idx;
    int j = 0, i_idx = 0;
    KalmanSegment* new_seg;
    for(int i = 0; i < numSegments; i++) {

        if(idxSeeds[i] > -1) { // olds
            new_segments[i] = segments[idxSeeds[i]];
            //       new_segments[i]->setClassIdx(i);  // casa
            cnt_olds ++;
            continue; //////////////////////
        }

        pair<int, int> p = measuredAreas[j];

        if(seeds[i].first > p.second && j < numMeasures - 1) {// && ->por las passed360
            j++;
            if(idxSeeds[i] == -1)
                i--;
            continue;
        }

        new_segments[i] = getNewSegment();
        new_seg = new_segments[i];
        //     new_segments[i]->setClassIdx(i);  // casa
        int left = 0, right = 0;
        if(seeds[i].first > p.second && j < numMeasures - 1) // && ->por las passed360
            j++;
        calculateLimits(i, j, &left, &right);
        if(left == 97)
            int foo = 0;

        new_segments[i]->setStartEnd(left, right);

        bool break_if_outl = true;
        new_segments[i]->estimate2PWKalman(seeds[i].first, seeds[i].second, break_if_outl);

        /*    if(!new_segments[i]->valid_idx.size()) {
      new_segments.remove(i);
      seeds.remove(i);
      numSegments--;
      i--;
      continue;
    }*/
        insertIdx(i);
        //     int insert_i = i;
        //     if(i > idx_1.size())
        //       insert_i = idx_1.size();
        //     QVector<int> v1, v2, v10, v11, v20, v21; // casa
        //     idx_1.insert(/*it_1 + */insert_i /*- cnt_olds*/, v1);
        //     idx_2.insert(/*it_2 + */insert_i /*- cnt_olds*/, v2);
        //     idx_10.insert(/*it_10 + */insert_i /*- cnt_olds*/, v10);
        //     idx_20.insert(/*it_11 + */insert_i /*- cnt_olds*/, v11);
        //     idx_11.insert(/*it_20 + */insert_i /*- cnt_olds*/, v20);
        //     idx_21.insert(/*it_21 + */insert_i /*- cnt_olds*/, v21);
    }

    segments = new_segments;

    if(full360)
        num_overlaps = segments.size();
    else
        num_overlaps = segments.size() - 1;
    QVector<int> new_possible_corners(num_overlaps);

    for(int i = 0; i < num_overlaps; i++) {
        if(i == num_overlaps - 1 && full360)
            i_idx = 0;
        else
            i_idx = i + 1;
        if(idxSeeds[i] > -1) { // olds
            //       if(idxSeeds[i] <= possible_corners.size()) // el último de los originales no tiene
            if(idxSeeds[i] < possible_corners.size()) // casa
                // el último de los originales no tiene
                new_possible_corners[i] = possible_corners[idxSeeds[i]]; // possible_corners
            if(idxSeeds[i_idx] > -1) // olds
                continue;
        }

        //     idx.clear();
        //     overlap =  overlapped(segments[i], segments[i_idx]);
        //     idx.clear();
        //     overlap =  idxOverlap(segments[i], segments[i_idx], i, idx, idx_1[i], idx_10[i], idx_11[i], idx_2[i], idx_20[i], idx_21[i]);

        int end_left = 0, end_right = 0;
        bool overlap =  idxOverlap(segments[i], segments[i_idx], i, idx_1[i], idx_2[i], &end_left, &end_right);
        if(!overlap) {

            /*      if(segments[i]->break_point_right < 0)
    int h = 0;  */

            if(verbose) // casa
                printf("Segmento %d (%d) separado de segmento %d (%d)\n", segments[i]->getClassIdx(), segments[i]->break_point_right, segments[i_idx]->getClassIdx(), segments[i_idx]->break_point_left);

            new_possible_corners[i] = false;
        }
        else {
            if(verbose) //casa
                printf("Corner entre segmento %d (%d) y segmento %d (%d)\n", segments[i]->getClassIdx(), end_right, segments[i_idx]->getClassIdx(), end_left);
            new_possible_corners[i] = true;
        }

    }
    possible_corners = new_possible_corners;
    if(verbose)
        printf("Fin buscando huecos\n\n");

}

void KalmanSet::insertIdx(int i) 
{ 
    int insert_i = i;
    if(i > idx_1.size())
        insert_i = idx_1.size();
    QVector<int> v1, v2, v10, v11, v20, v21; // casa
    idx_1.insert(/*it_1 + */insert_i /*- cnt_olds*/, v1);
    idx_2.insert(/*it_2 + */insert_i /*- cnt_olds*/, v2);
    idx_10.insert(/*it_10 + */insert_i /*- cnt_olds*/, v10);
    idx_20.insert(/*it_11 + */insert_i /*- cnt_olds*/, v11);
    idx_11.insert(/*it_20 + */insert_i /*- cnt_olds*/, v20);
    idx_21.insert(/*it_21 + */insert_i /*- cnt_olds*/, v21);
}

int KalmanSet::look4HolesStrict(void)
{
    int ret = 0;
    if(verbose) {
        printf("\n");
        printf("Buscando huecos strict\n");
    }

    int i_idx;
    int max_left = 0, min_right = 0;
    bool break_if_outliers = false;


    int end_i = full360 ? segments.size() : segments.size() - 1;
    int numMeasures = measuredAreas.size();
    int j = 0, i = 0;
    KalmanSegment* new_segment;
    pair<int, int> seed;
    pair<int, int> p;
    bool areaStarted = false;

    while(j < numMeasures && (i < end_i || end_i == 0)) {

        p = measuredAreas[j];
        bool a_passed = (p.first > p.second) || (full360 && p.first == 0 && p.second == numSensors - 1);
        KalmanSegment* s0 = segments[i];

        if(end_i != possible_corners.size())
            int para = 0;

        areaStarted = false;

        if(s0->break_point_left > measuredAreas[j].first && !a_passed) {   // hueco al inicio
            new_segment = getNewSegment(true);
            new_segment->setStartEnd(measuredAreas[j].first, s0->break_point_left - 1);
            seed = computeSeed(measuredAreas[j].first, s0->break_point_left - 1);
            new_segment->estimate2PWKalman(seed.first, seed.second, break_if_outliers);
            segments.insert(i, new_segment); // casa QVector
            possible_corners.insert(i, false);
            insertIdx(i);
            areaStarted = true;
            ret++;

            if(end_i)
                end_i++;
        }

        while(i < end_i){ // buscando huecos
            areaStarted = true;
            KalmanSegment* s1 = segments[i];
            //       if(!possible_corners[i]) {
            if(i == end_i - 1 && full360)
                i_idx = 0;
            else
                i_idx = i+1;

            //         KalmanSegment* s1 = segments[i];
            KalmanSegment* s2 = segments[i_idx];

            //         pair<int, int> p = measuredAreas[j];
            bool sameArea = false;
            if(p.first == 0 && p.second == numSensors - 1)
                sameArea = true;
            else if(p.first > p.second) {
                if((s1->break_point_right < p.second && s2->break_point_right <= p.second) || (s1->break_point_left >= p.first && s2->break_point_left > p.first))
                    sameArea = true;
            }
            else if(s1->break_point_left >= p.first && s2->break_point_left >= p.first && s1->break_point_right < p.second && s2->break_point_right <= p.second && !s2->passed360)
                sameArea = true;

            if(sameArea){
                if(!overlapped(s1, s2, &max_left, &min_right)) {
                    bool create = false;

                    if(full360 && (abs(min_right - max_left) > numSensors/2)) {
                        if(numSensors - min_right + max_left > 1)
                            create = true;
                    }
                    else if(max_left - min_right > 1)
                        create = true;
                    if(create) {
                        new_segment = getNewSegment(true);
                        new_segment->setStartEnd(min_right + 1, max_left - 1);
                        seed = computeSeed(min_right + 1, max_left - 1);
                        new_segment->estimate2PWKalman(seed.first, seed.second, break_if_outliers);

                        /*	      if(!new_segment->valid_idx.size()) {
        seeds.remove(i);
        i--;
        continue;
          }	*/
                        segments.insert(i_idx, new_segment);

                        if(i_idx <= end_i) {
                            possible_corners.insert(i_idx, false);
                            insertIdx(i);
                        }
                        end_i++;

                        ret++;
                        //               i++;
                    }
                    //             i++;
                }
                i++;
            }
            else /*if(j < numMeasures - 1)*/ {
                j++;
                //           areaStarted = false;
                break;
            }
            //       }
            //       else {
            //         i++;
            //         continue;
            //       }
        }

        if(i < end_i || end_i == 0) {
            KalmanSegment* sb = segments[i];
            // hueco al final
            if(p.second - p.first < numSensors - 1) {
                if(sb->break_point_right < p.second) {
                    min_right = sb->break_point_right;
                    max_left = p.second;
                    new_segment = getNewSegment(true);
                    new_segment->setStartEnd(min_right + 1, max_left);
                    seed = computeSeed(min_right + 1, max_left);
                    new_segment->estimate2PWKalman(seed.first, seed.second, break_if_outliers);
                    //           segments.push_back(new_segment);
                    segments.insert(i + 1, new_segment); // casa QVector
                    areaStarted = true;
                    if(i < end_i) {
                        possible_corners.insert(i + 1, false);
                        insertIdx(i);
                    }
                    end_i++;
                    ret++;
                    i++;
                }
            }
            i++;
            if(end_i == 0)  // para salir cuando solo hay un segmento en todo el set
                end_i++;
        }
    }

    KalmanSegment* sb = segments.back();
    p = measuredAreas.back();

    if(!areaStarted) {
        if(sb->break_point_left > measuredAreas.back().first/* && !a_passed*/) {   // hueco al inicio
            new_segment = getNewSegment(true);
            new_segment->setStartEnd(measuredAreas.back().first, sb->break_point_left - 1);
            seed = computeSeed(measuredAreas.back().first, sb->break_point_left - 1);
            new_segment->estimate2PWKalman(seed.first, seed.second, break_if_outliers);
            segments.insert(i, new_segment); // casa QVector
            possible_corners.insert(i, false);
            insertIdx(i);
            ret++;
        }
    }
    // hueco al final remirarcon full360 = true
    if(!full360) {
        if(sb->break_point_right < measuredAreas.back().second) {
            min_right = sb->break_point_right;
            max_left = measuredAreas.back().second;
            new_segment = getNewSegment(true);
            new_segment->setStartEnd(min_right + 1, max_left);
            seed = computeSeed(min_right + 1, max_left);
            new_segment->estimate2PWKalman(seed.first, seed.second, break_if_outliers);
            segments.push_back(new_segment); // casa QVector
            possible_corners.push_back(false);
            insertIdx(idx_1.size());
            ret++;
        }
    }

    numSegments = segments.size();

    if(verbose)
        printf("Fin buscando huecos\n\n");

    return ret;
}

void KalmanSet::resolveSimpleOverlaps(int reg_start, int reg_end) {
    int num_overlaps;
    bool overlap;
    int i_idx, j_ini, j;
    int i_idx2 = -1;
    bool reg_passed = false;

    if(full360)
        num_overlaps = segments.size();
    else
        num_overlaps = segments.size() - 1;

    if(reg_start == -1)
        reg_start = 0;
    if(reg_end == -1)
        reg_end = num_overlaps;

    int i = reg_start;
    while(i < reg_end) {
        KalmanSegment* sg_1 = segments[i];

        if(i == num_overlaps - 1 && full360)
            i_idx = reg_start;
        else
            i_idx = i + 1;
        KalmanSegment* sg_2 = segments[i_idx];

        KalmanSegment::Side side;
        bool removeSimple = false;
        KalmanSegment* sg;

        if(sg_1->valid_idx.size() <= 2) {
            removeSimple = true;
            side = KalmanSegment::Left;
            sg = sg_1;
        }
        else if(sg_2->valid_idx.size() <= 2) {
            removeSimple = true;
            side = KalmanSegment::Right;
            sg = sg_2;
        }
        if(removeSimple) {
            int max_left = 0, min_right = 0;
            bool overlap = overlapped(sg_1, sg_2, &max_left, &min_right);

            if(overlap) {
                int length = max_left <= min_right ? min_right - max_left + 1 : numSensors - max_left + min_right + 1;
                if(length == sg->valid_idx.size()) {
                    removeSimpleOverlap(&i, side);
                    num_overlaps--;
                    if(i >= 0) // casa
                        i--;
                }
            }
            reg_end--;
        }
        i++;
    }
}

void KalmanSet::resolveOverlaps(int reg_start, int reg_end)
{
    int num_overlaps;
    bool overlap;
    int i_idx, j_ini, j;
    int i_idx2 = -1;
    bool reg_passed = false;


    int size_1;
    int size_2;
    int n_1;
    int n_2;
    QVector<double> s2_1, s2_2;

    //   QVector<int> idx;

    if(full360)
        num_overlaps = segments.size();
    else
        num_overlaps = segments.size() - 1;

    if(reg_start == -1)
        reg_start = 0;
    if(reg_end == -1)
        reg_end = num_overlaps;

    int i = reg_start;
    while(i < reg_end) {
        KalmanSegment* sg_1 = segments[i];
        n_1 = sg_1->valid_idx.size();

        //     if(!possible_corners[i]) { // casa
        //       i++;
        //       continue;
        //     }

        if(i == num_overlaps - 1 && full360)
            i_idx = reg_start;
        else
            i_idx = i + 1;
        KalmanSegment* sg_2 = segments[i_idx];
        n_2 = sg_2->valid_idx.size();

        if(sg_1->getClassIdx() == 14 && sg_1->break_point_left == 89 && sg_1->break_point_right == 116)
            int foo = 0;
        if(sg_1->getClassIdx() == 20 && sg_1->break_point_left == 102 && sg_1->break_point_right == 180)
            int foo = 0;
        /*    for(j = 0; j < size_1; j++)
      cout << idx_1[i][j] << " ";
    cout << endl;
    for(j = 0; j < size_2; j++)
      cout << idx_2[i][j] << " ";
    cout << endl;  */

        //     idx.clear();
        //     overlap =  idxOverlap(segments[i], segments[i_idx], i, idx, idx_1[i], idx_10[i], idx_11[i], idx_2[i], idx_20[i], idx_21[i]);

        //     overlap =  idxOverlap(segments[i], segments[i_idx], i, idx_1[i], idx_2[i]);
        int end_left = 0;
        int end_right = 0;

        //     possible_corners[i] =  idxOverlap(sg_1, sg_2, i, idx_1[i], idx_2[i], &end_left, &end_right); // casa
        overlap =  idxOverlap(sg_1, sg_2, i, idx_1[i], idx_2[i], &end_left, &end_right); // casa

        //       cout << "Segment " << sg_1->classIdx << "-> ";
        //       for(int j = 0; j < sg_1->valid_idx.size(); j++)
        //         cout << sg_1->idx_r_ang[sg_1->valid_idx[j]] << " ";
        //       cout << endl;
        //       cout << "Segment " << sg_2->classIdx << "-> ";
        //       for(int j = 0; j < sg_2->valid_idx.size(); j++)
        //         cout << sg_2->idx_r_ang[sg_2->valid_idx[j]] << " ";
        //       cout << endl << endl;

        if(!overlap) { // casa
            if(end_left < 0 || end_right < 0) {
                num_overlaps--;
                reg_end--;
            }
            else {
                possible_corners[i] = overlap;
                i++;
            }
            continue;
        }
        possible_corners[i] = overlap;
        /////////
        //     size_1 = idx_1[i].size();
        //     size_2 = idx_2[i].size();
        //
        //     double upper_1, upper_2;
        //     double s2_1 = sg_1->getSigmaEst()*sg_1->getSigmaEst();
        //     double s2_2 = sg_2->getSigmaEst()*sg_2->getSigmaEst();
        //     if(n_1 <= 2)
        //       upper_1 = s2_1*1000000;
        //     else {
        //       chi_squared dist(n_1 - 2);
        //       upper_1 = (n_1 - 2) * s2_1 / quantile(dist, 0.05 / 2);
        //     }
        //     if(n_2 <= 2)
        //       upper_2 = s2_2*1000000;
        //     else {
        //       chi_squared dist(n_2 - 2);
        //       upper_2 = (n_2 - 2) * s2_2 / quantile(dist, 0.05 / 2);
        //     }
        //
        //     if(upper_1 < upper_2)
        //       j = size_1 - 1;
        //     else
        //       j = -1;


        ////////////
        size_1 = idx_1[i].size();
        size_2 = idx_2[i].size();
        s2_1.resize(size_1);
        s2_2.resize(size_2);

        //     cout << "segmentIdx " << sg_1->getClassIdx() << " ";
        for(j = 0; j < size_1; j++) {
            double s_2 = sg_1->s2_w_out[idx_1[i][j]]/(sg_1->weights_factor*sg_1->weights_factor);
            s2_1[j] =  sg_1->s2_w_out[idx_1[i][j]]/(sg_1->weights_factor*sg_1->weights_factor);
            //       cout << s_2 << " ";
        }
        //     cout << endl;
        //     cout << "segmentIdx " << sg_2->getClassIdx() << " ";
        for(j = 0; j < size_2; j++) {
            double s_2 = sg_2->s2_w_out[idx_2[i][j]]/(sg_2->weights_factor*sg_2->weights_factor);
            s2_2[j] = sg_2->s2_w_out[idx_2[i][j]]/(sg_2->weights_factor*sg_2->weights_factor);
            //       cout << s_2 << " ";
        }
        //     cout << endl << endl;

        j_ini = int(idx_1[i].size()/2);

        if(idx_1[i].size() == 1)
            j = 1; // si no siempre sale el 1
        else
            j = -1;

        if(n_1 <= 2)
            s2_1[j_ini] *= 1000000 / n_1;
        else {
            chi_squared dist(n_1 - 2);
            double upper_limit = (n_1 - 2) * s2_1[j_ini] / quantile(dist, 0.05 / 2);
            s2_1[j_ini] = upper_limit;
        }
        if(n_2 <= 2)
            s2_2[j_ini] *= 1000000 / n_2;
        else {
            chi_squared dist(n_2 - 2);
            double upper_limit = (n_2 - 2) * s2_2[j_ini] / quantile(dist, 0.05 / 2);
            s2_2[j_ini] = upper_limit;
        }

        if(s2_1[j_ini] < s2_2[j_ini])  {
            j = j_ini;  // para size==1 podría salir j=-1 (todos para 2) siendo todos para 1 realmente
            n_1 += j_ini + 1;
            n_2 -= j_ini + 1;
            for(j = j_ini + 1; j < size_1; j++) {
                if(n_1 <= 2)
                    s2_1[j] *= 1000000 / n_1;
                else {
                    chi_squared dist(n_1 - 2);
                    double upper_limit = (n_1 - 2) * s2_1[j] / quantile(dist, 0.05 / 2);
                    s2_1[j] = upper_limit;
                }
                if(n_2 <= 2)
                    s2_2[j] *= 1000000 / n_2;
                else {
                    chi_squared dist(n_2 - 2);
                    double upper_limit = (n_2 - 2) * s2_2[j] / quantile(dist, 0.05 / 2);
                    s2_2[j] = upper_limit;
                }
                if(s2_1[j] >= s2_2[j]) {
                    --j;
                    break;
                }
                n_1++;
                n_2--;
                if(n_2 <= 0)
                    int tt = 0;
            }
        }
        else {
            n_1 -= j_ini - 1;
            n_2 += j_ini - 1;
            for(j = j_ini - 1; j >= 0; j--) {
                if(n_1 <= 2)
                    s2_1[j] *= 1000000 / n_1;
                else {
                    chi_squared dist(n_1 - 2);
                    double upper_limit = (n_1 - 2) * s2_1[j] / quantile(dist, 0.05 / 2);
                    s2_1[j] = upper_limit;
                }
                if(n_2 <= 2)
                    s2_2[j] *= 1000000 / n_2;
                else {
                    chi_squared dist(n_2 - 2);
                    double upper_limit = (n_2 - 2) * s2_2[j] / quantile(dist, 0.05 / 2);
                    s2_2[j] = upper_limit;
                }

                if(s2_1[j] < s2_2[j])
                    break;  // evita j--
                n_1--;
                n_2++;
                if(n_1 <= 0)
                    int tt = 0;
            }
        }

        int i1_idx;
        int start = 0;
        //     bool borrar;
        //     if(j <= 0) {  // todos para el 2
        if(j < 0) {  // todos para el 2
            //       sg_1->restoreBreakPoint(KalmanSegment::Left);
            sg_1->setUserBreakPoint(sg_1->transform_i(sg_1->idx_r_ang[idx_1[i].front()] - 1), KalmanSegment::Right);

            if(sg_1->idx_r_ang[idx_1[i].front()] == sg_1->sensor_start && sg_1->passed360)
                i1_idx = sg_1->sensor_end;
            else
                i1_idx = sg_1->idx_r_ang[idx_1[i].front()] - 1;

            //       borrar = segments[i]->break_point_right < segments[i]->break_point_left && !segments[i]->passed360;

            //       if(borrar) {
            if(sg_1->isCollapsed()) {
                removeOverlap(&i, KalmanSegment::Left, reg_start, &reg_end);
                num_overlaps--;
                if(i >= 0) // casa
                    i--;
                if(i > 0) // volver al anterior
                    continue;

            }
            else {
                sg_2->setUserBreakPoint(sg_2->idx_r_ang[idx_2[i].front()], KalmanSegment::Left);

                QVector<int>::iterator left = find(sg_1->valid_idx.begin(), sg_1->valid_idx.end(), sg_1->inverse_idx_r_ang[sg_1->break_point_left]);

                QVector<int>::iterator right = find(sg_1->valid_idx.begin(), sg_1->valid_idx.end(), sg_1->inverse_idx_r_ang[sg_1->break_point_right]);
                start = min(left - sg_1->valid_idx.begin(), right - sg_1->valid_idx.begin());

                //         sg_1->reEstimate2PWKalman(start, -1);
                sg_1->reEstimate2PWKalman();

                //           cout << "Segment " << sg_1->classIdx << "-> ";
                //           for(int j = 0; j < sg_1->valid_idx.size(); j++)
                //             cout << sg_1->idx_r_ang[sg_1->valid_idx[j]] << " ";
                //           cout << sg_1->print();
                //           cout << endl << endl;

            }
        }

        else if(j < size_1 - 1) {
            /*      cout << "Idx_1[" << i << "]--> "; // casa
      for(int k = 0; k < idx_1[i].size(); k++)
        cout << sg_1->idx_r_ang[idx_1[i][k]] << " ";
      cout << endl;
      cout << "Idx_2[" << i << "]--> ";
      for(int k = 0; k < idx_2[i].size(); k++)
        cout << sg_2->idx_r_ang[idx_2[i][k]] << " ";
      cout << endl << endl;   */

            int jj = 1;  // el siguiente es del mismo lado
            sg_1->setUserBreakPoint(sg_1->idx_r_ang[idx_1[i][j]], KalmanSegment::Right);

            QVector<int>::iterator left = (find(sg_1->valid_idx.begin(), sg_1->valid_idx.end(), sg_1->inverse_idx_r_ang[sg_1->break_point_left]));

            QVector<int>::iterator right = (find(sg_1->valid_idx.begin(), sg_1->valid_idx.end(), sg_1->inverse_idx_r_ang[sg_1->break_point_right]));
            start = min(left - sg_1->valid_idx.begin(), right - sg_1->valid_idx.begin());

            //       sg_1->reEstimate2PWKalman(start, -1);
            sg_1->reEstimate2PWKalman();

            if(sg_2->current_side_mat[idx_2[i][j]] != sg_2->current_side_mat[idx_2[i][j + 1]] && abs(sg_2->idx_r_ang[idx_2[i][j]] - sg_2->idx_r_ang[idx_2[i][j + jj]]) > 1)  // casa el siguiente es por el otro lado, pero no son correlativos
                jj = 2;  // debe existir uno siguiente por el mismo lado

            sg_2->setUserBreakPoint(sg_2->idx_r_ang[idx_2[i][j + jj]], KalmanSegment::Left);

            left = find(sg_2->valid_idx.begin(), sg_2->valid_idx.end(), sg_2->inverse_idx_r_ang[sg_2->break_point_left]);

            right = find(sg_2->valid_idx.begin(), sg_2->valid_idx.end(), sg_2->inverse_idx_r_ang[sg_2->break_point_right]);
            start = min(left - sg_2->valid_idx.begin(), right - sg_2->valid_idx.begin());

            //       sg_2->reEstimate2PWKalman(start, -1);
            sg_2->reEstimate2PWKalman();

            //       cout << "Segment " << segments[i]->classIdx << "-> "; // casa
            //       for(int j = 0; j < sg_1->valid_idx.size(); j++)
            //         cout << sg_1->idx_r_ang[sg_1->valid_idx[j]] << " ";
            //       cout << endl;
            //       cout << sg_1->print();
            //       cout << "Segment " << sg_2->classIdx << "-> ";
            //       for(int j = 0; j < sg_2->valid_idx.size(); j++)
            //         cout << sg_2->idx_r_ang[sg_2->valid_idx[j]] << " ";
            //       cout << sg_2->print();
            //       cout << endl << endl;

            i_idx2 = -1;
            bool l_reg_passed = overlapSwap(&i, i_idx, reg_start, reg_end, num_overlaps, &i_idx2);
            if(l_reg_passed)
                reg_passed = true;
        }
        else {// todos para el 1
            //       sg_2->restoreBreakPoint( KalmanSegment::Right);
            sg_2->setUserBreakPoint(sg_2->transform_i(sg_2->idx_r_ang[idx_2[i].back()] + 1), KalmanSegment::Left);
            //         borrar = segments[i_idx]->break_point_right < segments[i_idx]->break_point_left && !segments[i_idx]->passed360;

            //       if(borrar) {
            if(sg_2->isCollapsed()) {
                removeOverlap(&i, KalmanSegment::Right, reg_start, &reg_end);
                num_overlaps--;
                if(i >= 0) // casa
                    i--;
            }
            else {
                sg_1->setUserBreakPoint(sg_1->idx_r_ang[idx_1[i].back()], KalmanSegment::Right);

                QVector<int>::iterator left = (find(sg_2->valid_idx.begin(), sg_2->valid_idx.end(), sg_2->inverse_idx_r_ang[sg_2->break_point_left]));

                QVector<int>::iterator right = (find(sg_2->valid_idx.begin(), sg_2->valid_idx.end(), sg_2->inverse_idx_r_ang[sg_2->break_point_right]));
                start = min(left - sg_2->valid_idx.begin(), right - sg_2->valid_idx.begin());

                //         sg_2->reEstimate2PWKalman(start, -1);
                sg_2->reEstimate2PWKalman();

                //           cout << "Segment " << sg_2->classIdx << "-> ";
                //           for(int j = 0; j < sg_2->valid_idx.size(); j++)
                //             cout << sg_2->idx_r_ang[sg_2->valid_idx[j]] << " ";
                //           cout << sg_2->print();
                //           cout << endl << endl;

                i_idx2 = -1;
                bool l_reg_passed = overlapSwap(&i, i_idx, reg_start, reg_end, num_overlaps, &i_idx2);
                if(l_reg_passed)
                    reg_passed = true;
            }
        }
        i++;

        //     int n_holes = look4HolesStrict();
        //     if(reg_passed && i == reg_end) {
        //       i = 0;
        //       reg_end = i_idx2;
        //     }
        //     else
        //       reg_end += n_holes;

        look4HolesStrict();

        if(reg_passed && i == reg_end) {
            i = 0;
            reg_end = i_idx2;
        }
        else {
            if(full360)
                reg_end = segments.size();
            else
                reg_end = segments.size() - 1;
        }


    }
    //   cout << endl;
}

void KalmanSet::removeSimpleOverlap(int* i, KalmanSegment::Side side)
{
    int i_del = *i - side + 1;
    //   removeSegment(i_del); // casa

    int num_overlaps;
    if(full360) {
        num_overlaps = segments.size();
        if(i_del == num_overlaps)  // casa
            i_del = 0;
    }
    else
        num_overlaps = segments.size() - 1;

    removeSegment(i_del, *i); // casa

    QVector< pair < int , int > >::iterator its = this->seeds.begin();
    seeds.erase(its + i_del);

}

void KalmanSet::removeOverlap(int* i, KalmanSegment::Side side, int reg_start, int* reg_end)
{
    int i_del = *i - side + 1;
    //   removeSegment(i_del); // casa

    int num_overlaps;
    if(full360) {
        num_overlaps = segments.size();
        if(i_del == num_overlaps)  // casa
            i_del = 0;
    }
    else
        num_overlaps = segments.size() - 1;

    removeSegment(i_del, *i); // casa
    num_overlaps--; // casa

    if(reg_start == -1)
        reg_start = 0;
    if(reg_end == 0)
        *reg_end = num_overlaps - 1;

    (*reg_end)--;

    int i_idx = -1;

    //   if(((*i == *reg_end && side == KalmanSegment::Right) || (*i > *reg_end && side == KalmanSegment::Left)) && full360)
    if(*i >= *reg_end - 1 && full360) // casa
        i_idx = reg_start;
    else {
        if(side == KalmanSegment::Left)
            i_idx = *i;
        else
            i_idx = *i + 1;
    }

    if(i_idx <= *reg_end)
        segments[i_idx]->restoreBreakPoint(side);
    
    int idx = *i - side;
    if(full360 && idx == num_overlaps)  // casa
        idx--;
    
    if((*i > reg_start && side == KalmanSegment::Left) || (i_idx <= *reg_end && side == KalmanSegment::Right)) {
        bool overlap = idxOverlap(segments[idx], segments[i_idx], idx, idx_1[idx], idx_2[idx]);
        if(overlap) {
            possible_corners[idx] =  true;
            (*i)--;
        }
        else
            possible_corners[idx] =  false;
    }
}

void KalmanSet::removeSegment(int i, int removeIdxs)
{
    QVector<KalmanSegment*>::iterator it = segments.begin();
    KalmanSegment* seg = *(it + i);
    //   seg->initValidIdx();
    currentSegments[seg->getClassIdx()] = false;
    seg->clean();
    segments.erase(it + i);
    int endIdxs = possible_corners.size();

    if(removeIdxs > -1 && removeIdxs < endIdxs) {
        if(endIdxs == idx_1.size()){
            QVector< QVector<int> >::iterator it1 = idx_1.begin() + removeIdxs;
            QVector< QVector<int> >::iterator it2 = idx_2.begin() + removeIdxs;
            QVector< QVector<int> >::iterator it10 = idx_10.begin() + removeIdxs;
            QVector< QVector<int> >::iterator it11 = idx_11.begin() + removeIdxs;
            QVector< QVector<int> >::iterator it20 = idx_20.begin() + removeIdxs;
            QVector< QVector<int> >::iterator it21 = idx_21.begin() + removeIdxs;
            idx_1.erase(it1);
            idx_2.erase(it2);
            idx_10.erase(it10);
            idx_11.erase(it11);
            idx_20.erase(it20);
            idx_21.erase(it21);
        }
        QVector<int>::iterator itp = possible_corners.begin() + removeIdxs;
        /*
    int ss = idx_1.size();*/
        //     it1 = it1 + i_del;

        possible_corners.erase(itp); // esto no va muy bien parece que no borra lo que tiene que borrar?
    }
    if(verbose)
        printf("Segmento borrado %d\n", seg->getClassIdx()); // casa

}

void KalmanSet::removeSegment(KalmanSegment * segment)
{
    QVector<KalmanSegment*>::iterator it = segments.begin();
    int i = 0;
    while(it < segments.end()) {
        if(*it == segment) {
            removeSegment(i);
            break;
        }
        ++it;
        i++;
    }
}

void KalmanSet::reEstimate(void)
{
    int start = 0;
    int i = -1;
    while(i < (int)segments.size() - 1) {
        i++;

        //     int removeP = segments[i]->valid_idx.size() - (segments[i]->break_point_right - segments[i]->break_point_left + 1);
        //     if(removeP == 0)
        //       start = segments[i]->valid_idx.size() - 1;
        //     else if(removeP > 0) {

        //     bool borrar = segments[i]->break_point_right < segments[i]->break_point_left && !segments[i]->passed360;

        //       if(borrar) {
        if(segments[i]->isCollapsed()) {
            if(verbose)
                printf("Segmento borrado %d\n",segments[i]->getClassIdx());
            removeSegment(i, i);
            i--;
            continue;
        }

        if(segments[i]->break_point_right < 0)
            int h = 0;

        QVector<int>::iterator left = find(segments[i]->valid_idx.begin(), segments[i]->valid_idx.end(), segments[i]->inverse_idx_r_ang[segments[i]->break_point_left]);

        KalmanSegment* seg = segments[i];
        QVector<int>::iterator right = find(segments[i]->valid_idx.begin(), segments[i]->valid_idx.end(), segments[i]->inverse_idx_r_ang[segments[i]->break_point_right]);

        start = min(left - segments[i]->valid_idx.begin(), right - segments[i]->valid_idx.begin());

        //     }
        //     else
        //       printf("segments(%d) too much points to remove\n", i);
        segments[i]->reEstimate2PWKalman(start, -1);
        if(verbose) {
            printf("Segment %d left %d right %d  num %d\n", segments[i]->getClassIdx(), segments[i]->break_point_left, segments[i]->break_point_right, segments[i]->getNumElems());

            //   [segments[i], rho_0_est, phi_0_est, sigma_sen, R_2] = ...
            //                     reestimate2PWKalman(segments[i], start, -1, do_plot, same_window);

            cout << "rho_0 " << segments[i]->getRho_0() << "   phi_0 " << segments[i]->getPhi_0()*180/M_PI << endl;
            cout << "rho_est " << segments[i]->getRhoEst() << "   phi_est " << segments[i]->getPhiEst()*180/M_PI << endl;
            cout << "sigma_est " << segments[i]->getSigmaEst()*1000 << endl << endl;
        }
    }
}

void KalmanSet::equalityTest(void )
{
    int i = 0;
    int i_plus1;
    //   int k = 0; //casa
    bool merged = false;
    //   KalmanSegment::Side pos2del = KalmanSegment::notSet;

    int cnt_segment = segments.size() - 1;
    if(full360)
        cnt_segment++;
    if(verbose)
        cout << endl;
    
    while (i < cnt_segment) {
        //     k++; // casa

        i_plus1 = i + 1;
        if(i_plus1 >= cnt_segment && full360)
            i_plus1 = i_plus1 - cnt_segment;

        if(verbose) // casa
            printf("\nSegmento %d y %d\n", segments[i]->getClassIdx(), segments[i_plus1]->getClassIdx());

        if(segments[i_plus1]->getClassIdx() == 4)
            int foo = 0;

        //       merged = mergeSegments(i, i_plus1, pos2del);
        if(segments[i_plus1]->break_point_left - segments[i]->break_point_right == 1 || (segments[i]->break_point_right == numSensors - 1 && segments[i_plus1]->break_point_left == 0 && full360))
            merged = mergeSegments(i, i_plus1);
        else
            merged = false;

        if(merged) {
            if(verbose)
                printf("merged\n\n");
            //       if(pos2del == KalmanSegment::Right)
            //         KalmanSegment* seg = segments[i_plus1];
            //         removeSegment(i_plus1);
            //       else if(pos2del == KalmanSegment::Left)
            //         removeSegment(i);

            //       if(!full360)
            cnt_segment--;
            if(i > 0)
                i--;
        }
        else
            i++;
    }
}

bool KalmanSet::mergeSegments(int i , int i_plus1, double confidence)
{
    bool merged = false;
    KalmanSegment * new_segment = 0;
    KalmanSegment * segment_1 = segments[i];
    KalmanSegment * segment_2 = segments[i_plus1];
    double probabil = 0;

    int size_1 = segment_1->valid_idx.size();
    int size_2 = segment_2->valid_idx.size();
    int cnt = -1;
    int idx_i;

    if(segment_1->classIdx == 25 && segment_2->classIdx == 14)
        int foo = 0;

    if(size_1 <= 3 && size_2 <= 3) {
        //     cout << "Tamaños insuficientes para mergeSegments()" << endl;;
        return merged;
    }
    else if(size_1 <= 3) {
        cnt = 0;
        //     pos2del = KalmanSegment::Left; // izq

        while(size_1 > 0) {
            if(segment_1->passed360) {
                idx_i = -1;
                for(int j = 0; j < size_1; j++) {
                    if(segment_1->idx_r_ang[segment_1->valid_idx[j]] < segment_1->break_point_left) {
                        if(segment_1->idx_r_ang[segment_1->valid_idx[j]] > idx_i)
                            idx_i = segment_1->idx_r_ang[segment_1->valid_idx[j]];
                    }
                }
                if(idx_i == -1)
                    idx_i = numSensors;
            }
            else
                //         idx_i = segment_1->break_point_right; // casa
                idx_i = segment_1->inverse_idx_r_ang[segment_1->break_point_right];

            merged = segment_2->addPoint(KalmanSegment::Left); // casa
            if(!merged)
                break;
            else {
                //       if(size_1 > 1)
                segment_1->setUserBreakPoint(segment_1->transform_i(segment_1->break_point_right - 1), KalmanSegment::Right);
                //           segment_1->setUserBreakPoint(segment_1->break_point_right + 1, KalmanSegment::Right);
                cnt++;
                size_1--;
            }
        }
        if(merged) {
            removeSegment(i, i);
        }
    }
    else if(size_2 <= 3) {
        cnt = 0;
        //     pos2del = KalmanSegment::Right; // dch
        new_segment = segment_1;
        while(size_2 > 0) {
            if(segment_2->passed360) {
                idx_i = numSensors + 1;
                for(int j = 0; j < size_2; j++) {
                    if(segment_2->idx_r_ang[segment_2->valid_idx[j]] > segment_2->break_point_right) {
                        if(segment_2->idx_r_ang[segment_2->valid_idx[j]] < idx_i)
                            idx_i = segment_2->idx_r_ang[segment_2->valid_idx[j]];
                    }
                }
                if(idx_i == numSensors + 1)
                    idx_i = 0;
            }
            else
                //         idx_i = segment_1->break_point_left; // casa
                idx_i = segment_1->inverse_idx_r_ang[segment_1->break_point_left];


            merged = segment_1->addPoint(KalmanSegment::Right); // casa
            if(!merged)
                break;
            else {
                //         if(size_2 > 1)

                segment_2->setUserBreakPoint(segment_2->transform_i(segment_2->break_point_left + 1), KalmanSegment::Left);
                //           segment_2->setUserBreakPoint(segment_2->break_point_left - 1, KalmanSegment::Left);
                cnt++;
                size_2--;
            }
        }
        if(merged) {
            removeSegment(i_plus1, i);
        }
    }
    else {
        double rho_0, phi_0, s2_0;
        QMatrix2x2 S_0;

        double s2_1 = segment_1->s2_w[segment_1->valid_idx.back()]/(segment_1->weights_factor*segment_1->weights_factor);
        double s2_2 = segment_2->s2_w[segment_2->valid_idx.back()]/(segment_2->weights_factor*segment_2->weights_factor);

        // test para ver si no son mayores que el doble de la otra
        int n_1 = segment_1->valid_idx.size();
        int n_2 = segment_2->valid_idx.size();

        if(!varianceTest(n_1, s2_1, n_2, s2_2)) {
            merged = false;
            return merged; // varianzas muy desiguales
        }

        rho_0 = (segment_1->beta_est_w[segment_1->valid_idx.back()].first*sqrt(s2_2) + segment_2->beta_est_w[segment_2->valid_idx.back()].first*sqrt(s2_1)) / (sqrt(s2_1) + sqrt(s2_2)); // pesos inversos
        if(segment_1->beta_est_w[segment_1->valid_idx.back()].second*segment_2->beta_est_w[segment_2->valid_idx.back()].second >= 0)
            phi_0 = (segment_1->beta_est_w[segment_1->valid_idx.back()].second*sqrt(s2_2) + segment_2->beta_est_w[segment_2->valid_idx.back()].second*sqrt(s2_1)) / (sqrt(s2_1) + sqrt(s2_2)); // cuidado determinacion angulo
        else {
            if(abs(segment_1->beta_est_w[segment_1->valid_idx.back()].second) + abs(segment_2->beta_est_w[segment_2->valid_idx.back()].second) > M_PI/2) {
                if(segment_1->beta_est_w[segment_1->valid_idx.back()].second < 0)
                    segment_1->beta_est_w[segment_1->valid_idx.back()].second = M_PI + segment_1->beta_est_w[segment_1->valid_idx.back()].second;
                else if(segment_2->beta_est_w[segment_2->valid_idx.back()].second < 0)
                    segment_2->beta_est_w[segment_2->valid_idx.back()].second = M_PI + segment_2->beta_est_w[segment_2->valid_idx.back()].second;

                phi_0 = (segment_1->beta_est_w[segment_1->valid_idx.back()].second*sqrt(s2_2) + segment_2->beta_est_w[segment_2->valid_idx.back()].second*sqrt(s2_1)) / (sqrt(s2_1) + sqrt(s2_2));
                if(phi_0 >= M_PI/2)
                    phi_0 = phi_0 - M_PI;
            }
            else
                phi_0 = (segment_1->beta_est_w[segment_1->valid_idx.back()].second*sqrt(s2_2) + segment_2->beta_est_w[segment_2->valid_idx.back()].second*sqrt(s2_1)) / (sqrt(s2_1) + sqrt(s2_2)); // cuidado determinacion angulo
        }

        S_0 = (segment_1->S_w[segment_1->valid_idx.back()]*(float)sqrt(s2_2) + segment_2->S_w[segment_2->valid_idx.back()]*(float)sqrt(s2_1)) * (float)(1/(sqrt(s2_1) + sqrt(s2_2)));

        //    weights_factor = rho_0*sqrt(2);
        double weights_factor = (segment_1->weights_factor*sqrt(s2_2) + segment_2->weights_factor*sqrt(s2_1))/ (float)(sqrt(s2_1) + sqrt(s2_2));

        s2_0 = (s2_1*size_1 + s2_2*size_2)/(size_1 + size_2)/**weights_factor*weights_factor*/;

        new_segment = getNewSegment(true);
        //     segments.push_back(new_segment);
        //     segments.insert(segments.begin() + i_plus1 + 1, new_segment);
        segments.insert(i_plus1 + 1, new_segment); // casa QVector
        //     if(segment_1->getClassIdx() == 3) {
        //       new_segment->sum_e2_w.fill(0);
        //       new_segment->sum_e2_w_out.fill(0);
        //     }
        new_segment->setStartEnd(segment_1->break_point_left, segment_2->break_point_right);
        //     new_segment->initKalman(rho_0, phi_0, S_0, s2_0, weights_factor);
        int start, end;

        if(n_1 + n_2 <= 2*semilong + 1) {
            start = segment_1->break_point_left;
            end = segment_2->break_point_right;
        }
        else if(n_1 > n_2) {
            start = segment_1->break_point_right - semilong;
            end = segment_1->break_point_right + semilong;
            if(full360) {
                start = start < 0 ? numSensors + start : start;
                end = end >= numSensors ? end - numSensors : end;
            }
        }
        else {
            start = segment_2->break_point_left - semilong;
            end = segment_2->break_point_left + semilong;
            if(full360) {
                start = start < 0 ? numSensors + start : start;
                end = end >= numSensors ? end - numSensors : end;
            }
        }

        bool break_if_outliers = false;

        //     new_segment->estimate2PWKalman(segment_1->break_point_right + 1, segment_1->break_point_right, break_if_outliers);
        new_segment->estimate2PWKalman(start, end, break_if_outliers);

        //     for(int j = 0; j < new_segment->valid_idx.size(); j++)
        //       cout << new_segment->idx_r_ang[new_segment->valid_idx[j]] << " ";
        //     cout << "  s " << new_segment->valid_idx.size() << endl;

        double ee = new_segment->sum_e2_w_out[new_segment->valid_idx.back()]/(new_segment->weights_factor*new_segment->weights_factor);
        double ee1 = segment_1->sum_e2_w_out[segment_1->valid_idx.back()]/(segment_1->weights_factor*segment_1->weights_factor);
        double ee2 = segment_2->sum_e2_w_out[segment_2->valid_idx.back()]/(segment_2->weights_factor*segment_2->weights_factor);

        if(verbose) {
            cout << new_segment->print() << endl;
        }

        if(!new_segment->auto_limited) {
            //       int df = new_segment->valid_idx.size() - 4/* - new_segment->outliers.size()*/;

            int df = round(((n_1 - 2)*s2_1 + (n_2 - 2)*s2_2) * ((n_1 - 2)*s2_1 + (n_2 - 2)*s2_2)/((n_1 - 2)*s2_1*s2_1 + (n_2 - 2)*s2_2*s2_2));

            /*      if(new_segment->getSigmaEst() < sqrt(s2_1) || new_segment->getSigmaEst() < sqrt(s2_2))
        merged = true;

      else */
            if(df > 0) {// d <=0 --> demasiados outliers
                fisher_f F(2, df);
                // 	if(ee1+ee2 <= 0)
                // 	  int foo = 0;
                double chow = (ee - (ee1+ee2))/2/((ee1+ee2)/df);
                if(chow > 0)
                    probabil = 1 - cdf(F, chow);
                else
                    probabil = 1;

                if(confidence < 0)  // esta es la p de la condición de outlier (dos de tres seguidos)
                    confidence = 2*(1 - confidenceLevel)*(1 - confidenceLevel) - (1 - confidenceLevel)*(1 - confidenceLevel)*(1 - confidenceLevel);

                if(fabs(segment_1->rho_0_est - segment_2->rho_0_est) < 10 &&
                        (fabs(segment_1->phi_0_est - segment_2->phi_0_est) < M_PI/180 ||
                         fabs(M_PI - fabs(segment_1->phi_0_est - segment_2->phi_0_est)) < M_PI/180) &&
                        new_segment->getSigmaEst() < KalmanSegment::MAXSIGMA/2)
                    merged = true;
                else if(probabil > confidence)
                    merged = true;
                else if(probabil > confidence / 10.0) {
                    KalmanSegment* seg = segment_1->getSigmaEst() < segment_2->getSigmaEst() ? segment_1 : segment_2;

                    int nn_1 = seg->valid_idx.size();
                    int nn_2 = new_segment->valid_idx.size();
                    double minSigma = seg->getSigmaEst();
                    double newSigma = new_segment->getSigmaEst();

                    if(varianceTest(nn_1, minSigma, nn_2, newSigma))
                        merged = true;
                }
            }

            //       pos2del = KalmanSegment::Right; // dch
            //       segments.insert(segments.begin() + i, new_segment);
            if(merged) {
                if(i < i_plus1) { // casa
                    removeSegment(i_plus1);
                    removeSegment(i, i);
                }
                else { // casa
                    //           if(i_plus1 == 0)
                    //             removeSegment(i_plus1);
                    //           else
                    //             removeSegment(i, i);
                    removeSegment(i_plus1);
                    removeSegment(i, i);
                }
            }
            else
                removeSegment(new_segment);
        }
        else
            removeSegment(new_segment);

        if(verbose) {
            cout << "prob. " << probabil << endl;
        }
    }

    if(cnt > 0 && verbose)
        printf("Se unieron %d puntos\n", cnt);

    return merged;
}

bool KalmanSet::varianceTest(int n_1, double s2_1, int n_2, double s2_2) {

    bool ret = false;
    double fexp = 100000;
    int n1, n2;

    if(s2_1 < 0.0001 && s2_2 < 0.0001)
        return true;

    if(s2_1 > s2_2) {
        fexp = s2_1 / s2_2;
        n1 = n_1 - 2;
        n2 = n_2 - 2;
    }
    else {
        fexp = s2_2 / s2_1;
        n1 = n_2 - 2;
        n2 = n_1 - 2;
    }
    if(n1 > 0 && n2 > 0) {
        fisher_f F(n1, n2);
        double prob_2 = 1 - cdf(F, fexp/2);
        double prob_05 = 1 - cdf(F, fexp*2);

        if((prob_2 >= 1 - confidenceLevel) || (prob_05 >= 1 - confidenceLevel))
            ret = true; // varianzas muy desiguales
    }
    else
        ret = false;

    return ret;
}

KalmanSegment * KalmanSet::getNewSegment(bool withIdx)
{
    KalmanSegment * ret_segment = 0;
    QVector<bool>::iterator it = currentSegments.begin();
    int cnt = 0;
    while(it < currentSegments.end()) {
        if(!(*it)) {
            ret_segment = segmentsStore[cnt];
            *it = true;
            usedSegments[cnt] = true;
            /*if(withIdx && ret_segment->getClassIdx() == -1)*/ // casa
            //         ret_segment->setClassIdx(cnt);
            break;
        }
        cnt++;
        ++it;
    }
    if(!ret_segment) {
        ret_segment = new KalmanSegment(numSensors, radius, angle, full360, twoStage);
        ret_segment->setClassIdx(maxNumSegments);
        ret_segment->setTValues(t_vals.data(), confidenceLevel);
        segmentsStore.push_back(ret_segment);
        maxNumSegments++;
        currentSegments.push_back(true);
        usedSegments.push_back(true);
    }
    return ret_segment;
}

bool KalmanSet::overlapSwap(int* i, int i_idx, int reg_start, int reg_end, int num_overlaps, int* i_idx2, bool withIdx)
{

    bool reg_passed = false;
    *i_idx2 = *i;
    int last_idx = -1;
    int last_idx2 = -1;
    bool overlap_swap = false;
    bool over = false;
    bool swap_last = false;
    int j_idx = i_idx;
    //   int j_idx = *i;
    int end_j = num_overlaps;
    while(j_idx < end_j)  {  // caso raro de que un seg se achique tanto por la izq que pierda el overlap con el de su dcha, tiene que ser sigma_sensor grande
        if(j_idx == num_overlaps - 1 && full360)
            *i_idx2 = reg_start;
        else
            *i_idx2 = j_idx + 1;

        //   overlap = isOverlap(regressions[j_idx], regressions[i_idx2]);
        over = isOver(segments[j_idx], segments[*i_idx2]);
        //    if(!overlap & possible_corners[j_idx]) then,
        if(over) {
            KalmanSegment* sg_1 = segments[j_idx];
            KalmanSegment* sg_2 = segments[*i_idx2];
            if(verbose) {
                printf("Cambiando segmento %d por %d\n", sg_1->getClassIdx(), sg_2->getClassIdx()); // casa
            }
            swap_last = full360 && *i_idx2 == 0 && end_j > 2;
            if(swap_last) {
                *i_idx2 = end_j - 2;
                *i = end_j - 3;
                i_idx = end_j - 2;
                segments.erase(segments.begin()); // temporalmente reg_end = reg_end - 1
                segments.insert(*i_idx2, sg_2);

                last_idx = j_idx;
                last_idx2 = *i;
                j_idx = j_idx + 1;

            }
            else {
                swap(segments[j_idx], segments[*i_idx2]);
                last_idx = j_idx;
                last_idx2 = *i_idx2;
                j_idx = j_idx + 1;
            }
            //       swap(idx_1[j_idx], idx_1[*i_idx2]); // casa
            //       swap(idx_2[j_idx], idx_2[*i_idx2]);
            //       swap(idx_10[j_idx], idx_10[*i_idx2]);
            //       swap(idx_11[j_idx], idx_11[*i_idx2]);
            //       swap(idx_21[j_idx], idx_21[*i_idx2]);
            //       swap(idx_20[j_idx], idx_20[*i_idx2]);
            if(withIdx) {
                swap(idx_1[last_idx], idx_2[last_idx]);
                swap(idx_10[last_idx], idx_20[last_idx]);
                swap(idx_11[last_idx], idx_21[last_idx]);
            }
            /*      int swap_corners = possible_corners[j_idx]; // casa
      possible_corners[j_idx] = possible_corners[*i_idx2];
      possible_corners[*i_idx2] = swap_corners; */

            //       last_idx = j_idx;
            //       last_idx2 = *i_idx2;
            //       j_idx = j_idx + 1;
            //       if(j_idx == num_overlaps - 1 && full360) {
            //         j_idx = reg_start;
            //         end_j = *i;
            //       }
            //       else
            //         j_idx = j_idx + 1;
            overlap_swap = true;
        }
        else
            break;
    }

    if(overlap_swap) {
        if(withIdx)
            possible_corners[last_idx] = false;
        bool overlap = false;

        int new_idx = -1;
        if(last_idx2 == reg_end - 1 && full360)
            new_idx = reg_start;
        else if(last_idx2 < reg_end && last_idx2 > 0) // sea full360 o no, last_idx2 > 1 es valido si hay overlap
            new_idx = last_idx2 + 1;
        else if(last_idx2 == 0)
            reg_passed = true;

        int end_left = 0, end_right = 0;
        if(new_idx > -1) {
            overlap =  idxOverlap(segments[last_idx2], segments[new_idx], last_idx2, idx_1[last_idx2], idx_2[last_idx2], &end_left, &end_right);

            if(!overlap) {
                if(verbose)
                    printf("Cambiar swap: Segmento %d (%d) separado de segmento %d (%d)\n", segments[last_idx2]->getClassIdx(), segments[last_idx2]->break_point_right, segments[new_idx]->getClassIdx(), segments[new_idx]->break_point_left);
                possible_corners[last_idx2] =  false;
            }
            else {
                if(verbose)
                    printf("Cambiar swap: Corner entre segmento %d (%d) y segmento %d (%d)\n", segments[last_idx2]->getClassIdx(), end_right, segments[new_idx]->getClassIdx(), end_left);
                possible_corners[last_idx2] =  true;
            }
        }

        overlap =  idxOverlap(segments[*i], segments[i_idx], *i, idx_1[*i], idx_2[*i], &end_left, &end_right);
        if(!overlap) {
            if(verbose)
                printf("Cambiar : Segmento %d %d (%d) separado de segmento %d %d (%d)\n", *i, segments[*i]->classIdx, segments[*i]->break_point_right, i_idx, segments[i_idx]->classIdx, segments[i_idx]->break_point_left);
            possible_corners[*i] = false;
        }
        else {
            if(verbose)
                printf("Cambiar : Corner entre segmento %d %d (%d) y segmento %d %d (%d)\n", *i, segments[*i]->classIdx, end_right, i_idx, segments[i_idx]->classIdx, end_left);
            possible_corners[*i] = true;
            (*i)--;
        }
    }
    return reg_passed;
}

// bool KalmanSet::isOver(KalmanSegment* segment_1, KalmanSegment* segment_2)
// { 
//   bool over = false;
//   
//   if(!segment_1->passed360 && segment_2->passed360) { // casa 
// //     if(segment_1->break_point_left > segment_2->break_point_right && segment_1->break_point_left < segment_1->numSensors/2)
//     if(segment_1->break_point_left > segment_2->break_point_left)
//       over = true;
//     else if(segment_1->break_point_left == segment_2->break_point_left && segment_1->getNumElems() > segment_2->getNumElems())
//       over = true;    
//   } 
//   else if(segment_1->passed360 && !segment_2->passed360) {
// //     if(segment_1->break_point_left > segment_2->break_point_right && segment_2->break_point_right > segment_2->numSensors/2)
//     if(segment_1->break_point_left > segment_2->break_point_left && segment_2->break_point_left > segment_2->numSensors/2)
//       over = true;
//     else if(segment_1->break_point_left == segment_2->break_point_left && segment_2->break_point_left > segment_2->numSensors/2 && segment_1->getNumElems() > segment_2->getNumElems())
//       over = true;
//     }
//   else {  // casa
// //     if(segment_1->break_point_left > segment_2->break_point_right)
//     if(segment_1->break_point_left > segment_2->break_point_left)
//       over = true;
//     else if(segment_1->break_point_left == segment_2->break_point_left && segment_1->getNumElems() > segment_2->getNumElems())
//       over = true;         
//   }
//   return over;
// }

bool KalmanSet::isOver(KalmanSegment* segment_1, KalmanSegment* segment_2)
{
    bool over = false;

    //   if(segment_1->break_point_left > segment_2->break_point_left)
    //     over = true;
    //   else if(segment_1->break_point_left == segment_2->break_point_left && segment_1->getNumElems() > segment_2->getNumElems())
    //     over = true;
    //   else if(!segment_1->passed360 && segment_1->break_point_left < numSensors/4 && segment_2->break_point_left > 3*numSensors/4)
    //     over = true;


    if(segment_1->passed360 && segment_2->passed360) {
        if(segment_1->break_point_left == segment_2->break_point_left)
            over = segment_1->getNumElems() > segment_2->getNumElems();
        else
            over = segment_1->break_point_left > segment_2->break_point_left;
    }
    else if(segment_1->passed360) {
        int limit = segment_1->break_point_left - numSensors / 2;
        if(segment_2->break_point_left > limit) {
            if(segment_1->break_point_left == segment_2->break_point_left)
                over = segment_1->getNumElems() > segment_2->getNumElems();
            else
                over = segment_1->break_point_left > segment_2->break_point_left;
        }
    }
    else if(segment_2->passed360) {
        int limit = segment_2->break_point_left - numSensors / 2;
        if(segment_1->break_point_left > limit) {
            if(segment_1->break_point_left == segment_2->break_point_left)
                over = segment_1->getNumElems() > segment_2->getNumElems();
            else
                over = segment_1->break_point_left > segment_2->break_point_left;
        }
        else {
            over = true;
        }
    }
    else {
        if(full360) {
            if(!(segment_1->break_point_left > numSensors / 2 && segment_2->break_point_left < segment_1->break_point_left - numSensors / 2 ))
                if(segment_1->break_point_left == segment_2->break_point_left)
                    over = segment_1->getNumElems() > segment_2->getNumElems();
                else
                    over = segment_1->break_point_left > segment_2->break_point_left;
        }
        else {
            //       if(segment_1->break_point_left == segment_2->break_point_left)
            //         over = segment_1->getNumElems() > segment_2->getNumElems();
            //       else
            //         over = segment_1->break_point_left > segment_2->break_point_left;
        }
    }

    
    //   if(!segment_1->passed360 && segment_2->passed360) { // casa
    // //     if(segment_1->break_point_left > segment_2->break_point_right && segment_1->break_point_left < segment_1->numSensors/2)
    //     if(segment_1->break_point_left > segment_2->break_point_left)
    //       over = true;
    //     else if(segment_1->break_point_left == segment_2->break_point_left && segment_1->getNumElems() > segment_2->getNumElems())
    //       over = true;
    //   }
    //   else if(segment_1->passed360 && !segment_2->passed360) {
    // //     if(segment_1->break_point_left > segment_2->break_point_right && segment_2->break_point_right > segment_2->numSensors/2)
    //     if(segment_1->break_point_left > segment_2->break_point_left && segment_2->break_point_left > segment_1->break_point_left - segment_1->numSensors/2)
    //       over = true;
    //     else if(segment_1->break_point_left == segment_2->break_point_left && segment_2->break_point_left > segment_1->numSensors/2 - segment_1->break_point_left && segment_1->getNumElems() > segment_2->getNumElems())
    //       over = true;
    //     }
    //   else if(!segment_1->passed360 && segment_2->passed360) {
    // //     if(segment_1->break_point_left > segment_2->break_point_right && segment_2->break_point_right > segment_2->numSensors/2)
    //     if(segment_1->break_point_left > segment_2->break_point_left && segment_1->break_point_left > segment_2->break_point_left - segment_1->numSensors/2)
    //       over = true;
    //     else if(segment_1->break_point_left == segment_2->break_point_left && segment_2->break_point_left > segment_1->numSensors/2 - segment_1->break_point_left && segment_1->getNumElems() > segment_2->getNumElems())
    //       over = true;
    //     }
    //   else {  // casa
    // //     if(segment_1->break_point_left > segment_2->break_point_right)
    //     if(segment_1->break_point_left > segment_2->break_point_left)
    //       over = true;
    //     else if(segment_1->break_point_left == segment_2->break_point_left && segment_1->getNumElems() > segment_2->getNumElems())
    //       over = true;
    //   }


    return over;
}

void KalmanSet::clean(void )
{

    segments.clear();

    int ns = maxNumSegments;
    QVector<bool>::iterator itu = usedSegments.begin();
    QVector<KalmanSegment*>::iterator its = segmentsStore.begin();

    for( ;itu < usedSegments.end(); ++itu, ++its) {
        if(*itu)
            (*its)->clean(true);
    }
    usedSegments.fill(false, maxNumSegments);
    currentSegments.fill(false, maxNumSegments);
    idx_1.clear();
    idx_2.clear();
    idx_10.clear();
    idx_20.clear();
    idx_11.clear();
    idx_21.clear();
    possible_corners.clear();
    idxSeeds.clear();
}

void KalmanSet::printResults(int minNumber)
{
    //   QString s_out;
    char* s_out;
    int end_i = segments.size();
    cout << endl << "Segmentation results : " << segments.size () << " segments" << endl << endl;
    for(uint i = 0; i < end_i; i++) {
        int numElements = 0;
        s_out = segments[i]->print(&numElements);
        if(numElements >= minNumber)
            cout << s_out << endl;
    }
}

QVector< double > KalmanSet::getFilterData(void)
{

    QVector< double > filterData(numSensors);
    int s_a = endPoints.size();
    int j_1, j_2, last_j = -1;
    double rho_0, phi_0;
    pair<int, int> ends;
    numSegments = segments.size();
    int cnt = 0;

    if(!numSegments) {
        for(int j = 0; j < numSensors; j++)
            filterData[j] = -1;

        return filterData;
    }

    if(full360)
        last_j = endPoints.at(0).first;
    
    for(int i = 0; i < numSegments; i++) {
        j_1 = endPoints.at(i).first;
        j_2 = endPoints.at(i).second;

        if(abs(last_j - j_1) > 1) {
            for(int j = last_j + 1; j <= j_1 - 1; j++) {
                filterData[j] = -1;
                cnt++;
            }
        }

        if(j_1 == j_2) {
            filterData[j_1] = radius[j_1];
            cnt++;
        }
        else if(j_1 < j_2) {
            for(int j = j_1; j <= j_2; j++) {
                filterData[j] = abs(rhoEst.at(i)/cos(angle[j] - phiEst.at(i)));
                cnt++;
            }
        }
        else {
            for(int j = j_1; j < numSensors; j++)
                filterData[j] = abs(rhoEst.at(i)/cos(angle[j] - phiEst.at(i)));
            for(int j = 0; j <= j_2; j++)
                filterData[j] = abs(rhoEst.at(i)/cos(angle[j] - phiEst.at(i)));
        }
        last_j = j_2;
    }
    if(!full360) {
        if(last_j < numSensors - 1) {
            for(int j = last_j + 1; j < numSensors; j++) {
                filterData[j] = -1;
                cnt++;
            }
        }
    }
    else {
        j_1 = endPoints.back().second;
        j_2 = endPoints.at(0).first;
        if(j_1 <= j_2) {
            if(j_1 < j_2 - 1) {
                for(int j = j_1 + 1; j <= j_2; j++)
                    filterData[j] = -1;
            }
        }
        else {
            for(int j = j_1 + 1; j < numSensors; j++)
                filterData[j] = -1;
            for(int j = 0; j <= j_2 - 1; j++)
                filterData[j] = -1;
        }
    }
    if(cnt != numSensors)
        int para = 0;
    return filterData;
}

QVector<pair < int, int> > KalmanSet::getConnectedAreas(void)
{
    QVector<pair < int, int> > connectedAreas;
    int end_i = possible_corners.size();
    if((full360 && end_i != segments.size()) || (!full360 && end_i != segments.size() - 1))
        cout << "Bad possible_corners" << endl;
    pair < int, int> p;
    connectedAreas.push_back(p);
    for(int i = 0; i < end_i; i++) {
        int i_1 = i == end_i - 1 && /*scaleSpace->getFull360()*/full360 ? 0 : i + 1;
        if(possible_corners[i])
            connectedAreas.back().second = i_1;
        else {
            p.first = i_1;
            p.second = i_1;
            connectedAreas.push_back(p);
        }
    }

    for(int i = 0; i < connectedAreas.size(); i++)
        cout << "(" << connectedAreas[i].first << ", " << connectedAreas[i].second << ")    ";
    cout << endl;
    return connectedAreas;
}

void KalmanSet::breakShortSegments() {
    numSegments = segments.size();
    pair<int, int> seed;
    KalmanSegment * segment;
    KalmanSegment *new_segment;
    for(int i = 0; i < numSegments; i++) {
        int seg_size = segments[i]->valid_idx.size();
        if((seg_size > 1 && seg_size <= 5) || segments[i]->sigma_est > KalmanSegment::MAXSIGMA/5) {
            segment = segments[i];
            for(int j = 0; j < seg_size; j++) {
                new_segment = getNewSegment(true);
                // 	new_segment->setStartEnd(segment->idx_r_ang[segment->valid_idx[j]], segment->idx_r_ang[segment->valid_idx[j]]);
                // 	seed = computeSeed(segment->idx_r_ang[segment->valid_idx[j]], segment->idx_r_ang[segment->valid_idx[j]]);
                int pos = segment->transform_i(segment->break_point_left + j);
                new_segment->setStartEnd(pos, pos);
                seed = computeSeed(pos, pos);
                new_segment->estimate2PWKalman(seed.first, seed.second, true);
                segments.insert(i + j + 1, new_segment); // casa QVector
                possible_corners.insert(i + j, false);
                insertIdx(i + j + 1);
            }
            removeSegment(i, i);
            numSegments += (seg_size - 1);
            i += (seg_size - 1);
        }
    }
}
bool KalmanSet::getTooMuchMeasures() const
{
    return tooMuchMeasures;
}

