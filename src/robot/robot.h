/***************************************************************************
                          robot.h  -  description
                             -------------------
    begin                : lun dic 8 2003
    copyright            : (C) 2003 by Jose M. Cuadra Troncoso
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

#ifndef ROBOT_H
#define ROBOT_H

#include <list>

#include <QObject>
#include <QDomDocument>
#include <qewdialogfactory.h>
#include <QPixmap>
#include <QMouseEvent>
#include <QHash>
#include <QList>

#include "robotsprite.h"
#include "sensorsset.h"
#include "abstractsensor.h"
#include "../control/basesimcontrolfactory.h"

/////////////////////  Cambiar
//  
// #define MAX_VEL    100.0   // cm. por s.
//
//////////////////////  *************


/* \defgroup robot Robot
    Informacion sobre del robot
 */

/* \defgroup robot_base Base del robot
    \ingroup robot
    Informacion sobre la base robot
 */ 

// class SensorsSet;
class RobotPosition;
class RobotKineticsTab;
class AbstractWorld;
class AbstractMotor;

using namespace std;

/*! \ingroup robot_base 
    \brief Controla todos los aspectos del robot ejecutando la simulacion,
    directamente o a traves de otras clases.
  */

class Robot : public QObject
{
    Q_OBJECT
    //bool is_crash;

  protected:
    friend class RobotBuilder;
    friend class RobotFactory;
    friend class TotSimulRobotFactory;
    friend class NomadSimRobotFactory;
    friend class NDAriaRobotFactory;    
    friend class RobotDrawer;
    friend class RobotKineticsTab;
    friend class NDAriaRobotKineticsTab;

  public:

    Robot(int n_id, QObject *parent = 0, QString name = "");
    Robot(const QDomElement& e, QObject *parent = 0,
          QString name = "");
    virtual ~Robot();

    double robotScale(void) { return robot_scale ; }
    RobotPosition* robotPosition(void) {return robot_position ; }
    bool walking(void);
    virtual void setStatus(bool on);
//     virtual void Period(int ap);
    int getAdvancePeriod(void) { return advance_period ; }
    virtual void rotation(double rot_);
    virtual void setVel2(double r_vel, double l_vel);    
    virtual void crash(void);
    virtual void advance(int stage) = 0;
//     bool detectCollisions(void);
    virtual bool okPosition(void) = 0;
    virtual void viewSensors(bool show);
//base    void setSensorMode(int sm, bool load_first = false) = 0;
//base    int getSensorMode(void) { return sensorMode ; };
    virtual void readSensors(void);
//     virtual void initSim(double init_rv, double init_lv) = 0;
    virtual void initSim(bool reset) = 0;
//     void nextCollision(void);
//base    void setTypeIner(int t_iner) { type_iner = t_iner ;};
    virtual bool stop(void) = 0;
    virtual void outputMotors(void) = 0;
    virtual void save(void);
    virtual void write(QDomDocument& doc, QDomElement& e);   
    virtual const QString type(void) = 0;
    const QString& getFileName(void) { return file_name ; }
    void setFileName(const QString& f_name) {file_name = f_name ; }

    SensorsSet* getSensorsSet(void) { return sensorsSet ; }
    virtual void sensorsVisible(bool on);
    virtual void initRecord(void);
    virtual void setAdvancePeriod(int ap);

    void setRobotSprite(RobotSprite* robot_sprite);
    virtual void setIntrinsicVars(const QDomElement& e_size);    
    QString fullName(void);
    double getInitRot(void) { return init_rot ; };
    double getInitRightVel(void) { return init_right_vel ; }
    double getInitLeftVel(void) { return init_left_vel ; }
    virtual double getLinearVel(void) { return vlin ; }
    virtual double getRadialVel(void) { return vrad ; }
    virtual void setRealWheelsSep(double realWheelsSep);
    virtual double getRealWheelsSep(void) { return realWheelsSep ; }
    double getWheelsSep() const { return wheelsSep; }
      
    void setInitRightVel(double irv) { init_right_vel = irv ; }
    void setInitLeftVel(double ilv) { init_left_vel = ilv ; }
    virtual void initMotors(void);
    void initSensors(void);
//base    void setNoise(double noise) { robot_sprite->noise_ratio = noise ; };
    RobotSprite* getSprite(void) { return robot_sprite ; }

    QVector<double> * motorsOutput(void) { return &motors_output ; }
    void setRecording(bool is_rec) { is_recording = is_rec ; }
    void setMonitoring(bool is_mon) { is_monitoring = is_mon ; }
    void setOldVel(double r_vel, double l_vel); 
    bool getSaveKinetics(void) { return saveKinetics ; }
    void setSaveKinetics(bool sk) { saveKinetics = sk ; }
    bool editKinetics(void);
    bool fileChanged(void)  {return file_changed ;}
    void setFileChanged(bool f_changed) { file_changed = f_changed ;}

