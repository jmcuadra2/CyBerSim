//
// C++ Interface: areacenterreactivecontrol
//
// Description:
//
//
// Author: Jose Manuel Cuadra Troncoso <jmcuadra@dia.uned.es>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef AREACENTERREACTIVECONTROL_H
#define AREACENTERREACTIVECONTROL_H

#include "nullsimcontrol.h"
#include "areacenter.h"
#include "areacenterpolygon.h"
#include <QObject>
#include <QFile>

#include "../slam/gui/slampainter.h"

class AbstractWorld;
class RobotPosition;
class ACControlRecorder;

/**
  @author Jose Manuel Cuadra Troncoso <jmcuadra@dia.uned.es>
*/
class AreaCenterReactiveControl : /*public QObject, */public NullSimControl
{

    Q_OBJECT
public:
    AreaCenterReactiveControl();

    virtual ~AreaCenterReactiveControl();

    int getSuperType(void);
    int getType(void);
    void doMonitor(void);
    void initElements(void);
    void sendOutputs(void);
    void connectIO(void);
    virtual void setWorld(AbstractWorld* world);

    //     void processCollisions(void);

    void setRecording(bool on, bool crono_reset);

    bool getShowPath(void) { return areaCenterPolygon->showAreaCenterPath(); }
    void setShowPath(bool on) { areaCenterPolygon->showAreaCenterPath(on); }

public slots:
    void slamPainterClosed(void) {slamPainter = 0 ;}
    void rawPainterClosed(void) {rawPainter = 0 ;}
    
protected:
    enum StallStatus {NotStalled, ForeStalled, BackStalled, ForeBackStalled};

protected:
    void propagate(void);
    void readInputs(void);
    void calculateAreaCenters(void);
    virtual void readRobotData(void);
    virtual void readGlobalPolygon(void);
    void addCommand(MotorCommand::MotorCommands type, double priority, double commandParam1, double commandParam2, double commandParam3); // los tres params de la pose
    MotorCommand writeCommand(MotorCommand::MotorCommands type, double priority, double commandParam1, double commandParam2, double commandParam3);

    QList<QewExtensibleDialog*> tabsForEdit(QewExtensibleDialog* parent);

    void calculateGoalSide(void);
    StallStatus getStalled(void);

    virtual void createAreaCenterPolygon(void);

    virtual void diamondConnectIO(void) {}

    //   signals:
    //     void sendToRecorder(double, int);

protected:
    AreaCenter *advanceAreaCenter, *globalAreaCenter;
    double robotDiameter;
    double robotScale;
    double localAngleStart, localAngleEnd;
    AbstractWorld* world;
    //     AreaCenterPolygon areaCenterPolygon;
    AreaCenterPolygon* areaCenterPolygon;
    AreaCenterPolygon::AdvanceStatus advanceStatus;
    double sensorMaxVal;
    double sensorSaturationVal;
    QVector<bool> detectedCollisionSectors;
    QVector<double> currentCollisionDistances;
    QVector<double> collisionDistances;
    int numCollisionSectors;

    bool isRecording, recorder_on;
    ACControlRecorder* recorder;

    double advanceRate;

    QVector< AbstractFusionNode* > areaCenterNodeList;

    AreaCenterPolygon::AdvanceStatus goalSidePreference;

    int falseReadings;

    bool suggestedSplit;
    double advanceRateThresSplit;
    StallStatus stalled;
    int countStall;
    double lvel, rvel;
    
    double pan;
    int blobArea;
    
    SlamPainter* slamPainter;
    SlamPainter* rawPainter;

    QFile fileoutput;
    QTextStream myLog;
    int segmentationTime;
    
};

#endif
