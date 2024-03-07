//
// C++ Interface: mdisubwininterface
//
// Description:
//
//
// Author: Jose Manuel Cuadra Troncoso <jmcuadra@dia.uned.es>, (C) 2009
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef MDISUBWININTERFACE_H
#define MDISUBWININTERFACE_H

#include <QSize>

class QMdiSubWindow;

/**
  @author Jose Manuel Cuadra Troncoso <jmcuadra@dia.uned.es>
*/
class MdiSubWinInterface
{
  public:
    MdiSubWinInterface();

    virtual ~MdiSubWinInterface();

    void setMdiSubWindow(QMdiSubWindow* mdiSubWindow);

    QMdiSubWindow* getMdiSubWindow() const {
      return mdiSubWindow;
    }

    void resizeMdi(QSize size);

  private:
    QMdiSubWindow* mdiSubWindow;

};

#endif
