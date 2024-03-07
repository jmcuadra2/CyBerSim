/***************************************************************************
                          neuron.cpp  -  description
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

#include <QMessageBox>
#include <QPainter>
#include <QPixmap>

#include <QList>

#include "neuron.h"
#include "netfactory.h"
#include "../disview/gui/neuronvaluesdialog.h"
#include "../neuraldis/xmloperator.h"
#include "../neuraldis/xmloperationslog.h"

Neuron::Neuron(unsigned long n_id, int lay, bool recmon,
               QObject *parent, const char *name)
    : RecMonDevice(n_id, recmon, parent, name)
{

    defaultValues();
    layer = lay;
    initQCanvasItems();

}


Neuron::Neuron(unsigned long n_id, bool recmon,
               QObject *parent, const char *name)
    : RecMonDevice(n_id, recmon, parent, name)
{

    defaultValues();
    initQCanvasItems();

}

Neuron::Neuron(ulong n_id, int lay, int sub_lay, double th, int actF,
               double initext, bool recmon, QObject *parent, const char *name)
    : RecMonDevice(n_id, recmon, parent, name)
{

    sub_layer = sub_lay;
    x = 0;
    y = 0;
    Threshold = th;
    Sum = 0.000000;
    Output = 0.000000;
    InitExtern = initext;
    activationObject = 0;
    activationFunction = -1;
    setActivationFunction(actF);
    selected = false;
    old_out = -1.0;
    neuronPix = 0;
    neuronSelPix = 0;
    layer = lay;
    learning_rate = 0.0;
    infoWhatsThis = "Hola neurona " + QString::number(num_id);

    initQCanvasItems();

}

Neuron::~Neuron()
{
    delete (qc_neuron_pix);
    delete (qc_texto);
    delete (qc_pix);
    delete (qc_pix_sel);

//    QListIterator<QPixmap*> it(qc_pix_a);
//    while(it.hasNext()){
//        delete it.next();
//    }
//    qc_pix_a.clear();

    if(activationObject)
        delete activationObject;
}

void Neuron::defaultValues(bool) {

    sub_layer = 0;
    x = 0;
    y = 0;
    Threshold = 0.000000;
    Sum = 0.000000;
    input_sum = Sum;
    Output = 0.000000;
    InitExtern = 0.000000;
    activationObject = 0;
    activationFunction = -1;
    setActivationFunction(NDMath::Identity);
    selected = false;
    old_out = -1.0;
    neuronPix = 0;
    neuronSelPix = 0;
    layer = 1;
    learning_rate = 0.0;
    infoWhatsThis = "Hola neurona " + QString::number(num_id) ;
    heightpix = 20;
    widthpix = 20;
    prev_sim_view = true; // para engañar a draw la primera vez

}


bool Neuron::setParams(const QDomElement& e)
{

    bool ok = false;
    bool tmp_ok;
    if(!e.isNull()) {
        sub_layer = e.attribute("sub_layer").toInt(&ok);
        if(!ok) {
            sub_layer = e.attribute("type_input").toInt(&ok);
        }

        layer = e.attribute("layer").toInt(&ok);
        Threshold = e.attribute("Threshold").toDouble(&ok);
        int actF = e.attribute("activationFunction").toInt(&ok);
        setActivationFunction(actF);
        InitExtern = e.attribute("InitExtern").toDouble(&ok);
        is_recmonable = bool(e.attribute("isRecMonable").toInt(&tmp_ok));
        if (actF == 1){
            SigmoidalA=e.attribute("SigmoidalA").toDouble(&ok);
            SigmoidalB=e.attribute("SigmoidalB").toDouble(&ok);
        }
    }
    if(!ok) {
        QMessageBox::warning(nullptr,tr("Neuron") + " " + QString::number(num_id),
                             tr("Error in basic parameters, using defaults"), QMessageBox::Ok);
        defaultValues(false);
    }
    return ok;
}

void Neuron::initQCanvasItems()
{
    qc_neuron_pix =  0;
    qc_texto = 0;
    qc_pix= 0;
    qc_pix_sel = 0;
    qc_neuron = 0;
}

void Neuron::num_idChange(ulong prev_n_i)
{
    num_id = prev_n_i + 1;
    emit num_idChanged(num_id);
}

bool Neuron::getSelected(void)
{
    return selected;
}

void Neuron::setSelected(bool sel)
{
    selected = sel;
}

void Neuron::input(double val, Synapse *synapse)
{

    if(synapse)
        Sum += synapse->input(val);
    else
        Sum += val;

}

void Neuron::setActivationFunction(const int& act_func)
{
    if(activationFunction != act_func) {
        activationFunction = act_func ;
        if(activationObject)
            delete activationObject;
        activationObject = NDMath::factoryFunction(activationFunction);
        if(!activationObject) {
            activationObject = NDMath::factoryFunction(NDMath::Identity);
            activationFunction = NDMath::Identity;
        }
    }

}

double Neuron::calculateOutput(void)
{

    Output = Sum + InitExtern;
    if(Output > Threshold)
        Output = activationObject->value(Output);
    else
        Output = 0.0;
    hasActvView();
    old_out = Output;
    input_sum = Sum;
    Sum = 0.0;
    return Output;

}

void Neuron::initOldOut(void)
{

    old_out = -1.0;

}

bool Neuron::edit(bool mode, bool adding)
{

    bool ret = false;
    QewExtensibleDialog* dialog = createDialog(mode, adding);
    if(dialog) {
        if(dialog->exec() == QewExtensibleDialog::Accepted) {
            acceptEdit();
            ret = true;
        }
        delete dialog;
    }
    return ret;

}

QewExtensibleDialog* Neuron::createDialog(bool mode, bool added)
{

    QewDialogFactory *factory = new QewDialogFactory();
    QewExtensibleDialog* main_dialog = factory->createDialog(QewExtensibleDialog::QewSimpleWidget);
    if(main_dialog->setUpDialog()) {
        neuronValuesDialog *dialog = new neuronValuesDialog(main_dialog);
        dialog->setUpDialog();
        dialog->init((Neuron*) this, mode, added);
        main_dialog->addExtensibleChild(dialog, main_dialog->windowTitle());
    }
    else {
        delete main_dialog;
        main_dialog = 0;
    }
    return main_dialog;
}

void Neuron::acceptEdit(void)
{

    emit changed();

}
int Neuron::rtti_dev(void) const
{

    return (THRESHOLD_DEVICE);

}

void Neuron::setWeightStabMode(int w_stabmode)
{
    QListIterator<Synapse *> it(Axon);
    while(it.hasNext())
        it.next()->setStabWeightMode(w_stabmode);
}

void Neuron::setWeightStabVal(double w_stabval)
{
    QListIterator<Synapse *> it(Axon);
    while(it.hasNext())
        it.next()->setStabWeightVal(w_stabval);
}

void Neuron::setInitWeights(double w_initval)
{
    QListIterator<Synapse *> it(Axon);
    while(it.hasNext())
        it.next()->setWeight(w_initval);
}

QList<RecMonConnection*> Neuron::getRecMonConnections(void)
{    
    QList<RecMonConnection *> ax;
    QListIterator<Synapse *> it(Axon);
    Synapse *synap;
    while(it.hasNext()) {
        synap = it.next();
        ax.append(synap);
    }
    return ax;
}

const QString Neuron::devName(bool isCap)
{
    return isCap ? tr("Neuron") : tr("neuron");   
}

void Neuron::write(QDomDocument& doc, QDomElement& e)

{
    XMLOperator* xml_operator = XMLOperator::instance();

    QDomElement tag_neuron = doc.createElement("Neuron");
    e.appendChild(tag_neuron);
    tag_neuron.appendChild(xml_operator->createTextElement(doc,
                                                           "type", QString::number(rtti_dev())));
    tag_neuron.appendChild(xml_operator->createTextElement(doc,
                                                           "num_id", QString::number(num_id)));
    QDomElement tag_cons = doc.createElement("constructor");
    tag_neuron.appendChild(tag_cons);
    write_constructor(tag_cons);
    QDomElement tag_neuron_sub = doc.createElement("SubObjects");
    tag_neuron.appendChild(tag_neuron_sub);
    write_subobjects(doc, tag_neuron_sub);

}

void Neuron::write_subobjects(QDomDocument& doc, QDomElement& e)
{

    QDomElement tag_ax = doc.createElement("Axon");
    e.appendChild(tag_ax);
    QListIterator<Synapse *> its(Axon);
    Synapse* syn;
    while(its.hasNext()) {
        syn = its.next();
        syn->write(doc, tag_ax);
    }
}

void Neuron::write_constructor(QDomElement& e)
{

    e.setAttribute("sub_layer", QString::number(sub_layer));
    e.setAttribute("layer", QString::number(layer));
    e.setAttribute("Threshold", QString::number(Threshold));
    e.setAttribute("InitExtern", QString::number(InitExtern));
    e.setAttribute("activationFunction",
                   QString::number(activationFunction));
    e.setAttribute("isRecMonable",
                   QString::number(isRecMonable()));
    if(activationFunction==1){
        e.setAttribute("SigmoidalA",QString::number(SigmoidalA));
        e.setAttribute("SigmoidalB",QString::number(SigmoidalB));
    }

}

void Neuron::initSubObjects(const QDomElement& e)
{

    NetFactory* factory = NetFactory::instance();
    XMLOperator* xml_operator = XMLOperator::instance();
    QDomElement e_ax = xml_operator->findTag("Axon", e);
    if(e_ax.isNull()) return;
    factory->buildAxon(Axon, e_ax);
    QListIterator<Synapse *> it(Axon);
    Synapse* syn;
    while(it.hasNext()) {
        syn = it.next();
        syn->setPointerFrom(this);
    }


}

int Neuron::connectionOffset(void)
{
    int id = 0;
    if(Axon.count())
        id = Axon.at(0)->getNumId();
    return id;
}

void Neuron::setNeuronPix(QPixmap* neuron_pix)
{

    neuronPix = neuron_pix;
    widthpix = neuronPix->width();
    heightpix = neuronPix->height();

}
void Neuron::setNeuronSelPix(QPixmap* neuron_sel_pix)
{   
    neuronSelPix = neuron_sel_pix;
}

void Neuron::draw(QGraphicsView* view, bool sim_view, QColor* color)
{
    QFont font("Arial", 10);
    bool can_draw_pixmaps = neuronPix ? true : false;

    if(sim_view) {
        if(!color)
            return;
        if(!prev_sim_view) {
            if (qc_neuron_pix && qc_neuron_pix->isVisible()){
                qc_neuron_pix->hide();
                qc_texto->hide();
            }
            if(!qc_neuron) {
                qc_neuron = createCanvasItem(view->scene());
//                qc_neuron->moveBy(x + width()/2.0, y + height()/2.0);
                qc_neuron->moveBy(x, y);
                qc_neuron->setZValue(10);
            }
            qc_neuron->show();
            prev_sim_view = sim_view; //true
        }

        qc_neuron->setBrush (QBrush(*color));
        view->update();
    }
    else {
        QPoint qpt (5,5);
        // elegir el pixmap en funcion de si esta seleccionada
        if (can_draw_pixmaps){
            if(prev_sim_view) {
                if(qc_neuron && qc_neuron->isVisible())
                    qc_neuron->hide();
                if(qc_pix_a.empty()) {
                    qc_pix = new QPixmap ( *(neuronPix));
                    listpixmaps.append(*qc_pix);
                    qc_pix_sel = new QPixmap ( *(neuronSelPix));
                    listpixmaps.append(*qc_pix_sel);
                    int listpixmaps_size = listpixmaps.size();
                    for(int i = 0; i < listpixmaps_size; i++){
                        qc_pix_a.append(&listpixmaps[i]);
                    }
                }
                if(!qc_neuron_pix) {
                    qc_neuron_pix = new AnimatedPixmapItem (qc_pix_a);
                    view->scene()->addItem(qc_neuron_pix);
                    qc_neuron_pix->setZValue(10);   // para que aparezcan por encima de las synapses
                }

                qc_neuron_pix->show();

                if (!qc_texto) {
                    qc_texto = new QGraphicsSimpleTextItem(QString::number(num_id));
                    view->scene()->addItem(qc_texto);
                    //qc_texto->setColor(*color);
                    qc_texto->setPen(QPen(QColor("black")));
                    qc_texto->setFont (font);

                }

                qc_texto->setZValue(30);
                qc_texto->show();
                prev_sim_view = sim_view; //false
            }

            qc_neuron_pix->setPos(qreal(x),qreal(y));
            qc_texto->setText(QString::number(num_id));

            if (num_id < 10)
                qc_texto->setPos(x + width()/2 - 3,y + height()/2 - 7 );
            else
                qc_texto->setPos(x + width()/2 - 8,y + height()/2 - 7);

            if(getSelected())
                qc_neuron_pix->setFrame(1);
            else
                qc_neuron_pix->setFrame(0);
        }
    }

}

QAbstractGraphicsShapeItem* Neuron::createCanvasItem(QGraphicsScene* scene)
{
    QGraphicsEllipseItem* item = new QGraphicsEllipseItem (0,0, width(), height());
    item->setStartAngle(16*1);
    item->setSpanAngle(16*360);
    scene->addItem(item);
    return (QAbstractGraphicsShapeItem*) item;
}
