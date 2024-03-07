#ifndef NEURONVALUESDIALOG_H
#define NEURONVALUESDIALOG_H

#include "ui_neuronvaluesdialog.h"
#include "../neuronviewfactory.h"
#include <qewextensibledialog.h>

class Neuron;
class QListWidgetItem;

class neuronValuesDialog : public QewExtensibleDialog, public Ui::neuronValuesDialog
{
    Q_OBJECT

public:
    neuronValuesDialog(QWidget* parent = 0, const char* name = 0);
    ~neuronValuesDialog();

    virtual void init(Neuron * n, bool mode, bool adding);
    virtual double getThreshold(void);
    virtual int getActivationFunction(void);
    virtual double getInitExtern(void);
    virtual void setDecimalsMantisa(int d, int m);
    virtual void setNewCaption(bool adding);

protected:
    Neuron *neuron;
    double initext;
    double thres;
    int type;
    int sub_layer;
    double out;
    int actFunc;
    bool rec_mon;
    bool mode_edit;
    NameClass_Map names_map;

    virtual double getOutput(void);
    virtual int getSubLayer(void);
    virtual double getSigmoidalA(void);
    virtual double getSigmoidalB(void);
    void saveThis(void);
    virtual void createActFuncList(void);

protected slots:
    virtual void languageChange();

    virtual void setSubLayer(int t);
    virtual void setActivationFunction(int d);
    virtual void setActivationFunction(QListWidgetItem* item);
    virtual void setSigmoidalValueA(double Value);
    virtual void setSigmoidalValueB(double Value);
    virtual void setInitExtern(double i);
    virtual void setOutput(double o);
    virtual void setThreshold(double t);
    virtual void setRecMonable(bool recmon);
    virtual void accept(void);
    virtual void ListBoxAF_selectionChanged(QListWidgetItem* newCurrent);


};

#endif // NEURONVALUESDIALOG_H
