//
// C++ Implementation: rangebeamsensor
//
// Description: 
//
//
// Author: Jose Manuel Cuadra Troncoso <jmcuadra@dia.uned.es>, (C) 2009
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "rangebeamsensor.h"

RangeBeamSensor::RangeBeamSensor(int id, int group_idx, QObject* parent, const char* name):
                    RangeLineSensor(id, group_idx, parent, name)
{
    initParams();
}

RangeBeamSensor::RangeBeamSensor(int id, int group_idx, double x_, double y_, double rot,
                                 double orient, QObject* parent, const char* name) :
                    RangeLineSensor(id, group_idx, x_, y_, rot, orient, parent, name)
{
    initParams();
}

RangeBeamSensor::RangeBeamSensor(int id, int group_idx, double rad, double rot,
                                 double orient, QObject* parent, const char* name) :
                    RangeLineSensor(id, group_idx, rad, rot, orient, parent, name)
{
    initParams();
}

RangeBeamSensor::RangeBeamSensor(int id, double rad, double start_ang, double end_ang, int group_idx, QObject* parent, const char* name): RangeLineSensor(id, rad, start_ang, end_ang, group_idx, parent, name)
{
    initParams();
}

RangeBeamSensor::~RangeBeamSensor()
{
    for(int i = 0; i < sensorLines.size(); i++){
        delete (sensorLines.at(i));
    }
}

int RangeBeamSensor::sensorSuperType(void)
{
    return LINEAR_SENSOR;
}

int RangeBeamSensor::sensorType(void)
{
    return RANGE_BEAM_SENSOR;
}

double RangeBeamSensor::sample(void)
{
    rawReading = getRawReading();
    if(rawReading >= sensorWrongVal) {
        if(prev_out < sensorCriticVal)
            prev_out = 0.9*prev_out;
        out = prev_out;
        sensorLine->setValidReading(false);

        emit getSensorVal(-1, group_index);
        if(emitSignal) {
            emit getSensorValue(-1);
            emit getSensorFValue(-1.0, group_index + 1);
        }
    }
    else {
        if(rawReading >= sensorMaxVal)
            rawReading = sensorMaxVal;
        out = 1.0 - (double) rawReading/sensorMaxVal;
        prev_out = out;
        sensorLine->setValidReading(true);
        emit getSensorVal(rawReading, group_index);
        if(emitSignal) {
            emit getSensorValue(rawReading);
            emit getSensorFValue((double)rawReading, group_index + 1);
        }
    }
    sensorLine->setLength(rawReading);
    x_reading = sensorLine->getGlobalIntersectionPoint().x();
    y_reading = sensorLine->getGlobalIntersectionPoint().y();

    writeMeasure();
    return out;
}

int RangeBeamSensor::getRawReading(bool obtain)
{
    double sumRawReading = 0; //sumatorio de rawReading correctos.
    double sumZStar = 0; //sumatorio de z_star.
    double sumWeightsRaw = 0;
    double sumWeightsZStar = 0;
    double sumWShort = 0;
    double sumWRand = 0;
    double sumLambda = 0;
    int correctRawReadings = 0;

    if(!obtain)
        return rawReading;

    int numReadings = rawReadings.size();
    int requiredNumReadings = int(numReadings*minApertureFraction + .5) + 1;
    requiredNumReadings = !requiredNumReadings ? 1 : requiredNumReadings;

    mapCollisionSolver.clear();

    for(int i = 0; i < numReadings; i++) {
        CollisionSolver* solver = sensorLines.at(i)->getSolver();
        int auxRawReading = sensorLines.at(i)->sample(false);

        sumWShort += sensorLines.at(i)->getWShort();
        sumWRand += sensorLines.at(i)->getWRand();
        sumLambda += sensorLines.at(i)->getLambda();

        addReadingToSolver(i, auxRawReading, solver);
        paintLine(i, auxRawReading);
    }

    QList<CollisionSolver*> keys = mapCollisionSolver.keys();

    rawReading = getSensorWrongVal(); //inicializo al max
    int zStar_star = getSensorWrongVal();

    for( int k = 0; k < keys.size() ; k++ ){  //for each e_k
        QVector<QPoint> readings_zstar = mapCollisionSolver.value(keys[k]);
        correctRawReadings = 0;
        sumRawReading = 0;
        sumWeightsRaw = 0;
        sumZStar = 0;
        sumWeightsZStar = 0;
        int numElementReadings = readings_zstar.size();

        for (int j = 0; j < numElementReadings; j++) { //for j in e_k
            double reading = readings_zstar.at(j).x();
            double z_star = readings_zstar.at(j).y();
            if(reading < sensorWrongVal){
                double weight = reading/sensorWrongVal;
                weight = 1/(1 + weight*weight);
                sumRawReading += reading*weight;
                sumWeightsRaw += weight;
                correctRawReadings++;
            }

            if(numElementReadings >= requiredNumReadings) {
                double weight = z_star/sensorWrongVal;
                weight = 1/(1 + weight*weight);
                sumZStar += z_star*weight;
                sumWeightsZStar += weight;
            }

        }
        if(correctRawReadings >= requiredNumReadings) {
            int rR = (int)(sumRawReading/sumWeightsRaw + .5);
            if(rR < rawReading) {
                rawReading = rR;
            }
        }

        if(numElementReadings >= requiredNumReadings) {
            int rR = (int)(sumZStar/sumWeightsZStar + .5);
            if(rR && (rR < zStar_star)) {zStar_star = rR;}
        }

    }

    double w_short = sumWShort/numReadings;
    double w_rand = sumWRand/numReadings;
    double lambda = sumLambda/numReadings;
    double w_hit;
    double w_max;
    if(rawReading >= sensorWrongVal) {
        w_hit = 0;
        w_max = 1 - w_short - w_rand;
        rawReading = NDMath::randBeamModel(zStar_star, lambda, sigma, w_short,
                                           w_hit, w_max, w_rand, sensorWrongVal);
    }
    else {
        w_hit = 1 - w_short - w_rand;
        w_max = 0;
        rawReading = NDMath::randBeamModel(rawReading, lambda, sigma, w_short,
                                           w_hit, w_max, w_rand, sensorWrongVal);
    }

    return rawReading;
}

