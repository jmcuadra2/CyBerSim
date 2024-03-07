//
// C++ Interface: accontroltab
//
// Description:
//
//
// Author: Jose Manuel Cuadra Troncoso <jmcuadra@dia.uned.es>, (C) 2009
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef ACCONTROLTAB_H
#define ACCONTROLTAB_H

#include "ui_accontroltab.h"
#include <qewextensibledialog.h>

class AreaCenterReactiveControl;

/**
  @author Jose Manuel Cuadra Troncoso <jmcuadra@dia.uned.es>
*/
class ACControlTab : public QewSimpleDialog, public Ui::ACControlTab
{
    Q_OBJECT
  public:
    ACControlTab(QWidget* parent = 0, const char* name = 0);

    ~ACControlTab();

    virtual void init(AreaCenterReactiveControl* ctrl);

  protected slots:
    void setShowPath(bool showPath);

  protected:
    bool getShowPath(void);
    void saveThis(void);

  protected:
    bool showPath;
    AreaCenterReactiveControl* control;

};

#endif
