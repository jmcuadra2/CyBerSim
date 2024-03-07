#ifndef DELNEURONDIALOG_H
#define DELNEURONDIALOG_H

#include "ui_delneurondialog.h"

class delNeuronDialog : public QDialog, public Ui::delNeuronDialog
{
    Q_OBJECT

public:
    delNeuronDialog(QWidget* parent = 0, const char* name = 0, bool modal = false, Qt::WindowFlags fl = Qt::WindowFlags());
    ~delNeuronDialog();

public slots:
    virtual int getNum_IdNeuron( void );
    virtual bool setNum_IdNeuron( int n_i );
    virtual int getLayer( void );
    virtual void setMinMaxValues( int n_i, int n_h, int n_o );
    virtual void setLayer( int l_ );
    virtual void getText( const QString & texto );

protected slots:
    void accept( void );
    virtual void languageChange();

protected:
    int num_IdNeuron;
    int layer;
    int n_hid;
    int n_in;
    int n_out;

    QButtonGroup* buttonGroupLayer;

};

#endif // DELNEURONDIALOG_H
