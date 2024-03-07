//
// C++ Implementation: areacenterslamcontrol
//
// Description:
//
//
// Author: Jose Manuel Cuadra Troncoso <jmcuadra@dia.uned.es>, (C) 2009
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "areacenterslamcontrol.h"
#include "smginterface.h"
#include <iostream>

using namespace std;

//#define COUT_DEBUG
//#define COUT_DEBUG_2

AreaCenterSLAMControl::AreaCenterSLAMControl(): AreaCenterReactiveControl(), pm_fi(PM_L_POINTS), pm_si(PM_L_POINTS), pm_co(PM_L_POINTS)
{
    for(int i=0;i<PM_L_POINTS;i++)
    {
        pm_fi[i] = ((((float)i)*M_PI)/180.0)-M_PI/2.0;
        pm_si[i] = sin(pm_fi[i]);
        pm_co[i] = cos(pm_fi[i]);
    }

    algo = ScanMatchingAlgo::factory(ScanMatchingAlgo::PM_PSM);
    pms_method = PMS_DIOSI;

    // PMSAlgo::setPMSVariables(const int method, const nlopt_algorithm nlopt_selection, const bool nlopt_grad_refinement_flag, const nlopt_algorithm nlopt_flag_grad_refinement, const int log_level_input)
    ((PMSAlgo*)algo)->setPMSVariables(PMS_DIOSI,NLOPT_LN_BOBYQA,false,NLOPT_LD_MMA,PMS_LOG_OFF);

    algo->setFi(pm_fi);
    algo->setSinFi(pm_si);
    algo->setCosFi(pm_co);

    cnt = 0;
    start = false;
    slamPainter = new SlamPainter();
    rawPainter = new SlamPainter();
    //   slamPainter->setParams(-12000, -12000, 12000, 12000, sensorMaxVal);
    slamPainter->hide();
    rawPainter->hide();

    // xxxins
    sim_time_step = 1;
    match_counter = 0;

    QString dir = QDir::currentPath();

    fileoutput.setFileName("/home/jose/kossforslam/slam/areacenterslamcontrol.txt");
    if(!fileoutput.open(QIODevice::WriteOnly))
    {
        cerr << "Cannot open file: " << "/home/jose/cybersim/head/examples/slam/areacenterslamcontrol.txt" << Qt::endl;
        cerr << "Exiting program, AreaCenterSLAMControl::AreaCenterSLAMControl()" << Qt::endl;
        exit(8);
    };
    myLog.setDevice(&fileoutput);

    connect(slamPainter, SIGNAL(destroyed(QObject*)), this, SLOT(slamPainterClosed()));
    connect(rawPainter, SIGNAL(destroyed(QObject*)), this, SLOT(rawPainterClosed()));
    // xxxins
    // xxxins

}


AreaCenterSLAMControl::~AreaCenterSLAMControl()
{
    if(slamPainter)
        delete slamPainter;
    if(rawPainter)
        delete rawPainter;
    fileoutput.flush();
    fileoutput.close();
}

int AreaCenterSLAMControl::getSuperType(void)
{
    return CODE_CONTROL;
}

int AreaCenterSLAMControl::getType(void)
{
    return AREA_CENTER_SLAM_CONTROL;
}

//void AreaCenterSLAMControl::connectIO(void) {
//    AreaCenterReactiveControl::connectIO();

//    if(slamPainter) {
//        slamPainter->setParams(world->getXMinPosition() * world->getWorldScale(), world->getYMinPosition() * world->getWorldScale(), world->getWidth() * world->getWorldScale(), world->getHeight() * world->getWorldScale(), sensorMaxVal*2, getSMGInterface()->getRobot()->getX(), getSMGInterface()->getRobot()->getY(), getSMGInterface()->getRobot()->getRotation()); // ñapa * 2
//        slamPainter->show();
//    }
//    if(rawPainter) {
//        rawPainter->setParams(world->getXMinPosition() * world->getWorldScale(), world->getYMinPosition() * world->getWorldScale(), world->getWidth() * world->getWorldScale(), world->getHeight() * world->getWorldScale(), sensorMaxVal*2, getSMGInterface()->getRobot()->getX(), getSMGInterface()->getRobot()->getY(), getSMGInterface()->getRobot()->getRotation()); // ñapa * 2
//        rawPainter->show();
//    }
//}

void AreaCenterSLAMControl::diamondConnectIO()
{
    if(!world && !getSMGInterface())
        return;

    if(slamPainter) {
        slamPainter->setParams(world->getXMinPosition() * world->getWorldScale(), world->getYMinPosition() * world->getWorldScale(), world->getWidth() * world->getWorldScale(), world->getHeight() * world->getWorldScale(), sensorMaxVal*2, getSMGInterface()->getRobot()->getX(), getSMGInterface()->getRobot()->getY(), getSMGInterface()->getRobot()->getRotation()); // ñapa * 2
        slamPainter->show();
    }
    if(rawPainter) {
        rawPainter->setParams(world->getXMinPosition() * world->getWorldScale(), world->getYMinPosition() * world->getWorldScale(), world->getWidth() * world->getWorldScale(), world->getHeight() * world->getWorldScale(), sensorMaxVal*2, getSMGInterface()->getRobot()->getX(), getSMGInterface()->getRobot()->getY(), getSMGInterface()->getRobot()->getRotation()); // ñapa * 2
        rawPainter->show();
    }
}

