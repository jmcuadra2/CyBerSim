/***************************************************************************
                          abstractsensor.h  -  description
                             -------------------
    begin                : Mon Feb 7 2005
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

#ifndef ABSTRACTSENSOR_H
#define ABSTRACTSENSOR_H



#include "sensorline.h"
#include "../control/inputgenerator.h"
#include "../neuraldis/measure.hpp"
#include "../world/radialfieldsource.h"


/*! \defgroup sensors Sensores
    \ingroup robot
    Informacion sobre los sensores
 */

class RobotPosition;
class AbstractWorld;
class SensorLine;

/*! \ingroup sensors
    \brief Esta es la clase base para los distintos tipo de sensores.

    Clase abstracta.
    
    Define la interfaz comn a la que se accede principalmente desde
    la clase SensorsSet.
    Casi todos sus métodos son virtuales puros,
    aunque implementa la funcion que establece el canvas donde se dibuja el mundo,
    la clase World y la representación visual del robot,
    la clase RobotSprite, para el sensor.
    Guarda las variables que sitúan al sensor con respecto al robot.
  */

class AbstractSensor  : public InputGenerator
{
    Q_OBJECT
    
protected:
    AbstractSensor(int id, int group_idx, QObject *parent = 0,
                   const char *name = 0);
    AbstractSensor(int id, int group_idx, double x_, double y_, double rot,
                   double orient, QObject *parent = 0,
                   const char *name = 0);
    AbstractSensor(int id, int group_idx, double rad, double rot,
                   double orient, QObject *parent = 0,
                   const char *name = 0);
    AbstractSensor(int id, double rad, double start_ang,
                   double end_ang, int group_idx, QObject *parent = 0,
                   const char *name = 0);
    AbstractSensor(int id, double rad, double start_ang,
                   double end_ang, int group_idx, double x_, double y_, QObject *parent = 0,
                   const char *name = 0);

public:
    ~AbstractSensor();

    virtual int sensorSuperType(void);
    virtual int sensorType(void);

    void initialize(void);
    
    virtual void advance(int stage) = 0;
    virtual void rotate(void) = 0;
    virtual void showField(bool show) = 0;
    virtual void moveBy(double dx, double dy) = 0;
    virtual void moveBy(double dx, double dy, double ang) = 0;
    /*! Documentando sample()
    */
    virtual int getRawReading(bool obtain = true) = 0;
    virtual double xReading(void) { return x_reading ; }
    virtual double yReading(void) { return y_reading ; }
    
    void write(QDomDocument& doc, QDomElement& e);
    
    virtual void setPosition(double x_, double y_, double rot);
    virtual void setRobotPosition(RobotPosition *rob_pos);
    virtual void setVisible(bool) { }
    void setGroupIdent(int group_idx) {group_index = group_idx ; }
    void setWorldScale(int scale_){scale = scale_;}
    int getWorldScale(void){return scale;}
    int getGroupIdent(void) { return group_index ; }
    double getRotation(void) const { return rotation; }
    double getRotationOrg(void) const { return rotation_org; }
    double getOrientation(void) const { return orientation; }
    
    virtual void setSensorCriticVal ( int criticVal );
    virtual int getSensorCriticVal(void) const { return sensorCriticVal; }
    virtual void setSensorMaxVal ( int maxVal );
    virtual int getSensorMaxVal(void) const { return sensorMaxVal; }
    virtual void setSensorWrongVal ( int wrongVal );
    virtual int getSensorWrongVal(void) const { return sensorWrongVal; }
    double getRadius(void) { return radius ; }
    virtual void setRadius(double rad) { radius = rad ; }

    virtual SensorLine* createSensorLine(AbstractWorld* world,
                                         DrawingItem::Drawing drawing_type = DrawingItem::Rectangle,
                                         const QString& color = "", const QString& color2 = "",
                                         double angle = 0, bool rotateWithRobot = true);
    
    virtual QList<SensorLine*> createSensorLines(AbstractWorld* world,
                                                 DrawingItem::Drawing drawing_type = DrawingItem::Rectangle,
                                                 const QString& color = "", const QString& color2 = "",
                                                 int resolution = 0);

    const Measure<>& getMeasure(void)  {return measure ; }

    void setEmitSignal(bool emitSignal) { this->emitSignal = emitSignal; }
    bool getEmitSignal() const { return emitSignal; }

    virtual QString toString(void);

    RobotPosition* getRobotPosition(void) { return robot_position ; }
    void setIdealSensor(bool idealSensor) { this->idealSensor = idealSensor; }
    bool getIdealSensor() const { return idealSensor; }
    SensorLine* getSensorLine() const { return sensorLine ; }

    virtual void initSensor(void) {}
    
public slots:
    void monitorClosed(QObject* obj = 0);

signals:
    void getSensorVal(int , int );
    void getSensorValue(int );
    void getSensorFValue(double ,int );
    void disconnectRecord(int /*ch_idx*/);

public:

    /*! Esta enumeración define los tipos de las subclases derivadas abstractas
    (clases abstractas que no son la clase base, pero de las que derivan
    las clases no abstractas) de sensores.

    \li \c AbstractSensor::POINTS_SAMPLED: Sensores cuyo campo es simulado
    mediante puntos, subclase base abstracta Sensor.
    Estos puntos son de la clase SensorPoint. El método \c sample()
    del sensor debe ser implementado en las clases para que devuelva
    un numero real positivo,
    si el punto ocupa la misma posición que otro objeto del mundo, o 0 si no.
    un numero real positivo o 0 . Sensores que pueden ser implementados
    mediante este super tipo son aquellos en los que el sensor manda una señal
    y recibe la señal reflejada, los de contacto y otros similares.
    
    \li \c AbstractSensor::RADIAL_FIELD: Sensores cuyo campo es radial,
    subclase base abstracta RadialFieldSensor.
    Los objetos del mundo que interactan con este super tipo deben
    derivarse de la clase RadialFieldSource y conectarse
    al sensor mediante los metodos \c setSources() y \c addSources() de éste.

*/    
    enum SuperTypes {POINTS_SAMPLED = 1000, RADIAL_FIELD = 2000,
                     FILE_SAMPLED = 3000, ARIA_SENSOR = 4000, LINEAR_SENSOR = 5000,
                     NOMAD_SENSOR = 6000};
    enum Types {KHEP_PROXIMITY = 100, KHEP_LIGHT = 200, KHEP_BUMP = 300, ARIA_SONAR = 400,
                ARIA_BUMP = 500, ARIA_SICK = 600, ARIA_CAMERA = 650,
                RANGE_LINE_SENSOR = 700, RANGE_BEAM_SENSOR = 750, BUMP_LINE_SENSOR = 800,
                GENERIC_RADIAL_FIELD = 900, FAKE_BLOB_CAMERA = 810, NOMAD_SONAR = 1000,
                NOMAD_BUMP = 1100, NOMAD_IR = 1200};


protected:
    virtual void initParams(void);
    virtual void commonValues(void);
    virtual void initMeasure(void) = 0;
    virtual void writeMeasure(void);

protected:
    double x_org, y_org, rotation_org, rotation, x, y, radius, start_angle, end_angle;
    double orientation;
    RobotPosition *robot_position;
    int scale;
    int group_index;
    int rawReading;
    bool polar_mode;
    int start_reading, end_reading;
    int sensorMaxVal;
    int sensorCriticVal;
    int sensorWrongVal;
    double x_reading, y_reading;
    SensorLine* sensorLine;

    NDPose* sensorPose;
    Measure<> measure;
    bool emitSignal;
    double aperture;

    char idChar[11];
    bool idealSensor;
};

#endif
