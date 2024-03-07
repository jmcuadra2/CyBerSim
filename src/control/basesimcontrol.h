/***************************************************************************
                          basesimcontrol.h  -  description
                             -------------------
    begin                : Thu Apr 14 2005
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

#ifndef BASESIMCONTROL_H
#define BASESIMCONTROL_H

#include <QDomDocument>
#include <qewextensibledialog.h>
#include <QList>
#include <QVector>
#include <QHash>

#include "abstractfusionnode.h"
#include "abstractmotorplan.h"
#include "motorcommand.h"

class SMGInterface;

class BaseSimControl : public QObject {

    Q_OBJECT

public:
    BaseSimControl();

    virtual ~BaseSimControl();

    virtual void initElements(void) = 0;
    virtual void think(void);
    virtual void sampleInputs(void);
    virtual void sendOutputs(void) = 0;
    virtual void save(void);
    virtual void save_special(QDomDocument& doc_control, QDomElement& e);
    virtual void write(QDomDocument& doc, QDomElement& e);
    virtual int getType(void) = 0;
    virtual int getSuperType(void) = 0;

    virtual void backPropagate(void) = 0;
    virtual void doMonitor(void) = 0;
    virtual void connectIO(void) = 0;
    virtual void setRecording(bool , bool ) {}
    virtual void setParams(const QDomElement& e) = 0;

    virtual QList <QewExtensibleDialog*> tabsForEdit(QewExtensibleDialog* ) = 0;
    virtual const QString& getControlFileName(void) { return control_filename ; }
    virtual void setControlDoc(QDomElement& e);
    virtual void setControlFileName(const QString& filename) { control_filename = filename ; }
    bool fileChanged(void)  {return file_changed ;}
    void setFileChanged(bool f_changed) { file_changed = f_changed ;}
    virtual void setSamplingPeriod(double s_period);
    double getSamplingPeriod(void) { return sampling_period ; }

    virtual void appendSubcontrol(BaseSimControl* subcontrol);
    virtual void appendSubcontrols(QList<BaseSimControl*> subcontrols);

    BaseSimControl* getSubcontrol(size_t idx) { return subcontrols[idx] ; }

    void setControlName(const QString& controlName) { this->controlName = controlName; }
    const QString& getControlName() { return controlName; }

    bool isActive() const { return active ; }

    AbstractMotorPlan* getMotorPlan(void) { return motorPlan ; }
    void setDefaultCommand(NDPose* const commandValues);
    
    SMGInterface* getSMGInterface() const { return smgInterface ; }
    virtual bool createSMGInterface(void) { return false ; }

public slots:
    virtual void activate(bool on);

protected:
    virtual void propagate(void) = 0;
    virtual void readInputs(void) = 0;
    virtual bool setSMGInterface(SMGInterface* smgInterface);

signals:
    void sendToRecorder(double, int);
    void activation(bool);
    
public:
    enum ControlSuperTypes {CODE_CONTROL = 1000, NET_CONTROL = 2000, MOTOR_CONTROL = 4000};
    enum ControlTypes {BASE_CONTROL = 0, NULL_CONTROL = 100, NULL_NOMAD_CONTROL = 150,
                       BASE_NET_CONTROL = 200, ISO_NET_CONTROL = 300, RANGEGROUP_CONTROL = 400, DIFFERENTIAL_MOTOR_CONTROL = 500, DRIVE_CONTROL = 600, AREA_CENTER_REACTIVE_CONTROL = 700,
                       AREA_CENTER_SLAM_CONTROL = 800, AREA_CENTER_SEGMENT_CONTROL = 900, AREA_CENTER_SLAM_SEGMENT_CONTROL = 1000,
                       AREA_CENTER_RIGHT_LEFT_REACTIVE_CONTROL = 1100
                      };

protected:
    QHash<QString, QVector< AbstractFusionNode* > > inputNodeMap;
    bool file_changed;
    QString control_filename;
    bool hasView;
    double sampling_period;
    QList<BaseSimControl*> subcontrols;
    QString controlName;
    AbstractMotorPlan* motorPlan;
    bool active;
    MotorCommand defaultCommand;

private:
    SMGInterface* smgInterface;

};

#endif
