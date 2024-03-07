/***************************************************************************
                          neuraldisview.h  -  description
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

#ifndef NEURALDISVIEW_H
#define NEURALDISVIEW_H

// include files for QT
#include <QWidget>
#include <QPainter>
#include <QPixmap>
#include <QStringList>
#include <QMap>

#include <QCloseEvent>
#include <QWheelEvent>
#include <QPaintEvent>
#include <QMouseEvent>
#include <QTextStream>

#include <QList>
#include <QVector>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QPainterPath>

#include "../net/animatedpixmapitem.h"
#include "../neuraldis/neuraldisdoc.h"
#include "../net/neuralnetwork.h"
#include "../neuraldis/ndmath.h"
#include "../neuraldis/settings.h"
#include "../neuraldis/mdisubwininterface.h"

#define PAINTERWIDTH    900
#define PAINTERHEIGHT   500
#define EXPORTWIDTH     12000
#define EXPORTHEIGHT    8500

#define ADDNEURON   1
#define DELNEURON   2
#define EDITNEURON  3
#define ADDSYNAPSE  4
#define DELSYNAPSE  5
#define EDITSYNAPSE 6
#define ADDISO    7
// NUEVAS CONSTANTES PARA LAS subREDES
#define ADDNETWORK  8
#define DELNETWORK  9
#define EDITNETWORK   10
// NUEVAS CONSTANTES PARA LAS VISTAS DE RED (NetView)
#define NETVIEW   11

#define NETINFO   12

class TextViewer;
class Leyend;
class NeuronViewFactory;
class SynapseViewFactory;
// nueva clase
class SubNetViewFactory;

/**
  *@author Jose M. Cuadra Troncoso
  */

class NeuralDisView : public QGraphicsView , public MdiSubWinInterface
{
    Q_OBJECT
public:
    NeuralDisView(QGraphicsScene *n_canvas = 0, QWidget *parent = 0,
                  NeuralNetwork* net_ = 0);
    ~NeuralDisView();
    bool getSimView(void);
    bool viewText(void) {
        return view_text ;
    }
    void initNet(void);
    bool isViewLeyend() {
        return view_leyend ;
    }
    void viewRawText(bool show);
    void viewLeyend(bool show);
    void drawNet();
    void setTool(int tool_);
    void exportFig(QTextStream &ts);
    void deactivateAllNeurons(void);
    void assignNeuronPixmap(Neuron *neuron);
    void paste();
    void update();

    Leyend *getLeyend() const;
    TextViewer *getTextViewer() const;
    void netViewConnected();

public slots:
    void slotDocumentChanged(const QString& texto);
    void setSimView(bool simv);
    void neuronConnectSim(Neuron *neuron);
    void simConnection(bool on);
    void setNeuronActive(Neuron * neuron, double val);
    void restoreSize(void);
    void hide(void);

protected:
    void assignNetPixmaps(void);
    void drawSelectionFrame(QMouseEvent *e);
    void setNeuronActive(Neuron * neuron, bool on);
    void synapseColors(void);
    void neuronsCoords(void);
    void selectNeuron(QMouseEvent *e, bool ctrl_butt);

    ulong selectAddNeuron(const QPoint& event_pos);
    void clearSelection(void);
    void editNeuron(QMouseEvent * e);
    void editSynapse(void);

    QVector<int> pos_sep(ulong n_neurons);
    void mousePressEvent(QMouseEvent * e);
    void mouseDoubleClickEvent(QMouseEvent * e);
    void wheelEvent(QWheelEvent * e);
    void setZoom(int delta);

protected slots:
    void contentsMouseMoveEvent(QMouseEvent* evt);
    void write_sinap(Synapse *synapse, QTextStream &ts);
    void write_neuron(Neuron *neuron, QTextStream &ts,
                      uint color, uint i);
    // queda pendiente definir el método write_subnet para dibujar subredes
    void write_subnet(SubNet *subnet, QTextStream &ts,
                      uint color, uint i);
private:
    int round_int(float f);
    QString color2rgb(const QColor);
    QPolygon mov_triang(QPolygon triang, int transl_x, int transl_y, float ang);

private slots:
    void closeEvent(QCloseEvent *e);

signals:
    void closeView(void);
    void enableMenu(bool enabl);
    void closeAllViews(void);

protected:
    NeuralNetwork* net;
    QStringList synapse_colors;
    QPixmap disIcon;
    QList<QPixmap *> lateralPix;
    AnimatedPixmapItem *qc_lateralPix;

    int tool;
    bool sim_view, sim_updview;
    bool view_text;
    NeuronViewFactory* neuronViewFactory;
    SynapseViewFactory* synapseViewFactory;
    // nueva variable
    SubNetViewFactory* subnetViewFactory;
    Leyend* leyend;
    TextViewer* textViewer;
    bool view_leyend;
    bool can_paint;

    // seleccionar area para netviews
    bool selection;
    QString selection_mode;
    QGraphicsRectItem *selectionbox;
    QPoint moving_start, qp_offset, selection_min;

    QList<QPixmap *> pixarr_move, pixarr_zoom, pixarr_redim;
    AnimatedPixmapItem *qc_sprite_move, *qc_sprite_zoom, *qc_sprite_redim;
    QPoint prev_event_pos;
    double zoom;

};

inline int NeuralDisView::round_int(float f)
{

    float f_int = float(floor(f));
    if (fabs(f - f_int) >= 0.5) {
        if (f >= 0)
            f_int += 1;
        else
            f_int += -1;
    }
    return int(f_int);

}

#endif