void AreaCenterSLAMControl::sendOutputs(void)
{
#ifdef COUT_DEBUG_2
    cout << "---insxxx AreaCenterSLAMControl::sendOutputs(void) \n";
#endif

    AreaCenterReactiveControl::sendOutputs();

    NDPose2D deltaOdometry;


    slamOdometry.setCoordinates(getSMGInterface()->getMotor()->getSlamOdometry().getPosition().at(0),
                                getSMGInterface()->getMotor()->getSlamOdometry().getPosition().at(1),
                                ScanMatchingAlgo::norm_a(getSMGInterface()->getMotor()->getSlamOdometry().getOrientation().at(0)));
    prevMatchedSlamOdometry = slamOdometry;

    NDPose2D poseSlam2;
    poseSlam2.setCoordinates(actualScan.rx() +
                             + PM_LASER_Y*2*fabs(sin(actualScan.th()/2)*sin(actualScan.th()/2)),
                             actualScan.ry() -
                             PM_LASER_Y*2*fabs(sin(actualScan.th()/2)*cos(actualScan.th()/2)), actualScan.th()*180/M_PI);
    correctedSlamOdometry = poseSlam2;

    if(prevMatchedSlamOdometry.getOrientation().at(0) >= 0)
        correctedSlamOdometry.setCoordinates(slamOdometry.getPosition().at(0) +
             PM_LASER_Y*2*fabs(sin(prevMatchedSlamOdometry.getOrientation().at(0)/2)*sin(prevMatchedSlamOdometry.getOrientation().at(0)/2)),
             prevMatchedSlamOdometry.getPosition().at(1) -
             PM_LASER_Y*2*fabs(sin(prevMatchedSlamOdometry.getOrientation().at(0)/2)*cos(prevMatchedSlamOdometry.getOrientation().at(0)/2)),
             slamOdometry.getOrientation().at(0));
    else
        correctedSlamOdometry.setCoordinates(slamOdometry.getPosition().at(0) +
             PM_LASER_Y*2*fabs(sin(prevMatchedSlamOdometry.getOrientation().at(0)/2)*sin(prevMatchedSlamOdometry.getOrientation().at(0)/2)),
             prevMatchedSlamOdometry.getPosition().at(1) +
             PM_LASER_Y*2*fabs(sin(prevMatchedSlamOdometry.getOrientation().at(0)/2)*cos(prevMatchedSlamOdometry.getOrientation().at(0)/2)),
             slamOdometry.getOrientation().at(0));


    rawOdometry.setCoordinates(getSMGInterface()->getMotor()->getRawOdometry().getPosition().at(0), getSMGInterface()->getMotor()->getRawOdometry().getPosition().at(1),
                               ScanMatchingAlgo::norm_a(getSMGInterface()->getMotor()->getRawOdometry().getOrientation().at(0)));

    ArTime robotTime = getSMGInterface()->getRobot()->getTimePoseTaken().toArTime();
    ArTime scanTime = actualScan.t().toArTime();
    ArTime prevScanTime = actualScan.t().toArTime();

//    if(scanTime.getSec() > robotTime.getSec() ||
//            (scanTime.getSec() == robotTime.getSec() &&
//             scanTime.getMSec() >= robotTime.getMSec()))
//        scanTime = robotTime;
//    else {

//        if(robotTime.getMSec() >= 100) {
//            scanTime.setSec(robotTime.getSec());
//            scanTime.setMSec(robotTime.getMSec() - 100);
//        }
//        else {
//            scanTime.setSec(robotTime.getSec() - 1);
//            scanTime.setMSec(robotTime.getMSec() + 900);
//        }
//    }

//    double scanT = scanTime.getSec() + scanTime.getMSec()/1000.0;
//    double robotT = robotTime.getSec() + robotTime.getMSec()/1000.0;

//    if(scanT > robotT - 0.000001)
//        scanTime = robotTime;
//    else if (scanT < robotT - 100.000001){
//        if(robotTime.getMSec() >= 200) {
//            scanTime.setSec(robotTime.getSec());
//            scanTime.setMSec(robotTime.getMSec() - 200);
//        }
//        else {
//            scanTime.setSec(robotTime.getSec() - 1);
//            scanTime.setMSec(robotTime.getMSec() + 800);
//        }
//    }
//    else if (scanT < robotT - 0.000001){
//        if(robotTime.getMSec() >= 100) {
//            scanTime.setSec(robotTime.getSec());
//            scanTime.setMSec(robotTime.getMSec() - 100);
//        }
//        else {
//            scanTime.setSec(robotTime.getSec() - 1);
//            scanTime.setMSec(robotTime.getMSec() + 900);
//        }
//    }

    if(!getSMGInterface()->getRobot()->getRealRobot())
        scanTime = robotTime; // para robot simulado
    //  if(scanTime.getSec() > robotTime.getSec() ||
    //          (scanTime.getSec() == robotTime.getSec() && scanTime.getMSec() > robotTime.getMSec()))
    //      scanTime = robotTime;

    //  cout << "   Slam " << slamOdometry;
    //  cout << "   Exact " << getSMGInterface()->getMotor()->getExactOdometry();
    //  cout << "   Raw " << rawOdometry << Qt::endl;

//    interpolSlam.addReading(robotTime, ArPose(slamOdometry.getPosition().at(0),
//                                              slamOdometry.getPosition().at(1),
//                                              ScanMatchingAlgo::norm_a(slamOdometry.getOrientation().at(0)) * 180 / M_PI));
//    interpolRaw.addReading(robotTime, ArPose(rawOdometry.getPosition().at(0),
//                                             rawOdometry.getPosition().at(1),
//                                             ScanMatchingAlgo::norm_a(rawOdometry.getOrientation().at(0)) * 180 / M_PI));
//    ArPose poseSlam;
//    ArPose poseRaw;
//    int res = interpolSlam.getPose(scanTime, &poseSlam);
//   res = interpolRaw.getPose(scanTime, &poseRaw);


//    cout << "Interpol " << res << " " << robotTime.getSec() << "." << QString::number(robotTime.getMSec()).rightJustified(3, '0').toStdString();
//    cout << " " << scanTime.getSec() << "." << QString::number(scanTime.getMSec()).rightJustified(3, '0').toStdString();
//    cout << " " << prevScanTime.getSec() << "." << QString::number(prevScanTime.getMSec()).rightJustified(3, '0').toStdString() << Qt::endl;
//    cout << "slamOdometry " << slamOdometry << Qt::endl;
//    cout << "poseSlam " << " " << poseSlam.getX() << " " << poseSlam.getY() << " " << poseSlam.getTh() << Qt::endl;
//    cout << "rawOdometry " << rawOdometry << Qt::endl;
//    cout << "poseRaw " << " " << poseRaw.getX() << " " << poseRaw.getY() << " " << poseRaw.getTh() << Qt::endl;

    //  double xSlam = slamOdometry.getPosition().at(0);
    //  double ySlam = slamOdometry.getPosition().at(1);
    //  double thSlam = slamOdometry.getOrientation().at(0);

    double xx = rawOdometry.getPosition().at(0) - poseRaw.getX();
    double yy = rawOdometry.getPosition().at(1) - poseRaw.getY();
    double thth = rawOdometry.getOrientation().at(0) - poseRaw.getThRad();

//    double xx = poseRaw.getX() - prevPoseRaw.getX();
//    double yy = poseRaw.getY() - prevPoseRaw.getY();
//    double thth = poseRaw.getThRad() - prevPoseRaw.getThRad();

    prevPoseRaw = poseRaw;

    poseSlam.setPose(slamOdometry.getPosition().at(0) - xx,
                     slamOdometry.getPosition().at(1) - yy,
                     (slamOdometry.getOrientation().at(0) - thth)*180/M_PI);

//    cout << "slamOdometry " << slamOdometry << Qt::endl;
//    cout << "poseSlam " << " " << poseSlam.getX() << " " << poseSlam.getY() << " " << poseSlam.getTh() << Qt::endl;
//    cout << "rawOdometry " << rawOdometry << Qt::endl;
//    cout << "poseRaw " << " " << poseRaw.getX() << " " << poseRaw.getY() << " " << poseRaw.getTh() << Qt::endl;


    double xSlam = poseSlam.getX();
    double ySlam = poseSlam.getY();
    double thSlam = poseSlam.getThRad();

    double increXSlam = slamOdometry.getPosition().at(0) - xSlam;
    double increYSlam = slamOdometry.getPosition().at(1) - ySlam;
    double increThSlam = ScanMatchingAlgo::norm_a(slamOdometry.getOrientation().at(0) - thSlam);

    double xPrevSlam = prevSlamOdometry.getPosition().at(0);
    double yPrevSlam = prevSlamOdometry.getPosition().at(1);
    double thPrevSlam = prevSlamOdometry.getOrientation().at(0);
    double trans = sqrt((xSlam - xPrevSlam)*(xSlam - xPrevSlam) + (ySlam - yPrevSlam)*(ySlam - yPrevSlam)); // x <--> y en algoritmo
    double deltaTh = fabs(thSlam - thPrevSlam);
    deltaTh = deltaTh >= M_PI ? 2*M_PI - deltaTh : deltaTh;


//    totalTrans += trans;
//    totalRot += deltaTh;

#ifdef COUT_DEBUG_2
    cout << "   Slam " << slamOdometry;
    cout << "   Exact " << getSMGInterface()->getMotor()->getExactOdometry();
    cout << "   Raw " << rawOdometry << Qt::endl;
    cout << "---insxxx AreaCenterSLAMControl::sendOutputs(void) calling scanmatching?? trans: " << trans << " >= 800 OR rotation: " << deltaTh << " >= " << 10/NDMath::RAD2GRAD << Qt::endl;
#endif
    bool tooMuchMeasures = getTooMuchMeasures();
    if(tooMuchMeasures)
        cout << "AreaCenterSLAMControl::sendOutputs(void) too much measures aborted scanmatching!!!!!!!!!!!!!!!!!" << Qt::endl;
    //  if((trans >= 800 || deltaTh >= 10/NDMath::RAD2GRAD) && !tooMuchMeasures) {
//    if((trans >= 800 || deltaTh >= 10/NDMath::RAD2GRAD) && !tooMuchMeasures && !differentTimeScan) {
    if((trans/400.0 + deltaTh/(10/NDMath::RAD2GRAD) >= 1) && !tooMuchMeasures && !differentTimeScan) {

        printSegmentationResults();

        //     actualScan.setT(sim_time_step);
        //     actualScan.setT(getSMGInterface()->getRobot());
        //     if((getSMGInterface()->getRobot()->getTimePoseTaken() - actualScan.t()).microseconds() > 50000) {
        //       xSlam = xPrevSlam;
        //       ySlam = yPrevSlam;
        //       thSlam = thPrevSlam;
        //       rawOdometry.setCoordinates(xPrevRaw, yPrevRaw, thPrevRaw);
        //     }
        TimeStamp start_tick = TimeStamp::nowMicro();
        actualScan.setRx(xSlam);
        actualScan.setRy(ySlam);
        actualScan.setTh(thSlam);

        for(int i = 4; i < PM_L_POINTS+4; i++) {
            double r = areaCenterNodeList[i]->result().getY().at(0);
            actualScan.setR(i-4, r);
            //       actualScan.setX(i-4, r*pm_co[i-4]);
            //       actualScan.setY(i-4, r*pm_si[i-4]);
            actualScan.setX(i-4, -r*pm_si[i-4]);
            actualScan.setY(i-4, r*pm_co[i-4]);
            actualScan.setBad(i-4, 0);
            actualScan.setSeg(i-4, 0);
        }

        rawActualScan = actualScan;
        calculateSegments(); //This is the good one. Keep it. ****INS****
        //AreaCenterSLAMControl::calculateSegments(); // Only if the old Diosi segmentation is being tested
        try{

#ifdef COUT_DEBUG
            ArTime now;
            now.setToNow();
            cout << "Robot pose time: " << " " << robotTime.getSec() << "." << QString::number(robotTime.getMSec()).rightJustified(3, '0').toStdString();
            cout << " Sensors measure time: " << scanTime.getSec() << "." << QString::number(scanTime.getMSec()).rightJustified(3, '0').toStdString();
            cout << " Now: " << " " << now.getSec() << "." << now.getMSec()<< Qt::endl;

            cout << "---insxxx AreaCenterSLAMControl::sendOutputs(void) calling scanmatching?? trans: " << trans << " >= 800 OR rotation: " << deltaTh << " >= " << 10/NDMath::RAD2GRAD << Qt::endl;
            //      cout << "   Slam " << correctedSlamOdometry << Qt::endl;
            cout << "   Slam " << poseSlam.getX() << " " << poseSlam.getY()
                 << " "<< poseSlam.getTh() << Qt::endl;
            cout << "   Exact " << getSMGInterface()->getMotor()->getExactOdometry() << Qt::endl;
            cout << "   Raw " << poseRaw.getX() << " " << poseRaw.getY()
                 << " "<< poseRaw.getTh() << Qt::endl;//rawOdometry

#endif

#ifdef COUT_DEBUG_2
            cout << "---AreaCenterSLAMControl::sendOutputs(void) pre-match() call. referenceScan = " << referenceScan << Qt::endl;
            cout << "---AreaCenterSLAMControl::sendOutputs(void) pre-match() call. actualScan = " << actualScan << Qt::endl;
#endif
            ++match_counter;
            ((PMSAlgo*)algo)->match(&referenceScan, &actualScan, &rawReferenceScan, &rawActualScan, pms_method);
            if(!algo->getMatchOk()) {
                cout << "AreaCenterSLAMControl::sendOutputs(void) aborted scanmatching!!!!!!!!!!!!!!!!!" << Qt::endl;
                // 	slamOdometry.setCoordinates(xSlam, ySlam, thSlam);
                // 	getSMGInterface()->getMotor()->setSlamOdometry(slamOdometry);
                // 	return;
            }


#ifdef COUT_DEBUG
            cout  <<" et: " << TimeStamp::nowMicro()-start_tick <<Qt::endl;
#endif

            double err_idx = algo->errorIndex(&referenceScan, &actualScan);

#ifdef COUT_DEBUG
            cout <<"Error index: "<<err_idx<< Qt::endl;
#endif
            //      if (match_counter > 10005)
            //       if (match_counter > 105)
            //         exit(0);
        }catch(int err){
            //     delete algo;
        };

#ifdef COUT_DEBUG
        cout <<"Match result: " << actualScan.rx() << " " << actualScan.ry() << " " << actualScan.th() << " " << actualScan.th()*ScanMatchingAlgo::PM_R2D << Qt::endl;
        cout <<"Match x-check result: " << referenceScan.rx() + actualScan.rx()*cos(referenceScan.th()) - actualScan.ry()*sin(referenceScan.th()) << " "
            << referenceScan.ry() + actualScan.rx()*sin(referenceScan.th()) + actualScan.ry()*cos(referenceScan.th()) << " "
            << actualScan.th()+referenceScan.th() << " "
            << Qt::endl;
#endif

//        NDPose2D newRaw;
//        newRaw.setCoordinates(getSMGInterface()->getMotor()->getRawOdometry().getPosition().at(0), getSMGInterface()->getMotor()->getRawOdometry().getPosition().at(1), getSMGInterface()->getMotor()->getRawOdometry().getOrientation().at(0));
        double dth = referenceScan.th();

//        QPointF increOdom(newRaw.getPosition().at(0) - rawOdometry.getPosition().at(0), newRaw.getPosition().at(1) - rawOdometry.getPosition().at(1));
//        double increTh = newRaw.getOrientation().at(0) - rawOdometry.getOrientation().at(0);
//        NDMath::selfRotateRad(increOdom, dth + increTh, QPointF());
//        QPointF increOdom;
//        double increTh = 0.0;

        QPointF newOdom(actualScan.rx(), actualScan.ry());
        NDMath::selfRotateRad(newOdom, dth, QPointF());

        //    newOdom += increOdom;
        actualScan.setRx(referenceScan.rx() + newOdom.x());
        actualScan.setRy(referenceScan.ry() + newOdom.y());
        actualScan.setTh(ScanMatchingAlgo::norm_a(actualScan.th() + dth/* + increTh*/));
        //    rawOdometry = newRaw;

#ifdef COUT_DEBUG
        cout <<"Prev result: " << referenceScan.rx() << " " << referenceScan.ry() << " " << referenceScan.th() << " " << referenceScan.th()*ScanMatchingAlgo::PM_R2D << Qt::endl;
        cout <<"Match result: " << actualScan.rx() << " " << actualScan.ry() << " " << actualScan.th() << " " << actualScan.th()*ScanMatchingAlgo::PM_R2D << Qt::endl;
#endif

        // están en coordenadas globales no hay qu rotar
        NDPose2D newRaw;
        newRaw.setCoordinates(getSMGInterface()->getMotor()->getRawOdometry().getPosition().at(0), getSMGInterface()->getMotor()->getRawOdometry().getPosition().at(1), getSMGInterface()->getMotor()->getRawOdometry().getOrientation().at(0));

        QPointF increOdom(newRaw.getPosition().at(0) - rawOdometry.getPosition().at(0), newRaw.getPosition().at(1) - rawOdometry.getPosition().at(1));
        double increTh = ScanMatchingAlgo::norm_a(newRaw.getOrientation().at(0) - rawOdometry.getOrientation().at(0));
//        NDMath::selfRotateRad(increOdom, dth, QPointF());

//        newOdom.setX(increXSlam + increOdom.x());
//        newOdom.setY(increYSlam + increOdom.y());
//        dth = actualScan.th() + increTh;
////        dth = actualScan.th();
//        NDMath::selfRotateRad(newOdom, dth, QPointF());
        newOdom.setX(increXSlam + increOdom.x());
        newOdom.setY(increYSlam + increOdom.y());
        dth = actualScan.th() + increThSlam + increTh;
        cout <<"newOdom: " << newOdom.x() << " " << newOdom.y() << " " << ScanMatchingAlgo::norm_a(dth) << Qt::endl;

//        slamOdometry.setCoordinates(actualScan.rx() + newOdom.x(),
//                                    actualScan.ry() + newOdom.y(),
//                                    ScanMatchingAlgo::norm_a(actualScan.th() + increThSlam));
        slamOdometry.setCoordinates(actualScan.rx() + newOdom.x(),
                                    actualScan.ry() + newOdom.y(),
                                    ScanMatchingAlgo::norm_a(dth));
        //    slamOdometry.setCoordinates(actualScan.rx(), actualScan.ry(),actualScan.th());
        prevMatchedSlamOdometry = slamOdometry;
        if(prevMatchedSlamOdometry.getOrientation().at(0) >= 0)
            correctedSlamOdometry.setCoordinates(slamOdometry.getPosition().at(0) +
                                                 PM_LASER_Y*2*fabs(sin(slamOdometry.getOrientation().at(0)/2)*sin(slamOdometry.getOrientation().at(0)/2)),
                                                 prevMatchedSlamOdometry.getPosition().at(1) -
                                                 PM_LASER_Y*2*fabs(sin(slamOdometry.getOrientation().at(0)/2)*cos(slamOdometry.getOrientation().at(0)/2)),
                                                 slamOdometry.getOrientation().at(0));
        else
            correctedSlamOdometry.setCoordinates(slamOdometry.getPosition().at(0) +
                                                 PM_LASER_Y*2*fabs(sin(slamOdometry.getOrientation().at(0)/2)*sin(slamOdometry.getOrientation().at(0)/2)),
                                                 slamOdometry.getPosition().at(1) +
                                                 PM_LASER_Y*2*fabs(sin(slamOdometry.getOrientation().at(0)/2)*cos(slamOdometry.getOrientation().at(0)/2)),
                                                 slamOdometry.getOrientation().at(0));

        getSMGInterface()->getMotor()->setSlamOdometry(correctedSlamOdometry);

        if(algo->getMatchOk() && !tooMuchMeasures) {
            rawReferenceScan = rawActualScan;
            referenceScan = actualScan;
            totalTrans += trans;
            totalRot += deltaTh;
            prevSlamOdometry = slamOdometry;
            //      if(slamPainter)
            //        drawScan(slamOdometry, slamPainter);
            //      if(rawPainter)
            //        drawScan(rawOdometry, rawPainter);
            poseSlam.setPose(actualScan.rx(), actualScan.ry(), actualScan.th());
            if(slamPainter) {
                NDPose2D pSlam;
                pSlam.setCoordinates(actualScan.rx(), actualScan.ry(), actualScan.th());
                drawScan(pSlam, slamPainter);
            }
            if(rawPainter) {
                NDPose2D pRaw;
                pRaw.setCoordinates(poseRaw.getX(), poseRaw.getY(), poseRaw.getThRad());
                drawScan(pRaw, rawPainter);
            }
            int loops = (int)(totalRot / (2*M_PI));
            double degrees = (totalRot - loops*2*M_PI) * NDMath::RAD2GRAD;

            //        NDPose2D poseSlam;
            //        poseSlam.setCoordinates(actualScan.rx() +
            //                                + PM_LASER_Y*2*fabs(sin(actualScan.th()/2)*sin(actualScan.th()/2)),
            //                                actualScan.ry() -
            //                                PM_LASER_Y*2*fabs(sin(actualScan.th()/2)*cos(actualScan.th()/2)),
            //                                actualScan.th());

            //    t13 = cos(rth - *ath) * LASER_Y + cos(rth) * (*ax - rx) + sin(rth) * (*ay - ry)- LASER_Y;
            //    t23 = -sin(rth - *ath) * LASER_Y - sin(rth) * (*ax - rx) + cos(rth) * (*ay - ry);

            //    correctedSlamOdometry = poseSlam;

            //        start_tick = TimeStamp::nowMicro()-start_tick;

            //        cout  << "Time " << TimeStamp::nowMicro();
            //        cout  << "Time " << start_tick << Qt::endl;
            start_tick = TimeStamp::nowMicro()-start_tick;
            myLog << segmentationTime << " ";
            myLog << start_tick.second << " ";
            getSMGInterface()->getMotor()->getExactOdometry().toFile(myLog);
            NDPose2D correctedRaw;
            correctedRaw.setCoordinates(poseRaw.getX(), poseRaw.getY(), poseRaw.getTh()*M_PI/180);
            correctedRaw.toFile(myLog);
            //    rawOdometry.toFile(myLog);
            NDPose2D pSlam;
            pSlam.setCoordinates(actualScan.rx(), actualScan.ry(), actualScan.th());
            pSlam.toFile(myLog);
            myLog << Qt::endl;
            fileoutput.flush();

#ifdef COUT_DEBUG
            cout << "---insxxx AreaCenterSLAMControl::sendOutputs(void) called scanmatching!! trans: " << trans << " >= 800 OR rotation: " << deltaTh << " >= " << 10/NDMath::RAD2GRAD << Qt::endl;

            cout << "   total trans: " << totalTrans/1000 << " total rotation: " << loops << " " << degrees  << Qt::endl;
            //        cout << "   Slam " << correctedSlamOdometry << Qt::endl;
            cout << "   Slam " << actualScan.rx() << " "  << actualScan.ry()<< " "  << actualScan.th()*180/M_PI << Qt::endl;
            //cout << "   Slam " << poseSlam << Qt::endl;
            cout << "   Exact " << getSMGInterface()->getMotor()->getExactOdometry() << Qt::endl;
            cout << "   Raw " << poseRaw.getX() << " " << poseRaw.getY()
                 << " "<< poseRaw.getTh() << Qt::endl; // rawOdometry
#endif
        }

    }

    //   // Time + pose estimation
    //   fprintf(fileoutput,"%d ",sim_time_step);
    //   // Exact, Raw & Slam odometry (get returns a NDPose)
    //   fprintf(fileoutput,"%f %f %f %f %f %f %f %f %f %f %f %f ",
    // //   getSMGInterface()->getRobot()->getSprite()->x(),
    // //   getSMGInterface()->getRobot()->getSprite()->y(),
    // //   getSMGInterface()->getRobot()->getSprite()->getRotation(),
    //   getSMGInterface()->getRobot()->getX(),
    //   getSMGInterface()->getRobot()->getY(),
    //   getSMGInterface()->getRobot()->getRotation(),
    //   getSMGInterface()->getMotor()->getExactOdometry().getPosition().at(0),
    //   getSMGInterface()->getMotor()->getExactOdometry().getPosition().at(1),
    //   getSMGInterface()->getMotor()->getExactOdometry().getOrientation().at(0),
    //   getSMGInterface()->getMotor()->getRawOdometry().getPosition().at(0),
    //   getSMGInterface()->getMotor()->getRawOdometry().getPosition().at(1),
    //   getSMGInterface()->getMotor()->getRawOdometry().getOrientation().at(0),
    //   getSMGInterface()->getMotor()->getSlamOdometry().getPosition().at(0),
    //   getSMGInterface()->getMotor()->getSlamOdometry().getPosition().at(1),
    //   getSMGInterface()->getMotor()->getSlamOdometry().getOrientation().at(0));
    //   fprintf(fileoutput,"\n");

    ++sim_time_step;

    //  getSMGInterface()->getMotor()->getExactOdometry().toFile(myLog);
    //  rawOdometry.toFile(myLog);
    //  correctedSlamOdometry.toFile(myLog);
    //  myLog << Qt::endl;

    if(!getSMGInterface()->getRobot()->getRealRobot()) {
    // Comentar las 3 líneas siguientes para robot Aria
      getSMGInterface()->getMotor()->velsToPose(deltaOdometry, AbstractMotor::SLAM);
    //   cout << "deltaOdometry " << deltaOdometry << Qt::endl;
      getSMGInterface()->getMotor()->accumOdometry(deltaOdometry, AbstractMotor::SLAM);
    }

    //  getSMGInterface()->getMotor()->setSlamOdometry(correctedSlamOdometry);

    //  interpolSlam.addReading(robotTime, ArPose(correctedSlamOdometry.getPosition().at(0),
    //               correctedSlamOdometry.getPosition().at(1),
    //               ScanMatchingAlgo::norm_a(correctedSlamOdometry.getOrientation().at(0)) * 180 / M_PI));
    //  interpolRaw.addReading(robotTime, ArPose(rawOdometry.getPosition().at(0),
    //               rawOdometry.getPosition().at(1),
    //               ScanMatchingAlgo::norm_a(rawOdometry.getOrientation().at(0)) * 180 / M_PI));

    //  cout << "slamOdometry " << getSMGInterface()->getMotor()->getSlamOdometry() << Qt::endl;

#ifdef COUT_DEBUG_2    
    cout << "---insxxx END OF AreaCenterSLAMControl::sendOutputs(void) \n";
#endif
}

