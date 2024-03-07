//
// C++ Implementation: subnetsynapse
//
// Description: 
//
//
// Author: Jose M. Cuadra Troncoso <jmcuadra@dia.uned.es>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "subnetsynapse.h"
#include "../neuraldis/xmloperator.h"

#include <QPolygon>

SubNetSynapse::SubNetSynapse(int id) :Synapse(1)
{
    initValues();
    this->id = id;
}

SubNetSynapse::SubNetSynapse(int id, Neuron *neuronFrom, SubNet *subnetTo,
                             unsigned long neuronToId) : Synapse(1)
{
    // desde neurona hacia subred
    initValues();
    this->id = id;
    n_from = neuronFrom;
    sn_to =subnetTo;
    this->neuronToId = neuronToId;
}

SubNetSynapse::SubNetSynapse(int id, SubNet *subnetFrom, unsigned long neuronFromId,
                             Neuron *neuronTo) : Synapse(1)
{
    initValues();
    this->id = id;
    sn_from = subnetFrom;
    this->neuronFromId = neuronFromId;
    n_to = neuronTo;
}

SubNetSynapse::SubNetSynapse(int id, SubNet *subnetFrom, unsigned long neuronFromId,
                             SubNet *subnetTo, unsigned long neuronToId)
    :Synapse(1)
{
    initValues();
    this->id = id;
    sn_from = subnetFrom;
    this->neuronFromId = neuronFromId;
    sn_to =subnetTo;
    this->neuronToId = neuronToId;
}

SubNetSynapse::SubNetSynapse(int id, ulong id_subnetFrom, ulong neuronFromId,
                             ulong id_subnetTo, ulong neuronToId) :Synapse(1)
{
    initValues();

    this->id = id;

    id_subnet_from = id_subnetFrom;
    id_neuron_from = neuronFromId;

    id_subnet_to = id_subnetTo;
    id_neuron_to = neuronToId;

}

void SubNetSynapse::initValues(void)
{

    n_from = 0;
    n_to = 0;
    sn_from = 0;
    sn_to = 0;

    id_neuron_from = 0;
    id_neuron_to = 0;
    id_subnet_from = 0;
    id_subnet_to = 0;

    neuronToId = 0;
    neuronFromId = 0;

}

SubNetSynapse::~SubNetSynapse()
{
}

void SubNetSynapse::setId(unsigned long id)
{
    this->id = id;
}

bool SubNetSynapse::setPointerTo(Neuron* neuron_to)
{
    n_to = neuron_to;
    return true;
}

Neuron* SubNetSynapse::getPointerTo(void)
{
    return n_to;
}

bool SubNetSynapse::setPointerFrom(Neuron* neuron_from)
{
    n_from = neuron_from;
    return true;
}

Neuron* SubNetSynapse::getPointerFrom(void)
{
    return n_from;
}

bool SubNetSynapse::setPointerToSubNet(SubNet* subnet_to)
{
    sn_to = subnet_to;
    return true;
}

SubNet* SubNetSynapse::getPointerToSubNet(void)
{
    return sn_to;
}

bool SubNetSynapse::setPointerFromSubNet(SubNet* subnet_from)
{
    sn_from = subnet_from;
    return true;
}

SubNet* SubNetSynapse::getPointerFromSubNet(void)
{
    return sn_from;
}

