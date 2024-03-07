/***************************************************************************
                          neuraldisview.cpp  -  description
                             -------------------
    begin                : mar oct 15 04:16:08 CEST 2002
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

#include <QColor>
#include <QPoint>
#include <QApplication>
#include <QMessageBox>
#include <QFile>
#include <QDir>
#include <QPixmap>
#include <QMouseEvent>
#include <QPaintEvent>
#include <QWheelEvent>
#include <QCloseEvent>
#include <QTextStream>
#include <QList>
#include <QPainterPath>
#include <QWhatsThis> 
#include <QTransform>
#include "../net/animatedpixmapitem.h"

#include "neuraldisview.h"
#include "leyend.h"
#include "neuronviewfactory.h"
#include "synapseviewfactory.h"
// nuevo include
#include "subnetviewfactory.h"
#include "../imagenes/neuraldis32.xpm"
#include "gui/textviewer.h"
#include "../neuraldis/dictionarymanager.h"
#include "../neuraldis/xmloperationslog.h"

NeuralDisView::NeuralDisView(QGraphicsScene *n_canvas, QWidget *parent, NeuralNetwork *net_)
    : QGraphicsView(n_canvas, parent), MdiSubWinInterface() //, 0,
{

    setAttribute(Qt::WA_DeleteOnClose);
    textViewer = 0; // no quitar
    leyend = 0; // no quitar
    can_paint = false;
    zoom = 1.0;
//    disIcon = QPixmap(neuraldis32_xpm);
//    setIcon(disIcon);
    setWindowTitle(tr("Designer"));
    setMinimumSize(300, 200);
    move(50, 50);
    setAlignment(Qt::AlignLeft | Qt::AlignTop);
    setResizeAnchor(QGraphicsView::NoAnchor);
    resize(PAINTERWIDTH, PAINTERHEIGHT);

    Settings* prog_settings = Settings::instance();
    QString app_dir = prog_settings->getAppDirectory();
    QString file = app_dir + "src/disview/imagenes/lateral.xpm";

    app_dir.replace("%1", "*");

    lateralPix << new QPixmap(file);
    qc_lateralPix = new AnimatedPixmapItem(lateralPix);
    scene()->addItem(qc_lateralPix);

    qc_lateralPix->setZValue(1);
    qc_lateralPix->setPos(20,25);
    qc_lateralPix->show();

    scene()->setBackgroundBrush(QBrush(QColor(230, 230, 230)));

//    QWidget *widgetMain;
//    widgetMain = qApp->mainWidget();

    neuronViewFactory = NeuronViewFactory::instance();
    if (!neuronViewFactory->readFileConf())
        QMessageBox::warning(nullptr, tr("DisView"),
                             QString(tr("No info on neuron pixmaps")),
                              QMessageBox::Ok);
    else
        can_paint = true;

    synapseViewFactory = SynapseViewFactory::instance();
    if (!synapseViewFactory->readFileConf()) {
        QMessageBox::warning(nullptr, tr("DisView"),
                             QString(tr("No info on synapses names")),
                              QMessageBox::Ok);
        can_paint = false;
    }


    subnetViewFactory = SubNetViewFactory::instance();
    if (!subnetViewFactory->readFileConf()) {
        QMessageBox::warning(nullptr, tr("DisView"),
                             QString(tr("No info on subnet pixmaps")),
                              QMessageBox::Ok);
        can_paint = false;
    }


    textViewer = new TextViewer(this, 0, 0);
    textViewer->initTextViewer(true);
    textViewer->hide();
    view_text = false;
    sim_view = false;
    sim_updview = false;
    tool = 0;
    synapseColors();
    net = net_;
    net->setSynapseColors(synapse_colors);
    assignNetPixmaps();
    leyend = new Leyend(net->weightsMask, synapse_colors, this);
    leyend->createLeyend();
    leyend->hide();
    view_leyend = false;

    // para las vistas de red
    selection = false;
    selection_mode = "none";

    selectionbox = 0;

    qc_sprite_move = 0;
    qc_sprite_zoom = 0;
    qc_sprite_redim = 0;

    zoom = 1.0;
    // tamaño minimo del rectangulo de seleccion
    selection_min.setX(50);
    selection_min.setY(50);

//    connect(this, SIGNAL(closeView(void)),
//            widgetMain, SLOT(slotFileClose(void)));
//    connect(this, SIGNAL(enableMenu(bool)),
//            widgetMain, SLOT(slotEnableDisMenu(bool)));
//    connect(leyend, SIGNAL(closeLeyend(void)),
//            widgetMain, SLOT(slotViewLeyend(void)));
//    connect(textViewer, SIGNAL(closeViewer(void)),
//            widgetMain, SLOT(slotViewRawText(void)));

    connect(net, SIGNAL(netChanged(void)), this, SLOT(update(void)));

    connect(net, SIGNAL(weightsDefChange(int, double)),
            leyend, SLOT(updateLeyend(int, double)));
    connect(net, SIGNAL(weightsDefChangeAll(void)),
            leyend, SLOT(updateLeyend(void)));

//    emit enableMenu(true);

    setRenderHint(QPainter::Antialiasing);

}

NeuralDisView::~NeuralDisView()
{

    // borrar elementos de la venta de seleccion
    if (selectionbox) {
        delete(selectionbox);
    }

    if (!pixarr_move.empty()) {
        for(int i = pixarr_move.size(); i >= 0; i--){
            delete pixarr_move.at(i);
        }
    }
    if (!pixarr_zoom.empty()) {
        for(int i = pixarr_zoom.size(); i >= 0; i--){
            delete pixarr_zoom.at(i);
        }
    }
    if (!pixarr_redim.empty()) {
        for(int i = pixarr_redim.size(); i >= 0; i--){
            delete pixarr_redim.at(i);
        }
    }
    if (qc_sprite_move) {
        delete(qc_sprite_move);
    }
    if (qc_sprite_zoom) {
        delete(qc_sprite_zoom);
    }
    if (qc_sprite_redim) {
        delete(qc_sprite_redim);
    }

    delete textViewer;
    textViewer = 0;
    delete leyend;
    leyend = 0;
    emit closeAllViews();
    emit enableMenu(false);

}

void NeuralDisView::closeEvent(QCloseEvent *e)
{

    emit closeView();
    e->ignore();

}

TextViewer *NeuralDisView::getTextViewer() const
{
    return textViewer;
}

void NeuralDisView::netViewConnected()
{
    emit enableMenu(true);
}

Leyend *NeuralDisView::getLeyend() const
{
    return leyend;
}

void NeuralDisView::hide(void)
{
    // Esta slot puede ser llamado al mostrarse mensajes sin estar inicializados textViewer y leyend

    if (textViewer)
        textViewer->hide();
    if (leyend)
        leyend->hide();
    QWidget::hide();
}

void NeuralDisView::slotDocumentChanged(const QString& texto)
{
    textViewer->slotTexto(texto); // Aquí textViewer->visorTexto que es un QTextBrowser se encarga de todo

    if (net->NumberOfNeurons > 0) {
        neuronsCoords();
    }

    update();

}

void NeuralDisView::viewRawText(bool show)
{

    if (view_text == show) return;
    if (show) {
        view_text = true;
        textViewer->show();
    } else {
        view_text = false;
        textViewer->hide();
    }

}

void NeuralDisView::viewLeyend(bool show)
{

    if (show) {
        view_leyend = true;
        leyend->show();
    } else {
        view_leyend = false;
        leyend->hide();
    }
}

void NeuralDisView::update()
{

    drawNet();

    scene()->update(scene()->sceneRect()); //NOTE: a lo mejor necesito poner un QRectF para saber qué área actualizar
}

void NeuralDisView::initNet(void)
{
    assignNetPixmaps();
}

void NeuralDisView::assignNeuronPixmap(Neuron *neuron)
{
    neuronViewFactory->assignNeuronPixmap(neuron);
}

void NeuralDisView::assignNetPixmaps(void)
{

    QList<Neuron*> neurons = net->neurons();
    QListIterator<Neuron*> it(neurons);

    Neuron *neuron;
    while (it.hasNext()){
        neuron = it.next();
        neuronViewFactory->assignNeuronPixmap(neuron);
    }

    // añado esto para la lista de subredes
    QList<SubNet *> subnets = net->subnets();
    QListIterator<SubNet *> itnv(subnets);

    SubNet* subnet;
    while (itnv.hasNext()) {
        subnet = itnv.next();
        subnetViewFactory->assignSubNetPixmap(subnet);
    }

}

void NeuralDisView::drawNet()
{

    // aquí se debería poder dibujar también las subredes
    if (!can_paint)
        return;

    QList<Neuron*> Neurons;
    Neurons = net->neurons();
    QListIterator<Neuron*> itn(Neurons);
    Neuron *neuron;
    Synapse *synapse;

    QListIterator<Synapse*> its(net->Axons);
    while(its.hasNext()) {
        synapse = its.next();
        synapse->draw(this, synapse_colors);
    }

    while(itn.hasNext()) {
        neuron = itn.next();
        QColor color = QColor(synapse_colors[6]);
        neuron->draw(this, sim_view, &color);
    }

}

void NeuralDisView::neuronsCoords(void)
{
    ulong i = 0;
    Neuron *neuron;
    QVector<int> startPos_sep(2);

    QListIterator<Neuron *> iti(net->Input_Neuron);
    QListIterator<Neuron *> ith(net->HiddenNeuron);
    QListIterator<Neuron *> ito(net->OutputNeuron);

    startPos_sep = pos_sep(net->num_InputN);

    while (iti.hasNext()) {
        neuron = iti.next();
        neuron->setX(startPos_sep[0] + i*startPos_sep[1]);
        neuron->setY(50);
        ++i;
    }


    // se modifica para mostrar las subredes al
    // mismo nivel que la capa oculta
    startPos_sep = pos_sep(net->num_HiddenN + net->NumberOfSubNets);
    i = 0;
    while (ith.hasNext()) {
        neuron = ith.next();
        neuron->setX(startPos_sep[0] + i*startPos_sep[1]);
        if (neuron->getSubLayer() < 50)
            neuron->setY(150);
        else if (neuron->getSubLayer() < 100)
            neuron->setY(200);
        else if (neuron->getSubLayer() < 150)
            neuron->setY(250);
        else if (neuron->getSubLayer() < 200)
            neuron->setY(300);
        /*    else if(neuron->getSubLayer() < 250)
               neuron->setY(350); */
        else
            neuron->setY(350);
        ++i;
    }

    // ahora se calculan las coords de las subnets
    SubNet *subnet;
    QListIterator<SubNet *> itsn(net->SubNets);
    while (itsn.hasNext()) {
        subnet = itsn.next();
        subnet->setX(startPos_sep[0] + i*startPos_sep[1]);
        subnet->setY(150);
        ++i;
    }


    startPos_sep = pos_sep(net->num_OutputN);
    i = 0;
    while (ito.hasNext()) {
        neuron = ito.next();
        neuron->setX(startPos_sep[0] + i*startPos_sep[1]);
        neuron->setY(450);
        ++i;
    }

}


