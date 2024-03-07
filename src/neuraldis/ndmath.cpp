/***************************************************************************
                          ndmath.cpp  -  description
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

#include "ndmath.h"
#include "abstractfunction.h"

//#include <stdlib.h>
#include <time.h>

NDMath NDMath::ndmath(0);
const double NDMath::RAD2GRAD = 180.0/M_PI;
const int NDMath::MAX_RAND = 1000;
const int NDMath::MAX_MOD = MAX_RAND*2;
const int NDMath::MAX_RAND2 = MAX_RAND*MAX_RAND;
const double NDMath::SQRT_2 = 1.4142135623730951;

double NDMath::randDouble(double min, double max, int prec)
{

    double mean = (max + min)/2.0;
    double ampl = max - min;
    double ret;
    int pot = 1;
    while(prec > 0) {
        pot *= 10;
        --prec;
    }
    int i_ampl = int(ampl*pot);
    ret = mean + double(rand()%i_ampl)/double(pot) - ampl/2.0;
    return ret;

}  

void NDMath::randRenew(void)
{

    srand((unsigned int)time((time_t *)NULL));

}
//void cartesianToPolar(double x, double y)
//{
//    modulus = sqrt(x*xi + y*y);
//    rotation = atan2(x, y);
//  return QPointF(modulus, rotation);
//}
double NDMath::randGaussian(bool reset)
{

    static bool generate = true;
    static double g_y = 0.0;
    int r2, x, y;
    double d_r2, d_x, d_y, d_sqrt, g_x;

    if(reset)
        generate = false;

    if(generate) {
        do {
            x = rand()%MAX_MOD - MAX_RAND;
            y = rand()%MAX_MOD - MAX_RAND;
            r2 = x*x + y*y;
        } while(r2 > MAX_RAND2 || r2 == 0);
        d_r2 = double(r2)/MAX_RAND2;
        d_x = double(x)/MAX_RAND;
        d_y = double(y)/MAX_RAND;
        d_sqrt = sqrt((-2*log(d_r2))/d_r2);
        g_x = d_x*d_sqrt;
        g_y = d_y*d_sqrt;
        generate = false;
        return g_x;
    }
    else {
        generate = true;
        return g_y;
    }

}

double NDMath::randBeamModel(double z_star, double lambda, double sigma, double w_short, double w_hit, double w_max, double w_rand, int max_val)
{
    double z_mix;
    double sum = w_short + w_hit + w_max + w_rand;
    if(sum <= 0)
        return z_star;
    else if(sum != 1) {
        w_short /= sum;
        w_hit /= sum;
        w_max /= sum;
        w_rand /= sum;
    }
    if((sigma <= 0 && w_hit >= 1))
        z_mix = z_star;
    else {
        double rand_status = randDouble();
        if(sum != 1) {
            w_short /= sum;
            w_hit /= sum;
            w_max /= sum;
            w_rand /= sum;
        }
        if(rand_status >= 1 - w_max)
            z_mix = max_val;
        else if(rand_status <= w_short) {
            double norma = 1 - exp(-lambda*z_star);
            z_mix = -log(1 - randDouble()* norma)/lambda;
        }
        else if(rand_status <= w_short + w_hit) {
            //      double under_cero = cdfStdGaussian(-z_star/sigma);
            //      double under_max = cdfStdGaussian((max_val - z_star)/sigma);
            //      double rand_gauss = randGaussian();
            //      while((rand_gauss < under_cero) && (rand_gauss > under_max))
            //        rand_gauss = randGaussian();
            //      z_mix = rand_gauss*sigma + z_star;
            ////       z_mix = (1 + rand_gauss*sigma/1000)*z_star; // 1000 mm sigma unitaria a 1m.
            z_mix = randDouble(-1, 1)*sigma*sqrt(3) + z_star; // ruido uniforme
        }
        else {
            z_mix = randDouble()*max_val;
        }
    }
    return z_mix;
}

// double NDMath::randTruncatedGaussian(double unif_x, double unif_y)
// {
// 
//   double r2, x, y;
//   double d_sqrt, g_x;
// 
//   do {
//     x = unif_x*2 - 1;
//     y = unif_y*1 - 1;
//     r2 = x*x + y*y;
//   } while(r2 > 1 || r2 == 0);
//   d_sqrt = sqrt((-2*log(r2))/r2);
//   g_x = x*d_sqrt;
//   g_y = y*d_sqrt;
// 
//   return g_x;
// 
// }

AbstractFunction* NDMath::factoryFunction(int type)
{

    AbstractFunction* f = nullptr;
    switch(type) {
    case NDMath::Identity :
        f = (AbstractFunction*)new IdentityFunction();
        break;
    case NDMath::Sigmoidal :
        f = (AbstractFunction*)new SigmoidalFunction();
        break;
    case NDMath::Perceptron :
        f = (AbstractFunction*)new PerceptronFunction();
        break;
        // se han anadido las funciones matematicas para la entrada y la salida
    case NDMath::Input :
        f = (AbstractFunction*)new InputFunction();
        break;
    case NDMath::Output :
        f = (AbstractFunction*)new OutputFunction();
        break;
    default:
        f = (AbstractFunction*)new IdentityFunction();
    }
    return f;

}

QRect NDMath::normalizeRect(const QPoint& point1, const QPoint& point2)
{
    int x_rect, y_rect, w_rect, h_rect;

    if(point1.x() <= point2.x()) {
        x_rect = point1.x();
        w_rect = point2.x() - point1.x();
    }
    else {
        x_rect = point2.x();
        w_rect = point1.x() - point2.x();
    }
    if(point1.y() <= point2.y()) {
        y_rect = point1.y();
        h_rect = point2.y() - point1.y();
    }
    else {
        y_rect = point2.y();
        h_rect = point1.y() - point2.y();
    }
    return QRect(x_rect, y_rect, w_rect + 1 , h_rect + 1);
}

void NDMath::scaleRect(QRectF &rect, double scale)
{
    QSizeF _size = rect.size();
    _size *= scale;
    rect.setSize(_size);
    QPointF pos(rect.x()*scale, rect.y()*scale);
    rect.moveTo(pos);
}
