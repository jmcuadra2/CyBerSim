#ifndef SYNAPSEVALUESDIALOG_H
#define SYNAPSEVALUESDIALOG_H

#include "ui_synapsevaluesdialog.h"
#include <qewextensibledialog.h>

class Synapse;

class synapseValuesDialog : public QewExtensibleDialog, public Ui::synapseValuesDialog
{
    Q_OBJECT

public:
    synapseValuesDialog(QWidget* parent = 0, const char* name = 0);
    ~synapseValuesDialog();

    virtual void init( Synapse * syn, bool mode, bool adding );
    virtual double getWeight( void );
    virtual uint getIdColor( void );
    virtual void setDecimalsMantisa( int n_d, int mant_ );

public slots:
    virtual void setNumId( int n_id );
    virtual void setWeight( double w_ );
    virtual void setWeightsList( void );
    virtual void setIdColor(int n_id);
    virtual void setIdColor(QTableWidgetItem * item);
    virtual void setWeight(QTableWidgetItem *item);
    virtual void setNewCaption( bool adding );

protected:
    bool rec_mon;
    int conn;
    QVector<uint> nonedit_pos;
    uint n_dec;
    double w;
    int n_item_color;
    uint mant;
    Synapse *synapse;
    bool mode_edit;

    virtual void saveThis( void );
    virtual QString formatFloat( double f );

protected slots:
    virtual void languageChange();
    virtual void setRecMonable( bool recmon );
    virtual void accept( void );

};

#endif // SYNAPSEVALUESDIALOG_H
