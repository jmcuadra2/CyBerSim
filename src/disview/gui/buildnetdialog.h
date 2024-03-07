#ifndef BUILDNETDIALOG_H
#define BUILDNETDIALOG_H

#include "ui_buildnetdialog.h"

class buildNetDialog : public QDialog, public Ui::buildNetDialog
{
    Q_OBJECT

public:
    buildNetDialog(QWidget* parent = 0, const char* name = 0, bool modal = false, Qt::WindowFlags fl = Qt::WindowFlags());
    ~buildNetDialog();

public slots:
    virtual int getNumInput( void );
    virtual void setNumInput( int n );
    virtual int getNumHidden( void );
    virtual int getNumOutput( void );
    virtual void setNumOutput( int n );
    virtual void setNumHidden( int n );

protected slots:
    void accept();

protected:
    int numInput;
    int numOutput;
    int numHidden;

protected slots:
    virtual void languageChange();

};

#endif // BUILDNETDIALOG_H
