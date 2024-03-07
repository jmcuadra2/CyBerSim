#ifndef ADDNETWORKDIALOG_H
#define ADDNETWORKDIALOG_H

#include "ui_addnetworkdialog.h"

class addNetworkDialog : public QDialog, public Ui::addNetworkDialog
{
    Q_OBJECT

public:
    addNetworkDialog(QWidget* parent = 0, const char* name = 0, bool modal = false, Qt::WindowFlags fl = Qt::WindowFlags());
    ~addNetworkDialog();

    virtual int getNum_IdNetwork( void );
    virtual bool setNum_IdNetwork( int n_i );

public slots:
    virtual void getText( const QString & texto );

protected slots:
    void accept( void );

protected:
    int num_IdNetwork;

protected slots:
    virtual void languageChange();

};

#endif // ADDNETWORKDIALOG_H
