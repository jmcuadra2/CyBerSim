/*                          neuron.h  -  description
                             -------------------
    begin                : Tue Oct 15 2002
    copyright            : (C) 2002 by Jose M. Cuadra Troncoso
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

#ifndef NEURON_H
#define NEURON_H

#include <qewdialogfactory.h>
#include <QPixmap>
#include <QList>

#include "../net/animatedpixmapitem.h"
#include "synapse.h"
#include "../recmon/recmondevice.h"
#include "../neuraldis/abstractfunction.h"
#include "../neuraldis/qcanvasarc.h"

class QPixmap;
class LearningLink;

/**
  *@author Jose M. Cuadra Troncoso
  */

class Neuron : public RecMonDevice
{

    Q_OBJECT

    friend class neuronValuesDialog;

public:
    Neuron(unsigned long n_id, int lay, bool recmon = false, QObject *parent = 0, const char *name = 0);
    Neuron(unsigned long n_id, bool recmon = false, QObject *parent = 0, const char *name = 0);
    Neuron(ulong n_i, int lay, int sub_lay, double th,
           int actF, double initext, bool recmon = false, QObject *parent = 0, const char *name = 0);
    ~Neuron();

    const int& getLayer(void) { return layer ; }
    const int& getSubLayer(void) { return sub_layer ; }
    void setSubLayer(const int& sub_lay) { sub_layer = sub_lay ; }
    const int& getX(void) { return x ; }
    void setX(const int& x_) { x = x_ ; }
    const int& getY(void) { return y ; }
    void setY(const int& y_) { y = y_ ; }
    
    const double& getThreshold(void) { return Threshold ; }
    void setThreshold(const double& thres) { Threshold = thres ; }
    const double& getInitExtern(void) { return InitExtern ; }
    void setInitExtern(const double& init_ext) { InitExtern = init_ext ; }
    QList<Synapse *> & getAxon(void) { return Axon ; }
    void setAxon(QList<Synapse *> & axon) { Axon = axon ; }
    void setActivationFunction(const int& act_func);
    const int& getActivationFunction(void) { return activationFunction ; }
    void setLearningRate(const double& l_rate) { learning_rate = l_rate ; }
    const double& getLearningRate(void) { return learning_rate ; }
    
    void setNeuronPix(QPixmap* neuron_pix);
    void setNeuronSelPix(QPixmap* neuron_sel_pix);
    virtual bool setParams(const QDomElement& e);
    void write_constructor(QDomElement& e);

    virtual bool getSelected(void);
    virtual void setSelected(bool sel);
    
    virtual void input(double val = 0.0, Synapse *synapse = 0);
    virtual double output(void) { return Output ; }
    virtual void setOutput(double out) {  Output = out ; }
    virtual double calculateOutput(void);
    virtual void initOldOut(void);
    
    bool edit(bool mode = true, bool adding = false);
    const QString devName(bool isCap);
    int rtti_dev(void) const;
    virtual int width(void) const;
    virtual int height(void) const;
    
    virtual void setWeightStabMode(int w_stabmode);
    virtual void setWeightStabVal(double w_stabval);
    virtual void setInitWeights(double w_initval);
    void write(QDomDocument& doc, QDomElement& e);
    virtual void initSubObjects(const QDomElement& e);

    virtual void draw(QGraphicsView* view, bool sim_view = false, QColor* color = 0);

    virtual void setLearningLink(LearningLink* ) {}
    QList<RecMonConnection *> getRecMonConnections(void);
    
    int connectionOffset(void);
    const QString& displayInfo(void) { return infoWhatsThis  ; }
    
    const double& getSigmoidalA(void) { return SigmoidalA ; }
    const double& getSigmoidalB(void) { return SigmoidalB ; }
    void setSigmoidalA(const double& sigA) { SigmoidalA = sigA; }
    void setSigmoidalB(const double& sigB) { SigmoidalB = sigB; }
    
public slots:

    void num_idChange(ulong prev_n_i);
    void emitToMonitor(void);
    void emitToRecorder(void);

protected slots:
    virtual void hasActvView(void);

protected:
    void write_subobjects(QDomDocument& doc, QDomElement& e);
    void defaultValues(bool to_base = true);
    virtual void acceptEdit(void);
    virtual QewExtensibleDialog* createDialog(bool mode, bool added);
    void initQCanvasItems();
    virtual QAbstractGraphicsShapeItem* createCanvasItem(QGraphicsScene* scene);
    

signals:
    void activateView(Neuron *, bool);
    void activateView(Neuron *, double);
    void changed(void);

public:
    enum Neuron_Class {THRESHOLD_DEVICE, ISO_DEVICE, INPUT_NEURON, ICO_DEVICE,
                       SUB_NET, MOTOR_COMMAND_NEURON};

protected:
    bool selected;
    double old_out;
    QString infoWhatsThis;
    int sub_layer;
    int layer;
    int x;
    int y;
    double Threshold;
    double Sum, input_sum;
    double Output;
    double InitExtern;
    QList<Synapse *> Axon;
    int activationFunction;
    AbstractFunction* activationObject;
    double learning_rate;
    double SigmoidalA, SigmoidalB;
    
    
    QPixmap *neuronPix, *neuronSelPix;
    int widthpix, heightpix;

    QPixmap *qc_pix, *qc_pix_sel;
    QList<QPixmap> listpixmaps;
    QList<QPixmap *> qc_pix_a;

    AnimatedPixmapItem *qc_neuron_pix;
    QGraphicsSimpleTextItem *qc_texto;
    
    QAbstractGraphicsShapeItem *qc_neuron;
    bool prev_sim_view;
    
};

inline void Neuron::emitToMonitor(void)
{

    emit emitMonVal(Output, signalOutput);
    emit emitMonVal(input_sum, signalInput);

}

inline void Neuron::emitToRecorder(void)
{

    emit emitRecVal(Output, signalOutput);
    emit emitRecVal(input_sum, signalInput);

}

inline void Neuron::hasActvView(void)
{

    if(int(old_out*8.999999) != int(Output*8.999999))
        emit activateView(this, Output);

}

inline int Neuron::width(void) const
{

    return widthpix;

}

inline int Neuron::height(void) const
{

    return heightpix;

}

#endif
