/***************************************************************************
                          isodevice.h  -  description
                             -------------------
    begin                : dom feb 22 2004
    copyright            : (C) 2004 by Jose M. Cuadra Troncoso
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

#ifndef ISODEVICE_H
#define ISODEVICE_H

#include "neuron.h"
#include "resonator.h"
#include "isoinputsynapse.h"

#include <QFile>
#include <QList>

class LearningLink;

/**
  *@author Jose M. Cuadra Troncoso
  */

class ISODevice : public Neuron
{

    Q_OBJECT

    friend class EditISODeviceTab;

public:
    ISODevice(ulong n_i, int sub_lay, double th, int actF, double initext,
              double l_r, bool recmon = false,
              QObject *parent = 0, const char *name = 0);
    ISODevice(ulong n_i, int lay, bool recmon = false,
              QObject *parent = 0, const char *name = 0);
    ISODevice(ulong n_i, bool recmon = false,
              QObject *parent = 0, const char *name = 0);
    ~ISODevice();

    virtual void clearResonators(void);
    virtual void setResonators(QList<Resonator *> resons);
    int connectionOffset(void) {return 1 ;}

    virtual QList<Resonator *> getResonators(void);
    int rtti_dev(void) const;
    const QString devName(bool isCap);
    Resonator * addReson(int n_id, double a, int n, bool isCS,
                         double w, bool wf, bool noneg);
    double calculateOutput(void);
    void input(double val, Synapse *synapse);
    void initOldOut(void);
    void setMaxSum(void);
    void setWeightStabMode(int w_stabmode);
    void setWeightStabVal(double w_stabval);
    void setInitWeights(double w_initval);
    QList<RecMonConnection *> getRecMonConnections(void);
    virtual void setPositiveWeights(bool yes);
    void write_constructor(QDomElement& e);
    void initSubObjects(const QDomElement& e);

    void setLearningLink(LearningLink* syn_link);

    bool setParams(const QDomElement& e);
    void addSubObject(Resonator* res);

    ISOInputSynapse* getUSInput(void) { return USInput ; }
    void setUSInput(ISOInputSynapse* us_input) { USInput = us_input ; }
    ISOInputSynapse* getCSInput(void) { return CSInput ; }
    void setCSInput(ISOInputSynapse* cs_input) { CSInput = cs_input ; }
    void setLearningDecay(double decay);
    virtual void setSamplingPeriod(double s_period);

public slots:
    void emitToMonitor(void);
    void emitToRecorder(void);
    void num_idChange(ulong prev_n_i);

protected:
    void write_subobjects(QDomDocument& doc, QDomElement& e);
    void connectResonators(void);
    void defaultValues(bool to_base = true);
    virtual double calculateDerivative(double const& old_value);
    QewExtensibleDialog* createDialog(bool mode, bool added);
    QAbstractGraphicsShapeItem* createCanvasItem(QGraphicsScene* scene);

protected slots:
    virtual void hasActvView(void);
       
protected:
    QList<Resonator *> resonators;
    ISOInputSynapse  *USInput, *CSInput;
    int cnt;
    double USVal, CSVal;
    int refrac_period;
    double maxSum;
    const int signalOutDiff;
    int init_refrac_period;
    double  old_diff;
    double derivative;
    
};

#endif