void RangeBeamSensor::addReadingToSolver(int idx, int reading, CollisionSolver* solver)
{
    if(mapCollisionSolver.contains(solver))
        mapCollisionSolver[solver].append(QPoint(reading, sensorLines.at(idx)->getZStar()));
    else {
        QVector<QPoint> rawReadingBlueLine;  // donde se matan? -> al morir mapCollisionSolver
        rawReadingBlueLine.append(QPoint(reading, sensorLines.at(idx)->getZStar()));
        mapCollisionSolver.insert(solver,rawReadingBlueLine);
    }
}

void RangeBeamSensor::paintLine(int idx, int auxRawReading)
{
    if(auxRawReading >= sensorWrongVal) {     //para pintar todas las lineas azules
        sensorLines.at(idx)->setValidReading(false);
        sensorLines.at(idx)->scale((double)(sensorLines.at(idx)->
                       getGlobalIntersectionPoint().x()),
                       (double)(sensorLines.at(idx)->getGlobalIntersectionPoint().y()));
    }
    else {
        sensorLines.at(idx)->setValidReading(true);
        sensorLines.at(idx)->scale(sensorLines.at(idx)->getGlobalIntersectionPoint().x(),
                                   sensorLines.at(idx)->getGlobalIntersectionPoint().y());
    }
}

void RangeBeamSensor::initParams(void)
{
    sensorMaxVal = 5000;
    sensorWrongVal = 5000;
    sensorCriticVal = 100;

    aperture = 15;
    beamResolution = 15;
    minApertureFraction = 1.0/3.0;

}

void RangeBeamSensor::setVisible(bool on){
    if(showBeam) {
        for(int i = 0; i < sensorLines.size(); i++){
            sensorLines.at(i)->setVisible(on);
        }
    }
    sensorLine->setVisible(on);
}

void RangeBeamSensor::showField(bool show){
    if(showBeam) {
        for(int i = 0; i < sensorLines.size(); i++){
            sensorLines.at(i)->setShowColor(show);   //me fije en linearsensor.cpp
        }
    }
    sensorLine->setShowColor(show);
}

void RangeBeamSensor::rotate(){
    for(int i = 0; i < sensorLines.size(); i++){
        sensorLines.at(i)->rotate();
    }
    sensorLine->rotate();
}

void RangeBeamSensor::advance(int stage){
    for(int i = 0; i < sensorLines.size(); i++){
        sensorLines.at(i)->advance(stage);
    }
    sensorLine->advance(stage);
}

void RangeBeamSensor::moveBy(double dx, double dy){
    //   for(int i = 0; i < sensorLines.size(); i++){
    //     //(sensorLines.at(i))->moveBy(dx,dy); en linearsensor.cpp esta comentado
    //   }
}

void RangeBeamSensor::moveBy(double dx, double dy, double ang){
    for(int i = 0; i < sensorLines.size(); i++){
        sensorLines.at(i)->rotate(dx,dy,ang); //me fije en linearsensor.cpp
    }
    sensorLine->rotate(dx,dy,ang);
}
