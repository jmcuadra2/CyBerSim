/*
    <one line to give the library's name and an idea of what it does.>
    Copyright (C) 2011  Jose Manuel Cuadra Troncoso <jmcuadra@dia.uned.es>

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*/

#include "areacenterslamsegmentcontrol.h"
#include "smginterface.h"
#include "../slam/scalekalmanalgo.h"

// #define COUT_DEBUG

AreaCenterSLAMSegmentControl::AreaCenterSLAMSegmentControl() : AreaCenterSLAMControl(), AreaCenterSegmentedControl()
{
    pms_method = PMS_KALMAN;
}

AreaCenterSLAMSegmentControl::~AreaCenterSLAMSegmentControl()
{

}

void AreaCenterSLAMSegmentControl::sendOutputs(void)
{
#ifdef COUT_DEBUG  
  cout << "---insxxx AreaCenterSLAMSegmentControl::sendOutputs(void) \n";
#endif  
  
  AreaCenterSLAMControl::sendOutputs();
  
#ifdef COUT_DEBUG  
  cout << "---insxxx END OF AreaCenterSLAMSegmentControl::sendOutputs(void) \n";
  #endif 
}

void AreaCenterSLAMSegmentControl::calculateSegments()
{
#ifdef COUT_DEBUG
  cout << "---insxxx AreaCenterSLAMSegmentControl::calculateSegments(void) \n";
#endif
//     algo->medianFilter(&referenceScan);
//     algo->medianFilter(&actualScan);
//     algo->findFarPoints(&referenceScan);
//     algo->findFarPoints(&actualScan);
//     algo->segmentScan(&referenceScan);

//     algo->segmentScan(&actualScan);


//    stream << scan.t()<< " " << scan.rx()<< " " << scan.ry()<< " " << scan.th()<< endl;

//    cout << "Slam " << getSMGInterface()->getMotor()->getSlamOdometry();
//    cout << "   Exact " << getSMGInterface()->getMotor()->getExactOdometry();
//    cout << "   Raw " << getSMGInterface()->getMotor()->getRawOdometry() << endl;

#ifdef COUT_DEBUG
  cout << "---insxxx AreaCenterSLAMSegmentControl::calculateSegments(void) filterData = " << endl;
  cout << "---insxxx [";
  for(int i = 0; i < 181; i++)
      cout << " " << filterData.at(i) << " ";
  cout << "]" << endl;
#endif
  
  actualScan.setR(filterData);
  
  for(int i=0; i < PM_L_POINTS; i++)
  {
//     ref.setX(i,ref.r(i)*pm_co[i]);
//     ref.setY(i,ref.r(i)*pm_si[i]);
//     if(ref.r(i) < 0)
//       ref.setBad(i, 1);
//     else
//       ref.setBad(i, 0);
    if(actualScan.r(i) < 0 || actualScan.r(i) >= sensorMaxVal*2)
      actualScan.setBad(i, 1);
    else
      actualScan.setBad(i, 0);
  }//for i  
  
  
#ifdef COUT_DEBUG  
  cout << "---insxxx AreaCenterSLAMSegmentControl::calculateSegments(void) BEFORE actualScan = " << actualScan << endl;
#endif
// Using segmentation; see scalekalmanalgo.h

  AreaCenterSegmentedControl::segmentsEnds = segmentation->getEndPoints();
  AreaCenterSLAMControl::segmentsEnds = segmentation->getEndPoints();
// Copy segmentsEnds segmentation into actualScan


  int cnt_segs = 0;
  int size_segs = AreaCenterSegmentedControl::segmentsEnds.size();
  if(size_segs == 0)
    return;
  pair<int, int> ends = AreaCenterSegmentedControl::segmentsEnds[cnt_segs];
  QPointF p; 
  
  for (int i = 0; i < PM_L_POINTS; i++) {
    if(actualScan.bad(i) == 1) {
      actualScan.setSeg(i, 0);
      continue;
    }
    if(ends.first == ends.second) {
      actualScan.setSeg(i, 0);
    } 
    else {
      actualScan.setSeg(i, cnt_segs + 1);
  //     if(rhoEst.size() == cnt_segs)
  //       rhoEst.append();
      p = (*segmentsPolygon)[i + 4];
      actualScan.setX(i, -p.y()); // en el algoritmo cambia el eje y por el x.
  //     actualScan.setX(i, p.x());
      actualScan.setY(i, p.x());
  //     actualScan.setY(i, p.y());
    }
    if(i == ends.second && i < PM_L_POINTS - 1){
      cnt_segs++;
      if(size_segs == cnt_segs) {
	ends = AreaCenterSegmentedControl::segmentsEnds[cnt_segs - 1];
	ends.first = ends.second + 1;
	ends.second = PM_L_POINTS - 1;	  
      }
      else
	ends = AreaCenterSegmentedControl::segmentsEnds[cnt_segs];
    }   
  }
  actualScan.setRhoEst(segmentation->getRhoEst());
  actualScan.setPhiEst(segmentation->getPhiEst());
  actualScan.setSigmaEst(segmentation->getSigmaEst());
  actualScan.setSegmentsEnds(segmentation->getEndPoints());
    
#ifdef COUT_DEBUG
  cout << "---insxxx AreaCenterSLAMSegmentControl::calculateSegments(void) AFTER actualScan = " << actualScan << endl;
#endif
  
  QVector<double> rhoEst = segmentation->getRhoEst();
  QVector<double> phiEst = segmentation->getPhiEst();
  
  int idx;
  int numSegments = AreaCenterSegmentedControl::segmentsEnds.size();
  double r_i, x, y;
  endPoints.resize(numSegments);
  
  for(int i = 0; i < numSegments; i++) {

    idx = AreaCenterSegmentedControl::segmentsEnds.at(i).first; 
    r_i = abs(rhoEst.at(i)/cos(angle[idx] - phiEst.at(i)));
    x = r_i*cos(angle[idx]);
    y = r_i*sin(angle[idx]);
    endPoints[i].first.setX(x);
    endPoints[i].first.setY(y);

    idx = AreaCenterSegmentedControl::segmentsEnds.at(i).second;    
    r_i = abs(rhoEst.at(i)/cos(angle[idx] - phiEst.at(i)));
    x = r_i*cos(angle[idx]);
    y = r_i*sin(angle[idx]);
    endPoints[i].second.setX(x);
    endPoints[i].second.setY(y);       
  
  }
  actualScan.setEndPoints(endPoints);

#ifdef COUT_DEBUG
    cout << "---insxxx END OF AreaCenterSLAMSegmentControl::calculateSegments(void) \n";
#endif    
    
}

void AreaCenterSLAMSegmentControl::printSegmentationResults()
{
  if(segmentation)
    segmentation->printResults(2);
}

void AreaCenterSLAMSegmentControl::initElements(void)
{
  AreaCenterSLAMControl::initElements();
  segmentation->setSensorMaxVal(sensorMaxVal*2); // ñapa *2
}

bool AreaCenterSLAMSegmentControl::getTooMuchMeasures() const
{
  bool ret = false;
  if(segmentation)
    ret = segmentation->getTooMuchMeasures();
  return ret;
}
