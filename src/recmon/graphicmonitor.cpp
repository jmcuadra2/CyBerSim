/***************************************************************************
                          graphicmonitor.cpp  -  description
                             -------------------
    begin                : miércoles feb 4 2004
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

#include "graphicmonitor.h"
#include "../neuraldis/xmloperator.h"
#include "../recmon/gui/graphmondialog.h"
#include "../neuraldis/ndmath.h"

#include <QMessageBox>
#include <QPaintEvent>
#include <QMouseEvent>
#include <QList>
#include <QVector>

#define GRAPH_WIDTH     280
#define GRAPH_HEIGHT    135
#define ADDL_WIDTH      65
#define ADDL_HEIGHT     35
#define ADDNOL_HEIGHT   25
#define XGRAPH          50
#define YGRAPH          30
#define YLABEL          3
#define YCAP            3
#define HCAP            12
#define WXLABEL         36
#define WYLABEL         45
#define HLABEL          20
#define MARG_CAP        10
#define MARG_LEYEND     20
#define OFFSET_LEYEND   10

int GraphicMonitor::N_MAX_GRAPHS = 6;

void GraphicMonitor::nMaxGraphsMessage(void)
{

//    QMessageBox::warning(nullptr, tr("GraphMonitor"),
//                         tr("Six graphics per monitor maximum"), tr("&Ok"));
    QMessageBox::warning(nullptr, tr("GraphMonitor"),
                         tr("Six graphics per monitor maximum"), QMessageBox::Ok);

}
GraphicMonitor::GraphicMonitor(const QDomElement& e,
                               QWidget *parent, QString name) : QWidget(parent)
{

    QString cap;
    bool back;
    int xupd;
    int n_xlab;
    int n_ylab;
    double y_min;
    double y_max;
    setObjectName(name);

    n_graphs = 0;
    bool ok;
    g_cnt_signal = n_graphs;
    if (!e.isNull()) {
        cap = e.attribute("caption");
        back = bool(e.attribute("back_color").toInt(&ok));
        xupd = e.attribute("xupdate").toInt(&ok);
        n_xlab = e.attribute("num_xlabels").toInt(&ok);
        n_ylab = e.attribute("num_ylabels").toInt(&ok);
        y_min = e.attribute("y_min").toDouble(&ok);
        y_max = e.attribute("y_max").toDouble(&ok);
    }
    else {
        cap = tr("Monitor");
        back = false;
        xupd = 60;
        n_xlab = 6;
        n_ylab = 7;
        y_min = 0.0;
        y_max = 1.0;
    }
    back_color = back;
    if (back) {  // papel
        bck_color = new QColor(Qt::white);
        fore_color = new QColor(Qt::black);
    }
    else {
        bck_color = new QColor(Qt::black);  // osciloscopio
        fore_color = new QColor("#ffe000");
    }
    fore_created_here = true;
    back_created_here = true;
    init_common(xupd, n_xlab, y_min, y_max, n_ylab, cap, 0, bck_color);

}

GraphicMonitor::GraphicMonitor(int xupd, int n_xlab,
                               double y_min, double y_max, int n_ylab,
                               const QString &cap, QColor *fore, QColor *back,
                               QWidget *parent, const char *name)
    : QWidget(parent)
{

    n_graphs = 0;
    g_cnt_signal = n_graphs;
    fore_created_here = false;
    back_created_here = false;
    setObjectName(name);
    init_common(xupd, n_xlab, y_min, y_max, n_ylab,
                cap, fore, back);

}

GraphicMonitor::GraphicMonitor(int n_g, QVector<int> grs_id, int xupd, int n_xlab,
                               double y_min, double y_max, int n_ylab,
                               const QString &cap, int back,
                               QWidget *parent, const char *name) : QWidget(parent)
{

    n_graphs = n_g;
    g_cnt_signal = n_graphs;
    graphs_ids = grs_id;
    setObjectName(name);
    if (n_graphs > N_MAX_GRAPHS)
        n_graphs = N_MAX_GRAPHS;
    back_color = back;
    if (back) {  // papel
        bck_color = new QColor(Qt::white);
        fore_color = new QColor(Qt::black);
    }
    else {
        bck_color = new QColor(Qt::black);  // osciloscopio
        fore_color = new QColor("#ffe000");
    }
    fore_created_here = true;
    back_created_here = true;
    init_common(xupd, n_xlab, y_min, y_max, n_ylab, cap, 0, bck_color);

}

GraphicMonitor::~GraphicMonitor()
{
    if (back_created_here)
        delete bck_color;
    if (fore_created_here) {
        if(!n_graphs)
            delete fore_color;
    }
    for(int i = 0; i < n_graphs; i++)
        delete fore_colors[i];
    graphsStorage.clear();
}

void GraphicMonitor::init_common(int xupd, int n_xlab, double y_min,
                                 double y_max, int n_ylab, const QString &cap,
                                 QColor *fore, QColor *back)
{

    titulo = cap;
    graphsRect.setRect(XGRAPH, YGRAPH, XGRAPH + GRAPH_WIDTH, YGRAPH + GRAPH_HEIGHT);
    pen.setWidth(1);

    setParams(xupd, n_xlab, y_min, y_max, n_ylab, fore, back);
    pen.setBrush(QBrush(*bck_color));
    crono = 0;
    init_crono = 0;
    reset_init_crono = false;
    init_reset_init_crono = false;
    accurateRestore = true;
    graph_cnt = 0;
    setWindowTitle(tr("Graphics monitor"));
    setMinimumSize(200, 100);
    resizeMon();

    if (n_graphs) {
        QVector<ValStorage> list;
        graphsStorage.fill(list, n_graphs);
        ValStorage p(0, 0);
        init_val_restored.fill(p, n_graphs);
        old_tval.fill(QPoint(0, 0), n_graphs);
        new_paintval.fill(QPoint(0, 0), n_graphs);
        prev_val_restored_equal.fill(false, n_graphs);
    }

}

void GraphicMonitor::resizeMon(void)
{

    if (n_xlabels) {
        if (n_graphs > 0) {
            resize(GRAPH_WIDTH + ADDL_WIDTH,
                   GRAPH_HEIGHT + ADDL_HEIGHT + MARG_LEYEND + 2*OFFSET_LEYEND);
        }
        else
            resize(GRAPH_WIDTH + ADDL_WIDTH,
                   GRAPH_HEIGHT + ADDL_HEIGHT);
    }
    else {
        if (n_graphs > 0) {
            resize(GRAPH_WIDTH + ADDL_WIDTH,
                   GRAPH_HEIGHT + ADDNOL_HEIGHT + MARG_LEYEND + 2*OFFSET_LEYEND);
        }
        else
            resize(GRAPH_WIDTH + ADDL_WIDTH, GRAPH_HEIGHT + ADDNOL_HEIGHT);
    }

}

void GraphicMonitor::paintEvent(QPaintEvent * e)
{

    if (graphsRect.contains(e->rect())) {
        p.begin(this);
        p.setBrush(QBrush(*bck_color));
        p.drawRect(e->rect());
        p.translate(XGRAPH, YGRAPH);
        for(int i = 0; i < n_graphs; i++) {
            pen.setColor(*(fore_colors.at(i)));
            p.setPen(pen);
            p.drawLine(old_tval[i], new_paintval[i]);
            old_tval[i] = new_paintval[i];
        }
        p.end();
    }
    else {
        p.begin(this);

        pen.setColor(Qt::black);
        p.setPen(pen);
        p.setBrush(QBrush(*bck_color));
        p.setFont(QFont("Helvetica", 8));

        setPainterWindow();
        drawCap(titulo.replace('_', ' '));
        drawGraph();
        drawLabels();
        drawLegend();
        restoreGraphs();

        p.end();
    }
}

void GraphicMonitor::setPainterWindow(void)
{
    if (n_xlabels) {
        if (n_graphs > 0) {
            p.setWindow(0, 0, GRAPH_WIDTH + ADDL_WIDTH, GRAPH_HEIGHT + ADDL_HEIGHT +
                        MARG_LEYEND + 2*OFFSET_LEYEND);

        }
        else
            p.setWindow(0, 0, GRAPH_WIDTH + ADDL_WIDTH, GRAPH_HEIGHT + ADDL_HEIGHT);
    }
    else {
        if (n_graphs > 0) {
            p.setWindow(0, 0, GRAPH_WIDTH + ADDL_WIDTH, GRAPH_HEIGHT + ADDNOL_HEIGHT +
                        MARG_LEYEND + 2*OFFSET_LEYEND);
        }
        else
            p.setWindow(0, 0, GRAPH_WIDTH + ADDL_WIDTH, GRAPH_HEIGHT + ADDNOL_HEIGHT);
    }
}

void GraphicMonitor::drawCap(const QString cap)
{

    p.save();
    p.setFont(QFont("Helvetica", 10));
    p.translate(XGRAPH, YCAP);
    p.drawText(p.boundingRect(MARG_CAP, 0, GRAPH_WIDTH - 2*MARG_CAP, HCAP,
                              Qt::AlignHCenter, cap), Qt::AlignHCenter, cap);
    p.restore();

}

void GraphicMonitor::drawLabels(bool all)
{

    if(all) {
        p.save();
        p.translate(YLABEL, YGRAPH);
        if (n_ylabels == 1)
            p.drawText(0, NDMath::roundInt(GRAPH_HEIGHT / 2.0) - HLABEL / 2 + 5,
                       WYLABEL, HLABEL, Qt::AlignRight,
                       QString::number((ymax + ymin) / 2.0, 'g', 4));
        else {
            for (int i = 0; i < n_ylabels ; i++) {
                p.drawText(0, (i + 1)*NDMath::roundInt(GRAPH_HEIGHT /
                        double(n_ylabels + 1)) - HLABEL / 2, WYLABEL, HLABEL,
                        Qt::AlignRight, QString::number(ymax - i*(ymax - ymin) /
                        (n_ylabels - 1), 'g', 4));
            }
        }
        p.restore();
    }

    p.save();
    p.translate(XGRAPH, YGRAPH + GRAPH_HEIGHT);
    for (int i = 0; i < n_xlabels; i++) {
        p.drawText(i*NDMath::roundInt(GRAPH_WIDTH / double(n_xlabels - 1)) -
                   WXLABEL / 2, 0, WXLABEL, HLABEL, Qt::AlignHCenter,
                   QString::number(i*double(xupdate) / (n_xlabels - 1) +
                   crono / (1000*xupdate)*xupdate, 'g', 5)); // no simplificar xupdate
    }
    p.restore();

}

void GraphicMonitor::drawGraph()
{

    p.save();
    p.translate(XGRAPH, YGRAPH);
    p.drawRect(0, 0, GRAPH_WIDTH, GRAPH_HEIGHT);
    pen.setColor(QColor(Qt::gray));
    p.setPen(pen);
    for (int i = 1; i < n_xlabels - 1; i++) {
        p.drawLine(NDMath::roundInt(i*GRAPH_WIDTH / double(n_xlabels - 1)), 1,
                   NDMath::roundInt(i*GRAPH_WIDTH / double(n_xlabels - 1)),
                   GRAPH_HEIGHT - 1);
    }
    for (int  i = 1; i <= n_ylabels ; i++) {
        p.drawLine(1, NDMath::roundInt(i*GRAPH_HEIGHT / double(n_ylabels + 1)),
               GRAPH_WIDTH - 1, NDMath::roundInt(i*GRAPH_HEIGHT / double(n_ylabels + 1)));
    }
    p.restore();

}

void GraphicMonitor::drawLegend()
{
    int length = 2;
    QString s_num;
    if (n_graphs == 1)
        length = 35;
    else if (n_graphs == 2)
        length = 20;
    else if (n_graphs == 3)
        length = 12;
    else if (n_graphs == 4)
        length = 9;
    else if (n_graphs == 5)
        length = 7;
    else if (n_graphs == 6)
        length = 5;

    p.save();
    p.setFont(QFont("Helvetica", 8));
    p.translate(XGRAPH, YGRAPH + GRAPH_HEIGHT + MARG_LEYEND + OFFSET_LEYEND);
    p.drawRect(0, 2 - MARG_LEYEND / 2, GRAPH_WIDTH, MARG_LEYEND);
    for (int i = 0; i < n_graphs; i++) {
        pen.setColor(*(fore_colors.at(i)));
        p.setPen(pen);
        if (n_graphs == 1)
            p.drawText(3, 7, titulo.section(" (", 0, 0).
                       replace('_', ' ').left(length));
        else  {
            s_num = QString::number(graphs_ids[i]);
            length = length > int(s_num.length()) ?
                        int(s_num.length()) : length;
            p.drawText(3, 7, QString::number(graphs_ids[i]).
                       rightJustified(length, ' ').right(length));
        }
        p.translate(30 + (length + 3)*3, 0);
    }
    p.restore();

}

void GraphicMonitor::restoreGraphs(void)
{
    int pix_x;
    int pix_y;
    QPoint prev_pix;
    p.save();
    p.translate(XGRAPH, YGRAPH);
    int t_org = crono - crono % (xupdate * 1000);

    for (int i = n_graphs - 1; i >= 0; i--) {
        QVector<ValStorage> vector = graphsStorage.at(i);
        uint v_count = vector.count();
        if (!v_count) continue;
        pen.setColor(*(fore_colors.at(i)));
        p.setPen(pen);
        pix_x = NDMath::roundInt(x_pixel * (vector[v_count - 1].first % (xupdate * 1000)));
        pix_y = getPixVal(vector[v_count - 1].second);
        if (!(v_count - 1) && !accurateRestore)
            p.drawPoint(QPoint(pix_x, pix_y));

        old_tval[i].setX(pix_x);
        old_tval[i].setY(pix_y);
        prev_pix.setX(pix_x);
        prev_pix.setY(pix_y);

        for (int j = (int)v_count - 2; j >= 0; j--) {
            if (vector[j].first >= t_org) {
                pix_x = NDMath::roundInt(x_pixel * (vector[j].first % (xupdate * 1000)));
                pix_y = getPixVal(vector[j].second);
                QPoint pix = QPoint(pix_x, pix_y);
                if (accurateRestore) {
                    if (prev_pix != pix)
                        p.drawLine(prev_pix, pix);
                }
                else
                    p.drawLine(prev_pix, pix);
                prev_pix = pix;
            }
            else {
                if (prev_pix.x() != 0) {
                    p.drawLine(prev_pix, QPoint(0, prev_pix.y()));
                    // así con accurateRestore = false no hay que guardar cada ValStorage
                    // sino los que corresponden a píxeles distintos
                    // (ver paintVals() último addStorage()),
                    // si faltan tiempos para llegar al origen de tiempos actual (x_pixel = 0)
                    // es porque corresponden a píxeles iguales prev_pix.
                    // esto puede reducir la cantidad de memoria en un
                    // factor de 30 a 50 al monitorizar dispositivos ISO
                    // en la simulación "khepera-iso.sim"
                }
                j = 0;
            }
        }
    }
    //   p.end();
    p.restore();
}

void GraphicMonitor::paintVals(double val)
{   
    paintVals(crono, val, 1);
}

void GraphicMonitor::paintVals(double val, int gr_id)
{    
    paintVals(crono, val, gr_id);
}


void GraphicMonitor::paintVals(int t, double val)
{   
    crono = t;
    paintVals(crono, val, 1);
}

void GraphicMonitor::paintVals(int t, double val, int gr_id)
{

    int time = t % (xupdate * 1000);
    int pix_x;
    int pix_val;
    QPoint trans_p(XGRAPH, YGRAPH);
    int graph;

    if ((graph = graphs_ids.indexOf(gr_id)) == -1) return;

    pix_val = getPixVal(val);

    if (!time || reset_init_crono) {
        if (t != init_crono) {
            --g_cnt_signal;
            pix_x = 0;
            old_tval[graph] = QPoint(pix_x, pix_val);
            new_paintval[graph] = old_tval[graph];
            update(QRect(pix_x + XGRAPH, pix_val + YGRAPH, 1, 1));
            if (!g_cnt_signal) {
                ++graph_cnt;
                g_cnt_signal = n_graphs;
                clearGraphsStorage();
                update();
            }
            init_val_restored[graph].first = t;
            init_val_restored[graph].second = val;
            if (g_cnt_signal == n_graphs) {
                for (int i = 0; i < n_graphs; i++)
                    addStorage(i, init_val_restored[i]);
            }
        }
        else {
            pix_x = NDMath::roundInt(x_pixel * (init_crono % (xupdate * 1000)));
            old_tval[graph] = QPoint(pix_x, pix_val);
            new_paintval[graph] = old_tval[graph];
            update(QRect(pix_x + XGRAPH, pix_val + YGRAPH, 1, 1));

            addStorage(graph, t, val);
            if (accurateRestore) {
                init_val_restored[graph].first = t;
                init_val_restored[graph].second = val;
            }
            --g_cnt_signal;
            if (!g_cnt_signal) {
                reset_init_crono = false;
                g_cnt_signal = n_graphs;
            }
        }
    }
    else {
        pix_x = NDMath::roundInt(x_pixel * time);
        QPoint pix_p = QPoint(pix_x, pix_val);
        if (accurateRestore) {
            if (init_val_restored[graph].second != val) {
                if (prev_val_restored_equal[graph]) {
                    addStorage(graph, init_val_restored[graph]);
                    prev_val_restored_equal[graph] = false;
                }
                addStorage(graph, t, val);
            }
            else
                prev_val_restored_equal[graph] = true;
            init_val_restored[graph].first = t;
            init_val_restored[graph].second = val;
        }
        if (old_tval[graph] != pix_p) {
            new_paintval[graph] = pix_p;
            update(QRect(normalizeRect(old_tval[graph], new_paintval[graph])));

            if (!accurateRestore)
                addStorage(graph, t, val);
        }
    }
}

QRect GraphicMonitor::normalizeRect(const QPoint& point1, const QPoint& point2) 
{
    // Normalizar los rects, o sea, que esten dados por su top-left, width y  height y trasladarlos

    return NDMath::normalizeRect(point1, point2).translated(XGRAPH, YGRAPH);
}

int GraphicMonitor::getPixVal(const double& val)
{
    int pix_value = NDMath::roundInt(GRAPH_HEIGHT / 2.0 -
                                     y_pixel * (val - (ymax + ymin) / 2.0));
    //   int pix_value = int(y_pixel*(val - (ymax + ymin)/2.0) + 0.5);
    if (pix_value < 0) pix_value = 0;
    else if (pix_value > GRAPH_HEIGHT) pix_value = GRAPH_HEIGHT;
    return pix_value;
}

void GraphicMonitor::addStorage(int gr_idx, int t, double val)
{
    ValStorage tval(t, val);
    graphsStorage[gr_idx].append(tval);
}

void GraphicMonitor::addStorage(int gr_idx, ValStorage tval)
{
    graphsStorage[gr_idx].append(tval);
}

void GraphicMonitor::clearGraphsStorage(void)
{
    VectorStorage::Iterator it;
    for (it = graphsStorage.begin(); it != graphsStorage.end(); ++it)
        (*it).clear();
    prev_val_restored_equal.fill(false, n_graphs);
}

void GraphicMonitor::setNXLabels(int nl)
{   
    n_xlabels = nl;
}

void GraphicMonitor::setNYLabels(int nl)
{   
    n_ylabels = nl;
    if (ymax > ymin)
        y_pixel = GRAPH_HEIGHT * (double(n_ylabels - 1) / double(n_ylabels + 1) /(ymax - ymin));
}


void GraphicMonitor::reset(void)
{

    g_cnt_signal = n_graphs;
    clearGraphsStorage();
    if (!init_reset_init_crono) {
        crono = 0;
        reset_init_crono = false;
        graph_cnt = 0;
    }
    if (n_graphs) {
        old_tval.fill(QPoint(0, 0), n_graphs);
        new_paintval.fill(QPoint(0, 0), n_graphs);
    }
    update();
}

void GraphicMonitor::setCronoReset(bool reset)
{    
    reset_init_crono = reset;
    init_reset_init_crono = reset;
}

void GraphicMonitor::setParams(int xupd, int n_xlab, double y_min, double y_max,
                               int n_ylab, QColor *fore, QColor *back)
{

    setColors(fore, back);
    if (xupd < 1)
        xupdate = 1;
    else
        xupdate = xupd;
    if (n_xlab < 2)
        n_xlabels = 2;
    else
        n_xlabels = n_xlab;
    if (n_ylab < 1)
        n_ylabels = 1;
    else
        n_ylabels = n_ylab;
    if (y_max <= y_min) {
        ymin = 0.0;
        ymax = 1.0;
    }
    else {
        ymin = y_min;
        ymax = y_max;
    }
    x_pixel = GRAPH_WIDTH / double(xupdate * 1000);
    y_pixel = GRAPH_HEIGHT * (double(n_ylabels - 1) / double(n_ylabels + 1) / (ymax - ymin));
    update();

}

void GraphicMonitor::setParams(int xupd, int n_xlab, double y_min, double y_max, int n_ylab, const QString& cap, bool back)
{

    if (back != getBackColor() || !bck_color) {
        back_color = back;
        if (back) {  // papel
            if (bck_color) {
                delete bck_color;
                delete fore_color;
            }
            bck_color = new QColor(Qt::white);
            back_created_here = true;
        }
        else {
            bck_color = new QColor(Qt::black);  // osciloscopio
            back_created_here = true;
        }
    }

    setWindowTitle(cap);
    setMultiColors();
    setParams(xupd, n_xlab, y_min, y_max, n_ylab, fore_color, bck_color);

}

void GraphicMonitor::setParams(const QDomElement& e)
{

    bool ok;
    QString cap = e.attribute("caption");
    bool back = bool(e.attribute("back_color").toInt(&ok));
    int xupd = e.attribute("xupdate").toInt(&ok);
    int n_xlab = e.attribute("num_xlabels").toInt(&ok);
    int n_ylab = e.attribute("num_ylabels").toInt(&ok);
    double y_min = e.attribute("y_min").toDouble(&ok);
    double y_max = e.attribute("y_max").toDouble(&ok);
    setParams(xupd, n_xlab, y_min, y_max, n_ylab, cap, back);

}
void GraphicMonitor::setColors(QColor *fore, QColor *back)
{

    if (fore)
        fore_color = fore;
    else {
        fore_color = new QColor(Qt::black);
        fore_created_here = true;
    }
    if (back)
        bck_color = back;
    else {
        bck_color = new QColor(Qt::white);
        back_created_here = true;
    }
    if (n_graphs > 0) {
        setMultiColors();
        fore_color = fore_colors.at(0);
    }

}

void GraphicMonitor::setMultiColors()
{

    for(int i = 0; i < n_graphs; i++)
        delete fore_colors[i];
    fore_colors.clear();

    if (*bck_color == QColor(Qt::white)) {  // papel
        if (n_graphs > 0)
            fore_colors.append(new QColor(Qt::black));
        if (n_graphs > 1)
            fore_colors.append(new QColor("#e00000"));
        if (n_graphs > 2)
            fore_colors.append(new QColor(Qt::blue));
        if (n_graphs > 3)
            fore_colors.append(new QColor("#009000"));
        if (n_graphs > 4)
            fore_colors.append(new QColor("#c05000"));
        if (n_graphs > 5)
            fore_colors.append(new QColor("#b000b0"));
    }
    else {                 // osciloscopio
        if (n_graphs > 0)
            fore_colors.append(new QColor("#ffe000"));
        if (n_graphs > 1)
            fore_colors.append(new QColor("#ff0000"));
        if (n_graphs > 2)
            fore_colors.append(new QColor("#00ff00"));
        if (n_graphs > 3)
            fore_colors.append(new QColor("#07f7f0"));
        if (n_graphs > 4)
            fore_colors.append(new QColor("#f777f7"));
        if (n_graphs > 5)
            fore_colors.append(new QColor("#ffffe0"));
    }
}

void GraphicMonitor::addMultiColors()
{
    if (*bck_color == QColor(Qt::white)) {  // papel
        if (n_graphs == 1)
            fore_colors.append(new QColor(Qt::black));
        else if (n_graphs == 2)
            fore_colors.append(new QColor("#e00000"));
        else if (n_graphs == 3)
            fore_colors.append(new QColor(Qt::blue));
        else if (n_graphs == 4)
            fore_colors.append(new QColor("#009000"));
        else if (n_graphs == 5)
            fore_colors.append(new QColor("#c05000"));
        else if (n_graphs == 6)
            fore_colors.append(new QColor("#b000b0"));
    }
    else {                 // osciloscopio
        if (n_graphs == 1)
            fore_colors.append(new QColor("#ffe000"));
        else if (n_graphs == 2)
            fore_colors.append(new QColor("#ff0000"));
        else if (n_graphs == 3)
            fore_colors.append(new QColor("#00ff00"));
        else if (n_graphs == 4)
            fore_colors.append(new QColor("#07f7f0"));
        else if (n_graphs == 5)
            fore_colors.append(new QColor("#f777f7"));
        else if (n_graphs == 6)
            fore_colors.append(new QColor("#ffffe0"));
    }

}

bool GraphicMonitor::edit(void)
{

    bool ret = false;
    graphmonDialog *dialog =
            new graphmonDialog(0, 0, false);
    dialog->move(100, 100);
    dialog->init(this, 4, 5);
    if (dialog->exec() == graphmonDialog::Accepted) {
        ret = true;
    }
    delete dialog;
    return ret;

}

void GraphicMonitor::setXUpd(int xu)
{

    xupdate = xu;
    x_pixel = GRAPH_WIDTH / double(xupdate * 1000);

}

void GraphicMonitor::setYMin(double m)
{

    ymin = m;
    if (ymax > ymin)
        y_pixel = GRAPH_HEIGHT * (double(n_ylabels - 1) / double(n_ylabels + 1)
                                  / (ymax - ymin));

}

void GraphicMonitor::setYMax(double m)
{

    ymax = m;
    if (ymax > ymin)
        y_pixel = GRAPH_HEIGHT * (double(n_ylabels - 1) / double(n_ylabels + 1)
                                  / (ymax - ymin));

}

int GraphicMonitor::getNXLabels(void)
{
    return n_xlabels;
}

int GraphicMonitor::getNYLabels(void)
{  
    return n_ylabels;
}

int GraphicMonitor::getXUpd(void)
{
    return xupdate;
}

double GraphicMonitor::getYMin(void)
{   
    return ymin;
}

double GraphicMonitor::getYMax(void)
{  
    return ymax;
}

void GraphicMonitor::delGraph(int gr_id)
{

    int graph;

    if (!n_graphs) return;
    if ((graph = graphs_ids.indexOf(gr_id)) == -1) return;
    for (int i = graph; i < n_graphs - 1; i++)
        graphs_ids[i] = graphs_ids[i + 1];
    --n_graphs;
    graphs_ids.resize(n_graphs);
    ValStorage p(0, 0);
    init_val_restored.fill(p, n_graphs);
    prev_val_restored_equal.fill(false, n_graphs);
    graphsStorage.resize(n_graphs);
    if (n_graphs == 0)
        resizeMon();
    fore_colors.removeAt(graph);
}

bool GraphicMonitor::addGraphs(const QDomElement& e)
{

    QDomNode n_lin_gr = e.firstChild();
    bool ok;
    while (!n_lin_gr.isNull()) {
        QDomElement e_lin_gr = n_lin_gr.toElement();;
        if (e_lin_gr.tagName() == "graph_id") {
            if (!addGraph(e_lin_gr.text().toInt(&ok)))
                return false;
        }
        else {
            n_lin_gr = n_lin_gr.nextSibling();
            continue;
        }
        n_lin_gr = n_lin_gr.nextSibling();
    }
    return true;

}

bool GraphicMonitor::addGraph(int gr_id)
{

    if (n_graphs >= N_MAX_GRAPHS) {
        nMaxGraphsMessage();
        return false;
    }
    else {
        graphs_ids.append(gr_id);
        ValStorage p(0, 0);
        init_val_restored.append(p);
        prev_val_restored_equal.append(false);
        old_tval.append(QPoint(0, 0));
        new_paintval.append(QPoint(0, 0));
        QVector<ValStorage> list;
        graphsStorage.append(list);

        ++n_graphs;
        if (n_graphs == 1)
            resizeMon();
        g_cnt_signal = n_graphs;
        addMultiColors();

        return true;
    }

}

QVector<int> GraphicMonitor::idGraphs(void)
{   
    return graphs_ids;
}

void GraphicMonitor::initCrono(int cr)
{

    init_crono = cr;
    crono = init_crono;
    clearGraphsStorage();
    if (init_crono) {
        graph_cnt = init_crono / (1000 * xupdate);
        reset_init_crono = true;
        init_reset_init_crono = true;
    }
    else {
        graph_cnt = 0;
        reset_init_crono = false;
        init_reset_init_crono = false;
    }
    update();
    return;

}

void GraphicMonitor::mouseDoubleClickEvent(QMouseEvent *)
{
    if (edit())
        update();
}

void GraphicMonitor::write(QDomDocument& doc, QDomElement& e)
{

    XMLOperator* xml_operator = XMLOperator::instance();
    e.appendChild(xml_operator->createTextElement(doc,
                                                  "mon_identification", objectName()));
    QDomElement tag_cons = doc.createElement( "constructor" );
    e.appendChild(tag_cons);
    writeParams(tag_cons);
    QDomElement tag_graph = doc.createElement( "graphs" );
    e.appendChild(tag_graph);
    int num_graphs = graphs_ids.count();
    int num_neg = 0;
    std::sort(graphs_ids.begin(), graphs_ids.end());  // ordenar los negativos por valor absoluto

    QVector<int> gr_ids(num_graphs);
//    for(uint i = 0; i < num_graphs; i++) {
//        if(graphs_ids[num_graphs - 1 -i] < 0) {
//            gr_ids[i] = graphs_ids[num_graphs - 1 -i];
//            ++ num_neg;
//        }
//        else if(graphs_ids[num_graphs - 1 -i] > 0)
//            gr_ids[i] = graphs_ids[i - num_neg];
//    }
    for(int i = 0; i < num_graphs; i++) {
        if(graphs_ids[num_graphs - 1 -i] < 0) {
            gr_ids[num_neg] = graphs_ids[num_graphs - 1 - i];
            ++ num_neg;
        }
        else if(graphs_ids[num_graphs - 1 -i] > 0)
            gr_ids[num_graphs - 1 - i] = graphs_ids[num_graphs - 1 - i];
    }

    for(int i = 0; i < num_graphs; i++)
        graphs_ids[i] = gr_ids[i];
    for(uint i = 0; i < num_graphs; ++i)
        tag_graph.appendChild(xml_operator->createTextElement(doc,
                                                              "graph_id", QString::number(graphs_ids[i])));

}

void GraphicMonitor::writeParams(QDomElement& e)
{

    e.setAttribute("num_graphs", QString::number(n_graphs));
    e.setAttribute("caption", titulo);
    e.setAttribute("back_color", QString::number(back_color));
    e.setAttribute("xupdate", QString::number(xupdate));
    e.setAttribute("num_xlabels", QString::number(n_xlabels));
    e.setAttribute("num_ylabels", QString::number(n_ylabels));
    e.setAttribute("y_min", QString::number(ymin));
    QString ss = e.attribute("y_min");
    e.setAttribute("y_max", QString::number(ymax));

}

bool GraphicMonitor::getBackColor(void)
{   
    bool ret = back_color ? true : false;
    return ret;
}

void GraphicMonitor::setTitulo(const QString& title)
{
    titulo = title;
    update();
}

const QString& GraphicMonitor::getTitulo(void)
{ 
    return titulo;
}     
