//
// C++ Interface: worldscaleform
//
// Description:
//
//
// Author: Jose Manuel Cuadra Troncoso <jmcuadra@dia.uned.es>, (C) 2009
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef WORLDSCALEFORM_H
#define WORLDSCALEFORM_H

#include <qewsimpledialog.h>
#include "ui_worldscaleform.h"

class AbstractWorld;

/**
  @author Jose Manuel Cuadra Troncoso <jmcuadra@dia.uned.es>
*/
class WorldScaleForm : public QewSimpleDialog, public Ui::WorldScaleForm
{

  Q_OBJECT
  public:
    WorldScaleForm(QWidget* parent = 0, const char* name = 0);

    ~WorldScaleForm();

    void init(AbstractWorld* world);

  protected slots:
    void saveThis(void);
    void setScale(int w_scale);

  private:
    int scale;
    AbstractWorld* world;

};

#endif