void AreaCenterSLAMControl::initElements(void)
{
    AreaCenterReactiveControl::initElements();

    rawOdometry.setCoordinates(0, 0, 0);
    slamOdometry.setCoordinates(0, 0, 0);

    referenceScan.setT(sim_time_step);

    referenceScan.setRx(-rawOdometry.getPosition().at(1));//*100 con. into cm
    referenceScan.setRy(rawOdometry.getPosition().at(0));
    referenceScan.setTh(ScanMatchingAlgo::norm_a(rawOdometry.getOrientation().at(0)));

    for(int i = 4; i < PM_L_POINTS + 4; i++) {
        double r = areaCenterNodeList[i]->result().getY().at(0);
        referenceScan.setR(i-4, r);
        referenceScan.setX(i-4, -r*pm_si[i-4]);
        referenceScan.setY(i-4, r*pm_co[i-4]);
        referenceScan.setBad(i-4, 0);
        referenceScan.setSeg(i-4, 0);
    }

    //   rawOdometry.setCoordinates(0, 0, 0);
    //   slamOdometry.setCoordinates(0, 0, 0);
    rawReferenceScan = referenceScan;
    rawActualScan = referenceScan;
    actualScan = referenceScan;
    calculateSegments();//This is the good one. Keep it. ****INS****
    //AreaCenterSLAMControl::calculateSegments(); // Only if the old Diosi segmentation is being tested
    referenceScan = actualScan;
    rawReferenceScan = referenceScan;
    rawActualScan = referenceScan;
    cnt = 0;
    start = true;
    double rot = getSMGInterface()->getRobot()->getRotation() +
            getSMGInterface()->getRobot()->getRotInit();
    if(slamPainter)
        slamPainter->setRotation(rot);
    if(rawPainter)
        rawPainter->setRotation(rot);
    totalTrans = 0;
    totalRot = 0;

    myLog << "0 0" << " ";
    getSMGInterface()->getMotor()->getExactOdometry().toFile(myLog);
    rawOdometry.toFile(myLog);
    slamOdometry.toFile(myLog);
    myLog << Qt::endl;
}

