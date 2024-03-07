//
// C++ Implementation: areacentersegmentedcontrol
//
// Description: 
//
//
// Author: Jose Manuel Cuadra Troncoso <jmcuadra@dia.uned.es>, (C) 2011
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "areacentersegmentedcontrol.h"
#include "../slam/scalekalmanalgo.h"
#include <QList>
#include <QVariant>

// #define COUT_DEBUG

AreaCenterSegmentedControl::AreaCenterSegmentedControl(): AreaCenterReactiveControl()
{
    segmentation = new ScaleKalmanAlgo();
    QList<QVariant> params;

    int numLevels = 24;
    int maxLevel = 1;
    double k_semi_amplitude = 6;
    double area_lim = 50;
    double k_filter = 2.5;
    double confidenceLevel1 = 5;
    double confidenceLevel2 = 0.99;
//    bool verbose = true;
    bool verbose = false;
    full360 = false;

    params.append(QVariant(numLevels));
    params.append(QVariant(maxLevel));
    params.append(QVariant(k_semi_amplitude));
    params.append(QVariant(area_lim));
    params.append(QVariant(k_filter));
    params.append(QVariant(confidenceLevel1));
    params.append(QVariant(confidenceLevel2));
    params.append(QVariant(verbose));

    //  int beamNumSensors = 181;
    //  int beamAmplitude = 180;
    //  int beamStart = -90;
    //  if(inputNodeMap.contains("LineLaser_AreaCenter_sensors")) {
    //      QVector< AbstractFusionNode* > inputNodeList = inputNodeMap.value("LineLaser_AreaCenter_sensors");
    //      beamNumSensors = inputNodeList.size();
    //      double start_ang = inputNodeList.first()->result().getPose()->getOrientation().at(0);
    //      double end_ang = inputNodeList.last()->result().getPose()->getOrientation().at(0);
    //      beamAmplitude = (int)((end_ang - start_ang)*NDMath::RAD2GRAD + 0.5);
    //      beamStart = (int)(start_ang*NDMath::RAD2GRAD + 0.5);
    //  }

    //  full360 = beamAmplitude == 360 ? true : false;

    //  sonar180 = full360 ? 0 : 4;
    //  if(inputNodeMap.contains("BeamSonar_AreaCenter_sensors") && !full360) {
    //      QVector< AbstractFusionNode* > inputNodeList = inputNodeMap.value("BeamSonar_AreaCenter_sensors");
    //      sonar180 = inputNodeList.size() / 2;
    //  }

    segmentation->setParams(params);
    //  segmentation->setBeamFeatures(beamNumSensors, beamAmplitude, beamStart);
    //  segmentation->init();
    //  filterData.resize(beamNumSensors);
    //  angle.resize(beamNumSensors);
    //  for(int i = 0; i < beamNumSensors; i++)
    //    angle[i] = (beamStart + i)/NDMath::RAD2GRAD;

    //  segmentation->initMeasures(filterData.data());
    segmentsPolygon = 0;

}

AreaCenterSegmentedControl::~AreaCenterSegmentedControl()
{
    delete segmentation;
}

//int AreaCenterSegmentedControl::getType(void)
//{
//  return AREA_CENTER_SEGMENT_CONTROL;
//}

void AreaCenterSegmentedControl::connectIO()
{
    AreaCenterReactiveControl::connectIO();


    int beamNumSensors = 181;
    int beamAmplitude = 180;
    int beamStart = -90;
    if(inputNodeMap.contains("LineLaser_AreaCenter_sensors")) {
        QVector< AbstractFusionNode* > inputNodeList = inputNodeMap.value("LineLaser_AreaCenter_sensors");
        beamNumSensors = inputNodeList.size();
        double start_ang = inputNodeList.first()->result().getPose()->getOrientation().at(0);
        double end_ang = inputNodeList.last()->result().getPose()->getOrientation().at(0);
        beamAmplitude = (int)((end_ang - start_ang)*NDMath::RAD2GRAD + 0.5);
        beamStart = (int)(start_ang*NDMath::RAD2GRAD + (start_ang < 0 ? -0.5 : 0.5));
    }

    full360 = beamAmplitude >= 360 - 360.0/beamNumSensors ? true : false;
    beamAmplitude = full360 ? 360 : beamAmplitude;

    sonar180 = full360 ? 0 : 4;
    if(inputNodeMap.contains("BeamSonar_AreaCenter_sensors") && !full360) {
        QVector< AbstractFusionNode* > inputNodeList = inputNodeMap.value("BeamSonar_AreaCenter_sensors");
        sonar180 = inputNodeList.size() / 2;
    }

    segmentation->setBeamFeatures(beamNumSensors, beamAmplitude, beamStart);
    segmentation->init();
    filterData.resize(beamNumSensors);
    angle.resize(beamNumSensors);
    for(int i = 0; i < beamNumSensors; i++)
        angle[i] = (beamStart + i)/NDMath::RAD2GRAD;

    segmentation->initMeasures(filterData.data());

}