QVector<int> NeuralDisView::pos_sep(ulong n_neurons)
{
    QVector<int> p_s(2);
    p_s[0] = 0;     //start position
    p_s[1] = 15;    // separator

    if (n_neurons <= 6)
        p_s[1] = 120;
    else if (n_neurons <= 10)
        p_s[1] = 70;
    else
        p_s[1] = 45;
    //   else if(n_neurons <= 18)
    //     p_s[1] = 45;
    //   else if(n_neurons <= 25)
    //     p_s[1] = 33;

    if (n_neurons % 2)
        p_s[0] = PAINTERWIDTH / 2 - (n_neurons / 2) * p_s[1];
    else
        p_s[0] = int(PAINTERWIDTH / 2 - (float(n_neurons - 1) / 2) * p_s[1]);
    p_s[0] = p_s[0] < 50 ? 50 : p_s[0];
    return p_s;
}




void NeuralDisView::synapseColors(void)
{
    synapse_colors.clear();
    synapse_colors.append("#000000"); //black"
    synapse_colors.append("#696969"); //grey41"
    synapse_colors.append("#b0b0b0"); //grey69"
    synapse_colors.append("#bfbfbf"); //grey75"
    synapse_colors.append("#d1d1d1"); //grey82"
    synapse_colors.append("#ffffff"); //white"
    synapse_colors.append("#0000ff"); //blue"
    synapse_colors.append("#00b0f0");
    synapse_colors.append("#00ffff"); //cyan"
    synapse_colors.append("#00f0b0");
    synapse_colors.append("#00ff00"); //green"
    synapse_colors.append("#b0f000");
    synapse_colors.append("#ffff00"); //yellow"
    synapse_colors.append("#f0b000");
    synapse_colors.append("#ff0000"); //red"
    synapse_colors.append("#ff00ff"); //magenta"
    synapse_colors.append("#32cd32"); //lime green"
    synapse_colors.append("#a52a2a"); //brown"
    synapse_colors.append("#b03060"); //maroon"
    synapse_colors.append("#ffd700"); //gold"
    synapse_colors.append("#7fffd4"); //aquamarine"
    synapse_colors.append("#ff3030"); //firebrick"
    synapse_colors.append("#daa520"); //goldenrod"
    synapse_colors.append("#8a2be2"); //blue violet"
    synapse_colors.append("#5f9ea0"); //cadet blue"
    synapse_colors.append("#ff7f50"); //coral"
    synapse_colors.append("#6495ed"); //cornflower blue"
    synapse_colors.append("#006400"); //dark green",
    synapse_colors.append("#556b2f"); //dark olive green"
    synapse_colors.append("#ffdab9"); //peach puff"
    synapse_colors.append("#ffefd5"); //papaya whip"
    synapse_colors.append("#ffe4c4"); //bisque"
    synapse_colors.append("#f0ffff"); //azure"
    synapse_colors.append("#e6e6fa"); //lavender"
    synapse_colors.append("#ffe4e1"); //misty rose",
    synapse_colors.append("#0000cd"); //medium blue"
    synapse_colors.append("#000080"); //navy blue",
    synapse_colors.append("#afeeee"); //pale turquoise"
    synapse_colors.append("#2e8b57"); //sea green"

}