void AreaCenterSLAMControl::sampleInputs(void)
{
    AreaCenterReactiveControl::sampleInputs();
    Measure<double> res = areaCenterNodeList[4]->result();
    TimeStamp time1 = areaCenterNodeList[4]->result().getTimeStamp();
    TimeStamp time2 = areaCenterNodeList[PM_L_POINTS + 3]->result().getTimeStamp();
    long long secs = (long)time1.seconds() + (long)time2.seconds();
    int mSecs = time1.microseconds() + time2.microseconds();
    if(secs % 2)
        mSecs += 1000000;
//    cout << "Time input " << time1.microseconds() << " " << time2.microseconds() << Qt::endl;
    TimeStamp time3(secs/2, mSecs/2, false);

    if(getSMGInterface()->getRobot()->getRealRobot()) {
        poseRaw.setPose(areaCenterNodeList[4]->result().getY().at(6), // encoder pose
                areaCenterNodeList[4]->result().getY().at(7),
                areaCenterNodeList[4]->result().getY().at(8)*180/M_PI);
        ArPose nnn(areaCenterNodeList[PM_L_POINTS + 3]->result().getY().at(6), // encoder pose
                areaCenterNodeList[PM_L_POINTS + 3]->result().getY().at(7),
                areaCenterNodeList[PM_L_POINTS + 3]->result().getY().at(8)*180/M_PI);

        int milS1 = time1.microseconds()/1000;
        int milS2 = time2.microseconds()/1000;
        if(milS1 < milS2 - 10 || 1000 + milS1 - milS2 < 10 || nnn != poseRaw) {
            differentTimeScan = true;
            cout << "differentTimeScan " << time1 << " "  << time2 << Qt::endl;
            cout << "Start " << poseRaw.getX() << " " << poseRaw.getY() << " " << poseRaw.getTh() << Qt::endl;
            cout << "End " << nnn.getX() << " " << nnn.getY() << " " << nnn.getTh() << Qt::endl;
        }
        else
            differentTimeScan = false;
    }
    else {
        poseRaw.setPose(getSMGInterface()->getMotor()->getRawOdometry().getPosition().at(0),
                       getSMGInterface()->getMotor()->getRawOdometry().getPosition().at(1),
                       getSMGInterface()->getMotor()->getRawOdometry().getOrientation().at(0)*180/M_PI);
        differentTimeScan = false;
    }
    if(start) {
        //referenceScan.setT(TimeStamp::nowMicro());
        //     referenceScan.setT(sim_time_step);
        referenceScan.setT(time3);
        referenceScan.setRx(-rawOdometry.getPosition().at(1));//*100 con. into cm
        referenceScan.setRy(rawOdometry.getPosition().at(0));
        referenceScan.setTh(ScanMatchingAlgo::norm_a(rawOdometry.getOrientation().at(0)));

        for(int i = 4; i < PM_L_POINTS + 4; i++) {
            double r = areaCenterNodeList[i]->result().getY().at(0);
            referenceScan.setR(i-4, r);
            referenceScan.setX(i-4, -r*pm_si[i-4]);
            referenceScan.setY(i-4, r*pm_co[i-4]);
            referenceScan.setBad(i-4, 0);
            referenceScan.setSeg(i-4, 0);
        }
        actualScan = referenceScan;

        calculateSegments();
        referenceScan = actualScan;

        algo->setSensorMaxVal(sensorMaxVal*2); // ñapa * 2

        if(slamPainter)
            drawScan(slamOdometry, slamPainter);
        if(rawPainter)
            drawScan(rawOdometry, rawPainter);
        printSegmentationResults();
        start = false;
    }
    else
        actualScan.setT(time3);
}

