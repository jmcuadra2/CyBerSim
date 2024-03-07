#ifndef SELECTSYNAPSEDIALOG_H
#define SELECTSYNAPSEDIALOG_H

#include "ui_selectsynapsedialog.h"
#include "../../net/neuron.h"

class selectSynapseDialog : public QDialog, public Ui::selectSynapseDialog
{
    Q_OBJECT

public:
    selectSynapseDialog(QWidget* parent = 0, const char* name = 0, bool modal = false, Qt::WindowFlags fl = Qt::WindowFlags());
    ~selectSynapseDialog();

public slots:
    virtual int getNum_IdSynapse( void );
    virtual int getNum_IdNeuronFrom( void );
    virtual bool validNum_IdNeuronFrom( int n_i );
    virtual void setNum_IdNeuronFrom( int n_i );
    virtual void setNum_IdSynapse( int tam );
    virtual int getLayerFrom( void );
    virtual void setMinMaxValues( int n_i, int n_h, int n_o, QList<Neuron *> In_Ptr,
                                  QList<Neuron *> Hid_Ptr, QList<Neuron *> Out_Ptr );
    virtual void setLayerFrom( int l_ );
    virtual void getTextFrom( const QString & texto );

protected slots:
    void accept();
    virtual void languageChange();

protected:
    QList<Neuron *> OutputPtr;
    QList<Neuron *> HiddenPtr;
    QList<Neuron *> InputPtr;
    int posInAxon;
    int n_out;
    int n_in;
    int num_IdNeuronFrom;
    int layerFrom;
    int n_hid;
    int num_IdSynapse;
    QVector<uint> num_syn;

    QButtonGroup* buttonGroupFrom;

};

#endif // SELECTSYNAPSEDIALOG_H