void NeuralDisView::contentsMouseMoveEvent(QMouseEvent* evt)
{
    if (selection) {
        QPoint p = transform().inverted().map(evt->pos());

        if (selection_mode == "move") {
            // accion: mover cuadro de seleccion
            double dx = p.x() - selectionbox->x() - qp_offset.x();
            double dy = p.y() - selectionbox->y() - qp_offset.y();
            // selection->moveBy( dx, dy );
            selectionbox->moveBy(dx, dy);
            drawSelectionFrame(evt);
            scene()->update();
        }

        if (selection_mode == "redim") {
            double dx = p.x() - qc_sprite_redim->x() - qp_offset.x();
            double dy = p.y() - qc_sprite_redim->y() - qp_offset.y();
            double my_width = selectionbox->rect().width() + dx;
            double my_height = selectionbox->rect().height() + dy;

            if (my_width < selection_min.x() || my_height < selection_min.y()) {
                return;
            }

            selectionbox->setRect(0,0,qreal(my_width), qreal(my_height));
            drawSelectionFrame(evt);
            scene()->update();
        }
    }

}

void NeuralDisView::drawSelectionFrame(QMouseEvent *e)
{

    Settings* prog_settings = Settings::instance();
    QPoint p = transform().inverted().map(e->pos());

    QString app_dir = prog_settings->getAppDirectory();
    QString file;

    if (!selectionbox) {
        selectionbox = new QGraphicsRectItem((qreal)p.x(), (qreal)p.y(),
                                             (qreal)200, (qreal)100);
        scene()->addItem(selectionbox);

        selectionbox->setBrush(Qt::NoBrush);
        selectionbox->setPen(QPen(Qt::red, 0, Qt::DashDotDotLine));
        selectionbox->setZValue(9999);
        selectionbox->show();
    }

    // move
    if (!qc_sprite_move) {
        file = app_dir + "src/disview/imagenes/netview_move.xpm";
        file.replace("%1", "*");
        QFileInfo fi(app_dir);

        foreach (QString entry, QDir(fi.path(), fi.fileName()).entryList())
            pixarr_move << new QPixmap(fi.path() + "/" + entry);

        if (!pixarr_move.empty()) {
            qc_sprite_move = new AnimatedPixmapItem(pixarr_move);
            scene()->addItem(qc_sprite_move);
            qc_sprite_move->setFrame(0);
        }

        qc_sprite_move->setZValue(9999);
    }

    qc_sprite_move->setPos(selectionbox->x(),selectionbox->y());
    qc_sprite_move->show();

    // zoom
    if (!qc_sprite_zoom) {
        file = app_dir + "src/disview/imagenes/netview_zoom.xpm";
        file.replace("%1", "*");
        QFileInfo fi(app_dir);

        foreach (QString entry, QDir(fi.path(), fi.fileName()).entryList())
            pixarr_zoom << new QPixmap(fi.path() + "/" + entry);

        if (!pixarr_zoom.empty()) {
            qc_sprite_zoom = new AnimatedPixmapItem(pixarr_zoom);
            scene()->addItem(qc_sprite_zoom);
            qc_sprite_zoom->setFrame(0);
        }

        qc_sprite_zoom->setZValue(9999);
    }

    qc_sprite_zoom->setPos(selectionbox->rect().x() + selectionbox->rect().width() -
                           qc_sprite_zoom->boundingRect().width(),
                           selectionbox->rect().y());
    qc_sprite_zoom->show();

    // redim
    if (!qc_sprite_redim) {
        file = app_dir + "src/disview/imagenes/netview_redim.xpm";
        file.replace("%1", "*");
        QFileInfo fi(app_dir);

        foreach (QString entry, QDir(fi.path(), fi.fileName()).entryList())
            pixarr_redim << new QPixmap(fi.path() + "/" + entry);

        if (!pixarr_redim.empty()) {
            qc_sprite_redim = new AnimatedPixmapItem(pixarr_redim);
            scene()->addItem(qc_sprite_redim);
            qc_sprite_redim->setFrame(0);
        }

        qc_sprite_redim->setZValue(9999);
    }
    qc_sprite_redim->setPos(selectionbox->rect().x() + selectionbox->rect().width() - qc_sprite_redim->boundingRect().width(),
                            selectionbox->rect().y() + selectionbox->rect().height() - qc_sprite_redim->boundingRect().height());
    qc_sprite_redim->show();

    scene()->update();
}

