//
// C++ Interface: areacenterslamcontrol
//
// Description:
//
//
// Author: Jose Manuel Cuadra Troncoso <jmcuadra@dia.uned.es>, (C) 2009
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef AREACENTERSLAMCONTROL_H
#define AREACENTERSLAMCONTROL_H

#include "areacenterreactivecontrol.h"
#include <QTextStream>
#include <QFile>
#include <Aria.h>

#include "../slam/pmsalgo.h"

/**
  @author Jose Manuel Cuadra Troncoso <jmcuadra@dia.uned.es>
*/
class AreaCenterSLAMControl : public virtual AreaCenterReactiveControl
{

  public:
    AreaCenterSLAMControl();

    virtual ~AreaCenterSLAMControl();

    void sendOutputs(void);
    void initElements(void);
    int getSuperType(void);
    int getType(void);
    void sampleInputs();
//    void connectIO(void);
    virtual bool getTooMuchMeasures() const { return false; }
    
protected:
    virtual void calculateSegments();
    virtual void drawScan(NDPose pose, SlamPainter* painter);
    virtual void printSegmentationResults() {}
    void diamondConnectIO(void);

  protected:
    PMScan referenceScan, actualScan, rawActualScan, rawReferenceScan;
    NDPose2D rawOdometry;
//     NDPose* exactOdometry;
    NDPose2D slamOdometry, prevSlamOdometry, correctedSlamOdometry, prevMatchedSlamOdometry;
    QVector<PM_TYPE>   pm_fi;//contains precomputed angles (0-180)
    QVector<PM_TYPE>   pm_si;//contains sinus of angles
    QVector<PM_TYPE>   pm_co;//contains cos of angles
    ScanMatchingAlgo * algo; 
//     SlamPainter* slamPainter;
    QVector<pair<int, int> > segmentsEnds;
    QVector< pair<QPointF,QPointF> > endPoints;

    int cnt;
    bool start;
    
    int sim_time_step;
    int match_counter;
//    FILE* fileoutput;
//    QFile fileoutput;
//    QTextStream myLog;

    double totalTrans, totalRot;

    ArInterpolation interpolSlam;
    ArInterpolation interpolRaw;
    int pms_method;

    bool differentTimeScan;

    ArPose poseRaw, poseSlam, prevPoseRaw, prevPoseSlam;
};

#endif
