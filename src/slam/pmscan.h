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
#ifndef PMSCAN_H
#define PMSCAN_H

#include <QVector>
#include <QPointF>
#include "../neuraldis/timestamp.hpp"

//#define PM_L_POINTS   181 //maximum nu
//#define PM_TYPE double  // change it to double for higher accuracy
typedef double PM_TYPE;

#define PM_PSD_SCANNER      0
#define PM_HOKUYO_URG_04LX  1
#define PM_SICK_LMS200      2
#define PM_HOKUYO_UTM_30LX  3

//STEP 2) Set the type your laser range finder here:
  #define PM_LASER PM_SICK_LMS200
//  #define PM_LASER PM_HOKUYO_URG_04LX
//  #define PM_LASER PM_PSD_SCANNER
//  #define PM_LASER PM_HOKUYO_UTM_30LX

#if PM_LASER ==  PM_PSD_SCANNER
  #define PM_LASER_NAME       "PSD_Scanner" //< The name of the laser range finder.
  #define PM_L_POINTS         200 //< Maximum number of points in a scan.
  #define PM_FOV              360 //< Field of view of the laser range finder in degrees.
  #define PM_MAX_RANGE        4000 //< [mm] Maximum valid laser range .
  #define PM_MIN_VALID_POINTS 100 //< Minimum number of valid points for scan matching.
  #define PM_SEARCH_WINDOW    50  //< Half window size which is searched for correct orientation.
  #define PM_CORRIDOR_THRESHOLD 25.0 //< Threshold for angle variation between points to determine if scan was taken of a corridor.
#elif PM_LASER ==  PM_HOKUYO_URG_04LX
  #define PM_LASER_NAME       "Hokuyo URG-04LX" //< The name of the laser range finder.
  #define PM_L_POINTS         682 //< Maximum number of points in a scan.
  #define PM_FOV              240 //< Field of view of the laser range finder in degrees.
  #define PM_MAX_RANGE        5300 //< [mm] Maximum valid laser range.
  #define PM_MIN_VALID_POINTS 200 //< Minimum number of valid points for scan matching.
  #define PM_SEARCH_WINDOW    80  //< Half window size which is searched for correct orientation.
  #define PM_CORRIDOR_THRESHOLD 25.0 //< Threshold for angle variation between points to determine if scan was taken of a corridor.
#elif PM_LASER ==  PM_SICK_LMS200
  #define PM_LASER_NAME       "Sick LMS" //< The name of the laser range finder.
  #define PM_L_POINTS         181  //< Maximum number of points in a scan.
  #define PM_FOV              180  //< Field of view of the laser range finder in degrees.
  #define PM_MAX_RANGE        8000 //< [mm] Maximum valid laser range.
  #define PM_MIN_VALID_POINTS 30   //< Minimum number of valid points for scan matching.
  #define PM_SEARCH_WINDOW    20   //< Half window size which is searched for correct orientation.
  #define PM_CORRIDOR_THRESHOLD 25.0 //< Threshold for angle variation between points to determine if scan was taken of a corridor.
#elif PM_LASER ==  PM_HOKUYO_UTM_30LX
  #define PM_LASER_NAME       "HOKUYO UTM-30LX" //< The name of the laser range finder.
  #define PM_L_POINTS         1081  //< Maximum number of points in a scan.
  #define PM_FOV              270  //< Field of view of the laser range finder in degrees.
  #define PM_MAX_RANGE        7000  //< [mm] Maximum valid laser range. (3000 for this sensor.)
  #define PM_MIN_VALID_POINTS 300   //< Minimum number of valid points for scan matching.
  #define PM_SEARCH_WINDOW    200   //< Half window size which is searched for correct orientation.
  #define PM_CORRIDOR_THRESHOLD 25.0 //< Threshold for angle variation between points to determine if scan was taken of a corridor.
#endif

//#define PM_LASER_NAME       "Sick LMS" // The name of the laser range finder.
//#define PM_L_POINTS         181  // Maximum number of points in a scan.
//#define PM_FOV              180  // Field of view of the laser range finder in degrees.
//#define PM_MAX_RANGE        8000 // [mm] Maximum valid laser range.
//#define PM_MIN_VALID_POINTS 30   // Minimum number of valid points for scan matching.
//#define PM_SEARCH_WINDOW    20   // Half window size which is searched for correct orientation.
//#define PM_CORRIDOR_THRESHOLD 25.0 // Threshold for angle variation between points to determine if scan was taken of a corridor.

/**
  @author Jose Manuel Cuadra Troncoso <jmcuadra@dia.uned.es>
*/

//typedef float PM_TYPE; // change it to double for higher accuracy

class PMScan
{
  public:
    PMScan();