void NeuralDisView::wheelEvent(QWheelEvent * e)
{
    setZoom(e->angleDelta().y());
    e->ignore();
}

void NeuralDisView::setZoom(int delta)
{
    double zoom_rate = 1 + delta / 120.0 / 25; // 120 punto de rueda en Qt
    if (zoom_rate) {
        zoom *= zoom_rate;
        QTransform m = transform();
        m.scale(zoom_rate, zoom_rate);
        setTransform(m);
    }
}

void NeuralDisView::mouseDoubleClickEvent(QMouseEvent * e)
{
    switch (tool) {
    case 0:
        if (net->fromSelNeuron  && net->toSelNeuron) {
            selectNeuron(e, true);
            editSynapse();
        }
        else
            editNeuron(e);
    }
}

void NeuralDisView::mousePressEvent(QMouseEvent *e)
{

    // las constantes para 'tool' se definen en neuraldisview.h
    ulong ret_add = 1;
    Neuron *neuron;
    Synapse *synapse;
    SubNet *subnet;
    prev_event_pos = e->pos();
    switch (tool) {
    case 0:
        selectNeuron(e, true);
        break;

    case ADDNEURON:

        ret_add = selectAddNeuron(e->pos());
        if (ret_add > 0) {

            DictionaryManager* dict = DictionaryManager::instance();
            int neuron_type = dict->chooseClass(neuronViewFactory->getNamesMap(),
                                                tr("Choose a type of neuron"));

            if (neuron_type >= 0) {
                neuron = net->addNeuron(ret_add, net->getLayer(), neuron_type);
                if (neuron) {
                    neuronViewFactory->assignNeuronPixmap(neuron);
                    neuron->setSelected(true);
                    net->fromSelNeuron = neuron;
                    update();
                }
            }
        }
        break;

    case DELNEURON:
        selectNeuron(e, false);
        if (net->fromSelNeuron) {
//            if (QMessageBox::information(nullptr, tr("Del neuron"),
//                     QString(tr("Delete") + " %1 " + tr("number:") + " %2 " + tr("?")).
//                     arg(net->fromSelNeuron->devName(false)).
//                     arg(net->fromSelNeuron->getNumId()),
//                     tr("&Ok"), tr("&Cancel")) == 0) {
            if (QMessageBox::information(nullptr, tr("Del neuron"),
                                         QString(tr("Delete") + " %1 " + tr("number:") + " %2 " + tr("?")).
                                         arg(net->fromSelNeuron->devName(false)).
                                         arg(net->fromSelNeuron->getNumId()),
                                         QMessageBox::Ok | QMessageBox::Cancel) == QMessageBox::Ok) {
                net->delNeuron(net->fromSelNeuron->getNumId());
                update();
            }
        }
        break;

    case EDITNEURON:
        editNeuron(e);
        break;

    case ADDSYNAPSE:
        selectNeuron(e, true);
        if (net->fromSelNeuron  && net->toSelNeuron) {
            DictionaryManager* dict = DictionaryManager::instance();
            int syn_type = dict->chooseClass(synapseViewFactory->getNamesMap(),
                                             tr("Choose a type of synapse"));
            if (syn_type >= 0)
                net->addSynapse(net->fromSelNeuron->getNumId(),
                                net->toSelNeuron->getNumId(), syn_type);
        }
        break;

    case DELSYNAPSE:
        selectNeuron(e, true);
        if (net->fromSelNeuron  && net->toSelNeuron) {
            synapse = net->findSynapse(net->fromSelNeuron->getNumId(),
                                       net->toSelNeuron->getNumId());
            if (synapse) {
//                if (QMessageBox::information(nullptr, tr("Del synapse"),
//                             QString(tr("Delete") + " %1\n" + tr("from neuron:") +
//                             " %2 " + tr("to neuron:") + " %3").
//                             arg(synapse->connName(false)).
//                             arg(net->fromSelNeuron->getNumId()).
//                             arg(net->toSelNeuron->getNumId()),
//                             tr("&Ok"), tr("&Cancel")) == 0) {
                if (QMessageBox::information(nullptr, tr("Del synapse"),
                                             QString(tr("Delete") + " %1\n" + tr("from neuron:") + " %2 " + tr("to neuron:") + " %3").
                             arg(synapse->connName(false)).
                             arg(net->fromSelNeuron->getNumId()).
                             arg(net->toSelNeuron->getNumId()),
                             QMessageBox::Ok | QMessageBox::Cancel) == QMessageBox::Ok) {
                    net->delSynapse(synapse->getNumId());
                }
            } else
                QMessageBox::information(nullptr, tr("Del synapse"), QString(tr("No synapse") +
                            "\n" + tr("from neuron:") + " %1 " + tr("to neuron:") + " %2").
                            arg(net->fromSelNeuron->getNumId()).
                            arg(net->toSelNeuron->getNumId()), QMessageBox::Ok);
        }
        break;

    case EDITSYNAPSE:
        selectNeuron(e, true);
        if (net->fromSelNeuron  && net->toSelNeuron)
            editSynapse();
        break;
    case ADDNETWORK:
        subnet = net->addNetwork(ret_add, "");
        if (subnet) {
            subnetViewFactory->assignSubNetPixmap(subnet);
            subnet->setSelected(true);
            update();
        }

        break;

    case DELNETWORK:
        // utilizo el mismo para neuronas y subredes
        selectNeuron(e, false);
//        QMessageBox::information(this, tr("Intento borrar subred"),
//                                 tr("Not yet implemented"),
//                                 tr("&Ok"));
        QMessageBox::information(this, tr("Trying to delete subnet"),
                                 tr("Not yet implemented"),
                                 QMessageBox::Ok);
        // jose manuel        net->delNetwork(net->fromSelSubNet->getNumId());
        update();

        break;
    case EDITNETWORK:
        selectNeuron(e, false);
        if (net->fromSelSubNet) {
//            QMessageBox::information(this, tr("Intento editar subred"),
//                                     tr("Not yet implemented"),
//                                     tr("&Ok"));
            QMessageBox::information(this, tr("Trying to edit subnet"),
                                     tr("Not yet implemented"),
                                     QMessageBox::Ok);

        }

        break;
    case NETINFO:
        selectNeuron(e, false);
        if (net->fromSelNeuron) {
            QWhatsThis::showText(QPoint(0,0),net->fromSelNeuron->displayInfo()) ;
        }
        break;
    }

}


