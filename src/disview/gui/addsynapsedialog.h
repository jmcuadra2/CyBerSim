#ifndef ADDSYNAPSEDIALOG_H
#define ADDSYNAPSEDIALOG_H

#include "ui_addsynapsedialog.h"

class addSynapseDialog : public QDialog, public Ui::addSynapseDialog
{
    Q_OBJECT

public:
    addSynapseDialog(QWidget* parent = 0, const char* name = 0, bool modal = false, Qt::WindowFlags fl = Qt::WindowFlags());
    ~addSynapseDialog();

public slots:
    virtual int getNum_IdNeuronFrom( void );
    virtual int getNum_IdNeuronTo( void );
    virtual bool setNum_IdNeuronFrom( int n_i );
    virtual bool setNum_IdNeuronTo( int n_i );
    virtual int getLayerFrom( void );
    virtual int getLayerTo( void );
    virtual void setMinMaxValues( int n_i, int n_h, int n_o );
    virtual void setLayerFrom( int l_ );
    virtual void setLayerTo( int l_ );
    virtual void getTextFrom( const QString & texto );
    virtual void getTextTo( const QString & texto );

protected slots:
    void accept();
    virtual void languageChange();

protected:
    int n_out;
    int n_in;
    int num_IdNeuronFrom;
    int layerFrom;
    int n_hid;
    int num_IdNeuronTo;
    int layerTo;

    QButtonGroup* buttonGroupTo;
    QButtonGroup* buttonGroupFrom;

};

#endif // ADDSYNAPSEDIALOG_H
