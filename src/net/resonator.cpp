/***************************************************************************
                          resonator.cpp  -  description
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

#include <QMessageBox>

#include "resonator.h"
#include "../disview/gui/resonatordialog.h"
#include "../neuraldis/xmloperator.h"
#include "../neuraldis/ndmath.h"

double Resonator::sampling_period_base = 0.01;

Resonator::Resonator(int n_id, bool recmon,
                     QObject *parent, const char *name)
    : NeuralConnection(n_id, recmon, parent, name)
{

  defaultValues();

}

Resonator::Resonator(int n_id, double a, int n, bool cs,
                     double w, bool wf, bool posW, bool recmon,
                     QObject *parent, const char *name)
    : NeuralConnection(n_id, recmon, parent, name)
{

  if(a <= 0.0 || n < 1) return;
  damp = a;
  hper = n;
  th =  3.1415926535/hper;
  setSamplingPeriod(sampling_period);
  weightFix = wf;
  isCS = cs;
  isolink_weight = 0.0;
  weight = w;
  posWeight = posW;
  stabWeight = WS_SYMMETRIC;
  learningDecay = 0;
  sampling_period = 0.02;

}

Resonator::~Resonator()
{
}

void Resonator::defaultValues(bool to_base)
{

  if(to_base) NeuralConnection::defaultValues();
  damp = 3.0;
  hper = 100;
  isCS = true;
  weight = 0.0;
  weightFix = false;
  posWeight = true;
  stabWeight = WS_SYMMETRIC;
  learningDecay = 0;
  sampling_period = 0.02;

}

bool Resonator::setParams(const QDomElement& e)
{

  bool ok = false;
  bool tmp_ok = false;
  if(!e.isNull())
  {
    damp = e.attribute("damp").toDouble(&ok);
    hper = e.attribute("hper").toInt(&ok);
    isCS = bool(e.attribute("isCS").toInt(&ok));
    weight = e.attribute("weight").toDouble(&ok);
    weightFix = bool(e.attribute("weightFix").toInt(&ok));
    posWeight = bool(e.attribute("posWeight").toInt(&ok));
    is_recmonable = bool(e.attribute("isRecMonable").toInt(&tmp_ok));
    if(!tmp_ok)
      is_recmonable = true;
  }

  if(!ok)
  {
//    QMessageBox::warning(nullptr,tr("Resonator") + " " + QString::number(num_id),
//                         tr("Error in basic parameters, using defaults"),tr("&Ok"));
    QMessageBox::warning(nullptr,tr("Resonator") + " " + QString::number(num_id),
                         tr("Error in basic parameters, using defaults"), QMessageBox::Ok);
    defaultValues(false);
  }
  th =  3.1415926535/hper;
  setSamplingPeriod(sampling_period);
  isolink_weight = 0.0;
  return ok;

}

const QString Resonator::connName(bool isCap)
{

  return isCap ? tr("Resonator") : tr("resonator");

}

//double Resonator::flts(double xn)  hecha inline
//{
//
//  yn = (xn + c1*yn_1 - c2*yn_2)/norm;
//  yn_2 = yn_1;
//  yn_1 = yn;
//  return yn;
//
//}

bool Resonator::edit(bool mode, bool)
{

  bool ret = false;
  resonatorDialog *dialog =
    new resonatorDialog(0, 0, false);
  dialog->init(this, mode);
  if( dialog->exec() == resonatorDialog::Accepted )
  {
    acceptEdit();
    ret = true;
  }
  delete dialog;
  return ret;

}

void Resonator::acceptEdit(void)
{

  th = 3.1415926535/hper;
  setSamplingPeriod(sampling_period);
  emit changed();

}

void Resonator::setDamp(double a)
{

  damp = a;

}

double Resonator::getDamp(void)
{

  return damp;

}

void Resonator::setHalfPeriod(int hp)
{

  hper = hp;

}

int Resonator::getHalfPeriod(void)
{

  return hper;

}

//bool Resonator::isCStim(void)
//{
//
//  return isCS;
//
//}

void Resonator::setCStim(bool yes)
{

  isCS = yes;

}

void Resonator::setWeight(double w)
{

  weight = w;
  if(posWeight)
  {
    if(weight < 0.0) weight = 0.0;
  }

}

void Resonator::stabilizeWeight(double w, double /*weights_sum*/)
{

  double dif = w - weight;
  if(stabWeight == WS_SYMMETRIC)
    dif *= exp(-fabs(weight)*stabWeightVal);
  else if(stabWeight == WS_ASYMMETRIC)
  {
    if(dif >= 0.0)
      dif *= exp(-fabs(weight)*stabWeightVal);
    else
      dif *= exp(-fabs(weight)*stabWeightVal/(1 + fabs(weight)*stabWeightVal));
  }
  dif -= learningDecay*weight;
  weight += dif;
  if(posWeight)
  {
//     if(weight < 0.01*weights_sum) {
    if(weight < 0.0) {
      weight = 0.0;
//       dif = 0.0;
    }
  }
  emit weightDif(dif);
}

