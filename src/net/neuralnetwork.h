/***************************************************************************
        neuralnetwork.h  -  description
           -------------------
    begin    : Tue Oct 15 2002
    copyright    : (C) 2002 by Jose M. Cuadra Troncoso
    email    : jmcuadra@dia.uned.es
 ***************************************************************************/

/***************************************************************************
 *                     *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.           *
 *                     *
 ***************************************************************************/

#ifndef NEURALNETWORK_H
#define NEURALNETWORK_H

#include <iostream>
#include <fstream>

#include "neuron.h"
#include "synapse.h"
// nuevo include 
#include "subnet.h"
#include "../neuraldis/neuraldisdoc.h"

#include <QPointer>
#include <QObject>
#include <QStringList>
#include <QPixmap>
#include <QDomDocument>
#include <QClipboard>
#include <QList>
#include <QVector>

#include "../disview/gui/addneurondialog.h"
#include "../disview/gui/delneurondialog.h"
#include "../disview/gui/editneurondialog.h"
#include "../disview/gui/addsynapsedialog.h"
#include "../disview/gui/addsynapsenetdialog.h"
#include "../disview/gui/selectsynapsedialog.h"
#include "../disview/gui/delnetworkdialog.h"
#include "../disview/gui/addnetworkdialog.h"
#include "../disview/gui/editnetworkdialog.h"
#include "../disview/gui/selectsynapsenetdialog.h"
#include "../disview/neuronviewfactory.h"
#include "../disview/subnetviewfactory.h"

class Synapse;
class NetFactory;
class XMLOperationsLog;

/**
  *@author Jose M. Cuadra Troncoso
  */

class NeuralNetwork : public QObject
{

    Q_OBJECT

    friend class NetBuilder;
    friend class NeuralDisView;

public:
    NeuralNetwork(void);
    ~NeuralNetwork();
    QString name(void);
    QList<Neuron *>& neurons(void);
    // nuevo método para obtener la lista de subredes
    QList<SubNet *>& subnets(void);
    bool notEmpty(void) { return bool(NumberOfNeurons) ; }
    QList<Neuron *> getInputLayer(void) { return Input_Neuron ; }
    QList<Neuron *> getHiddenLayer(void) { return HiddenNeuron ; }
    QList<Neuron *> getOutputLayer(void) { return OutputNeuron ; }
    void clearNet(bool save_);
    QString saveNet(void);
    
    void addNeuron(void);
    Neuron* addNeuron(ulong n_i, int layer, int type = Neuron::THRESHOLD_DEVICE,
                      bool edit = true, bool write_undo = true);
    void editNeuron(void);
    void delNeuron(void);
    void delNeuron(ulong n_i);
    void addSynapse(void);
    
    void addSynapse(ulong n_from, ulong n_to, int type = Synapse::FIXED,
                    bool edit = true, bool write_undo = true);
    
    //método addsynapse para subredes
    void addSynapse(ulong n_fromsubnet,ulong n_tosubnet, ulong n_from,
                    ulong n_to, int type = Synapse::FIXED);

    void delSynapse(void);
    void delSynapse(ulong n_s);

    void editSynapse(void);

    /* métodos usados en undo y redo */
    bool loadSynapse(const QDomElement& elem);
    bool deleteSynapse(const QDomElement& elem);
    bool editSynapse(const QDomElement& elem);
    bool loadNeuron(const QDomElement& elem);
    bool deleteNeuron(const QDomElement& elem);
    bool editNeuron(const QDomElement& elem);
    bool loadSubNet(const QDomElement& elem);
    bool deleteSubNet(const QDomElement& elem);
    bool editSubNet(const QDomElement& elem);
    
    Synapse* findSynapse(ulong n_from, ulong n_to);
    /* nuevos métodos para añadir, editar y borrar subredes */

    void addNetwork(const QString& subnet_name);
    SubNet* addNetwork(ulong id_network, const QString& subnet_name, bool edit=false );
    /* cambio QString parámetro devuelto antes void*/
    QString editNetwork(void);
    void delNetwork(void);
    void delNetwork(ulong id_network);

    /* funciones para undo y redo */
    bool undo();
    bool redo();


    /* funciones para copy/paste */
    bool cut();
    bool copy();
    bool paste();


    int getLayer(void);
    void setLayer(int l);
    int getLayerTo(void);
    void setLayerTo(int l);
    Neuron* neuronPointer(ulong n_neuron);
    // necesito puntero para subredes nuevo
    SubNet* subNetPointer(ulong n_subnet);
    // fin ultimos cambios

    void setSynapseColors(QStringList syn_col);

public slots:
    void setName(QString n);
    void isChanged(void);

protected:
    void nonEditableWeights(void);
    void clearSelection(void);
    void linkNumId(void);
    void setNetDoc(QDomDocument& doc) { netDoc = doc ; }

signals:
    void netChanged(QString texto) ;
    void netChanged(void);
    void weightsDefChange(int id_color, double weight);
    void weightsDefChangeAll(void);
    void neuronAddSim(Neuron *);
    void netClosed(void);

public:
    enum Layers {LAYER_INPUT, LAYER_HIDDEN, LAYER_OUTPUT};

protected:
    QList<Neuron *> Neurons;
    QList<Neuron *> Input_Neuron;
    QList<Neuron *> HiddenNeuron;
    QList<Neuron *> OutputNeuron;
    unsigned long NumberOfSynapses;
    unsigned long NumberOfNeurons;
    unsigned long num_InputN;
    unsigned long num_HiddenN;
    unsigned long num_OutputN;
    QList<Synapse *> Axons;
    QString _name;
    int selectedLayer;
    int selectedLayerTo;
    NetFactory* factory;
    Neuron *fromSelNeuron, *toSelNeuron;
    Neuron *currentNeuron;
    // añado esto aquí para seleccionar la subred borrar si no se necesita
    SubNet *fromSelSubNet, *toSelSubNet;
    
    NeuronViewFactory* neuronViewFactory;
    SubNetViewFactory* subnetViewFactory;
    // tengo que añadir la factoría para las subredes
    bool can_draw_pixmaps;
    
    Synapse *currentSynapse;
    QVector<double> weightsMask;
    QDomDocument netDoc;

    XMLOperationsLog * undoLog;
    
    // lista de subredes
    unsigned long NumberOfSubNets;
    QList<SubNet *> SubNets;
    SubNet *currentSubNet;
    // sinapsis de subredes
    unsigned long NumberOfSubNetSynapses;
    
};
#endif