void SubNetSynapse::draw(QGraphicsView* view)
{

    Neuron *neuron_from, *neuron_to;
    SubNet *subnet_from, *subnet_to;
    double ang = 0.0;
    int xcenter_from = 0, ycenter_from = 0, xcenter_to = 0, ycenter_to = 0;
    QPolygon triang;
    triang.setPoints(3, 3, 0, -3, 3, -3, -3);

    neuron_from = n_from;
    neuron_to = n_to;
    subnet_from = sn_from;
    subnet_to = sn_to;

    // 1.- averiguar from
    if (neuron_from) {
        xcenter_from = int(neuron_from->getX()) + (neuron_from->width() + 1)/2;
        ycenter_from = int(neuron_from->getY()) + (neuron_from->height() + 1)/2;
    }
    else if (subnet_from) {
        xcenter_from = int(subnet_from->getX()) + (subnet_from->getWidth() + 1)/2;
        ycenter_from = int(subnet_from->getY()) + (subnet_from->getHeight() + 1)/2;
    }
    else  {
        //algo no esta bien
        return;
    }

    // 2.- averiguar to
    if (neuron_to) {
        xcenter_to = int(neuron_to->getX()) + (neuron_to->width() + 1)/2;
        ycenter_to = int(neuron_to->getY()) + (neuron_to->height() + 1)/2;
    }
    else if (subnet_to) {
        xcenter_to = int(subnet_to->getX()) + (subnet_to->getWidth() + 1)/2;
        ycenter_to = int(subnet_to->getY()) + (subnet_to->getHeight() + 1)/2;
    }
    else {
        // algo no esta bien
        return;
    }

    QPen pen(Qt::black, 1);

    QBrush brush(Qt::black);

    if (!line) {
        line = new QGraphicsLineItem ( );
        view->scene()->addItem(line);
    }

    line->setPen(pen);
    line->setZValue(1);
    line->setLine (QLineF(xcenter_from, ycenter_from, xcenter_to, ycenter_to));
    line->show();

    // flecha
    if(xcenter_to == xcenter_from) {
        if(ycenter_to >= ycenter_from)
            ang = 90.0;
        else
            ang = -90.0;
    } else {
        if(xcenter_to >= xcenter_from)
            ang = atan(double(ycenter_to - ycenter_from)/
                       double(xcenter_to - xcenter_from))* 180/3.1415926;
        else
            ang = 180.0 + atan(double(ycenter_to - ycenter_from)/
                               double(xcenter_to - xcenter_from))* 180/3.1415926;
    }

    double rad = M_PI / 180.0;
    double size = 8.0;

    double x0 = double(xcenter_from + xcenter_to)/2;
    double y0 = double(ycenter_from + ycenter_to )/2;

    double ang2 = ang-150.0;
    double x2= size * cos(ang2*rad);
    double y2= size * sin(ang2*rad);

    double ang3 = ang+150.0;
    double x3= size * cos(ang3*rad);
    double y3= size * sin(ang3*rad);

    if (!arrow) {
        arrow = new QPolygon (3);
    }

    arrow->setPoint( 0, QPoint( 0 , 0 ));
    arrow->setPoint( 1, QPoint( (int)x2, (int)y2 ));
    arrow->setPoint( 2, QPoint( (int)x3, (int)y3 ));

    if (!flecha) {
        flecha = new QGraphicsPolygonItem();
        view->scene()->addItem(flecha);
    }

    flecha->setPolygon(*arrow);
    flecha->setPos(x0,y0);
    flecha->setZValue(20);

    flecha->setBrush(brush);
    flecha->setPen(pen);
    flecha->show();

}

// nuevo método para escribir las sinapsis que conectan subredes y subredes con neuronas
// tengo que redefinir estos métodos ya que serán diferentes al de sinapsis

void SubNetSynapse::write (QDomDocument& doc, QDomElement& e)
{

    XMLOperator* xml_operator = XMLOperator::instance();

    QDomElement tag_syn = doc.createElement("SubNetSynapse");
    e.appendChild(tag_syn);

    tag_syn.appendChild(xml_operator->createTextElement(doc,
                                                        "type", QString::number(type())));

    tag_syn.appendChild(xml_operator->createTextElement(doc,
                                                        "num_id", QString::number(id)));
    // aqui incluyo los elementos de from de subred y neurona a to subred y neurona
    // from y to de las neuronas
    if (sn_from == 0){
        tag_syn.appendChild(xml_operator->createTextElement(doc,
                                                            "from", QString::number(n_from->getNumId())));
    }
    else{
        tag_syn.appendChild(xml_operator->createTextElement(doc,
                                                            "from", QString::number(sn_from->getNumId())+":" +QString::number(neuronFromId)));
    }

    if (sn_to == 0){
        tag_syn.appendChild(xml_operator->createTextElement(doc,
                                                            "to", QString::number(n_to->getNumId( ))));
    }
    else{
        tag_syn.appendChild(xml_operator->createTextElement(doc,
                                                            "to", QString::number(sn_to->getNumId())+":"+ QString::number(neuronToId)));
    }
    // aqui incluyo los elementos de from de subred y neurona a to subred y neurona

    QDomElement tag_cons = doc.createElement("constructor");
    tag_syn.appendChild(tag_cons);

    write_constructor(tag_cons);

}  

void SubNetSynapse::write_constructor(QDomElement& e)
{

    e.setAttribute("color", QString::number(IdColor));
    e.setAttribute("weight", QString::number(weight ));

}

int SubNetSynapse::type(void) const
{
    return Synapse::SUBNET;
}

ulong SubNetSynapse::getIdSubNetFrom(){
    return id_subnet_from;
}

ulong SubNetSynapse::getIdSubNetTo(){
    return id_subnet_to;
}

ulong SubNetSynapse::getIdNeuronFrom(){
    return id_neuron_from;
}

ulong SubNetSynapse::getIdNeuronTo(){
    return id_neuron_to;
}

void SubNetSynapse::set_neuronToId (ulong id)
{
    neuronToId = id;
}

void SubNetSynapse::set_neuronFromId (ulong id)
{
    neuronFromId = id;
}