void NeuralDisView::selectNeuron(QMouseEvent *e, bool ctrl_butt)
{

    Neuron *neuron = 0, *old_neuron = 0;
    SubNet *subnet = 0, *old_subnet = 0;
    ulong i, j;
    bool click_out = true, updt = false;

    QPoint e_pos;
    QPointF topleft = mapToScene(viewport()->rect().topLeft());
    e_pos = QPoint(e->position().toPoint().x(), e->position().toPoint().y()); //TODO RV: No encuentro equivalencia para esto-> + QPoint(contentsX(), contentsY());
    e_pos = transform().inverted().map(QPoint(e_pos)) + topleft.toPoint();

    if (ctrl_butt && (QGuiApplication::keyboardModifiers() == Qt::ControlModifier))
        old_neuron = net->toSelNeuron;
    else
        old_neuron = net->fromSelNeuron;

    // nuevo para subredes

    if (ctrl_butt && (QGuiApplication::keyboardModifiers() == Qt::ControlModifier))
        old_subnet = net->toSelSubNet;
    else
        old_subnet = net->fromSelSubNet;

    if (QRect(25, 25, width()  - 50, 75).contains(e_pos)) {
        if (ctrl_butt && (QGuiApplication::keyboardModifiers() == Qt::ControlModifier))
            net->setLayerTo(NeuralNetwork::LAYER_INPUT);
        else
            net->setLayer(NeuralNetwork::LAYER_INPUT);

        for (i = 0;i < net->num_InputN;i++) {
            neuron = net->Input_Neuron.at(i);
            if (QRect(neuron->getX(), neuron->getY(), neuron->width(), neuron->height()).
                    contains(e_pos)) {
                click_out = false;
                if (old_neuron) {
                    old_neuron->setSelected(false);
                    updt = true;
                }
                old_neuron = neuron;
                if (!neuron->getSelected()) {
                    neuron->setSelected(true);
                    updt = true;
                }
                break;
            }
        }
    }
    else if (QRect(25, 125, width()  - 50, 275).contains(e_pos)) {
        if (ctrl_butt && (QGuiApplication::keyboardModifiers() == Qt::ControlModifier))
            net->setLayerTo(NeuralNetwork::LAYER_HIDDEN);
        else
            net->setLayer(NeuralNetwork::LAYER_HIDDEN);

        for (i = 0;i < net->num_HiddenN;i++) {
            neuron = net->HiddenNeuron.at(i);
            if (QRect(neuron->getX(), neuron->getY(), neuron->width(), neuron->height()).
                    contains(e_pos)) {
                click_out = false;
                if (old_neuron) {
                    old_neuron->setSelected(false);
                    updt = true;
                }
                old_neuron = neuron;
                if (!neuron->getSelected()) {
                    neuron->setSelected(true);
                    updt = true;
                }
                break;
            }
            // nuevo para subredes
            for (j = 0;j < net->NumberOfSubNets;j++) {
                subnet = net->SubNets.at(j);
                if (QRect(subnet->getX(), subnet->getY(), subnet->getWidth(), subnet->getHeight()).
                        contains(e_pos)) {
                    click_out = false;
                    if (old_subnet) {
                        old_subnet->setSelected(false);
                        updt = true;
                    }
                    old_subnet = subnet;
                    if (!subnet->getSelected()) {
                        subnet->setSelected(true);
                        updt = true;
                    }
                    break;
                }
            }
        }
    }
    else if (QRect(25, 425, width() - 50, 75).contains(e_pos)) {
        if (ctrl_butt && (QGuiApplication::keyboardModifiers() == Qt::ControlModifier))
            net->setLayerTo(NeuralNetwork::LAYER_OUTPUT);
        else
            net->setLayer(NeuralNetwork::LAYER_OUTPUT);

        for (i = 0;i < net->num_OutputN;i++) {
            neuron = net->OutputNeuron.at(i);
            if (QRect(neuron->getX(), neuron->getY(), neuron->width(), neuron->height()).
                    contains(e_pos)) {
                click_out = false;
                if (old_neuron) {
                    old_neuron->setSelected(false);
                    updt = true;
                }
                old_neuron = neuron;
                if (!neuron->getSelected()) {
                    neuron->setSelected(true);
                    updt = true;
                }
                break;
            }
        }
    }

    if (click_out) {
        clearSelection();
    }
    else {

        if (ctrl_butt && (QGuiApplication::keyboardModifiers() == Qt::ControlModifier)) {
            net->toSelNeuron = neuron;
        }
        else {
            net->fromSelNeuron = neuron;
        }

        if (updt) {
            update();
        }

    }

}

