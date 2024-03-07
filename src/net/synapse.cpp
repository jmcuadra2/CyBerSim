/***************************************************************************
                          synapse.cpp  -  description
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

#include "synapse.h"
#include "neuron.h"
#include "subnet.h"
#include "../neuraldis/xmloperator.h"
#include "../neuraldis/ndmath.h"
#include "../disview/gui/synapsevaluesdialog.h"
#include "../neuraldis/qcanvasarrow.h"
#include "../neuraldis/xmloperationslog.h"

#include <QMessageBox>
#include <QPen>
#include <QPainter>
#include <QList>
#include <QVector>

QVector<double> Synapse::weightsMask = QVector<double>();

QStringList Synapse::synapse_colors = QStringList("");	

Synapse::Synapse(unsigned long n_i, bool recmon,
                 QObject *parent, const char *name)
    : NeuralConnection(n_i, recmon, parent, name)
{

    defaultValues();

    // inicializacion elementos graficos
    line = 0;
    flecha = 0;
    arrow = 0;
    syn_arc = 0;

}

Synapse::Synapse(unsigned long n_i, uchar n_color, double w, ulong To_,
                 bool recmon, QObject *parent, const char *name)
    : NeuralConnection(n_i, recmon, parent, name)
{

    IdColor = n_color;
    weight = w;
    To = To_;
    ptrTo = 0;
    ptrFrom = 0;
    Input = 0.0;

    // inicializacion elementos graficos
    line = 0;
    flecha = 0;
    arrow = 0;
    syn_arc = 0;

}

Synapse::~Synapse()
{

    if(line)
        delete (line);
    if(flecha)
        delete (flecha);
    if(arrow)
        delete (arrow);
    if(syn_arc)
        delete (syn_arc);

}

void Synapse::defaultValues(bool to_base)
{

    if(to_base) NeuralConnection::defaultValues();
    IdColor = 0;
    weight = 1.000000;
    To = 0;
    ptrTo = 0;
    ptrFrom = 0;
    Input = 0.0;

}

bool Synapse::setParams(const QDomElement& e)
{

    bool ok = false;
    bool tmp_ok = false;
    if(!e.isNull()) {
        IdColor = e.attribute("color").toUShort(&ok);
        weight = e.attribute("weight").toDouble(&ok);
        To = e.attribute("To").toULong(&ok);
        is_recmonable = bool(e.attribute("isRecMonable").toInt(&tmp_ok));
    }
    if(!ok) {
        QMessageBox::warning(nullptr,tr("Synapse") + " " + QString::number(num_id),
                             tr("Error in basic parameters, using defaults"), QMessageBox::Ok);
        defaultValues(false);
    }
    return ok;

}

void Synapse::setTo(ulong to)
{
    To = to;
}

bool Synapse::edit(bool mode, bool added)
{

    bool ret = false;

    QewExtensibleDialog* dialog = createDialog(mode, added);
    if(dialog) {
        if(dialog->exec() == QewExtensibleDialog::Accepted) {
            acceptEdit();
            ret = true;
        }
        delete dialog;
    }
    return ret;

}

QewExtensibleDialog* Synapse::createDialog(bool mode, bool added)
{

    QewDialogFactory *factory = new QewDialogFactory();
    QewExtensibleDialog* main_dialog = factory->createDialog(QewExtensibleDialog::QewSimpleWidget);
    if(main_dialog->setUpDialog()) {
        synapseValuesDialog *dialog = new synapseValuesDialog(main_dialog);
        dialog->setUpDialog();
        dialog->init((Synapse*) this, mode, added);
        main_dialog->addExtensibleChild(dialog, "");
    }
    else {
        delete main_dialog;
        main_dialog = 0;
    }
    return main_dialog;
}

void Synapse::acceptEdit(void)
{

    if(weightsMask[IdColor] != weight) {
        weightsMask[IdColor] = weight;
        emit weightsDefChange(IdColor, weight);
    }
    emit changed();

}

void Synapse::nonEditableWeights(QListIterator<Synapse *> it)
{

    int n_weights = Synapse::weightsMask.count();
    int tam;
    Synapse *synapse;

    while(it.hasNext()) {
        synapse = it.next();
        if(synapse != this) {
            tam = noeditw.size();
            if(tam > 0) {
                if(noeditw.contains(synapse->IdColor)) {
                    noeditw.resize(tam + 1);
                    noeditw[tam] = synapse->IdColor;
                }
            }
            else {
                noeditw.resize(1);
                noeditw[0] = synapse->IdColor;
            }
            if(noeditw.size() == n_weights)
                break;
        }
    }

}

const QString Synapse::connName(bool isCap)
{

    return isCap ? tr("Synapse") : tr("synapse");

}

void Synapse::write(QDomDocument& doc, QDomElement& e)
{

    XMLOperator* xml_operator = XMLOperator::instance();

    QDomElement tag_syn = doc.createElement("Synapse");
    e.appendChild(tag_syn);
    
    tag_syn.appendChild(xml_operator->createTextElement(doc,
                                                        "type", QString::number(type())));
    tag_syn.appendChild(xml_operator->createTextElement(doc,
                                                        "num_id", QString::number(num_id)));

    QDomElement tag_cons = doc.createElement("constructor");
    tag_syn.appendChild(tag_cons);

    write_constructor(tag_cons);

}

void Synapse::write_constructor(QDomElement& e)
{

    e.setAttribute("color", QString::number(IdColor));
    e.setAttribute("weight", QString::number(weight ));
    e.setAttribute("To", QString::number(To));
    e.setAttribute("isRecMonable",QString::number(isRecMonable()));

    if (ptrFrom){
        e.setAttribute("From", QString::number(ptrFrom->getNumId()));
    }

}


void Synapse::draw(QGraphicsView* view, QStringList colors)
{

    Neuron *neuron_from, *neuron_to;
    // nuevo
    double ang = 0.0;
    int xcenter_from = 0, ycenter_from = 0, xcenter_to = 0, ycenter_to = 0;
    QPolygon triang;
    triang.setPoints(3, 3, 0, -3, 3, -3, -3);

    neuron_from = ptrFrom;
    neuron_to = ptrTo;

    if(!neuron_from || !neuron_to)
        return;

    xcenter_from = int(neuron_from->getX()) + (neuron_from->width() + 1)/2;
    ycenter_from = int(neuron_from->getY()) + (neuron_from->height() + 1)/2;
    xcenter_to = int(neuron_to->getX()) + (neuron_to->width() + 1)/2;
    ycenter_to = int(neuron_to->getY()) + (neuron_to->height() + 1)/2;

    QBrush brush(QColor(colors[6 + IdColor]));
    QPen pen(brush, 2);

    if(neuron_from == neuron_to) {

        // se dibuja un pequeño arco con origen y destino en la propia neurona
        if (!syn_arc) {
            syn_arc = new QCanvasArc ((float)neuron_from->getX(), (float)neuron_from->getY() - neuron_from->height(),
                                      (float)neuron_from->width(), (float)neuron_from->height() * 1.5, 0, 16*360) ;
            view->scene()->addItem(syn_arc);
        }
        syn_arc->setPen(pen);
        syn_arc->setBrush(brush);
//        syn_arc->setPos(neuron_from->getX(),neuron_from->getY() - neuron_from->height());
        syn_arc->show();

    }
    else {
        if (!line) {
            line = new QGraphicsLineItem ();
            view->scene()->addItem(line);
        }

        line->setPen(pen);
        line->setZValue(1);
        line->setLine (QLineF(xcenter_from, ycenter_from, xcenter_to, ycenter_to));
        line->show();
    }

    // flecha
    if(xcenter_to == xcenter_from) {
        if(ycenter_to > ycenter_from)
            ang = 90.0;
        else if(ycenter_to < ycenter_from)
            ang = -90.0;
        else
            ang = 0.0;
    } else {
        if(xcenter_to >= xcenter_from)
            ang = atan(double(ycenter_to - ycenter_from)/
                       double(xcenter_to - xcenter_from))* 180/3.1415926;
        else
            ang = 180.0 + atan(double(ycenter_to - ycenter_from)/
                               double(xcenter_to - xcenter_from))* 180/3.1415926;
    }

    double rad = M_PI / 180.0;
    double size = 12.0;

    double x0;
    double y0;
    if(xcenter_to == xcenter_from && ycenter_to == ycenter_from) {
        x0 = xcenter_from + neuron_from->width()/2.0 - size/3.0;
        y0 = ycenter_from - 1.5*neuron_from->height();
    }
    else {
        x0 = double(xcenter_from + xcenter_to)/2;
        y0 = double(ycenter_from + ycenter_to)/2;
    }

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

bool Synapse::setPointerTo(Neuron* neuron_to)
{

    ptrTo = neuron_to;
    To = ptrTo->getNumId();
    return true;

}

Neuron* Synapse::getPointerTo(void)
{
    return ptrTo;
}

int Synapse::type(void) const
{
    return Synapse::FIXED;
}

int Synapse::superType(void) const
{
    return NeuralConnection::SYNAPSE ;
}    
