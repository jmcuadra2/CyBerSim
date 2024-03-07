//
// C++ Implementation: differentialmotor
//
// Description:
//
//
// Author: Jose Manuel Cuadra Troncoso <jmcuadra@dia.uned.es>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//

#include "differentialmotor.h"
#include "robot.h"
#include "../slam/scanmatchingalgo.h"

#include <iostream>

// #define COUT_DEBUG

using namespace std;

DifferentialMotor::DifferentialMotor(QString motorName, Robot* robot) : AbstractMotor(motorName)
{
    this->robot = robot;
    wheelsSeparation = this->robot->getRealWheelsSep();
    inerl = 0.0;
    inerr = 0.0;
    noise1 = 0.1;
    noise2 = 0.1;
    m_noise1 = 1;
    m_noise2 = 1;
    type_iner = 2;
    advance_period = this->robot->getAdvancePeriod();
    max_vel = robot->getMaxVel();

    random_cnt = 0;
    generateRand = true;
    turning = false;

    rawOdometry = new NDPose2D();
    exactOdometry = new NDPose2D();
    slamOdometry = new NDPose2D();
    velsPose = new NDPose2D();
}


DifferentialMotor::~DifferentialMotor()
{
}

void DifferentialMotor::setModel(double noise1, double noise2, int type_iner, RandomDist randomDist)
{

    this->noise1 = noise1;
    this->noise2 = noise2;
    this->type_iner = type_iner;
    this->randomDist = randomDist;
}

void DifferentialMotor::addInertia(void)
{
    double vabs;
    if ((vabs = fabs(vel1 - prev_vel1) * 100) > 0.01)
        inerl = vabs * vabs * type_iner * type_iner / 2000.0;
    else
        inerl = 0;
    if ((vabs = fabs(vel2 - prev_vel2) * 100) > 0.01)
        inerr = vabs * vabs * type_iner * type_iner / 2000.0 ;
    else
        inerr = 0;
    vel2 = (vel2 + inerr * prev_vel2) / (1 + inerr);
    vel1 = (vel1 + inerl * prev_vel1) / (1 + inerl);

}

// void DifferentialMotor::applyModel(void)
// {
//   double m_noise1 = 0, m_noise2 = 0;
//
//   double dth = (vel1 - vel2) * max_vel / wheelsSeparation * advance_period / 1000;
//   QPointF d  = NDMath::polarToCartesian((vel1 + vel2) * max_vel / 2.0 * advance_period / 1000 * 10, dth);
//
//   NDMath::selfRotateRad(d, -rawOdometry->getOrientation().at(0), QPointF());  // para poder sumar abajo
//
//   rawOdometry->setCoordinates(rawOdometry->getPosition().at(0) + d.x(), rawOdometry->getPosition().at(1) + d.y(), rawOdometry->getOrientation().at(0) + dth);
//
//   addInertia();
//
//   if (randomDist == UNIFORM) {
//     m_noise1 = 1 + NDMath::randDouble4(-1.0, 1.0) * noise1 / 100.0;
//     m_noise2 = 1 + NDMath::randDouble4(-1.0, 1.0) * noise2 / 100.0;
//   }
//   else if (randomDist == GAUSS) {
//     m_noise1 = 1 + NDMath::randGaussian() * noise1 / 100 / 3; // 3, 99% confidence
//     m_noise2 = 1 + NDMath::randGaussian() * noise2 / 100.0 / 3;
//   }
//
//   m_noise1 = m_noise1 > 2 ? 2 : m_noise1; // noise no pasa de 100%
//   m_noise1 = m_noise1 < 0 ? 0 : m_noise1;
//   m_noise2 = m_noise2 > 2 ? 2 : m_noise2;
//   m_noise2 = m_noise2 < 0 ? 0 : m_noise2;
//
//   vel1 *= m_noise1;
//   vel2 *= m_noise2;
//   prev_vel1 = vel1;
//   prev_vel2 = vel2;
//
//   dth = (vel1 - vel2) * max_vel / wheelsSeparation * advance_period / 1000;
//   d  = NDMath::polarToCartesian((vel1 + vel2) * max_vel / 2.0 * advance_period / 1000 * 10, dth);
//
//   NDMath::selfRotateRad(d, -exactOdometry->getOrientation().at(0), QPointF());  // para poder sumar abajo
//
//   exactOdometry->setCoordinates(exactOdometry->getPosition().at(0) + d.x(), exactOdometry->getPosition().at(1) + d.y(), exactOdometry->getOrientation().at(0) + dth);
//
// }

