#ifndef EDITNEURONDIALOG_H
#define EDITNEURONDIALOG_H

#include "ui_editneurondialog.h"

class editNeuronDialog : public QDialog, public Ui::editNeuronDialog
{
    Q_OBJECT

public:
    editNeuronDialog(QWidget* parent = 0, const char* name = 0, bool modal = false, Qt::WindowFlags fl = Qt::WindowFlags());
    ~editNeuronDialog();

public slots:
    virtual int getNum_IdNeuron( void );
    virtual bool setNum_IdNeuron( int n_i );
    virtual int getLayer( void );
    virtual void setMinMaxValues( int n_i, int n_h, int n_o );
    virtual void setLayer( int l_ );
    virtual void getText( const QString & texto );

protected slots:
    void accept();
    virtual void languageChange();

protected:
    int n_out;
    int n_in;
    int num_IdNeuron;
    int layer;
    int n_hid;

    QButtonGroup* buttonGroupLayer;

};

#endif // EDITNEURONDIALOG_H
