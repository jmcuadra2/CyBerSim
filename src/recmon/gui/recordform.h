//
// C++ Interface: recordform
//
// Description:
//
//
// Author: Jose Manuel Cuadra Troncoso <jmcuadra@dia.uned.es>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef RECORDFORM_H
#define RECORDFORM_H



#include <qewextensibledialog.h>

#include "ui_recordform.h"

class FileRecorder;

/**
  @author Jose Manuel Cuadra Troncoso <jmcuadra@dia.uned.es>
*/
class RecordForm : public QewExtensibleDialog, protected Ui::RecordForm
{
  Q_OBJECT
  public:
    RecordForm (QWidget* parent = 0, const char* name = 0);

    ~RecordForm();

    virtual bool getFileRec ( void );
    virtual QString getFile ( void );
    void setId ( QString const& /*id*/ ){};
    virtual void init(FileRecorder* recorder);

  public slots:
    virtual void setFileRec ( bool fr );
    virtual void setFile ( QString const& f );
    virtual void setIniDir ( QString const& d );
    virtual void chooseFile ( void );

  protected slots:
    void saveThis(void);

  protected:
    bool fileRec;
    QString ini_dir;
    QString fileName;
    QString id;
    FileRecorder* recorder;

};

#endif
