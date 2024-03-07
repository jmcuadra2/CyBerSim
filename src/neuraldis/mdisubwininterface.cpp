//
// C++ Implementation: mdisubwininterface
//
// Description: 
//
//
// Author: Jose Manuel Cuadra Troncoso <jmcuadra@dia.uned.es>, (C) 2009
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "mdisubwininterface.h"
#include <QMdiArea>
#include <QMdiSubWindow>
#include <QLayout>

MdiSubWinInterface::MdiSubWinInterface()
{
    mdiSubWindow = 0;
}

MdiSubWinInterface::~MdiSubWinInterface()
{
//   mdiSubWindow->mdiArea()->removeSubWindow(mdiSubWindow);
    if(mdiSubWindow)
        mdiSubWindow->close();
}

void MdiSubWinInterface::setMdiSubWindow(QMdiSubWindow* mdiSubWindow) {
  this->mdiSubWindow = mdiSubWindow;
  mdiSubWindow->setAttribute(Qt::WA_DeleteOnClose);
  mdiSubWindow->adjustSize();
//   mdiSubWindow->update();
//   QLayout* layout = mdiSubWindow->widget()->layout();
//   if(!layout){}
    
//   this->mdiSubWindow->setGeometry(mdiSubWindow->x(), mdiSubWindow->y(), mdiSubWindow->widget()->frameGeometry().width(), mdiSubWindow->widget()->frameGeometry().height());
}

void MdiSubWinInterface::resizeMdi(QSize size)
{
  if(mdiSubWindow)
    mdiSubWindow->resize(size);
}
