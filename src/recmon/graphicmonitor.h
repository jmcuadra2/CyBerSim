/***************************************************************************
                          graphicmonitor.h  -  description
                             -------------------
    begin                : mi�feb 4 2004
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

#ifndef GRAPHICMONITOR_H
#define GRAPHICMONITOR_H

#include <QWidget>
#include <QPainter>
# include <QList>
#include <QDomDocument>
#include <QPair>
#include <QPaintEvent>
#include <QMouseEvent>

/**
  *@author Jose M. Cuadra Troncoso
  */

class GraphicMonitor : public QWidget
{
    Q_OBJECT
    
    typedef QPair<int, double> ValStorage;
    friend class GraphsScreen;

public:
    GraphicMonitor(int xupd, int n_xlab,
                   double y_min, double y_max, int n_ylab,
                   const QString& cap, QColor *fore = 0, QColor *back = 0,
                   QWidget *parent=0, const char *name=0);
    GraphicMonitor(int n_g, QVector<int> grs_id, int xupd,
                   int n_xlab, double y_min, double y_max, int n_ylab,
                   const QString &cap, int back = 0, QWidget *parent = 0,
                   const char *name = 0);
    GraphicMonitor(const QDomElement& e, QWidget *parent = 0,
                   QString name = "");
    ~GraphicMonitor();

public:
    void setNXLabels(int nl);
    void setNYLabels(int nl);
    void setXUpd(int xu);
    void setYMin(double m);
    void setYMax(double m);
    int getNXLabels(void);
    int getNYLabels(void);
    int getXUpd(void);
    double getYMin(void);
    double getYMax(void);
    void setParams(int xupd, int n_xlab, double y_min, double y_max,
                   int n_ylab, QColor *fore = 0, QColor *back = 0);
    void setParams(int xupd, int n_xlab, double y_min,
                   double y_max, int n_ylab, const QString& cap, bool bck_color);
    virtual void setParams(const QDomElement& e);
    virtual bool edit(void);
    void setColors(QColor *fore = 0, QColor *back = 0);
    virtual void write(QDomDocument& doc, QDomElement& e);
    virtual void writeParams(QDomElement& e);
    void setMultiColors(void);
    void addMultiColors(void);
    bool addGraphs(const QDomElement& e);
    bool addGraph(int gr_id);
    void delGraph(int gr_id);
    QVector<int> idGraphs(void);
    bool getBackColor(void);
    static void nMaxGraphsMessage(void);
    void setTitulo(const QString& title);
    const QString& getTitulo(void);
    bool getAccurateRestore(void) { return accurateRestore ; }
    void setAccurateRestore(bool accres){ accurateRestore = accres ; }

public slots:
    void paintVals(int t, double val);
    virtual void paintVals(int t, double val, int gr_id);
    void paintVals(double val, int gr_id);
    void paintVals(double val);
    void reset(void);
    void Crono(int cr);
    void initCrono(int cr);
    void setCronoReset(bool reset);

protected slots:
    void paintEvent( QPaintEvent * );
    void mouseDoubleClickEvent(QMouseEvent * e);

protected:
    void drawCap(QString cap);
    void drawLabels(bool all = true);
    void drawGraph(void);
    void drawLegend(void);
    void restoreGraphs(void);
    void clearGraphsStorage(void);
    void init_common(int xupd, int n_xlab, double y_min, double y_max, int n_ylab,
                     const QString &cap, QColor *fore, QColor *back);
    void resizeMon(void);
    void setPainterWindow(void);
    void addStorage(int gr_idx, int t, double val);
    void addStorage(int gr_idx, ValStorage tval);
    int getPixVal(const double& val);
    QRect normalizeRect(const QPoint& point1, const QPoint& point2);

public:
    static int N_MAX_GRAPHS;

private:
    QVector<int> graphs_ids;
    int n_graphs;
    QString titulo;
    int xupdate, n_xlabels, n_ylabels;
    double ymin, ymax;
    QColor *fore_color, *bck_color;
    QList<QColor*> fore_colors;
    double x_pixel, y_pixel;
    QVector<QPoint> old_tval;
    QVector<QPoint> new_paintval;
    QVector<bool> prev_val_restored_equal;
    int graph_cnt;
    int g_cnt_signal;
    int crono, init_crono;
    bool reset_init_crono, init_reset_init_crono;
    int back_color;
    typedef QVector<QVector<ValStorage> > VectorStorage;
    VectorStorage graphsStorage;
    QPainter p_graph;
    QVector<ValStorage> init_val_restored;
    bool accurateRestore;
    bool fore_created_here;
    bool back_created_here;

    QPainter p;
    QPen pen;
    QRect graphsRect;
};

inline void GraphicMonitor::Crono(int cr)
{   
    crono = cr;
}

#endif