//void Resonator::addWeight(double w)
//{
//
//  weight += w*isolink_weight;
//
//}

void Resonator::write(QDomDocument& doc, QDomElement& e)
{

  XMLOperator* xml_operator = XMLOperator::instance();

  QDomElement tag_res = doc.createElement("Resonator");
  e.appendChild(tag_res);

  tag_res.appendChild(xml_operator->createTextElement(doc,
                      "type", "resonator"));
  tag_res.appendChild(xml_operator->createTextElement(doc,
                      "num_id", QString::number(num_id)));
  QDomElement tag_cons = doc.createElement("constructor");
  tag_res.appendChild(tag_cons);

  write_constructor(tag_cons);

}

void Resonator::write_constructor(QDomElement& e)
{

  e.setAttribute("damp", QString::number(damp));
  e.setAttribute("hper", QString::number(hper));
  e.setAttribute("isCS", QString::number(isCS));
  e.setAttribute("weight", QString::number(weight));
  e.setAttribute("weightFix", QString::number(weightFix));
  e.setAttribute("posWeight", QString::number(posWeight));
  e.setAttribute("isRecMonable",
                 QString::number(isRecMonable()));

}

int Resonator::type(void) const
{

  return ISO_RESONATOR;

}

int Resonator::superType(void) const
{

  return NeuralConnection::RESONATOR ;

}

//bool Resonator::getWeightFix(void)
//{
//
//  return weightFix;
//
//}

void Resonator::setWeightFix(bool w_fix)
{

  weightFix = w_fix;

}


double Resonator::getLinkWeight(void)
{

  return isolink_weight;

}


void Resonator::setLinkWeight(double link_w)
{

  isolink_weight = link_w;

}

void Resonator::setLearningDecay(double decay)
{
  learningDecay = decay*0.000001;
}

void Resonator::setSamplingPeriod(double s_period)
{
  if(s_period > 0.0) {
     s_period = sampling_period_base; // solo en prueba quitar cuanto antes

    sampling_period = s_period;
//     double final_sampling_period = sampling_period_base*sampling_period_base/sampling_period;
//     r = exp(-damp*final_sampling_period);
//     n_max = int(1/th*atan((th/final_sampling_period)/damp)-0.5);
//     damp *= sampling_period_base/sampling_period;
    
    r = exp(-damp*sampling_period);
    n_max = int(1/th*atan((th/sampling_period)/damp)-0.5);
    norm = pow(r, n_max)*sin((n_max+1)*th)/sin(th);
    c1 =  2*r*cos(th);
    c2 = r*r;
//     norm = 1/(1-c1+c2); // límite respuesta escalón
    yn = 0.0;
    yn_1 = 0.0;
    yn_2 = 0.0;
  } 
}

double Resonator::getSamplingPeriod(void)
{
  return sampling_period;
}

