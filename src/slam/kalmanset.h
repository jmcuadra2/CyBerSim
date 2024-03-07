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
#ifndef KALMANSET_H
#define KALMANSET_H

#include "kalmansegment.h"

using namespace std;

/**
    @author Jose Manuel Cuadra Troncoso <jmcuadra@dia.uned.es>
*/
class KalmanSet{
public:
    KalmanSet(size_t numSensors, double sensorField, double probabil, double startAngle = -90, const bool& twoStage = true);
    
    ~KalmanSet();

    void doEstimation(QVector< pair < int , int > > measuredAreas, QVector < pair <int, int> > seeds, bool verbose = false);
    //     void initRadius(double radius[]);
    void setRadius(double radius[]);
    void createSegments(void);
    void equalityTest(void);
    void clean(void);
    void printResults(int minNumber = 0);
    double* t_values(void);

    QVector< pair < int , int > > getEndPoints() const { return endPoints; }
    QVector< double > getRhoEst() const { return rhoEst; }
    QVector< double > getPhiEst() const { return phiEst; }
    QVector< double > getSigmaEst() const { return sigmaEst; }
    QVector< double > getFilterData(void);
    QVector<pair < int, int> > getConnectedAreas(void);
    QVector< QMatrix2x2 > getSw() const { return Sw; }

    static bool overlapped(int numSensors, bool full360, pair<int, int> segment_1, pair<int, int> segment_2, int* max_left, int* min_right);
    static const double RAD2GRAD;

    bool getTooMuchMeasures() const;

protected:
    void look4Corners(void);
    void look4Holes(void);
    int look4HolesStrict(void);
    void newSeed(int start, int last, QVector< pair < int , int > >& seeds_new, bool prepend = false);
    pair < int , int > computeSeed(int start, int last);

    void calculateLimits(int i, int j, int* left, int* right);
    
    void insertSegments(void);
    //     bool idxOverlap(KalmanSegment* segment_1 , KalmanSegment* segment_2, const int& cnt_seg, QVector<int>& idx, QVector<int>& idx_i1, QVector<int>& idx_i10, QVector<int>& idx_i11, QVector<int>& idx_i2, QVector<int>& idx_i20, QVector<int>& idx_i21);

    bool overlapped(KalmanSegment * segment_1, KalmanSegment * segment_2, int* max_left, int* min_right);

    bool idxOverlap(KalmanSegment* segment_1 , KalmanSegment* segment_2, const int& cnt_seg, QVector<int>& idx_i1, QVector<int>& idx_i2, int* end_left = 0, int* end_right = 0);

    void resolveSimpleOverlaps(int reg_start = -1, int reg_end = -1);
    void resolveOverlaps(int reg_start = -1, int reg_end = -1);

    bool isOver(KalmanSegment* segment_1, KalmanSegment* segment_2);
    bool overlapSwap(int* i, int i_idx, int reg_start, int reg_end, int num_overlaps, int* last_idx2, bool withIdx = true);

    void removeSimpleOverlap(int* i, KalmanSegment::Side side);
    void removeOverlap(int* i, KalmanSegment::Side side, int reg_start = -1, int* reg_end = 0);
    void removeSegment(int i, int removeIdxs = -1);
    void removeSegment(KalmanSegment* segment);
    
    KalmanSegment* getNewSegment(bool withIdx = false);
    void reEstimate(void);
    //     bool mergeSegments(int i , int i_plus1, KalmanSegment::Side& pos2del, double confidence = -1);
    bool mergeSegments(int i , int i_plus1, double confidence = -1);
    
    bool varianceTest(int n_1, double s2_1, int n_2, double s2_2);
    void insertIdx(int i);
    
    void breakShortSegments();

protected:
    QVector<KalmanSegment*> segmentsStore;
    QVector<bool> currentSegments;
    QVector<bool> usedSegments; // para saber cuales se ha usado y limpiarlos
    QVector<KalmanSegment*> segments;
    QVector< pair < int , int > > seeds;
    QVector<int> idxSeeds;
    uint numSensors;
    double *angle, *radius;
    double confidenceLevel, sensorField, sensorAperture, startAngle;
    bool full360;
    QVector <double> t_vals;
    int numSegments;
    int maxNumSegments;
    int semilong;

    QVector < QVector<int> > idx_1;
    QVector < QVector<int> > idx_2;
    QVector < QVector<int> > idx_10;
    QVector < QVector<int> > idx_20;
    QVector < QVector<int> > idx_11;
    QVector < QVector<int> > idx_21;
    QVector<int> possible_corners;

    bool verbose;

    QVector<pair<int, int> > endPoints;
    QVector<double> rhoEst;
    QVector<double> phiEst;
    QVector<double> sigmaEst;
    QVector< QMatrix2x2 > Sw;

    QVector< pair < int , int > > measuredAreas;
    int numMeasures;
    
    bool twoStage;
    bool tooMuchMeasures;
    
};

#endif
