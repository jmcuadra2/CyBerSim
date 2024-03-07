/***************************************************************************
                          abstractworld.h  -  description
                             -------------------
    begin                : Tue Nov 11 2003
    copyright            : (C) 2003 by Jose M. Cuadra Troncoso
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


#ifndef ABSTRACTWORLD_H
#define ABSTRACTWORLD_H

#include <QGraphicsScene>
#include <QObject>
#include <QTextStream>
#include <QMap>
#include <QList>
#include <QGraphicsItem>
#include <QGraphicsView>
#include "quadtree.h"
#include <QRect>

#include "collisionsolver.h"
#include "lamp.h"
#include "calculadorbresenham.h"

class QMdiArea;
class SensorLine;
class DrawingItem;
class WorldView;
class SolidObject;
class Material;

//  #define SCALE_FIG_2_PIXEL        10  // unidades xfig por pixel

class AbstractWorld : public QObject
{

    Q_OBJECT
protected:
    AbstractWorld( QMdiArea *w_work = 0, QObject *parent=0, const char *name=0);

public:
    ~AbstractWorld();

    enum SequenceResolution {R_NATIVE, R_800x600, R_1024x768};
    enum SequenceFormat {PNG, JPG, TIFF, BMP};

    QMdiArea * getWorkspace(void);
    bool isEmpty(void);
    QGraphicsScene * getCanvas(void);
    int getWorldScale(void) {
        return world_scale ;
    }
    void setWorldScale(int world_scale) {
        this->world_scale = world_scale ;
    }
    WorldView * view();

    const QString& getFileName(void) {
        return worldName ;
    };
    void setFileName(const QString& w_name) {
        worldName = w_name;
    }
    double getXMinPosition(void) {
        return x_min_position ;
    }
    double getYMinPosition(void) {
        return y_min_position ;
    }
    double getXRobotHome(void) {
        return x_robot_home ;
    }
    double getYRobotHome(void) {
        return y_robot_home ;
    }
    double getHeadingRobotHome(void) {
        return heading_robot_home ;
    }
    double getHeight(void) const {
        return height ;
    } ;
    double getWidth(void) const {
        return width ;
    }

    virtual double xTransform(const double& x) {
        return x ;
    } // del mundo real al canvas

    virtual double yTransform(const double& y) {
        return y ;
    }

    virtual double headingTransform(const double& head) {
        return head ;
    }

    virtual double inverseXTransform(const double& x) {
        return x ;
    } // del canvas al mundo real

    virtual double inverseYTransform(const double& y) {
        return y ;
    }
    virtual double inverseHeadingTransform(const double& head) {
        return head ;
    }
    
    virtual double xRobotTransform(const double& x) {
        return x ;
    } // del mundo real al canvas

    virtual double yRobotTransform(const double& y) {
        return y ;
    }
    virtual double headingRobotTransform(const double& head) {
        return head ;
    }
    virtual double inverseXRobotTransform(const double& x) {
        return x ;
    } // del canvas al mundo real

    virtual double inverseYRobotTransform(const double& y) {
        return y ;
    }

    virtual double inverseHeadingRobotTransform(const double& head) {
        return head ;
    }

    QList<CollisionSolver*> getCollisionSolvers(void);
    QList<CollisionSolver*> getCollisionSolvers(const double& /*x1*/, const double& /*y1*/, const double& /*x2*/, const double& /*y2*/) {
        return solid_objects ;
    }

    void addSensorLine(SensorLine* const sensorLine, QGraphicsItem* const drawing);
    void addFreeAreaRib(SensorLine* sensorLine);
    void clearFreeAreaRibbing(void) {
        freeAreaRibbing.clear() ;
    }

    void changeRibColor(int lineIdx, double value);  // out = false --> mainColor

    virtual void exportToFig(QTextStream& /*ts*/, bool /*changeCoordinates*/) {}

    void sendSources(void);

    virtual int toFigUnits(void) const {
        return 1 ;
    }

    void scaleByUser(void);
    void writeXML(void);
    bool readXML(QString world);

    void setSensorsBoundingRect(QRect boundRect = QRect()) {
        this-> boundRect = boundRect ;
    }

    void setProbabilisticParams(double lambda, double w_short);
    double getLambda() const {
        return lambda;
    }

    double getW_short() const {
        return w_short;
    }

    double getW_rand() const {
        return w_rand;
    }

    void update(int crono = 0, int update_period = 1);

    void setFilmed(bool on);
    void setSequenceResolution(const SequenceResolution& resolution);
    SequenceResolution getSequenceResolution() const;
    void setSequenceFormat(const SequenceFormat& format);
    SequenceFormat getSequenceFormat() const;
    void setSequenceName(const QString& sequenceName);
    QString getSequenceName() const;
    bool videoPreferences(void);
    void centerInRobot(bool centeredInRobot);
    void setEmbeddedFullView(bool embeddedFullView) {
        this->embeddedFullView = embeddedFullView;
    }

    bool getEmbeddedFullView() const  {
        return embeddedFullView;
    }

    void getDefaultMaterials(void);
    void changeMaterials(void);

    void addRobotItem(QGraphicsItem* robotItem);

    void simClosed();
    void setUpScene(QRect worldRect);
    
    QuadTree* getQuadTree();
    CalculadorBresenham* getBresenhamCalculador() {return &bresenhamCalculador; }

