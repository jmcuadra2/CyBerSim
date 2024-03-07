//
// C++ Implementation: nomadsensor
//
// Description: 
//
//
// Author: Javier Garcia Misis <>, (C) 2010
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "nomadsensor.h"

NomadSensor::NomadSensor(int id, double rad, double start_ang, double end_ang, int group_idx, QObject* parent, const char* name): AbstractSensor(id, rad, start_ang, end_ang, group_idx, parent, name)
{
}


NomadSensor::NomadSensor(int id, int group_idx, double rad, double rot, double orient, QObject* parent, const char* name): AbstractSensor(id, group_idx, rad, rot, orient, parent, name)
{
}


NomadSensor::NomadSensor(int id, int group_idx, double x_, double y_, double rot, double orient, QObject* parent, const char* name): AbstractSensor(id, group_idx, x_, y_, rot, orient, parent, name)
{
}


NomadSensor::NomadSensor(int id, int group_idx, QObject* parent, const char* name): AbstractSensor(id, group_idx, parent, name)
{
}


NomadSensor::~NomadSensor()
{
}

void NomadSensor::advance(int stage)
{
    if (sensorLine) sensorLine->advance(stage);
}

void NomadSensor::moveBy(double dx, double dy)
{
}

void NomadSensor::moveBy(double dx, double dy, double ang)
{
    if (sensorLine) sensorLine->rotate(dx,dy,ang);
}

void NomadSensor::rotate(void)
{
    if (sensorLine) sensorLine->rotate();
}


void NomadSensor::setPosition(double x_, double y_, double rot)
{
    AbstractSensor::setPosition(x_, y_, rot);
    moveBy(x_,y_,rot);
}

void NomadSensor::setVisible(bool on)
{
    if(ident==2005)
        std::cout << "setVisible ident=" << ident <<" on=" << on << std::endl;
    sensorLine->setVisible(on);
}

void NomadSensor::showField(bool show)
{
    sensorLine->setShowColor(show);
}

void NomadSensor::setRobotPosition(RobotPosition *rob_pos){
    AbstractSensor::setRobotPosition(rob_pos);
}



double NomadSensor::sample(void){
    rawReading = getRawReading();

    if(rawReading >= sensorWrongVal) {
    //     if(prev_out < sensorCriticVal)
    //       prev_out = 0.9*prev_out;
        out = prev_out;
        sensorLine->setValidReading(false);
        sensorLine->scale(xReading(), yReading());
    }
    else {
        if(rawReading >= sensorMaxVal)
        rawReading = sensorMaxVal;
        out = 1.0 - double(rawReading)/sensorMaxVal;
        prev_out = out;
        sensorLine->setValidReading(true);  
        sensorLine->scale(xReading(), yReading());    
    }

    if(sensorLine->isValidReading()) {
    //     emit getSensorVal(rawReading, group_index);
        if(emitSignal) {
        emit getSensorValue(rawReading);
        emit getSensorFValue((double)rawReading, group_index + 1);
        }         
    }
    else {
    //     emit getSensorVal(-1, group_index);
        if(emitSignal) {
        emit getSensorValue(-1);
        emit getSensorFValue(-1.0, group_index + 1);
        }           
    }

    writeMeasure();
    return out;
}

int NomadSensor::getRawReading(bool obtain){

  rawReading = rawReading < 0 ? sensorWrongVal : rawReading;
  rawReading = rawReading > sensorWrongVal ? sensorWrongVal : rawReading;

  sensorLine->setLength(rawReading);
  x_reading = sensorLine->getGlobalIntersectionPoint().x();
  y_reading = sensorLine->getGlobalIntersectionPoint().y();

  return rawReading;
}

void NomadSensor::setConnector(Nomad::Connector * con){
    connector_ = con;
}

Nomad::Connector * NomadSensor::getConnector(){
    return connector_;
}
