#ifndef EDITINPUTNEURONTAB_H
#define EDITINPUTNEURONTAB_H

#include "ui_editinputneurontab.h"
#include <qewextensibledialog.h>

class InputNeuron;

class EditInputNeuronTab : public QewExtensibleDialog, public Ui::EditInputNeuronTab
{
    Q_OBJECT

public:
    EditInputNeuronTab(QWidget* parent = 0, const char* name = 0);
    ~EditInputNeuronTab();

    virtual void init( InputNeuron * in_neuron );
    virtual int getIdentification( void );
    virtual const QString& getGroupID( void );

public slots:
    virtual void setIdentification( int id );
    virtual void setGroupID(const QString& group );

protected:
    InputNeuron* input_neuron;
    int ident;
    QString sensorGroupID;

protected slots:
    virtual void languageChange();

    void saveThis( void );


};

#endif // EDITINPUTNEURONTAB_H