void AreaCenterSLAMControl::calculateSegments()
{

#ifdef COUT_DEBUG_2
    cout << "---AreaCenterSLAMControl::calculateSegments(PMScan* scan) \n";
#endif
    //    algo->pm_preprocessScan(&actualScan);

    algo->medianFilter(&actualScan);
    algo->findFarPoints(&actualScan);
    algo->segmentScan(&actualScan);

    QVector< int > segs = actualScan.seg();
    int s = segs.size();
    int cnt_s = -1;
    segmentsEnds.clear();

    if(segs.at(0)) {
        cnt_s++;
        segmentsEnds.append(std::pair<int, int>(0,0));
    }

    for(int i = 1; i < s; i++){ // tomando todos los puntos en segmentos
        if(segs.at(i))
            segmentsEnds.append(std::pair<int, int>(i,i));
    }

    int idx;
    int numSegments = segmentsEnds.size();
    endPoints.resize(numSegments);
    QVector<double> angle(181);
    for(int i = 0; i < 181; i++)
        angle[i] = (-90 + i)/NDMath::RAD2GRAD;

    for(int i = 0; i < numSegments; i++) {
        idx = segmentsEnds.at(i).first;
        endPoints[i].first.setX(actualScan.r(idx) * cos(angle[idx]));
        endPoints[i].first.setY(actualScan.r(idx) * sin(angle[idx]));

        idx = segmentsEnds.at(i).second;
        endPoints[i].second.setX(actualScan.r(idx) * cos(angle[idx]));
        endPoints[i].second.setY(actualScan.r(idx) * sin(angle[idx]));
    }
#ifdef COUT_DEBUG_2
    cout << "---END OF AreaCenterSLAMControl::calculateSegments(PMScan* scan) \n";
#endif

}

