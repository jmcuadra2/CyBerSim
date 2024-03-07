#ifndef SELECTSYNAPSENETDIALOG_H
#define SELECTSYNAPSENETDIALOG_H

#include "ui_selectsynapsenetdialog.h"
#include "../../net/subnet.h"

#include <QVector>

class selectSynapseNetDialog : public QDialog, public Ui::selectSynapseNetDialog
{
    Q_OBJECT

public:
    selectSynapseNetDialog(QWidget* parent = 0, const char* name = 0, bool modal = false, Qt::WindowFlags fl = Qt::WindowFlags());
    ~selectSynapseNetDialog();

    virtual void setSubNets( QList<SubNet *> * subnets );
    virtual void setCurrentMinMaxValues( int ni, int nh, int no );

public slots:
    virtual int getNum_IdSynapse( void );
    virtual void setNum_IdSynapse( int tam );
    virtual int getNum_IdNeuronFrom( void );
    virtual bool setNum_IdNeuronFrom( int n_i );
    virtual int getLayerFrom( void );
    virtual int getLayerTo( void );
    virtual void setMinMaxValues( int n_i, int n_h, int n_o );
    virtual void setLayerFrom( int l_ );
    virtual void setLayerTo( int l_ );
    virtual void getTextFrom( const QString & texto );
    virtual void getTextTo( const QString & texto );
    virtual void accept();
    virtual void radioFromSubNet_pressed();
    virtual void radioFromNetwork_pressed();
    virtual void radioToSubNet_pressed();
    virtual void radioToNetwork_pressed();
    virtual bool setNum_IdNetFrom( int i );
    virtual bool setNum_IdNetTo( int i );
    virtual int getNum_IdNetFrom( void );
    virtual int getNum_IdNetTo();
    virtual void updateFromNeurons( int n_ );
    virtual void updateToNeurons( int n_ );
    virtual void updateFromCurrentNetwork();
    virtual void updateToCurrentNetwork();
    virtual void setMinMaxValues( int n_i, int n_h, int n_o,
                                  QList<Neuron*> In_Ptr, QList<Neuron*> Hid_Ptr,
                                  QList<Neuron*> Out_Ptr );

protected slots:
    virtual void languageChange();

protected:
    QList<Neuron *> OutputPtr;
    QList<Neuron *> HiddenPtr;
    QList<Neuron *> InputPtr;
    QVector<uint> num_syn;
    int num_IdSynapse;
    int current_no;
    int current_nh;
    int current_ni;
    QList<SubNet *> *subnets;
    int n_out;
    int n_in;
    int num_IdNeuronFrom;
    int layerFrom;
    int n_hid;
    int num_IdNeuronTo;
    int layerTo;
    int num_IdNetFrom;
    int num_IdNetTo;

    QButtonGroup* buttonGroupTo;
    QButtonGroup* buttonGroupFrom;

};

#endif // SELECTSYNAPSENETDIALOG_H
