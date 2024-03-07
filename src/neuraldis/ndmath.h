/***************************************************************************
                          ndmath.h  -  description
                             -------------------
    begin                : Tue Mar 1 2005
    copyright            : (C) 2005 by Jose M. Cuadra Troncoso
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

#ifndef NDMATH_H
#define NDMATH_H

#include <cmath>
#include <stdlib.h>
#include <QRect>
#include <QPointF>

#define vectorArray(x, mx) NDMath::containerFromArray<double>(x, mx, sizeof(mx))
// #include <QVector>

using namespace std;

class AbstractFunction;

/**
  *@author Jose M. Cuadra Troncoso
  */

class NDMath {

protected:
    static NDMath ndmath;
    NDMath(int) {}
    NDMath& operator=(NDMath&);
    NDMath(const NDMath&);

public:
    static void selfRotateGrad(double& x, double& y, const double& x_center,
                               const double& y_center, const double& ang);
    static void selfRotateGrad(QPointF& point, const double& ang, QPointF const& center = QPointF());
    static void selfRotateRad(double& x, double& y, const double& x_center,
                              const double& y_center, const double& ang);
    static void selfRotateRad(QPointF& point, const double& ang, QPointF const& center = QPointF());
    static QPointF& rotateGrad(double x, double y, double x_center,
                               double y_center, double ang);
    
    static QPointF& rotateGradCanvas(double x, double y, double x_center, double y_center, double ang);
    
    static QPointF& rotateRad(const double& x, const double& y,
                              const double& x_center, const double& y_center,
                              const double& ang);
    static QPointF& polarToCartesian(double r, double th);
    static QPointF& cartesianToPolar(double x, double y);
    
    static double randDouble(double min, double max, int prec = 4);
    static double randDouble4(double min, double max);
    static double randDouble(void);
    static void randRenew(void);
    static double randGaussian(bool reset = false);
    static double randBeamModel(double z_star, double lambda, double sigma, double w_short, double w_hit, double w_max, double w_rand, int max_val);
    static double cdfStdGaussian(double val);
    
    static AbstractFunction* factoryFunction(int type);
    
    static QRect normalizeRect(const QPoint& point1, const QPoint& point2);
    static void scaleRect(QRectF& rect, double scale);

    static int roundInt(double d) { return int(floor(d + 0.5)) ; }

    template<typename T, template <typename ELEM> class Container>
    static Container<size_t> sortContainer(Container<T>& container, bool ascending = true)
    {
        Container<size_t> permutedIndexes;
        bool swapped;
        T swap_val;
        size_t swap_idx;
        int containerSize = container.size();

        for(int i = 0; i < containerSize; i++)
            permutedIndexes.append(i);
        
        if(ascending) {
            do  {
                swapped = false;
                for(int i = 0; i < containerSize - 1; i++) {
                    if(container[i] > container[i + 1]) {
                        swap_val = container[i];
                        container[i] = container[i + 1];
                        container[i + 1] = swap_val;
                        swap_idx = permutedIndexes[i];
                        permutedIndexes[i] = permutedIndexes[i + 1];
                        permutedIndexes[i + 1] = swap_idx;
                        swapped = true;
                    }
                }
            } while(swapped);
        }
        else {
            do  {
                swapped = false;
                for(int i = 0; i < containerSize - 1; i++) {
                    if(container[i] < container[i + 1]) {
                        swap_val = container[i];
                        container[i] = container[i + 1];
                        container[i + 1] = swap_val;
                        swap_idx = permutedIndexes[i];
                        permutedIndexes[i] = permutedIndexes[i + 1];
                        permutedIndexes[i + 1] = swap_idx;
                        swapped = true;
                    }
                }
            } while(swapped);
        }

        return permutedIndexes;
    }