void AreaCenterSLAMControl::drawScan(NDPose pose, SlamPainter *painter)
{

    QPointF traslation(pose.getPosition().at(0), pose.getPosition().at(1));
    double rotation = pose.getOrientation().at(0);
    //  cout << "DrawScan " << traslation.x() << " "<< traslation.y() << " " << rotation*180/M_PI << Qt::endl;

    int numSegments = endPoints.size();
    QVector<int> sizes;
    QVector< pair<QPointF,QPointF> > l_endPoints(endPoints);
    int segCounter = -1;
    for(int i = 0; i < numSegments; i++) {
        segCounter++;

        double d1 = l_endPoints[i].first.x()*l_endPoints[i].first.x() + l_endPoints[i].first.y()*l_endPoints[i].first.y();
        double d2 = l_endPoints[i].second.x()*l_endPoints[i].second.x() + l_endPoints[i].second.y()*l_endPoints[i].second.y();

        //     if(d1 >= sensorMaxVal * sensorMaxVal || d2 >= sensorMaxVal * sensorMaxVal) {
        if(d1 >= sensorMaxVal * sensorMaxVal * 4 - 0.001 || d2 >= sensorMaxVal * sensorMaxVal * 4  - 0.001) { // ñapa * 2 * 2
            l_endPoints.remove(i);
            i--;
            numSegments--;
            continue;
        }
        NDMath::selfRotateRad(l_endPoints[i].first, rotation, QPointF());
        l_endPoints[i].first += traslation;
        NDMath::selfRotateRad(l_endPoints[i].second, rotation, QPointF());
        l_endPoints[i].second += traslation;
        sizes.append(segmentsEnds.at(segCounter).second - segmentsEnds.at(segCounter).first + 1); // revisar full360
    }
    painter->paintSlam(l_endPoints, sizes, 0);

}

