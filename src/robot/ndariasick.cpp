//
// C++ Implementation: ndariasick
//
// Description: 
//
//
// Author: Jose Manuel Cuadra Troncoso <jmcuadra@dia.uned.es>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "ndariasick.h"
#include <Aria.h>

int NDAriaSick::sensorSuperType(void)
{
    return AbstractSensor::ARIA_SENSOR;
}

int NDAriaSick::sensorType(void )
{
    return AbstractSensor::ARIA_SICK;
}

NDAriaSick::NDAriaSick(int id, int group_idx, double x_, double y_, double rot, double orient, SickInfoWrapper* sick_inf, QObject* parent, const char* name): AriaRangeDevice(id, group_idx, x_, y_, rot, orient, sick_inf, parent, name)
{
    sick_info = sick_inf;
    initParams();
}

NDAriaSick::NDAriaSick(int id, int group_idx, double rad, double rot, double orient, SickInfoWrapper* sick_inf, QObject* parent, const char* name): AriaRangeDevice(id, group_idx, rad, rot, orient, sick_inf, parent, name)
{
    sick_info = sick_inf;
    initParams();
}

NDAriaSick::NDAriaSick(int id, int group_idx, SickInfoWrapper* sick_inf, QObject* parent, const char* name): AriaRangeDevice(id, group_idx, sick_inf, parent, name)
{
    sick_info = sick_inf;
    initParams();
}

NDAriaSick::NDAriaSick(int id, double rad, double start_ang, double end_ang, int group_idx, SickInfoWrapper* sick_inf, QObject* parent, const char* name): AriaRangeDevice(id, rad, start_ang, end_ang, group_idx, sick_inf, parent, name)
{
    sick_info = sick_inf;
    initParams();
}

NDAriaSick::~NDAriaSick()
{
}



double NDAriaSick::sample(void)
{
    AriaRangeDevice::sample();
    if(sensorLine->isValidReading()) {
        //     emit getSensorVal(rawReading, group_index + 1);
        if(emitSignal) {
            emit getSensorValue(rawReading);
            emit getSensorFValue((double)rawReading, group_index + 1);
        }
    }
    else {
        //     emit getSensorVal(-1, group_index + 1);
        if(emitSignal) {
            emit getSensorValue(-1);
            emit getSensorFValue(-1.0, group_index + 1);
        }
    }
    //       ArLog::log(ArLog::Terse, "%d Time = %lu.%lu Value = %f", i, sensor_info->getSecs(), sensor_info->getMSecs(), ranges[i].value());
    //   if(group_index == 0 || group_index == 180)
    //     ArLog::log(ArLog::Terse, "%d Time taken = %lu.%03lu  Time get = %lu.%03lu", group_index, sensor_info->getSecs(group_index), sensor_info->getMSecs(group_index), sensor_info->getTimeGet().getSec(), sensor_info->getTimeGet().getMSec());
    writeMeasure();
    return out;
}

// double NDAriaSick::sample(void)
// {
//   AriaRangeDevice::sample();
//   int lcd_ident = group_index + 2;
//   lcd_ident = lcd_ident >= 4 ? lcd_ident + 1 : lcd_ident;
//   if(sensorLine->isValidReading())
//     emit getSensorVal(lcd_ident, rawReading);
//   else
//     emit getSensorVal(lcd_ident, -1);
//   return out;
// }

void NDAriaSick::initParams(void)
{
    sensorMaxVal = 30000;
    sensorWrongVal = 32000;
    sensorCriticVal = 100;
    start_reading = int(start_angle + 90);
    end_reading = int(end_angle + 90);
    aperture = 1/NDMath::RAD2GRAD;
    idealSensor = true;

    x_org = 0; // Ñapa hasta que se pasen x_org e y_org en los constructores polares
    y_org = 0;
}

// int NDAriaSick::getRawReading(bool obtain)
// {
//   if(!obtain) return rawReading;
// //   x_reading = sensor_info->getXGlobal((start_reading + end_reading)/2);
// //   y_reading= sensor_info->getYGlobal((start_reading + end_reading)/2);
//   rawReading = sick_info->getRange((start_reading + end_reading)/2);
//   sensorLine->setLength(rawReading);
//   x_reading = sensorLine->getGlobalIntersectionPoint().x();
//   y_reading = sensorLine->getGlobalIntersectionPoint().y();
//   return rawReading;
// }

// int NDAriaSick::getRawReading(bool obtain)
// {
//   if(!obtain) return rawReading;
//   int min = sensorWrongVal;
//   int i_min = start_reading;
//   for(int i = start_reading; i < end_reading; i++) {
//     if(min > sick_info->getRange(i)) {
//       i_min = i;
//       min = sick_info->getRange(i_min);
//     }
//   }
//   if(min < sensorMaxVal) {
//     x_reading = sensor_info->getXGlobal(i_min);
//     y_reading= sensor_info->getYGlobal(i_min);
//   }
// //   else if(min < sensorWrongVal) {
// //     x_reading = sensor_info->getX(i_min);
// //     y_reading= sensor_info->getY(i_min);  
// //     double mod = sqrt(x_reading*x_reading + y_reading*y_reading);
// //     if(mod) {
// //       x_reading *= sensorMaxVal/mod;
// //       x_reading *= sensorMaxVal/mod;
// //     }
// //   }
//   else {
//     x_reading = sensor_info->getXGlobal((start_reading + end_reading)/2);
//     y_reading= sensor_info->getYGlobal((start_reading + end_reading)/2);
//   }
// //   x_reading = min;
// //   y_reading = 0;  
//   return min;
// }

void NDAriaSick::initMeasure(void)
{
    vector<size_t> yDimensions(1,9);
    //  yDimensions[0] = 1;
    //  yDimensions[1] = 2;
    sensorPose = NDPose::createPose(NDPose::Pose2D);
    sensorPose->setCoordinates(x_org, y_org, orientation/NDMath::RAD2GRAD);
    measure.initMeasure(NDPose::Pose2D, idChar, "mm", yDimensions);
    measure.setPose(sensorPose);
}

void NDAriaSick::writeMeasure()
{
    measure.setPose(sensorPose);
    //    measure.setTimeStamp();
    vector<double> mm(9);
    if(rawReading >= sensorWrongVal && !getIdealSensor())
        mm[0] = -1;
    else
        mm[0] = (double)rawReading;
    mm[1] = aperture;
    mm[2] = sensor_info->getXLocal(group_index);
    mm[3] = sensor_info->getYLocal(group_index);
    mm[4] = sensor_info->getSecs(group_index);
    mm[5] = sensor_info->getMSecs(group_index);
    mm[6] = sensor_info->getEncoderPose(group_index).getX();
    mm[7] = sensor_info->getEncoderPose(group_index).getY();
    mm[8] = sensor_info->getEncoderPose(group_index).getThRad();

    TimeStamp t(mm[4], mm[5]*1000);
    measure.setTimeStamp(t);
    measure.setY(mm);
}