    template <typename T, template<typename ELEM> class Container>
    static void containerFromArray(Container<T>& cont, T array[], size_t asize)
    {
        cont.assign(&array[0], &array[asize/sizeof(T)]);
    }
    
public:
    static const double RAD2GRAD;
    // he cambiado este enum para que se añadan input y output
    // enum FunctionType {Identity, Sigmoidal, Perceptron, Parabolic};
    enum FunctionType {Identity, Sigmoidal, Perceptron, Parabolic, Input, Output};
    enum MeasureStatus {Short, Hit, Max, Rand};
private:
    static const int MAX_RAND;
    static const int MAX_MOD;
    static const int MAX_RAND2;
    static const double SQRT_2;
    
};

/**
  *@author Jose M. Cuadra Troncoso
  */



inline QPointF& NDMath::rotateGrad(double x, double y, double x_center, double y_center, double ang)
{

    static QPointF new_p;
    ang /= RAD2GRAD;
    new_p.setX(x_center + x*cos(ang) - y*sin(ang));
    new_p.setY(y_center + x*sin(ang) + y*cos(ang));
    return new_p;

}

inline QPointF& NDMath::rotateGradCanvas(double x, double y, double x_center, double y_center, double ang)
{

    static QPointF new_p;
    ang /= RAD2GRAD;
    new_p.setX(x_center + (x-x_center)*cos(ang) - (y-y_center)*sin(ang));
    new_p.setY(y_center + (x-x_center)*sin(ang) + (y-y_center)*cos(ang));
    return new_p;

};

inline void NDMath::selfRotateGrad(double& x, double& y, const double& x_center, const double& y_center, const double& ang)
{

    double rad_ang = ang/RAD2GRAD;
    double xx = x;
    double yy = y;
    x = x_center + xx*cos(rad_ang) - yy*sin(rad_ang);
    y = y_center + xx*sin(rad_ang) + yy*cos(rad_ang);

};

inline void NDMath::selfRotateGrad(QPointF& point, const double& ang, QPointF const& center)
{
    double x = point.x();
    double y = point.y();
    selfRotateGrad(x, y, center.x(), center.y(), ang);
    point.setX(x);
    point.setY(y);
};

inline QPointF& NDMath::rotateRad(const double& x, const double& y, const double& x_center, const double& y_center, const double& ang)
{

    static QPointF new_p;
    new_p.setX(x_center + x*cos(ang) - y*sin(ang));
    new_p.setY(y_center + x*sin(ang) + y*cos(ang));
    return new_p;

};

inline void NDMath::selfRotateRad(double& x, double& y, const double& x_center, const double& y_center, const double& ang)
{

    double xx = x;
    double yy = y;
    x = x_center + xx*cos(ang) - yy*sin(ang);
    y = y_center + xx*sin(ang) + yy*cos(ang);

};

inline void NDMath::selfRotateRad(QPointF& point, const double& ang, QPointF const& center)
{
    double x = point.x();
    double y = point.y();
    selfRotateRad(x, y, center.x(), center.y(), ang);
    point.setX(x);
    point.setY(y);
};

inline QPointF& NDMath::polarToCartesian(double r, double th)
{

    static QPointF new_p;
    th /= RAD2GRAD;
    new_p.setX(r*cos(th));
    new_p.setY(r*sin(th));
    return new_p;

};

inline QPointF& NDMath::cartesianToPolar(double x, double y)
{

    static QPointF new_p;
    new_p.setX(sqrt(x*x + y*y));
    double ang = atan2(y, x) * RAD2GRAD;
    new_p.setY(ang);
    //   new_p.setY(ang < 0 ? ang + 360 : ang);
    return new_p;

};

inline double NDMath::randDouble4(double min, double max)
{

    double ampl = max - min;
    double ret;
    int i_ampl = int(ampl*10000);
    ret = (max + min)/2.0 + double(rand()%i_ampl)/10000.0 - ampl/2.0;
    return ret;

};


inline double NDMath::randDouble(void)
{

    double ret = double(rand()%1000000)/1000000.0;
    return ret;

};   

inline double NDMath::cdfStdGaussian(double val) 
{
    return .5*(1 + erf(val/SQRT_2));
}

#endif