// void AreaCenterSLAMControl::drawScan(void)
// {
// 
//   QPointF traslation(slamOdometry.getPosition().at(0),  slamOdometry.getPosition().at(1));
//   double rotation = slamOdometry.getOrientation().at(0);
//   int numSegments = endPoints.size();
//   QVector<int> sizes;
//   QVector< pair<QPointF,QPointF> > endPoints(endPoints);
//   for(int i = 0; i < numSegments; i++) {    
//     double d1 = endPoints[i].first.x()*endPoints[i].first.x() + endPoints[i].first.y()*endPoints[i].first.y();
//     double d2 = endPoints[i].second.x()*endPoints[i].second.x() + endPoints[i].second.y()*endPoints[i].second.y();
//     if(d1 >= sensorMaxVal * sensorMaxVal || d2 >= sensorMaxVal * sensorMaxVal) {
//       continue;
//     }
//     NDMath::selfRotateRad(endPoints[i].first, rotation, QPointF());
//     endPoints[i].first += traslation; 
//     NDMath::selfRotateRad(endPoints[i].second, rotation, QPointF());
//     endPoints[i].second += traslation;
//     sizes.append(segmentsEnds.at(i).second - segmentsEnds.at(i).first + 1); // revisar full360
//   }
//   slamPainter->paintSlam(endPoints, sizes, 0);
// }