ulong NeuralDisView::selectAddNeuron(const QPoint& event_pos)
{

    Neuron *neuron;
    ulong i;
    ulong ret = 0;

    QPoint e_pos;
    e_pos = transform().inverted().map(QPoint(event_pos.x(), event_pos.y()));

    if (QRect(25, 25, width() - 50, 75).contains(e_pos)) {
        net->setLayer(NeuralNetwork::LAYER_INPUT);
        for (i = 0;i < net->num_InputN;i++) {
            neuron = net->Input_Neuron.at(i);
            if (event_pos.x() < neuron->getX())
                break;
        }
        ret = i + 1;
    }

    if (QRect(25, 125, width() - 50, 225).contains(e_pos)) {
        net->setLayer(NeuralNetwork::LAYER_HIDDEN);
        for (i = 0;i < net->num_HiddenN;i++) {
            neuron = net->HiddenNeuron.at(i);
            if (event_pos.x() < neuron->getX())
                break;
        }
        ret = i + net->num_InputN + 1;
    }

    if (QRect(25, 375, width() - 50, 75).contains(e_pos)) {
        net->setLayer(NeuralNetwork::LAYER_OUTPUT);
        for (i = 0;i < net->num_OutputN;i++) {
            neuron = net->OutputNeuron.at(i);
            if (event_pos.x() < neuron->getX())
                break;
        }
        ret = i + net->num_InputN + net->num_HiddenN + 1;
    }

    if (ret) {
        if (net->fromSelNeuron) {
            net->fromSelNeuron->setSelected(false);
            net->fromSelNeuron = 0;
        }
    }

    return ret;
}

void NeuralDisView::clearSelection(void)
{

    bool updt = false;

    if (net->fromSelNeuron) {
        net->fromSelNeuron->setSelected(false);
        net->fromSelNeuron = 0;
        updt = true;
    }
    if (net->toSelNeuron) {
        net->toSelNeuron->setSelected(false);
        net->toSelNeuron = 0;
        updt = true;
    }

    if (updt)
        update();

    net->setLayer(-1);

}

void NeuralDisView::editNeuron(QMouseEvent * e)
{
    selectNeuron(e, false);
    if (net->fromSelNeuron) {
        ulong n_id = net->fromSelNeuron->getNumId();
        XMLOperationsLog* undoLog = XMLOperationsLog::instance();
        undoLog->insertComment("DEBUG:: edit neuron :: num_id=" + QString::number(n_id));
        undoLog->insertLogOperation(net->fromSelNeuron, "edit_neuron");
        if (!net->fromSelNeuron->edit())
            undoLog->removeLastEditOperation();
        update();
    }
}

void NeuralDisView::editSynapse(void)
{
    Synapse *synapse = net->findSynapse(net->fromSelNeuron->getNumId(), net->toSelNeuron->getNumId());
    if (synapse) {
        ulong n_id = synapse->getNumId();
        XMLOperationsLog* undoLog = XMLOperationsLog::instance();
        undoLog->insertComment("DEBUG:: edit synapse :: num_id=" + QString::number(n_id));
        undoLog->insertLogOperation(synapse, "edit_synapse");
        if (!synapse->edit())
            undoLog->removeLastEditOperation();
    }
    else
//        QMessageBox::information(nullptr, tr("Edit synapse"),
//                         QString(tr("No synapse") + "\n" + tr("from neuron:") +
//                                 " %1 " + tr("to neuron:") + " %2").
//                         arg(net->fromSelNeuron->getNumId()).
//                         arg(net->toSelNeuron->getNumId()), tr("&Ok"));
        QMessageBox::information(nullptr, tr("Edit synapse"),
                                 QString(tr("No synapse") + "\n" + tr("from neuron:") +
                                         " %1 " + tr("to neuron:") + " %2").
                                 arg(net->fromSelNeuron->getNumId()).
                                 arg(net->toSelNeuron->getNumId()), QMessageBox::Ok);
    update();
}

void NeuralDisView::setTool(int tool_)
{
    tool = tool_;
    clearSelection();
}

void NeuralDisView::exportFig(QTextStream &ts)
{

    ulong i;
    Neuron *neuron_from = 0;
    Synapse *synapse;

    QListIterator<Synapse *> it(net->Axons);
    while (it.hasNext()) {
        synapse = it.next();
        write_sinap(synapse, ts);
    }

    for (i = 0; i < net->num_InputN; i++) {              // Neuronas
        neuron_from = net->Input_Neuron.at(i);
        write_neuron(neuron_from, ts, 51, i);
    }

    for (i = 0; i < net->num_HiddenN; i++) {
        neuron_from = net->HiddenNeuron.at(i);
        if (neuron_from->rtti_dev() == Neuron::ISO_DEVICE ||
                neuron_from->rtti_dev() == Neuron::ICO_DEVICE)
            write_neuron(neuron_from, ts, 5, i + net->num_InputN);
        else
            write_neuron(neuron_from, ts, 52, i + net->num_InputN);
    }

    for (i = 0; i < net->num_OutputN; i++) {
        neuron_from = net->OutputNeuron.at(i);
        write_neuron(neuron_from, ts, 39, i +
                     net->num_InputN + net->num_HiddenN);
    }

    leyend->export_leyend(ts);

    return;
}

