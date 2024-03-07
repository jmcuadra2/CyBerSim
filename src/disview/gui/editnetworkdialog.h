#ifndef EDITNETWORKDIALOG_H
#define EDITNETWORKDIALOG_H

#include "ui_editnetworkdialog.h"

class editNetworkDialog : public QDialog, public Ui::editNetworkDialog
{
    Q_OBJECT

public:
    editNetworkDialog(QWidget* parent = 0, const char* name = 0, bool modal = false, Qt::WindowFlags fl = Qt::WindowFlags());
    ~editNetworkDialog();

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

#endif // EDITNETWORKDIALOG_H
