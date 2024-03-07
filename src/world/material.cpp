//
// C++ Implementation: material
//
// Description: 
//
//
// Author: Jose Manuel Cuadra Troncoso <jmcuadra@dia.uned.es>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "material.h"

#include <qewdialogfactory.h>

#include "../neuraldis/ndmath.h"
#include "../robot/abstractsensor.h"

Material* Material::factoryDefaultMaterial(int sensorType, bool real)
{
  Material* material = 0;
  switch(sensorType) {
    case AbstractSensor::RANGE_LINE_SENSOR :
      if(real)
        material = new Material(AbstractSensor::RANGE_LINE_SENSOR, 80/NDMath::RAD2GRAD, 85/NDMath::RAD2GRAD, 0, 0);
      else
        material = new Material(AbstractSensor::RANGE_LINE_SENSOR, 90/NDMath::RAD2GRAD, 90/NDMath::RAD2GRAD, 0, 0);
      break;
    case AbstractSensor::RANGE_BEAM_SENSOR :
      if(real)
        material = new Material(AbstractSensor::RANGE_BEAM_SENSOR, 30/NDMath::RAD2GRAD, 60/NDMath::RAD2GRAD, 0, 0);
      else
        material = new Material(AbstractSensor::RANGE_LINE_SENSOR, 90/NDMath::RAD2GRAD, 90/NDMath::RAD2GRAD, 0, 0);
      break;
    case AbstractSensor::BUMP_LINE_SENSOR :
      material = new Material(AbstractSensor::BUMP_LINE_SENSOR, 90/NDMath::RAD2GRAD, 90/NDMath::RAD2GRAD, 0, 0);
      break;
    case AbstractSensor::FAKE_BLOB_CAMERA :
      if(real)
        material = new Material(AbstractSensor::FAKE_BLOB_CAMERA, 90/NDMath::RAD2GRAD, 90/NDMath::RAD2GRAD, 0, 0);
      else
        material = new Material(AbstractSensor::FAKE_BLOB_CAMERA, 90/NDMath::RAD2GRAD, 90/NDMath::RAD2GRAD, 0, 1);  // transparente
      break;                  
  }
  return material;
}

Material::Material()
{
  //a_hit = 90/NDMath::RAD2GRAD;
  //a_max = 90/NDMath::RAD2GRAD;
//   a_hit = 90/NDMath::RAD2GRAD;
//   a_max = 90/NDMath::RAD2GRAD;
  sensorType = 0;
  w_rand = 0;
  a_hit = 90/NDMath::RAD2GRAD;
  a_max = 90/NDMath::RAD2GRAD;
  transparency = 0.0;
}

Material::Material(int sensorType, double a_hit, double a_max, double w_rand, double transparency)
{
  this->sensorType = sensorType;
  this->a_hit = a_hit;
  this->a_max = a_max;
  this->w_rand = w_rand;
  this->transparency = transparency;
}

Material::~Material()
{
}

double Material::randomize(double z_star, double incidence_angle, double lambda, double sigma, double w_short, int max_val)
{
  if(isTransparent()) return max_val;  // por ahora
  
  double w_hit, w_max;
  double w_short_rand  = w_short + w_rand;
  double abs_incidence_angle =fabs(incidence_angle);
  if(abs_incidence_angle < a_hit) {
    w_max = 0;
    w_hit =1 - w_short_rand;
  } 
  else if(abs_incidence_angle > a_max) {
    w_hit = 0;
    w_max =1 - w_short_rand;
  }
  else {
    w_max = (1 - w_short_rand)*(a_max - abs_incidence_angle)/(a_max - a_hit);
    w_hit = 1 - w_short_rand - w_max;
  }
  double ret = NDMath::randBeamModel(z_star, lambda, sigma, w_short, w_hit, w_max, w_rand, max_val);
  return ret;
}

double Material::randomBounce(double z_star, double incidence_angle, int max_val)
{
  if(isTransparent()) return max_val;
  
  double w_max;
  double ret;
  double abs_incidence_angle =fabs(incidence_angle);
  if(abs_incidence_angle < a_hit) {
    w_max = 0;          
  } 
  else if(abs_incidence_angle > a_max) {
    w_max =1;     
  }
  else {
    w_max = (a_max - abs_incidence_angle)/(a_max - a_hit);
  }
  if(w_max > NDMath::randDouble())
    ret = max_val;
  else
    ret = z_star;
  return ret;
}

void Material::setParams(const QDomElement& e) {
  // Establece los parámetros a_hit y a_max del material.
  bool ok;
  if (e.attribute("sensor_type") != QString()) {
    int l_sensorType = e.attribute("sensor_type").toInt(&ok);
    if(ok)
      sensorType = l_sensorType;
  }
  if (e.attribute("a_hit") != QString()) {
    double l_a_hit = e.attribute("a_hit").toDouble(&ok);
    if(ok)
      a_hit = l_a_hit;
  }    
  if (e.attribute("a_max") != QString()) {
    double l_a_max = e.attribute("a_max").toDouble(&ok);
    if(ok)
      a_max = l_a_max;
  }    
  if (e.attribute("transparency") != QString()) {
    double l_transparency = e.attribute("transparency").toDouble(&ok);
    if(ok)
      transparency = l_transparency;
  }    
  if (e.attribute("w_rand") != QString()) {
    double l_w_rand = e.attribute("w_rand").toDouble(&ok);
    if(ok)
      w_rand = l_w_rand;
  }    
}

void Material::getParams(QDomElement& e) {
  e.setAttribute("sensor_type", sensorType);
  e.setAttribute("a_hit", a_hit);
  e.setAttribute("a_max", a_max);
  e.setAttribute("transparency", transparency);
  e.setAttribute("w_rand", w_rand);

}

QewExtensibleDialog* Material::createDialog() {
  editLineDialog* dialog = new editLineDialog();
//   dialog->setParams(w_rand, a_hit, a_max, transparency);
  dialog->init((Material*) this);
  return dialog;
}

bool Material::edit(void)
{
  bool ret = false;
  QewDialogFactory *factory = new QewDialogFactory();
  QewExtensibleDialog* main_dialog = factory->createDialog(QewExtensibleDialog::QewSimpleWidget);
  if(main_dialog->setUpDialog()) {
    main_dialog->addExtensibleChild(createDialog(), "Material");
    ret = (main_dialog->exec() == QewExtensibleDialog::Accepted);
  }
  delete main_dialog;
  return ret;
}
