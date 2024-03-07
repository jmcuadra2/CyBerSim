/***************************************************************************
                          synapse.h  -  description
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

#ifndef SYNAPSE_H
#define SYNAPSE_H

#include <qewdialogfactory.h>

#include "neuralconnection.h"

#include <QList>
#include <QVector>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QGraphicsItem>
#include <QPolygon>

#include "../neuraldis/qcanvasarc.h"

class Neuron;
// añado la clase subnet para poder conectar subredes con neuronas
class SubNet;

/**
  *@author Jose M. Cuadra Troncoso
  */

class Synapse : public NeuralConnection
{

    Q_OBJECT

    friend class synapseValuesDialog;

public:
    Synapse(unsigned long n_i,  bool recmon = false,
            QObject *parent = 0, const char *name = 0);
    Synapse(unsigned long n_i, uchar n_color, double w, ulong To_,
            bool recmon = false, QObject *parent = 0,
            const char *name = 0);
    ~Synapse();

    bool setParams(const QDomElement& e);
    void write(QDomDocument& doc, QDomElement& e);
    void write_constructor(QDomElement& e);

    virtual void draw(QGraphicsView* view, QStringList colors);

    virtual bool setPointerTo(Neuron* neuron_to);
    virtual Neuron* getPointerTo(void);
    virtual bool setPointerFrom(Neuron* neuron_from);
    virtual Neuron* getPointerFrom(void);

    virtual void nonEditableWeights(QListIterator<Synapse *> it);
    virtual QVector<uint> getNonEditableWeights(void) { return noeditw ; }
    virtual void setIdColor(uchar id_color) { IdColor = id_color ; }
    virtual uchar getIdColor(void) { return IdColor ; }
    virtual ulong getTo(void) { return To ; }
    virtual double input(double inp);
    bool edit(bool mode = true, bool added = false);
    const QString connName(bool isCap);
    int type(void) const;
    int superType(void) const;

public slots:
    virtual void setTo(ulong to);

protected:
    void defaultValues(bool to_base = true);
    void acceptEdit(void);
    virtual QewExtensibleDialog* createDialog(bool mode, bool added);

signals:
    void weightsDefChange(int , double );

public:
    enum Syn_Class {FIXED = 0, ISO_INPUT = 1,
                    LEARNING_LINK = 3, LEARNER = 4, HEBBIAN = 5, SUBNET = 7};
    // nuevo tipo de sinapsis para subredes la denomino SUBNET
    static QVector<double> weightsMask;
    static QStringList synapse_colors;

protected:
    QVector<uint> noeditw;
    double Input;
    uchar IdColor;
    Neuron *ptrTo;
    Neuron *ptrFrom;
    unsigned long To;

    QGraphicsLineItem *line;
    QGraphicsPolygonItem *flecha;
    QPolygon *arrow;
    QCanvasArc *syn_arc;

};

inline bool Synapse::setPointerFrom(Neuron* neuron_from)
{

    ptrFrom = neuron_from;
    return true;

}

inline Neuron* Synapse::getPointerFrom(void)
{  
    return ptrFrom;
}

inline double Synapse::input(double inp)
{   
    Input = inp;
    return inp*weight;
}

#endif