void NeuralDisView::write_sinap(Synapse *synapse, QTextStream &ts)
{
    // líneas y flechas
    uint j;
    float ang = 0.0;
    int xcenter_from = 0, ycenter_from = 0, xcenter_to = 0, ycenter_to = 0;

    QPolygon triang, n_triang;
    QPoint t_p;
    float scale_x, scale_y, scale_;
    Neuron* neuron_from = synapse->getPointerFrom();
    Neuron* neuron_to = synapse->getPointerTo();

    QString ini_linea1, ini_circ1, ini_triang1, ini_arc1;
    QString ini_linea2, ini_circ2, ini_triang2, ini_arc2;

    triang.setPoints(3, 100, 0, -50, 50, -50, -50);

    scale_x = float(EXPORTWIDTH) / float(PAINTERWIDTH);
    scale_y = float(EXPORTHEIGHT) / float(PAINTERHEIGHT);
    scale_ = scale_x;

    ini_linea1 = "2 1 0 2 ";
    ini_linea2 = " 65 -1 -1 0.000 0 0 7 0 0 2\n\t";
    ini_arc1 = "1 1 0 2 ";
    ini_arc2 = " -1 65 -1 -1 0.000 1 0.000 ";
    ini_triang1 = "2 3 0 2 ";
    ini_triang2 = " 60 -1 20 0.000 0 0 7 0 0 4\n\t";

    xcenter_from = round_int(scale_x * neuron_from->getX()) +
            (round_int(scale_ * neuron_from->width()) + 1) / 2 + 500;
    ycenter_from = round_int(scale_y * neuron_from->getY()) +
            (round_int(scale_ * neuron_from->height()) + 1) / 2 + 500;
    xcenter_to = round_int(scale_x * neuron_to->getX()) +
            (round_int(scale_ * neuron_to->width()) + 1) / 2 + 500;
    ycenter_to = round_int(scale_y * neuron_to->getY()) +
            (round_int(scale_ * neuron_to->height()) + 1) / 2 + 500;

    if (neuron_from == neuron_to) {
        ts << ini_arc1 <<  32 + synapse->getIdColor() << ini_arc2;
        ts << xcenter_from << " " << ycenter_from  -
              (round_int(2*scale_*neuron_from->height()) + 1) / 2 << " ";
        ts << int(1.5*(ceil(scale_*neuron_from->width()) + 1) / 2)
           << " " << (round_int(2*scale_*neuron_from->height()) + 1) / 2 << " ";
        ts << xcenter_from << " " << ycenter_from -
              (round_int(2*scale_*neuron_from->height()) + 1) / 2 << " ";
        ts << xcenter_from << " " << ycenter_from << "\n";

        n_triang = mov_triang(triang, xcenter_from , ycenter_from  -
                              (round_int(2 * scale_ * neuron_from->height()) + 1) + 20, 0.0);
        ts << ini_triang1 <<  32 + synapse->getIdColor() << " " <<
              32 + synapse->getIdColor() << ini_triang2;
        for (j = 0; j < 3; j++) {
            t_p =  n_triang.point(j);
            ts << t_p.x() << " " << t_p.y() << " ";
        }
        t_p =  n_triang.point(0);
        ts << t_p.x() << " " << t_p.y() <<  "\n";
    } else {
        ts << ini_linea1 <<  32 + synapse->getIdColor() << " " <<
                                32 + synapse->getIdColor()
           << ini_linea2;
        ts << xcenter_from  << " " << ycenter_from  << " ";
        ts << xcenter_to << " " << ycenter_to  << "\n";

        if (xcenter_to == xcenter_from) {
            if (ycenter_to >= ycenter_from)
                ang = M_PI / 2;
            else
                ang = -M_PI / 2;
        } else {
            if (xcenter_to >= xcenter_from)
                ang = atan(double(ycenter_to - ycenter_from) /
                           double(xcenter_to - xcenter_from));
            else
                ang = M_PI + atan(double(ycenter_to - ycenter_from) /
                                  double(xcenter_to - xcenter_from));
        }
        n_triang = mov_triang(triang, (xcenter_from + xcenter_to) / 2,
                              (ycenter_from + ycenter_to) / 2, -ang);
        ts << ini_triang1 <<  32 + synapse->getIdColor() << " " <<
                                    32 + synapse->getIdColor() << ini_triang2;
        for (j = 0; j < 3; j++) {
            t_p =  n_triang.point(j);
            ts << t_p.x() << " " << t_p.y() << " ";
        }
        t_p =  n_triang.point(0);
        ts << t_p.x() << " " << t_p.y() <<  "\n";
    }
}

QString NeuralDisView::color2rgb(const QColor color)
{

    QRgb scolor;
    uint r, g, b;
    QString sscol;

    scolor = color.rgb();
    r = qRed(scolor);
    g = qGreen(scolor);
    b = qBlue(scolor);
    sscol = '#' + QString::number(r, 16).rightJustified(2, '0') + QString::number(g, 16).rightJustified(2, '0') +
            QString::number(b, 16).rightJustified(2, '0');
    return sscol;

}