void DifferentialMotor::applyModel(void)
{
    double k1 = max_vel / 2.0 * advance_period / 1000 * 10;
    double k2 = max_vel / wheelsSeparation * advance_period / 1000;

    double dth = (vel2 - vel1) * k2;
    double prev_dth = (prev_vel2 - prev_vel1) * k2;
    double v_lin = (vel1 + vel2) * k1;
    double prev_lin = (prev_vel1 + prev_vel2) * k1;

#ifdef COUT_DEBUG
    cout << "---insxxx DifferentialMotor::applyModel(void) dth: " << dth << " prev_dth: " << prev_dth << " -dth: " << -dth << endl;
    cout << "---insxxx DifferentialMotor::applyModel(void) v_lin: " << v_lin << " prev_lin: " << prev_lin << endl;
#endif

    QPointF d  = NDMath::polarToCartesian(v_lin, dth);
    // **ins**  QPointF d  = NDMath::polarToCartesian(v_lin, -dth);
#ifdef COUT_DEBUG
    cout << "---insxxx DifferentialMotor::applyModel(void) first dx: " << d.x() << " dy: " << d.y() << endl;
#endif

    NDMath::selfRotateRad(d, rawOdometry->getOrientation().at(0), QPointF());  // para poder sumar abajo
    //   cout << "Slam 2" << *rawOdometry << endl;
#ifdef COUT_DEBUG
    cout << "---insxxx DifferentialMotor::applyModel(void) second dx: " << d.x() << " dy: " << d.y() << " orientation: " << rawOdometry->getOrientation().at(0) << endl;
#endif

    rawOdometry->setCoordinates(rawOdometry->getPosition().at(0) + d.x(), rawOdometry->getPosition().at(1) + d.y(), ScanMatchingAlgo::norm_a(rawOdometry->getOrientation().at(0) + dth));
    // **ins**  rawOdometry->setCoordinates(rawOdometry->getPosition().at(0) + d.x(), rawOdometry->getPosition().at(1) + d.y(), ScanMatchingAlgo::norm_a(rawOdometry->getOrientation().at(0) - dth));

#ifdef COUT_DEBUG
    cout << "---insxxx DifferentialMotor::applyModel(void) raw_x: " << rawOdometry->getPosition().at(0) + d.x() << " raw_y: " << rawOdometry->getPosition().at(1) + d.y() << " raw_theta: " << ScanMatchingAlgo::norm_a(rawOdometry->getOrientation().at(0) - dth) << endl;
#endif

    //   cout << "Slam 3" << *rawOdometry << endl;

    addInertia();

    generateRand = true;

    //   if(random_cnt > 20) {
    //     generateRand = true;
    //     random_cnt = 0;
    //     turning = false;
    //   }
    //   else if ((fabs(dth) > 0.03) && (((dth > 0) - (dth < 0)) == ((prev_dth > 0) - (prev_dth < 0)))) {
    //     if(!turning) {
    //       turning = true;
    //       random_cnt = 0;
    //     }
    //     else
    //       ++random_cnt;
    //   }
    // //   else if(random_cnt > 20) {
    // //     generateRand = true;
    // //     random_cnt = 0;
    // //   }
    //   else {
    //     random_cnt++;
    //     generateRand = false;
    //     turning = false;
    //   }

    if (randomDist == UNIFORM) {
        if (generateRand) {
            m_noise1 = 1 + NDMath::randDouble4(-1.0, 1.0) * noise1 / 100.0;
            m_noise2 = 1 + NDMath::randDouble4(-1.0, 1.0) * noise2 / 100.0;
            m_noise1 = m_noise1 > 2 ? 2 : m_noise1; // noise no pasa de 100%
            m_noise1 = m_noise1 < 0 ? 0 : m_noise1;
            m_noise2 = m_noise2 > 2 ? 2 : m_noise2;
            m_noise2 = m_noise2 < 0 ? 0 : m_noise2;
        }

#ifdef COUT_DEBUG    
        cout << "---insxxx DifferentialMotor::applyModel(void) UNIFORM: noise1: " << noise1 << " noise2: " << noise2 << endl;
        cout << "---insxxx DifferentialMotor::applyModel(void) UNIFORM: m_noise1: " << m_noise1 << " m_noise2: " << m_noise2 << endl;
#endif

        vel1 *= m_noise1;
        vel2 *= m_noise2;

#ifdef COUT_DEBUG
        cout << "---insxxx DifferentialMotor::applyModel(void) UNIFORM: vel1: " << vel1 << " vel2: " << vel2 << endl;
#endif

    }
    else if (randomDist == GAUSS) {
        if (generateRand) {
            m_noise1 = 1 + NDMath::randGaussian() * noise1 / 100.0 / 3.0; // 3, 99% confidence
            m_noise2 = 1 + NDMath::randGaussian() * noise2 / 10.0 / 3.0;
            m_noise1 = m_noise1 > 2 ? 2 : m_noise1; // noise no pasa de 100%
            m_noise1 = m_noise1 < 0 ? 0 : m_noise1;
            m_noise2 = m_noise2 > 2 ? 2 : m_noise2;
            m_noise2 = m_noise2 < 0 ? 0 : m_noise2;
        }

#ifdef COUT_DEBUG
        cout << "---insxxx DifferentialMotor::applyModel(void) GAUSS: random check 1: " << NDMath::randGaussian() << " random check 2: " << NDMath::randGaussian() << endl;
        cout << "---insxxx DifferentialMotor::applyModel(void) GAUSS: noise1: " << noise1 << " noise2: " << noise2 << endl;
        cout << "---insxxx DifferentialMotor::applyModel(void) GAUSS: m_noise1: " << m_noise1 << " m_noise2: " << m_noise2 << endl;
        cout << "---insxxx DifferentialMotor::applyModel(void) GAUSS BEFORE: v_lin: " << v_lin << " dth: " << dth << " vel1: " << vel1 << " vel2: " << vel2 << endl;
#endif

        PM_TYPE v_lin_0 = v_lin, dth_0 = dth; // trace to be deleted
        PM_TYPE vel1_0 = vel1, vel2_0 = vel2; // trace to be deleted

        v_lin *= m_noise1;
        dth *= m_noise2;

        vel2 = (k2 * v_lin + k1 * dth) / (2 * k1 * k2);
        vel1 = (v_lin - k1 * vel2) / k1;

        PM_TYPE delta_v = v_lin-v_lin_0, delta_th = dth-dth_0; // trace to be deleted
        PM_TYPE delta_v1 = vel1-vel1_0, delta_v2 = vel2-vel2_0; // trace to be deleted

#ifdef COUT_DEBUG
        cout << "---insxxx DifferentialMotor::applyModel(void) GAUSS  AFTER: v_lin: " << v_lin << " dth: " << dth << " vel1: " << vel1 << " vel2: " << vel2 << endl;
#endif

        PM_TYPE delta_v_lin_pct = 0.0,delta_dth_pct = 0.0; // trace to be deleted
        PM_TYPE delta_v1_pct = 0.0,delta_v2_pct = 0.0; // trace to be deleted

        if (v_lin != 0.0)
            delta_v_lin_pct =  100.0*(delta_v / v_lin);

        if (dth != 0.0)
            delta_dth_pct =  100.0*(delta_th / dth);

        if (vel1 != 0.0)
            delta_v1_pct =  100.0*(delta_v1 / vel1);

        if (vel2 != 0.0)
            delta_v2_pct =  100.0*(delta_v2 / vel2);

#ifdef COUT_DEBUG   
        cout << "---insxxx DifferentialMotor::applyModel(void) delta_v_lin_pct: " << delta_v_lin_pct << " delta_th_pct: " << delta_dth_pct << endl;
        cout << "---insxxx DifferentialMotor::applyModel(void) delta_v1_pct: " << delta_v1_pct << " delta_v2_pct: " << delta_v2_pct << endl;
#endif     
    }

    //   m_noise1 = m_noise1 > 2 ? 2 : m_noise1; // noise no pasa de 100%
    //   m_noise1 = m_noise1 < 0 ? 0 : m_noise1;
    //   m_noise2 = m_noise2 > 2 ? 2 : m_noise2;
    //   m_noise2 = m_noise2 < 0 ? 0 : m_noise2;
    //
    //   vel1 *= m_noise1;
    //   vel2 *= m_noise2;
    prev_vel1 = vel1;
    prev_vel2 = vel2;

    NDPose2D deltaOdometry;
    velsToPose(deltaOdometry, AbstractMotor::EXACT);
    //   accumOdometry(deltaOdometry, AbstractMotor::EXACT);
    //   cout << "deltaOdometry  " <<  deltaOdometry << endl;

    //   dth = (vel1 - vel2) * max_vel / wheelsSeparation * advance_period / 1000;
    //   d  = NDMath::polarToCartesian((vel1 + vel2) * max_vel / 2.0 * advance_period / 1000 * 10, dth);
    //
    //   cout << "exactOdometry->getOrientation() " << exactOdometry->getOrientation().at(0) << endl;
    //
    //   NDMath::selfRotateRad(d, -exactOdometry->getOrientation().at(0), QPointF());  // para poder sumar abajo

#ifdef COUT_DEBUG  
    cout << "---insxxx DifferentialMotor::applyModel(void) deltaOdometry: " << deltaOdometry << endl;
#endif

    accumOdometry(deltaOdometry, AbstractMotor::EXACT);

    //   exactOdometry->setCoordinates(exactOdometry->getPosition().at(0) + deltaOdometry.getPosition().at(0), exactOdometry->getPosition().at(1) + deltaOdometry.getPosition().at(1), exactOdometry->getOrientation().at(0) + deltaOdometry.getOrientation().at(0));

}