    virtual QList<QewExtensibleDialog*> createKineticsDialog(QewExtensibleDialog* parent);
    void setMaxVel(double m_vel);
    double getMaxVel(void) { return max_vel ; }
    virtual void* getClient(void) = 0;
    virtual double getY(void) = 0;
    virtual double getX(void) = 0;  
    virtual double getRotation(void) = 0;
    void setWorld(AbstractWorld* robot_world);
    AbstractWorld* getWorld(void) { return world ; }
 
    virtual void setDriving(bool on);
    
    const QHash<QString, AbstractMotor*> getMotors(void) { return motors ; }    
//     QList< BaseSimControl * > getMotorControls() const { return motorControls ; }
    void addMotor(const QString& sg_id, AbstractMotor* motor);
    virtual void setMainMotor(void) = 0;

    double getDiameter() const { return diamtr ; }
    virtual double getLeftVel(void) { return lvel ; }
    virtual double getRightVel(void) { return rvel ; }
    
    void setTimePoseTaken(TimeStamp timePoseTaken) { this->timePoseTaken = timePoseTaken ;}
    TimeStamp getTimePoseTaken(void) const { return timePoseTaken ;}
  
  public slots:

//  A revisar junto con la nueva jerarquí­a de World
    void worldCleared(void);
    void worldLoaded(void);
//    
    virtual void recordRobot(double x, double y, double rot,
                             double vx, double vy, double vrot, bool forw);
    virtual void setX(double x_);
    virtual void setY(double y_); 
    virtual void setXY(double x_, double y_);
    virtual void getMouseEvent(QMouseEvent* e);
    virtual void getKeyEvent(QKeyEvent* event, bool key_on);    
    list< int >* getActualSensorsList() { return actualSensorsList; }
    AbstractMotor* getMainMotor(void);
//     BaseSimControl* getMainControl(void); 
    void setPaused(bool paused) { this->paused = paused; } 
    
    bool getRealRobot(void) const { return realRobot; }

    double getRotInit() const;
    virtual void setRotInit(double value) {}

    virtual void updateOdometries(bool paintRaw = true, bool paintSlam = true);

  signals:
    void rVel(double);
    void lVel(double);
    void signalBump(void);
    void pause(void);
    void outWorld(void);
    void velMonitor(double, int); //  vel
    void rotMonitor(double, int);
    void sendToRecorder(double, int);
//  A revisar junto con la nueva jerarquia de World    
    void sendRadius(const double& );
    
    void sendKeyEvent(QKeyEvent* /*event*/, bool /*key_on*/); //driving
    void sendDrived(bool drived); //driving

  protected:
    virtual void setCanvas(QGraphicsScene* w_canvas);
    bool isCrash(void);
    virtual void save_special(QDomElement& ) {}
    virtual void write_special(QDomDocument& , QDomElement& ) {}

  public:
  enum sensorModeVals{KHEPERA_I, KHEPERA_II};
  enum robotType{TOTAL_SIMULATED, ARIA_ROBOT};

protected:
  double x, y, diamtr;
  //     double odomX, odomY, exactOdomX, exactOdomY;
    NDPose2D rawOdometry;
    NDPose2D exactOdometry;
    NDPose2D slamOdometry;
        
    double rvel, lvel;
    double init_rot;     
    int advance_period;
    
    RobotPosition* robot_position;
    RobotSprite *robot_sprite;
    QVector<double> motors_output;
    bool is_recording;
    bool is_monitoring;
    bool driving;

    SensorsSet* sensorsSet;
    int num_id;
    double robot_scale;
    int frame;
    double xVel, yVel;
    bool status_;
    double rot;
   bool is_crash;
    bool is_bump;

    bool crono_reset;
    double init_right_vel, init_left_vel, old_right_vel, old_left_vel;
    bool contact, prev_contact;     

    bool sensors_visible;
    QString file_name;
    QString full_name;
    double real_diamtr;
    double realWheelsSep, wheelsSep;
    QString robot_image_file;
    bool saveKinetics;
    bool file_changed;
    double vlin, vrad;
    double max_vel;
    
    list< int > * actualSensorsList;
    AbstractWorld* world;

    QHash<QString, AbstractMotor*> motors;
    AbstractMotor* mainMotor;
    
    bool paused;

    double velUnitsConversion; // a cm.
    
    bool realRobot;  
    
    double prevX, prevY, prevHead;
    
    TimeStamp timePoseTaken;

    double xInit, yInit, rotInit;
};


#endif


