/***************************************************************************
                          sensorfactory.h  -  description
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

#ifndef SENSORFACTORY_H
#define SENSORFACTORY_H

class RadialFieldSensor;   
class PointsSampledSensor;
class QObject;
class AriaSensor;
class AriaNetPacketWrapper;
class LinearSensor;
class RobotPosition;
class SensorLine;
class ArClientBase;
class NomadSensor;

/**
  *@author Jose M. Cuadra Troncoso
  */


class SensorFactory {

protected:
    SensorFactory();

public:
    static SensorFactory* instance(void);
    ~SensorFactory();

    RadialFieldSensor* factoryRadialFieldSensor(int type, int id, int group_idx,
                                    double rad, double rot, double orient,
                                    double arc_, double no_f_val, double satur_val,
                                    double normval,
                                    QObject *parent = 0, const char *name = 0);

    RadialFieldSensor* factoryRadialFieldSensor(int type, int id, int group_idx,
                                    QObject *parent = 0, const char *name = 0);

    PointsSampledSensor* factoryPointsSampledSensor(int type, int id, int group_idx,
                                    double rad, double rot, double orient,
                                    QObject *parent = 0, const char *name = 0);

    PointsSampledSensor* factoryPointsSampledSensor(int type, int id, int group_idx,
                                    QObject *parent = 0,  const char *name = 0);

    AriaSensor* factoryAriaSensor(int type, int id, int group_idx, double rad,
                                  double rot, double orient, AriaNetPacketWrapper* wrapper,
                                  QObject *parent = 0, const char *name = 0);
    
    AriaSensor* factoryAriaSensor(int type, int id, int group_idx,
                                  AriaNetPacketWrapper* wrapper,
                                  QObject *parent = 0, const char *name = 0);
    
    AriaSensor* factoryAriaSensor(int type, int id, double rad, int group_idx,
                          double start_ang, double end_ang,
                          AriaNetPacketWrapper* wrapper, QObject *parent = 0, const char *name = 0);
    
    AriaSensor* factoryAriaSensor(int type, int id, double x, double y, double orient, int group_idx, AriaNetPacketWrapper* wrapper, QObject *parent = 0, const char *name = 0);
    
    AriaNetPacketWrapper* factoryAriaInfoWrapper(int type, ArClientBase* client);
    
    LinearSensor* factoryLinearSensor(int type, int id, double rad, int group_idx,
                                      double start_ang, double end_ang, QObject *parent = 0, const char *name = 0);

    LinearSensor* factoryLinearSensor(int type, int id, double rad, int group_idx,
                                      double x, double y,double start_ang, double end_ang,
                                      QObject *parent = 0, const char *name = 0);
    
    LinearSensor* factoryLinearSensor(int type, int id, int group_idx, double rad,
                                      double rot, double orient,
                                      QObject *parent = 0, const char *name = 0);

    LinearSensor* factoryLinearSensor(int type, int id, int group_idx,
                                      QObject *parent = 0, const char *name = 0);

    NomadSensor* factoryNomadSensor(int type, int id, double rad, int group_idx,
                                    double start_ang, double end_ang,
                                    QObject *parent = 0, const char *name = 0);
    
    NomadSensor* factoryNomadSensor(int type, int id, int group_idx, double rad,
                                    double rot, double orient,
                                    QObject *parent = 0, const char *name = 0);

    NomadSensor* factoryNomadSensor(int type, int id, int group_idx,
                                    QObject *parent = 0, const char *name = 0);
    
protected:
    static SensorFactory* factory;
};

#endif