void AreaCenterSegmentedControl::initElements()
{
    AreaCenterReactiveControl::initElements();
    segmentation->setSensorMaxVal(sensorMaxVal*2); // ñapa *2
}

void AreaCenterSegmentedControl::readGlobalPolygon(void)
{
    double d, x, y;
    double max = -1;
    int numNodes = areaCenterNodeList.size();
    //   areaCenterPolygon->clear();

#ifdef COUT_DEBUG
    cout << "---insxxx AreaCenterSegmentedControl::readGlobalPolygon(void) \n";
#endif
    TimeStamp start_tick = TimeStamp::nowMicro();

    for(int i = sonar180; i < numNodes - sonar180; i++) { // 8 sonar
        d = areaCenterNodeList[i]->result().getY().at(0);
        if(d < 0)
            int pp = 0;
        max = d > max ? d : max;
        filterData[i - sonar180] = d;
        //     NDPose* pose = areaCenterNodeList[i]->result().getPose();
        //     NDPose ppose = *pose;
        //     std::cout << ppose << std::endl;
    }

#ifdef COUT_DEBUG
    cout << "---insxxx AreaCenterSegmentedControl::readGlobalPolygon(void) BEFORE SEGMENTATION filterData = " << endl;
    cout << "---insxxx [";
    for(int i = 0; i < beamNumSensors; i++)
        cout << " " << filterData.at(i) << " ";
    cout << "]" << endl;

    cout << "---insxxx END OF AreaCenterSegmentedControl::readGlobalPolygon(void) \n";
#endif

    segmentation->setMeasures(filterData.data());
    segmentation->execute();
    segmentation->printResults();
    filterData = segmentation->getFilterData();

    falseReadings = 0;
    //   max = max < 1000 ? 1000 : max;
    for(int i = 0; i < numNodes; i++) {
        if((i < sonar180) || (i >= numNodes - sonar180))
            d = areaCenterNodeList[i]->result().getY().at(0);
        else
            d = filterData.at(i - sonar180);
        //     if(d >= 0 && (d < 1))
        //       int pp = 0;
        if(d < 0.0) {
            //       d = max;
            falseReadings++;
            /*    x = areaCenterNodeList[i]->result().getY().at(2)/1000.0*max; // 1000 viene asignacion de max y de InputFusionNode2D::proceessInformation()
      y = areaCenterNodeList[i]->result().getY().at(3)/1000.0*max; */
            x = areaCenterNodeList[i]->result().getY().at(2); // 1000 viene asignacion de max y de InputFusionNode2D::proceessInformation()
            y = areaCenterNodeList[i]->result().getY().at(3);
            //       areaCenterPolygon->setAdvanceStatus(AreaCenterPolygon::Conservative);
            //       continue;
        }
        else {
            if((i < sonar180) || (i >= numNodes - sonar180)) {
                x = areaCenterNodeList[i]->result().getY().at(2);
                y = areaCenterNodeList[i]->result().getY().at(3);
            }
            else {
                x = d*cos(angle.at(i - sonar180));
                y = d*sin(angle.at(i - sonar180));
            }
        }
        //     areaCenterPolygon->append(QPoint(x, y));
        (*areaCenterPolygon)[i].setX(x);
        (*areaCenterPolygon)[i].setY(y);
    }
    //   if(falseReadings) {
    //     if(advanceStatus == AreaCenterPolygon::All) {
    //       areaCenterPolygon->setAdvanceStatus(AreaCenterPolygon::Conservative);
    //       areaCenterPolygon->setAdvanceMaxRange(double(numNodes - falseReadings)/numNodes*sensorMaxVal);
    //     }
    //   }

    segmentsPolygon->setEndPoints(segmentation->getEndPoints());
    segmentsPolygon->setRhoEst(segmentation->getRhoEst());
    segmentsPolygon->setPhiEst(segmentation->getPhiEst());
    //   segmentsPolygon->setConnectedAreas(segmentation->getConnectedAreas());

    start_tick = TimeStamp::nowMicro()-start_tick;
    segmentationTime = start_tick.second;

#ifdef COUT_DEBUG  
    cout << "---insxxx AreaCenterSegmentedControl::readGlobalPolygon(void) AFTER SEGMENTATION filterData = " << endl;
    cout << "---insxxx [";
    for(int i = 0; i < beamNumSensors; i++)
        cout << " " << filterData.at(i) << " ";
    cout << "]" << endl;

    cout << "---insxxx END OF AreaCenterSegmentedControl::readGlobalPolygon(void) \n";
#endif
}

void AreaCenterSegmentedControl::createAreaCenterPolygon(void)
{
#ifdef COUT_DEBUG 
    cout << "---insxxx AreaCenterSegmentedControl::createAreaCenterPolygon(void) \n";
#endif
    segmentsPolygon = new AreaCenterPolygonSegmented();
    areaCenterPolygon = segmentsPolygon;
    areaCenterPolygon->setWorld(world);

#ifdef COUT_DEBUG 
    cout << "---insxxx END OF AreaCenterSegmentedControl::createAreaCenterPolygon(void) \n";
#endif
}