void DifferentialMotor::accumOdometry(NDPose odometry, OdometryType type)
{
    QPointF newOdom(odometry.getPosition().at(0), odometry.getPosition().at(1));
    double rot = odometry.getOrientation().at(0);

    NDPose2D* odometryP;
    if (type == RAW)
        odometryP = dynamic_cast<NDPose2D*>(rawOdometry);
    else if (type == EXACT)
        odometryP = dynamic_cast<NDPose2D*>(exactOdometry);
    else if (type == SLAM)
        odometryP = dynamic_cast<NDPose2D*>(slamOdometry);

    //   cout << "prev accumOdometry(): " << *odometryP << endl;

    NDMath::selfRotateRad(newOdom, odometryP->getOrientation().at(0), QPointF());  // para poder sumar abajo

    odometryP->setCoordinates(odometryP->getPosition().at(0) + newOdom.x(), odometryP->getPosition().at(1) + newOdom.y(), ScanMatchingAlgo::norm_a(odometryP->getOrientation().at(0) + rot));

    //   cout << "accumOdometry(): " << *odometryP << endl;
}

void DifferentialMotor::calcAccumOdometry(NDPose& odometry, OdometryType type)
{
    QPointF newOdom(odometry.getPosition().at(0), odometry.getPosition().at(1));
    double rot = odometry.getOrientation().at(0);

    NDPose* odometryP;
    if (type == RAW)
        odometryP = rawOdometry;
    else if (type == EXACT)
        NDPose* odometryP = exactOdometry;
    else if (type == SLAM)
        odometryP = slamOdometry;

    NDMath::selfRotateRad(newOdom, -odometryP->getOrientation().at(0), QPointF());  // para poder sumar abajo

    //   cout << " Slam odometryP->getOrientation() " << odometryP->getOrientation().at(0) << endl;

    vector<double> v_th(1);
    v_th[0] = ScanMatchingAlgo::norm_a(odometryP->getOrientation().at(0) + rot);
    vector<double> v_d(2);
    v_d[0] = odometryP->getPosition().at(0) + newOdom.x();
    v_d[1] = odometryP->getPosition().at(1) + newOdom.y();
    odometry.setCoordinates(v_d, v_th);

    //   odometry.setCoordinates(odometryP->getPosition().at(0) + newOdom.x(), odometryP->getPosition().at(1) + newOdom.y(), odometryP->getOrientation().at(0) + rot);
}