public slots:
    void zoomOut(void);
    void zoomIn(void);
    void rotateView(void);
    virtual void setRobotRadius(const double& rad);
    void restoreInitialZoom(void);
    virtual void robotPosition(int x, int y);
    void showQuadtree(bool show);
    void drawCell(QPoint center, uint _size, Qt::GlobalColor color = Qt::green);
    void drawMovingBresenham(QList<QPoint> lineBres, Qt::GlobalColor color = Qt::red);

protected slots:
    virtual void setDragRect(QGraphicsRectItem* const rect);

signals:
    void worldSources(const QHash<QString, QList<RadialFieldSource*> >&);
    void worldChanged(void);
    void worldLoaded(void);
    void closeWorld(void);
    void worldCleared(void);
    void drawingsIdent(QList<int> );

protected:
    virtual bool load(QTextStream& ts) = 0;
    void reload(void);
    virtual void clear(void);
    virtual void clearFree(void);

    virtual QGraphicsLineItem* drawLine(const int& x_min_pos, const int& y_max_pos, const int& x_max_pos, const int& y_min_pos, QColor color,
                                        const bool& line_obstacle = false);
    virtual CollisionSolver* factoryCollisionSolver(const int& type, SolidObject* coll_object);

    virtual QList<QGraphicsItem*> selectedItems(int rtti);

    SolidObject* factorySolidObject(QGraphicsScene *w_canvas, int rtti, QPolygonF pa, uint thickness = 1, QColor pen_color = QColor("black"),
                                    QColor fill_color = QColor("silver"), float depth = 400, int area_fill = 1);
    SolidObject* factorySolidObject(QGraphicsScene *w_canvas, const QString& type,
                                    QPolygonF pa, uint thickness = 1, QColor pen_color = QColor("black"), QColor fill_color = QColor("silver"), float depth = 400, int area_fill = 1);

    QPolygonF getXMLPoints(const QDomElement& e_points, const QString& type);
    void setXMLMaterials(const QDomElement& e_materials, CollisionSolver* solver);
    void getXMLDynamics(const QDomElement& e_dynamics, CollisionSolver* solver);

    void addRFSource(QGraphicsScene* canvas, int x, int y, const QString& nameId);
    void drawGoalIcon(const int& x, const int& y, QColor color, const QString& texto, Qt::BrushStyle = Qt::SolidPattern);
    void addText(const int& x, const int& y, const QString& texto);

    QList<QPixmap> drawEllipsePix(double radius_x, double radius_y, double angle, int thickness = 1, QColor pen_color = QColor("black"), QColor fill_color = QColor("silver"), int area_fill = 1);

protected:
    QString worldName;
    bool world_empty;
    QMdiArea *w_space;
    QGraphicsScene *w_canvas;
    WorldView *canvas_view;
    QGraphicsView* fullSmallView;

    int world_scale;
    double x_min_position, y_min_position, x_max_position, y_max_position, width, height;
    double x_robot_home, y_robot_home;
    double heading_robot_home;
    int margin;
    int sceneMargin;

    QList<CollisionSolver*> solid_objects, reached_objects;
    QGraphicsRectItem *dragRect;
    QRect boundRect, oldBoundRect;
    QMap<QGraphicsItem*, SensorLine*> sensorLines;
    QList<SensorLine*> freeAreaRibbing;
    QHash<QString, QList<RadialFieldSource*> > sources_dict;

    QString worldText;

    bool reloading;

    double w_rand, lambda, w_short, a_hit, a_max, transparency;
    bool materialsDefined;
    QList<Material*> defaultMaterials;
    QDomElement defaultMaterialsDom;

    bool isFilmed;
    SequenceResolution sequenceResolution;
    SequenceFormat sequenceFormat;
    QString sequenceFormatName;
    QPoint sequenceResolutionValues;
    QString sequenceName;

    int xRobot, yRobot, prevXRobot, prevYRobot;
    double dist;
    bool centeredInRobot, embeddedFullView;

    int icon_goal_side;
    QColor c_orange, c_black, c_goal, c_home, c_head;

    QList<QGraphicsItem*> robotItems;
    QList<QGraphicsItem*> quadTreeItems;
    void drawTree(QVector<QPair<QPoint, uint> > *cells);
    CalculadorBresenham bresenhamCalculador;

    QuadTree *quadtree;
    CollisionSolver* pointSolver;

    QRectF quadTreeRect;
    QRectF worldRect;
    uint size, levels, levelsX,levelsY,sizeX,sizeY;
    double zoom;
    int cellSize;
    int quadCellSize;
    QPoint prevCellCenter;
    
    bool quadtreeCreated;

};

#endif
