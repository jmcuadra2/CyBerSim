#ifndef ADDNEURONDIALOG_H
#define ADDNEURONDIALOG_H

#include "../neuronviewfactory.h"
#include "ui_addneurondialog.h"

class addNeuronDialog : public QDialog, public Ui::addNeuronDialog
{
    Q_OBJECT

public:
    addNeuronDialog(QWidget* parent = 0, const char* name = 0, bool modal = false, Qt::WindowFlags fl = Qt::WindowFlags());
    ~addNeuronDialog();

public slots:
    virtual int getNum_IdNeuron( void );
    virtual bool setNum_IdNeuron( int n_i );
    virtual int getLayer( void );
    virtual void setMinMaxValues( int n_i, int n_h, int n_o );
    virtual void setLayer( int l_ );
    virtual int getDevice( void );
    virtual void setDevice( int d );
    virtual void setDevice(QListWidgetItem *item );

protected slots:
    void accept();
    
protected:
  void init(void);

protected:
    NameClass_Map names_map;
    int n_out;
    int n_in;
    int num_IdNeuron;
    int layer;
    int n_hid;
    int device;
    QButtonGroup* buttonGroupLayer;

protected slots:
    virtual void languageChange();

};

#endif // ADDNEURONDIALOG_H