    ~PMScan();

//     friend ostream &operator<<(ostream &stream, PMScan const& scan)
//     {
//       stream << "PMScan ";
//       stream << scan.t()<< " " << scan.rx()<< " " << scan.ry()<< " " << scan.th()<< endl;
//
//       stream << "r = [";
//       for(unsigned int i = 0; i < PM_L_POINTS; i++)
//         stream << scan.r().at(i) << " ";
//       stream << "] " << endl;
//
//       stream << "x = [";
//       for(unsigned int i = 0; i < PM_L_POINTS; i++)
//         stream << scan.x().at(i) << " ";
//       stream << "] " << endl;
//
//       stream << "y = [";
//       for(unsigned int i = 0; i < PM_L_POINTS; i++)
//         stream << scan.y().at(i) << " ";
//       stream << "] " << endl;
//
//       stream << "bad = [";
//       for(unsigned int i = 0; i < PM_L_POINTS; i++)
//         stream << scan.bad().at(i) << " ";
//       stream << "] " << endl;
//
//       stream << "seg = [";
//       for(unsigned int i = 0; i < PM_L_POINTS; i++)
//         stream << scan.seg().at(i) << " ";
//       stream << "] " << endl;
//
//       return stream;
//     }

    friend ostream &operator<<(ostream &stream, PMScan const& scan)
    {
      stream << "PMScan ";
      stream << scan.t()<< " " << scan.rx()<< " " << scan.ry()<< " " << scan.th()<< endl;

      stream<< " i\t" << "   r\t" << "bad\t" << "seg" << endl;
      for(unsigned int i = 0; i < PM_L_POINTS; i++) {
    stream << i << "\t";
        stream << scan.r().at(i) << "\t";
        stream << scan.bad().at(i) << " \t";
        stream << scan.seg().at(i) << " ";
    stream << endl;
      }

      return stream;
    }

    void setT(TimeStamp t) {
      m_t = t;
    }

    TimeStamp t() const {
      return m_t;
    }

    void setRx(const PM_TYPE& rx) {
      m_rx= rx;
    }

    PM_TYPE rx() const {
      return m_rx;
    }

    void setRy(const PM_TYPE& ry) {
      m_ry = ry;
    }

    PM_TYPE ry() const {
      return m_ry;
    }

    void setTh(const PM_TYPE& th) {
      m_th = th;
    }

    PM_TYPE th() const {
      return m_th;
    }

    void setR(const QVector< PM_TYPE >& r) {
      m_r = r;
    }

    void setR(int i, PM_TYPE r) {
      m_r[i] = r;
    }

    QVector< PM_TYPE > r() const {
      return m_r;
    }

    PM_TYPE r(int i) const {
      return m_r.at(i);
    }

    void setX(const QVector< PM_TYPE >& x) {
      m_x = x;
    }

    void setX(int i, PM_TYPE x) {
      m_x[i] = x;
    }

    QVector< PM_TYPE > x() const {
      return m_x;
    }

    PM_TYPE x(int i) const {
      return m_x.at(i);
    }

    void setY(const QVector< PM_TYPE >& y) {
      m_y = y;
    }

    void setY(int i, PM_TYPE y) {
      m_y[i] = y;
    }

    QVector< PM_TYPE > y() const {
      return m_y;
    }

    PM_TYPE y(int i) const {
      return m_y.at(i);
    }

    void setBad(const QVector< int >& bad) {
      m_bad = bad;
    }

    void setBad(int i, int bad) {
      m_bad[i] = bad;
    }

    QVector< int > bad() const {
      return m_bad;
    }

    int bad(int i) const {
      return m_bad.at(i);
    }

    void setSeg(const QVector< int >& seg) {
      m_seg = seg;
    }

    void setSeg(int i, int seg) {
      m_seg[i] = seg;
    }

    QVector< int > seg() const {
      return m_seg;
    }

    int seg(int i) const {
      return m_seg.at(i);
    }

    void setRhoEst(QVector<double> rhoEst) { this->rhoEst = rhoEst; }
    void setPhiEst(QVector<double> phiEst) { this->phiEst = phiEst; }
    void setSigmaEst(QVector<double> sigmaEst) { this->sigmaEst = sigmaEst; }
    void setSegmentsEnds(QVector<pair<int, int> > segmentsEnds);
    void setEndPoints(QVector< pair<QPointF,QPointF> > endPoints) { this->endPoints = endPoints; }

    QVector<double> getRhoEst(void) const { return rhoEst; }
    QVector<double> getPhiEst(void) const { return phiEst; }
    QVector<double> getSigmaEst(void) const { return sigmaEst; }
    QVector<pair<int, int> > getSegmentsEnds() { return segmentsEnds; }
    QVector< pair<QPointF,QPointF> > getEndPoints() { return endPoints; }
    QVector<int> getSegmentsSizes() { return segmentsSizes; }

  protected:
    TimeStamp   m_t;    //time when scan was taken
    PM_TYPE  m_rx;   //robot odometry pos
    PM_TYPE  m_ry;   //robot odometry pos
    PM_TYPE  m_th;   //robot orientation
    QVector<PM_TYPE>  m_r;//[mm] - 0 or negative ranges denote invalid readings.
    QVector<PM_TYPE>  m_x;//[mm]
    QVector<PM_TYPE>  m_y;//[mm]
    QVector<int>     m_bad;// 0 if OK
    //sources of invalidity - too big range;
    //moving object; occluded;mixed pixel
    QVector<int>     m_seg;//number describing into which segment the point belongs to

    QVector<double> rhoEst;
    QVector<double> phiEst;
    QVector<double> sigmaEst;
    QVector<pair<int, int> > segmentsEnds;
    QVector< pair<QPointF,QPointF> > endPoints;
    QVector<int> segmentsSizes;
};

#endif
