//
// C++ Implementation: accontroltab
//
// Description: 
//
//
// Author: Jose Manuel Cuadra Troncoso <jmcuadra@dia.uned.es>, (C) 2009
//
// Copyright: See COPYING file that comes with this distribution
//
//

#include "accontroltab.h"
#include "../areacenterreactivecontrol.h"

ACControlTab::ACControlTab(QWidget* parent, const char* name) : QewSimpleDialog(parent, name)
{
  setupUi(this);

  connect(pathCheckBox, SIGNAL(toggled(bool)), this, SLOT(setShowPath(bool)));
}

ACControlTab::~ACControlTab()
{
}

void ACControlTab::init( AreaCenterReactiveControl * ctrl )
{
  control = ctrl;
  setShowPath(control->getShowPath());
}

void ACControlTab::setShowPath(bool showPath)
{
  if(this->showPath != showPath) {
    this->showPath = showPath;
    pathCheckBox->setChecked(showPath);
  }
}

bool ACControlTab::getShowPath(void)
{
  return showPath;
}

void ACControlTab::saveThis(void)
{
  control->setShowPath(getShowPath());
}
