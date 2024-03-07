/***************************************************************************
                          resonator.h  -  description
                             -------------------
    begin                : dom feb 22 2004
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

/**
  *@author Jose M. Cuadra Troncoso
  */

#ifndef RESONATOR_H
#define RESONATOR_H

#include "neuralconnection.h"

class Resonator : public NeuralConnection
{
    Q_OBJECT

  public:
    Resonator(int n_id, bool recmon = true,
                  QObject *parent = 0, const char *name = 0);   
    Resonator(int n_id, double a, int n, bool cs,
              double w, bool wf, bool posW, bool recmon = false,
              QObject *parent = 0, const char *name = 0);    
    ~Resonator();

    virtual bool setParams(const QDomElement& e);
    double flts(double xn);
    bool edit(bool mode = true, bool added = false);
    void setDamp(double a);
    double getDamp(void);
    void setHalfPeriod(int hp);
    int getHalfPeriod(void);
    bool isCStim(void) const;
    void setCStim(bool yes);
    bool getWeightFix(void);
    void setWeightFix(bool w_fix);
    double getLinkWeight(void);
    void setLinkWeight(double link_w);
    void stabilizeWeight(double w, double weights_sum = 0.0);       
    void setWeight(double w);
    const QString connName(bool isCap);
    void write(QDomDocument& doc, QDomElement& e);
    void write_constructor(QDomElement& e);
    int type(void) const;
    int superType(void) const;
    void setLearningDecay(double decay);
    void setSamplingPeriod(double s_period);
    double getSamplingPeriod(void);
    	
  public slots:
    void addWeight(double w);
//    void emitToMonitor(void);

  protected:
    void defaultValues(bool to_base = true); 
    void acceptEdit(void);    

  public:
    enum Types {ISO_RESONATOR = 100};

  protected:
    double damp, b, yn, yn_1, yn_2, f, Q, r, th, norm, c1, c2;
    int hper, n_max;
    bool isCS;
    bool weightFix;
    double isolink_weight;
    double learningDecay;
    double sampling_period;
    static double sampling_period_base;

};

inline double Resonator::flts(double xn)
{

  yn = xn + c1*yn_1 - c2*yn_2;
  yn_2 = yn_1;
  yn_1 = yn;
  return yn/norm;

};

inline bool Resonator::isCStim(void) const
{

  return isCS;

};

inline void Resonator::addWeight(double w)
{

  weight += w*isolink_weight;

};

inline bool Resonator::getWeightFix(void)
{

  return weightFix;

};

//inline void Resonator::emitToMonitor(void)
//{
//
//  emit emitMonVal(yn/norm, num_id);
//
//};

#endif
