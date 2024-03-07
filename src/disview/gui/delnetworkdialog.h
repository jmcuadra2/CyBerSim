#ifndef DELNETWORKDIALOG_H
#define DELNETWORKDIALOG_H

#include "ui_delnetworkdialog.h"

class delNetworkDialog : public QDialog, public Ui::delNetworkDialog
{
    Q_OBJECT

public:
    delNetworkDialog(QWidget* parent = 0, const char* name = 0, bool modal = false, Qt::WindowFlags fl = Qt::WindowFlags());
    ~delNetworkDialog();

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

#endif // DELNETWORKDIALOG_H