void NeuralDisView::write_neuron(Neuron *neuron, QTextStream &ts,
                                 uint color, uint i)
{
    // cálculos y números

    float scale_x, scale_y, scale_;
    int x_center, y_center, radx, rady, leng;

    const QString ini_circ1 = "1 3 0 2 0 ";
    const QString ini_circ2 = " 55 -1 30 0.000 1 0.000 ";
    const QString ini_box1 = "2 2 0 2 0 ";
    const QString ini_box2 = " 55 -1 30 0.000 0 0 -1 0 0 5 ";
    const QString ini_text = "4 0 0 50 -1 2 12 0.0000 4 ";
    scale_x = float(EXPORTWIDTH) / float(PAINTERWIDTH);
    scale_y = float(EXPORTHEIGHT) / float(PAINTERHEIGHT);
    scale_ = scale_x;

    radx = (round_int(scale_ * neuron->width()) + 1) / 2;
    rady = (round_int(scale_ * neuron->height()) + 1) / 2;
    x_center = round_int(scale_x * neuron->getX()) + 500 + radx;
    y_center = round_int(scale_y * neuron->getY()) + 500 + rady;
    leng = int(ceil(log10(double(i + 2)))) * 90;

    if (neuron->rtti_dev() == Neuron::ISO_DEVICE ||
                        neuron->rtti_dev() == Neuron::ICO_DEVICE) {
        ts << ini_box1 <<  color << ini_box2 << "\n\t";
        ts << x_center - radx << " " << y_center - rady << " " <<  x_center + radx << " ";
        ts << y_center - rady <<  " " << x_center + radx  << " " << y_center + rady << " ";
        ts << x_center - radx << " " << y_center + rady << " " << x_center - radx << " "
                                << y_center - rady << "\n";
    }
    else {
        ts << ini_circ1 <<  color << ini_circ2;
        ts << x_center << " " << y_center << " " <<  radx << " ";
        ts << rady <<  " " << x_center  << " " << y_center << " ";
        ts << x_center + radx << " " << y_center + rady << "\n";
    }
    ts << ini_text << 135 << " " << leng << " " << x_center - leng / 2 << " " <<
                            y_center + 135 / 2 << " ";
    ts << i + 1 << "\\001" << "\n";
}

QPolygon NeuralDisView::mov_triang(QPolygon triang, int transl_x, int transl_y, float ang)
{

    QPolygon n_triang;
    QPoint p;
    n_triang.setPoints(3, 0, 0, 0, 0, 0, 0);
    float x, y;
    uint i;

    for (i = 0; i < 3; i++) {
        p = triang.point(i);
        x = float(p.x()) * cos(ang) + float(p.y()) * sin(ang);
        y = -float(p.x()) * sin(ang) + float(p.y()) * cos(ang);
        n_triang.setPoint(i, round_int(x), round_int(y));
    }
    n_triang.translate(transl_x, transl_y);
    return n_triang;

}

void NeuralDisView::neuronConnectSim(Neuron *neuron)
{
    connect(neuron, SIGNAL(activateView(Neuron *, double)),
            this, SLOT(setNeuronActive(Neuron *, double)));
}

void NeuralDisView::simConnection(bool on)
{

    Neuron *neuron;

    QListIterator<Neuron *> it(net->neurons());
    if (sim_updview == on)
        return;
    sim_updview = on;
    if (on) {
        while (it.hasNext()) {
            neuron = it.next();
            connect(neuron, SIGNAL(activateView(Neuron *, double)),
                    this, SLOT(setNeuronActive(Neuron *, double)));
        }
    }
    else {
        while (it.hasNext()) {
            neuron = it.next();
            disconnect(neuron, SIGNAL(activateView(Neuron *, double)),
                       this, SLOT(setNeuronActive(Neuron *, double)));
        }
    }

}

void NeuralDisView::setNeuronActive(Neuron *neuron, bool on)
{

    if (sim_view) return;
    if (on) {
        if (neuron->getSelected())
            return;
    }
    else {
        if (!neuron->getSelected())
            return;
    }

    neuron->draw(this, sim_view);
    this->scene()->update();

}

void NeuralDisView::setNeuronActive(Neuron *neuron, double val)
{

    if (!sim_view)
        return;
    uint color_idx = 6 + int(val * 8.999999);
    color_idx = color_idx > 14 ? 14 : color_idx;
    QColor color = QColor(synapse_colors[color_idx]);
    neuron->draw(this, sim_view, &color);
    this->scene()->update();

}

void NeuralDisView::deactivateAllNeurons(void)
{

    Neuron *neuron;
    QListIterator<Neuron *> it(net->neurons());
    while (it.hasNext()) {
        neuron = it.next();
        setNeuronActive(neuron, false);
    }

}

bool NeuralDisView::getSimView(void)
{
    return sim_view;
}

void NeuralDisView::setSimView(bool simv)
{

    static bool old_val = false;
    sim_view = simv;
    if (old_val != sim_view) {
        update();
        old_val = sim_view;
    }

}

void NeuralDisView::restoreSize(void)
{
    parentWidget()->move(50, 50);
    resize(PAINTERWIDTH, PAINTERHEIGHT);
}


void NeuralDisView::write_subnet(SubNet */*subnet*/, QTextStream &/*ts*/,
                                 uint /*color*/, uint /*i*/)
{
    // pendiente definir el dibujo de las vistas de red (subredes)
}

void  NeuralDisView::paste()
{

    QString text_tmp = QApplication::clipboard()->text(QClipboard::Clipboard);

    QDomDocument clip;
    if (!clip.setContent(text_tmp, false)) {
        return;
    }

    XMLOperator* xml_operator = XMLOperator::instance();
    QDomElement xml_neuron = xml_operator->findTag("Neuron", clip.documentElement(), false);
    QDomElement xml_synapse = xml_operator->findTag("Synapse", clip.documentElement(), false);

    if (!xml_neuron.isNull()) {
        ulong ret_add = selectAddNeuron(prev_event_pos);
        if (ret_add > 0) {
            QDomElement xml_numid = xml_operator->findTag("num_id", clip.documentElement(), false);
            if (!xml_numid.isNull()) {
                xml_operator->changeTextInElement(xml_numid, QString::number(ret_add));
                QApplication::clipboard()->setText(clip.toString());
                net->paste();
            }
        }
    }
    if (!xml_synapse.isNull()) {
        if (net->fromSelNeuron  && net->toSelNeuron) {
            QDomElement e_construct = xml_operator->findTag("constructor", xml_synapse, false);
            if (!e_construct.isNull()) {
                e_construct.setAttribute("From", net->fromSelNeuron->getNumId());
                e_construct.setAttribute("To", net->toSelNeuron->getNumId());
                QApplication::clipboard()->setText(clip.toString());
                net->paste();
            }
        }
    }
}
