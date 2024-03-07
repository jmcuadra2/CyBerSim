//
// C++ Interface: simfilmdialog
//
// Description:
//
//
// Author: Jose Manuel Cuadra Troncoso <jmcuadra@dia.uned.es>, (C) 2009
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef SIMFILMDIALOG_H
#define SIMFILMDIALOG_H

#include <qewsimpledialog.h>
#include "ui_simfilmdialog.h"

class AbstractWorld;

/**
  @author Jose Manuel Cuadra Troncoso <jmcuadra@dia.uned.es>
*/
class SimFilmDialog : public QewSimpleDialog, public Ui::SimFilmDialog
{
  Q_OBJECT
  public:
    SimFilmDialog(QWidget* parent = 0, const char* name = 0);

    ~SimFilmDialog();

    void init(AbstractWorld* world);
    
  protected slots:
    void saveThis(void);
//     void setResolution(const AbstractWorld::SequenceResolution& resolution);
    void setSequenceName(const QString& sequenceName);
//     void setFormat(const AbstractWorld::SequenceFormat& format);
    void setResolution(const int& resolution);
    void setFormat(const int& format);
    void chooseFolderSequence(void);
    void setEmbedded(bool embedded);
        
  private:
//     AbstractWorld::SequenceResolution resolution;
//     AbstractWorld::SequenceFormat format;
    int resolution;
    int format;
    QString sequenceName;    
    AbstractWorld* world;
    bool embeddedFullView;
};

#endif
