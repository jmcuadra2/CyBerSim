/***************************************************************************
                          sensorfactory.cpp  -  description
                             -------------------
    begin                : Wed Jul 20 2005
    copyright            : (C) 2005 by Jose M. Cuadra Troncoso
    email                : jmcuadra@dia.uned.es
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "sensorfactory.h"
#include "kheplightsensor.h"
#include "irsensor.h"
#include "bumpsensor.h"
#include "ndariasonar.h"
#include "ndariasick.h"
#include "ndariabumper.h"
#include "rangelinesensor.h"
#include "bumplinesensor.h"
#include "ndariasonar.h"
#include "genericradialfieldsensor.h"
// #include "rangebeamsensor.h"
#include "fakeblobcamera.h"
#include "ariacamera.h"

//Nomad sensor 
#include "nomadbumpsensor.h"
#include "nomadirsensor.h"
#include "nomadsonarsensor.h"

SensorFactory* SensorFactory::factory = nullptr;

SensorFactory::SensorFactory(){
}
SensorFactory::~SensorFactory(){
}

SensorFactory* SensorFactory::instance(void)
{

    if(!factory)
        factory = new SensorFactory();
    return factory;

}

RadialFieldSensor* SensorFactory::
factoryRadialFieldSensor(int type, int id, int group_idx,
                         double rad, double rot, double orient, double arc_,
                         double no_f_val, double satur_val, double normval,
                         QObject *parent, const char *name)
{

    RadialFieldSensor* sensor = nullptr;
    if(type == AbstractSensor::KHEP_LIGHT) {
        KhepLightSensor* lsen;
        lsen = new KhepLightSensor(id, group_idx, rad, rot, orient, arc_, no_f_val, satur_val, normval, parent, name);
        sensor = (RadialFieldSensor*)lsen;
    }
    else if(type == AbstractSensor::GENERIC_RADIAL_FIELD) {
        GenericRadialFieldSensor* gsen;
        gsen = new GenericRadialFieldSensor(id, group_idx, rad, rot, orient, arc_, no_f_val, satur_val, normval, parent, name);
        sensor = (RadialFieldSensor*)gsen;
    }
    return sensor;

}

RadialFieldSensor* SensorFactory::
factoryRadialFieldSensor(int type, int id, int group_idx,
                         QObject *parent, const char *name)
{

    RadialFieldSensor* sensor = nullptr;
    if(type == AbstractSensor::KHEP_LIGHT) {
        KhepLightSensor* lsen;
        lsen = new KhepLightSensor(id, group_idx, parent, name);
        sensor = (RadialFieldSensor*)lsen;
    }
    else if(type == AbstractSensor::GENERIC_RADIAL_FIELD) {
        GenericRadialFieldSensor* gsen;
        gsen = new GenericRadialFieldSensor(id, group_idx, parent, name);
        sensor = (RadialFieldSensor*)gsen;
    }
    return sensor;

}

PointsSampledSensor* SensorFactory::
factoryPointsSampledSensor(int type, int id, int group_idx,
                           double rad, double rot, double orient,
                           QObject *parent, const char *name)
{

    PointsSampledSensor* sensor = nullptr;
    if(type == AbstractSensor::KHEP_PROXIMITY) {
        IRSensor* irsen = new IRSensor(id, group_idx, rad,
                                       rot, orient, parent, name);
        sensor = (PointsSampledSensor*)irsen;
    }
    else if(type == AbstractSensor::KHEP_BUMP) {
        BumpSensor* bsen = new BumpSensor(id, group_idx, rad,
                                          rot, orient, parent, name);
        sensor = (PointsSampledSensor*)bsen;
    }
    return sensor;

}

PointsSampledSensor* SensorFactory::
factoryPointsSampledSensor(int type, int id, int group_idx,
                           QObject *parent, const char *name)
{

    PointsSampledSensor* sensor = nullptr;
    if(type == AbstractSensor::KHEP_PROXIMITY) {
        IRSensor* irsen = new IRSensor(id, group_idx, parent, name);
        sensor = (PointsSampledSensor*)irsen;
    }
    else if(type == AbstractSensor::KHEP_BUMP) {
        BumpSensor* bsen = new BumpSensor(id, group_idx, parent, name);
        sensor = (PointsSampledSensor*)bsen;
    }
    return sensor;

}

AriaSensor* SensorFactory::
factoryAriaSensor(int type, int id, int group_idx, double rad, double rot, double orient, AriaNetPacketWrapper* wrapper, QObject *parent, const char *name)
{
    AriaSensor* sensor = nullptr;
    if(type == AbstractSensor::ARIA_SONAR) {
        NDAriaSonar* senSonar = nullptr;
        SonarInfoWrapper* sonar_info = dynamic_cast<SonarInfoWrapper*>(wrapper);
        if(sonar_info)
            senSonar = new NDAriaSonar(id, group_idx, rad, rot, orient, sonar_info, parent, name);
        sensor = (AriaSensor*)senSonar;
    }
    else if(type == AbstractSensor::ARIA_SICK) {
        NDAriaSick* laser;
        SickInfoWrapper* sick_info = dynamic_cast<SickInfoWrapper*>(wrapper);
        if(sick_info) {
            laser = new NDAriaSick(id, group_idx, rad, rot, orient, sick_info, parent, name);
            sensor = (AriaSensor*)laser;
        }
    }
    else if(type == AbstractSensor::ARIA_BUMP) {
        BumperInfoWrapper* bump_info = dynamic_cast<BumperInfoWrapper*>(wrapper);
        if(bump_info)
            sensor = (AriaSensor*)new NDAriaBumper(id, group_idx, rad, rot, orient, bump_info, parent, name);
    }
    else if(type == AbstractSensor::ARIA_CAMERA) {
        BlobCameraPacketWrapper* camera_info = dynamic_cast<BlobCameraPacketWrapper*>(wrapper);
        if(camera_info)
            sensor = (AriaSensor*)new AriaCamera(id, group_idx, rad, rot, orient, camera_info, parent, name);
    }
    return sensor;

}

AriaSensor* SensorFactory::factoryAriaSensor(int type, int id, int group_idx,
                                             AriaNetPacketWrapper* wrapper, QObject *parent, const char *name)
{

    AriaSensor* sensor = nullptr;
    if(type == AbstractSensor::ARIA_SONAR) {
        SonarInfoWrapper* sonar_info = dynamic_cast<SonarInfoWrapper*>(wrapper);
        if(sonar_info)
            sensor = (AriaSensor*)new NDAriaSonar(id, group_idx, sonar_info, parent, name);
    }
    else if(type == AbstractSensor::ARIA_SICK) {
        SickInfoWrapper* sick_info = dynamic_cast<SickInfoWrapper*>(wrapper);
        if(sick_info)
            sensor = (AriaSensor*)new NDAriaSick(id, group_idx, sick_info, parent, name);
    }
    else if(type == AbstractSensor::ARIA_BUMP) {
        BumperInfoWrapper* bump_info = dynamic_cast<BumperInfoWrapper*>(wrapper);
        if(bump_info)
            sensor = (AriaSensor*)new NDAriaBumper(id, group_idx, bump_info, parent, name);
    }
    else if(type == AbstractSensor::ARIA_CAMERA) {
        BlobCameraPacketWrapper* camera_info =
                            dynamic_cast<BlobCameraPacketWrapper*>(wrapper);
        if(camera_info)
            sensor = (AriaSensor*)new AriaCamera(id, group_idx, camera_info, parent, name);
    }
    return sensor;

}

AriaSensor* SensorFactory::
factoryAriaSensor(int type, int id, double rad, int group_idx, double start_ang, double end_ang, AriaNetPacketWrapper* wrapper, QObject *parent, const char *name)
{

    AriaSensor* sensor = nullptr;
    if(type == AbstractSensor::ARIA_SONAR) {
        SonarInfoWrapper* sonar_info = dynamic_cast<SonarInfoWrapper*>(wrapper);
        if(sonar_info)
            sensor = (AriaSensor*)new NDAriaSonar(id, rad, start_ang, end_ang, group_idx, sonar_info,
                                     parent, name);
    }
    else if(type == AbstractSensor::ARIA_SICK) {
        SickInfoWrapper* sick_info = dynamic_cast<SickInfoWrapper*>(wrapper);
        if(sick_info)
            sensor = (AriaSensor*)new NDAriaSick(id, rad, start_ang, end_ang, group_idx, sick_info,
                                    parent, name);
    }
    else if(type == AbstractSensor::ARIA_CAMERA) {
        BlobCameraPacketWrapper* camera_info =
                            dynamic_cast<BlobCameraPacketWrapper*>(wrapper);
        if(camera_info)
            sensor = (AriaSensor*)new AriaCamera(id, rad, start_ang, end_ang, group_idx, camera_info,
                                    parent, name);
    }
    return sensor;

}

AriaSensor* SensorFactory::factoryAriaSensor(int type, int id, double x, double y,
                                double orient, int group_idx, AriaNetPacketWrapper* wrapper,
                                QObject *parent, const char *name)
{
    AriaSensor* sensor = nullptr;
    if(type == AbstractSensor::ARIA_SONAR) {
        NDAriaSonar* senSonar = nullptr;
        SonarInfoWrapper* sonar_info = dynamic_cast<SonarInfoWrapper*>(wrapper);
        if(sonar_info)
            senSonar = new NDAriaSonar(id, group_idx, x, y, 0, orient, sonar_info,
                                       parent, name);
        sensor = (AriaSensor*)senSonar;
    }
    else if(type == AbstractSensor::ARIA_SICK) {
        NDAriaSick* laser;
        SickInfoWrapper* sick_info = dynamic_cast<SickInfoWrapper*>(wrapper);
        if(sick_info) {
            laser = new NDAriaSick(id, group_idx, x, y, 0, orient, sick_info, parent, name);
            sensor = (AriaSensor*)laser;
        }
    }
    else if(type == AbstractSensor::ARIA_BUMP) {
        BumperInfoWrapper* bump_info = dynamic_cast<BumperInfoWrapper*>(wrapper);
        if(bump_info)
            sensor = (AriaSensor*)new NDAriaBumper(id, group_idx, x, y, 0, orient, bump_info,
                                      parent, name);
    }
    else if(type == AbstractSensor::ARIA_CAMERA) {
        BlobCameraPacketWrapper* camera_info =
                dynamic_cast<BlobCameraPacketWrapper*>(wrapper);
        if(camera_info)
            sensor = (AriaSensor*)new AriaCamera(id, group_idx, x, y, 0, orient, camera_info,
                                    parent, name);
    }
    return sensor;
}

AriaNetPacketWrapper* SensorFactory::factoryAriaInfoWrapper(int type, ArClientBase* client)
{
    AriaNetPacketWrapper* wrapper = nullptr;
    if(type == AbstractSensor::ARIA_SONAR)
        wrapper = (AriaNetPacketWrapper*)new SonarInfoWrapper();
    else if(type == AbstractSensor::ARIA_SICK)
        wrapper = (AriaNetPacketWrapper*)new SickInfoWrapper();
    else if(type == AbstractSensor::ARIA_BUMP)
        wrapper = (AriaNetPacketWrapper*)new BumperInfoWrapper();
    else if(type == AbstractSensor::ARIA_CAMERA)
        wrapper = (AriaNetPacketWrapper*)new BlobCameraPacketWrapper();
    if(wrapper)
        wrapper->setClient(client);
    return wrapper;
}

LinearSensor* SensorFactory::factoryLinearSensor(int type, int id, double rad,
                                int group_idx, double start_ang, double end_ang,
                                QObject *parent, const char *name)
{
    LinearSensor* sensor = nullptr;
    if(type == AbstractSensor::RANGE_LINE_SENSOR) {
        RangeLineSensor* rangelinesen = new RangeLineSensor(id, rad, start_ang, end_ang,
                                                            group_idx, parent, name);
        sensor = (LinearSensor*)rangelinesen;
    }
    else if(type == AbstractSensor::BUMP_LINE_SENSOR) {
        BumpLineSensor* bumplinesen = new BumpLineSensor(id, rad, start_ang, end_ang,
                                                         group_idx, parent, name);
        sensor = (LinearSensor*)bumplinesen;
    }
    else if(type == AbstractSensor::RANGE_BEAM_SENSOR) {
        RangeBeamSensor* beamsen = new RangeBeamSensor(id, rad, start_ang, end_ang,
                                                       group_idx, parent, name);
        sensor = (LinearSensor*)beamsen;
    }
    else if(type == AbstractSensor::FAKE_BLOB_CAMERA) {
        FakeBlobCamera* camera = new FakeBlobCamera(id, group_idx, parent, name);
        sensor = (LinearSensor*)camera;
    }
    return sensor;
}

LinearSensor* SensorFactory::factoryLinearSensor(int type, int id, double rad,
                                    int group_idx, double x, double y, double start_ang,
                                    double end_ang, QObject *parent, const char *name)
{
    LinearSensor* sensor = nullptr;
    if(type == AbstractSensor::RANGE_LINE_SENSOR) {
        RangeLineSensor* rangelinesen = new RangeLineSensor(id, rad, start_ang, end_ang,
                                                            group_idx, x, y, parent, name);
        sensor = (LinearSensor*)rangelinesen;
    }

    return sensor;
}


LinearSensor* SensorFactory::factoryLinearSensor(int type, int id, int group_idx, double rad, double rot, double orient, QObject *parent, const char *name)
{
    LinearSensor* sensor = nullptr;
    if(type == AbstractSensor::RANGE_LINE_SENSOR) {
        RangeLineSensor* rangelinesen = new RangeLineSensor(id, group_idx,rad, rot, orient,
                                                            parent, name);
        sensor = (LinearSensor*)rangelinesen;
    }
    else if(type == AbstractSensor::BUMP_LINE_SENSOR) {
        BumpLineSensor* bumplinesen = new BumpLineSensor(id, group_idx,rad, rot, orient,
                                                         parent, name);
        sensor = (LinearSensor*)bumplinesen;
    }
    else if(type == AbstractSensor::RANGE_BEAM_SENSOR) {
        RangeBeamSensor* beamsen = new RangeBeamSensor(id, group_idx,rad, rot, orient,
                                                       parent, name);
        sensor = (LinearSensor*)beamsen;
    }
    else if(type == AbstractSensor::FAKE_BLOB_CAMERA) {
        FakeBlobCamera* camera = new FakeBlobCamera(id, group_idx, parent, name);
        sensor = (LinearSensor*)camera;
    }
    return sensor;

} 

LinearSensor* SensorFactory::factoryLinearSensor(int type, int id, int group_idx,
                                                 QObject *parent, const char *name)
{
    LinearSensor* sensor = nullptr;
    if(type == AbstractSensor::RANGE_LINE_SENSOR) {
        RangeLineSensor* rangelinesen = new RangeLineSensor(id, group_idx, parent, name);
        sensor = (LinearSensor*)rangelinesen;
    }
    else if(type == AbstractSensor::BUMP_LINE_SENSOR) {
        BumpLineSensor* bumplinesen = new BumpLineSensor(id, group_idx, parent, name);
        sensor = (LinearSensor*)bumplinesen;
    }
    else if(type == AbstractSensor::RANGE_BEAM_SENSOR) {
        RangeBeamSensor* beamsen = new RangeBeamSensor(id, group_idx, parent, name);
        sensor = (LinearSensor*)beamsen;
    }
    else if(type == AbstractSensor::FAKE_BLOB_CAMERA) {
        FakeBlobCamera* camera = new FakeBlobCamera(id, group_idx, parent, name);
        sensor = (LinearSensor*)camera;
    }
    return sensor;
}




NomadSensor* SensorFactory::factoryNomadSensor(int type, int id, double rad, int group_idx,
                                               double start_ang, double end_ang,
                                               QObject *parent, const char *name)
{
    NomadSensor* sensor = nullptr;
    if(type == AbstractSensor::NOMAD_BUMP) {
        NomadBumpSensor* nbumpsen = new NomadBumpSensor(id, rad, start_ang, end_ang,
                                                        group_idx, parent, name);
        sensor = (NomadSensor*)nbumpsen;
    }
    else if(type == AbstractSensor::NOMAD_SONAR) {
        NomadSonarSensor* nsonarsen = new NomadSonarSensor(id, rad, start_ang, end_ang,
                                                           group_idx, parent, name);
        sensor = (NomadSensor*)nsonarsen;
    }
    else if(type == AbstractSensor::NOMAD_IR) {
        NomadIRSensor* nirsen = new NomadIRSensor(id, rad, start_ang, end_ang, group_idx,
                                                  parent, name);
        sensor = (NomadSensor*)nirsen;
    }
    return sensor;
}

NomadSensor* SensorFactory::factoryNomadSensor(int type, int id, int group_idx, double rad,
                                               double rot, double orient,
                                               QObject *parent, const char *name)
{
    NomadSensor* sensor = nullptr;
    if(type == AbstractSensor::NOMAD_BUMP) {
        NomadBumpSensor* nbumpsen = new NomadBumpSensor(id, group_idx,rad, rot, orient,
                                                        parent, name);
        sensor = (NomadSensor*)nbumpsen;
    }
    else if(type == AbstractSensor::NOMAD_SONAR) {
        NomadSonarSensor* nsonarsen = new NomadSonarSensor(id, group_idx,rad, rot, orient,
                                                           parent, name);
        sensor = (NomadSensor*)nsonarsen;
    }
    else if(type == AbstractSensor::NOMAD_IR) {
        NomadIRSensor* nirsen = new NomadIRSensor(id, group_idx,rad, rot, orient, parent,
                                                  name);
        sensor = (NomadSensor*)nirsen;
    }
    return sensor;
    
} 

NomadSensor* SensorFactory::factoryNomadSensor(int type, int id, int group_idx,
                                               QObject *parent, const char *name)
{
    NomadSensor* sensor = nullptr;
    if(type == AbstractSensor::NOMAD_BUMP) {
        NomadBumpSensor* nbumpsen = new NomadBumpSensor(id, group_idx, parent, name);
        sensor = (NomadSensor*)nbumpsen;
    }
    else if(type == AbstractSensor::NOMAD_SONAR) {
        NomadSonarSensor* nsonarsen = new NomadSonarSensor(id, group_idx, parent, name);
        sensor = (NomadSensor*)nsonarsen;
    }
    else if(type == AbstractSensor::NOMAD_IR) {
        NomadIRSensor* nirsen = new NomadIRSensor(id, group_idx, parent, name);
        sensor = (NomadSensor*)nirsen;
    }
    return sensor;
}