// void DifferentialMotor::applyModel(void)
// {
//   double m_noise1 = 0, m_noise2 = 0;
//
//   addInertia();
//
//   if (randomDist == UNIFORM) {
//     m_noise1 = 1 + NDMath::randDouble4(-1.0, 1.0) * noise1 / 100.0;
//     m_noise2 = 1 + NDMath::randDouble4(-1.0, 1.0) * noise2 / 100.0;
//   }
//   else if(randomDist == GAUSS) {
//     m_noise1 = 1 + NDMath::randGaussian() * noise1 / 100 / 3; // 3, 99% confidence
//     m_noise2 = 1 + NDMath::randGaussian() * noise2 / 100.0 / 3;
//   }
//
//   m_noise1 > 2 ? 2 : m_noise1; // noise no pasa de 100%
//   m_noise1 < 0 ? 0 : m_noise1;
//   m_noise2 > 2 ? 2 : m_noise2;
//   m_noise2 < 0 ? 0 : m_noise2;
//
//   vel1 *= m_noise1;
//   vel2 *= m_noise2;
//   prev_vel1 = vel1;
//   prev_vel2 = vel2;
//
//   double dth = (vel1 - vel2)*max_vel/wheelsSeparation*advance_period/1000;
//   QPointF d  = NDMath::polarToCartesian((vel1 + vel2) * max_vel/2.0 * advance_period/1000*10, dth);
//
//   NDMath::selfRotateRad(d, -odometry->getOrientation().at(0), QPointF());  // para poder sumar abajo
//
//   odometry->setCoordinates(odometry->getPosition().at(0) + d.x(), odometry->getPosition().at(1) + d.y(), odometry->getOrientation().at(0) + dth);
//
// }

