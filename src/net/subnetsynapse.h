//
// C++ Interface: subnetsynapse
//
// Description: 
//
//
// Author: Jose M. Cuadra Troncoso <jmcuadra@dia.uned.es>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef SUBNETSYNAPSE_H
#define SUBNETSYNAPSE_H

#include "synapse.h"
#include "subnet.h"
#include "neuron.h"


/**
    @author Jose M. Cuadra Troncoso <jmcuadra@dia.uned.es>
*/


class SubNetSynapse : public Synapse
{
    Q_OBJECT

public:
    SubNetSynapse(int id);
    SubNetSynapse(int id, Neuron *neuronFrom, SubNet *subnetTo, unsigned long neuronToId);
    SubNetSynapse(int id, SubNet *subnetFrom, unsigned long neuronFromId, Neuron *neuronTo);
    SubNetSynapse(int id, SubNet *subnetFrom, unsigned long neuronFromId,
                  SubNet *subnetTo, unsigned long neuronToId);

    SubNetSynapse(int id, ulong id_subnetFrom, ulong neuronFromId,
                  ulong id_subnetTo, ulong neuronToId);

    ~SubNetSynapse();

    void initValues(void);
    void setId(unsigned long id);

    // de momento se redefinen estos metodos
    virtual bool setPointerTo (Neuron* neuron_to);
    virtual Neuron* getPointerTo(void);
    virtual bool setPointerFrom(Neuron* neuron_from);
    virtual Neuron* getPointerFrom(void);

    virtual bool setPointerToSubNet(SubNet* subnet_to);
    virtual SubNet* getPointerToSubNet(void);
    virtual bool setPointerFromSubNet(SubNet* subnet_from);
    virtual SubNet* getPointerFromSubNet(void);

    void set_neuronToId (ulong id);
    void set_neuronFromId (ulong id);

    ulong getIdSubNetFrom(void);
    ulong getIdSubNetTo(void);
    ulong getIdNeuronFrom(void);
    ulong getIdNeuronTo(void);

    virtual void draw(QGraphicsView* view);

    // para escribir en el fichero XML
    void write(QDomDocument& doc, QDomElement& e);
    void write_constructor(QDomElement& e);
    // nuevo método para ver type
    int type(void) const;

private:
    // este identificador es independiente del de synapses 'normales'
    int id;

    // elementos to / from
    Neuron *n_from, *n_to;
    SubNet *sn_from, *sn_to;

    // elementos to / from (id)
    ulong id_neuron_from, id_neuron_to;
    ulong id_subnet_from, id_subnet_to;

    // neuronas dentro de la subnet
    ulong neuronToId, neuronFromId;

};

#endif
