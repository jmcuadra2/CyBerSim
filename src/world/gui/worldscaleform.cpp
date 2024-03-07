//
// C++ Implementation: worldscaleform
//
// Description:
//
//
// Author: Jose Manuel Cuadra Troncoso <jmcuadra@dia.uned.es>, (C) 2009
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "worldscaleform.h"

#include "../abstractworld.h"

WorldScaleForm::WorldScaleForm(QWidget* parent, const char* name)
    : QewSimpleDialog(parent, name)
{
  setupUi(this);
  scale = 1;
  world = 0;

  QObject::connect(scaleSpinBox, SIGNAL(valueChanged(int)), this, SLOT(setScale(int)));
}


WorldScaleForm::~WorldScaleForm()
{
}

void WorldScaleForm::init(AbstractWorld* world)
{
  this->world = world;
  setScale(world->getWorldScale());
}

void WorldScaleForm::setScale(int w_scale)
{
  if(scale != w_scale) {
    scale = w_scale;
    scaleSpinBox->setValue(scale);
  }
}

void WorldScaleForm::saveThis(void)
{
  if(world)
    world->setWorldScale(scale);
//     world->setWorldScale(NDMath::roundInt(world->getWorldScale()/scale));
}