void DifferentialMotor::reset(bool full)
{
    AbstractMotor::reset(full);
    inerl = 0.0;
    inerr = 0.0;
    if (full) {
        rawOdometry->setCoordinates(0, 0, 0);
        exactOdometry->setCoordinates(0, 0, 0);
        slamOdometry->setCoordinates(0, 0, 0);
        QPointF traslation(slamOdometry->getPosition().at(0),  slamOdometry->getPosition().at(1));
        double rotation = slamOdometry->getOrientation().at(0);
    }
}

void DifferentialMotor::velsToPose(NDPose& velsPose, OdometryType type)
{
    NDPose* odometryP;

    //   cout << "velsPose 1" << velsPose << endl;

    double dth = (vel2 - vel1) * max_vel / wheelsSeparation * advance_period / 1000;
    // **ins**  double dth = (vel1 - vel2) * max_vel / wheelsSeparation * advance_period / 1000;
    QPointF d  = NDMath::polarToCartesian((vel1 + vel2) * max_vel / 2.0 * advance_period / 1000 * 10, dth);

    if (type == RAW)
        odometryP = rawOdometry;
    else if (type == EXACT)
        //     odometryP = exactOdometry;
        NDPose* odometryP = exactOdometry;
    else if (type == SLAM)
        odometryP = slamOdometry;

    //   NDMath::selfRotateRad(d, -odometryP->getOrientation().at(0), QPointF());  // para poder sumar abajo

    vector<double> v_th(1);
    //   v_th[0] = odometryP->getOrientation().at(0) + dth;
    vector<double> v_d(2);
    //   v_d[0] = odometryP->getPosition().at(0) + d.x();
    //   v_d[1] = odometryP->getPosition().at(1) + d.y();

    //   cout << "odometryP->getOrientation() " << exactOdometry->getOrientation().at(0) << endl;
    v_th[0] = dth;

    v_d[0] = d.x();
    v_d[1] = d.y();

    velsPose.setCoordinates(v_d, v_th);

    //    velsPose.setCoordinates(odometryP->getPosition().at(0) + d.x(), odometry->getPosition().at(1) + d.y(), odometryP->getOrientation().at(0) + dth);

    //   cout << "velsPose 2" << velsPose << endl << endl;
}
