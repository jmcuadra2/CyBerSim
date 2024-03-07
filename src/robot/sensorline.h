//
// C++ Interface: sensorline
//
// Description:
//
//
// Author: Jose Manuel Cuadra Troncoso <jmcuadra@dia.uned.es>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef SENSORLINE_H
#define SENSORLINE_H

#include <QColor>

#include "robotposition.h"
#include "../neuraldis/ndmath.h"
#include "../world/drawings.h"
#include "../world/abstractworld.h"
#include "../world/calculadorbresenham.h"

/**
  @author Jose Manuel Cuadra Troncoso <jmcuadra@dia.uned.es>
*/

class BeamRandomizer;

class SensorLine
{
public:

    SensorLine(int id, double x_org, double y_org, double x_end, double y_end,
               double org_rotation, AbstractWorld* world, RobotPosition *rob_pos,
               const QString& visible_color, const QString& second_color,
               DrawingItem::Drawing type_draw, int sensorType = 0,
               bool rotateWithRobot = true);

    ~SensorLine();
    void advance(int stage);
    void rotate(void);
    void rotate(double rx, double ry, double ang);
    void rotate(double ang);
    void rotateTo(double ang);
    
    int sample(bool fullProb = true);
    int sampleSimple(bool fullProb = true);

    void scale(double x_reading, double y_reading);
    void setColor(const QColor& color);
    void saturateColor(double value);
    void setShowColor(bool show);

    void setSensorVals(int maxval, int wrongval);
    void setVisible(bool on) {
        if (drawing) drawing->setVisible(on) ;
    }

    QPointF getGlobalIntersectionPoint(void) {
        return QPointF(world->inverseXRobotTransform(global_intersection_point.x()), world->inverseYRobotTransform(global_intersection_point.y())) ;
    }

    QPointF getLocalIntersectionPoint(void) {
        return  QPointF(world->inverseXRobotTransform(local_intersection_point.x()) - world->inverseXRobotTransform(0), world->inverseYRobotTransform(local_intersection_point.y()) - world->inverseYRobotTransform(0)) ;
    }

    void setValidReading(bool valid);
    bool isValidReading(void);
    void setProbabilisticParams(double sigma);

    int getIdent() const {
        return ident ;
    }

    bool isUserVisible(void) const;
    void setLength(int lenght);

    CollisionSolver* getSolver() const {
        return solver;
    }

    double getLambda() const { return lambda; }
    double getWShort() const { return w_short; }
    double getWRand() const { return w_rand; }
    double getZStar() const { return z_star; }
    
    void setSensorType(int sensorType) { this->sensorType = sensorType; }
    double getOriginalRotation() const { return originalRotation; }

    void setRotateWithRobot(bool val){rotateWithRobot_ = val;}
    
    DrawingItem* getDrawingItem() const { return drawing; }
    QuadTree* getQuadtree();

public:
    enum noSample {SP_NO_SAMPLE = -1000000000};

private:
    RobotPosition *robot_position;
    double x_org_old_trans, y_org_old_trans, x_end_old_trans, y_end_old_trans;
    double rotation, originalRotation;
    int ident, val;
//    int x_org, yy_org, xx_end, yy_end;
    double  max_size;
    int max_val, wrong_val;

    DrawingItem::Drawing typeDraw;
    DrawingItem* drawing;
    AbstractWorld* world;
    QPointF global_intersection_point, local_intersection_point;

    double sigma, z_star, w_short, lambda, w_rand;
    CollisionSolver* solver;
    Material* material;

    bool validReading;
    int sensorType;
    bool rotateWithRobot_;

protected:
    CalculadorBresenham *calculador;
    QList<QPoint> lineaBresenham;
    QuadTree *qTree;
};

inline void SensorLine::rotate(double rx, double ry, double ang)
{    
    QPointF n_p;
    if (drawing) {
        n_p = NDMath::rotateGrad(x_end_old_trans, y_end_old_trans, rx, ry, ang);
        drawing->setGeometry(int(x_org_old_trans), int(y_org_old_trans), int(n_p.x()), int(n_p.y()));
    }
}

#endif
