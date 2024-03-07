#ifndef ADDSYNAPSENETDIALOG_H
#define ADDSYNAPSENETDIALOG_H

#include "ui_addsynapsenetdialog.h"
#include "../../net/subnet.h"
#include "../synapseviewfactory.h"

class QListWidgetItem;

class addSynapseNetDialog : public QDialog, public Ui::addSynapseNetDialog
{
    Q_OBJECT

public:
    addSynapseNetDialog(QWidget* parent = 0, const char* name = 0, bool modal = false, Qt::WindowFlags fl = Qt::WindowFlags());
    ~addSynapseNetDialog();

    virtual int getNum_IdNeuronFrom(void);
    virtual int getNum_IdNeuronTo(void);
    virtual int getLayerFrom(void);
    virtual int getLayerTo(void);
    virtual int getType(void);
    virtual void setType(int d);
    virtual void getTextFrom(const QString & texto);
    virtual void getTextTo(const QString & texto);
    virtual int getNum_IdNetFrom(void);
    virtual int getNum_IdNetTo();
    virtual void setSubNets(QList<SubNet *> * subnets);
    virtual void setCurrentMinMaxValues(int ni, int nh, int no);
    virtual bool isCurrentNetworkFrom(void);
    virtual bool isCurrentNetworkTo(void);

public slots:
    virtual bool setNum_IdNeuronFrom(int n_i);
    virtual bool setNum_IdNeuronTo(int n_i);
    virtual void setMinMaxValuesFrom(int n_i, int n_h, int n_o);
    virtual void setMinMaxValuesTo(int n_i, int n_h, int n_o);
    virtual void setLayerFrom(int l_);
    virtual void setLayerTo(int l_);
    virtual bool setNum_IdNetFrom(int i);
    virtual bool setNum_IdNetTo(int i);
    virtual void updateToNeurons(int);
    virtual void updateToCurrentNetwork();

protected slots:
    void accept();
    virtual void languageChange();

    virtual void setType(QListWidgetItem *item);
    virtual void radioFromNetView_pressed();
    virtual void radioFromNetwork_pressed();
    virtual void radioToNetView_pressed();
    virtual void radioToNetwork_pressed();
    virtual void updateFromNeurons(int);
    virtual void updateFromCurrentNetwork();

private:
    void init();

protected:
    bool current_network_from;
    int current_no;
    int current_nh;
    int current_ni;
    QList<SubNet *> *subnets;
    int num_IdNeuronFrom;
    int layerFrom;
    int num_IdNeuronTo;
    int layerTo;
    int num_IdNetFrom;
    int num_IdNetTo;
    bool current_network_to;
    int objectFrom_ni;
    int objectFrom_nh;
    int objectFrom_no;
    int objectTo_ni;
    int objectTo_nh;
    int objectTo_no;
    int type;
    NameClass_Map names_map;

    QButtonGroup* buttonGroupTo;
    QButtonGroup* buttonGroupFrom;

};

#endif // ADDSYNAPSENETDIALOG_H
