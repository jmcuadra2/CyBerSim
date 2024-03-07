#ifndef EDITHEBBIANSYNTAB_H
#define EDITHEBBIANSYNTAB_H

#include "ui_edithebbiansyntab.h"
#include <qewextensibledialog.h>

class HebbianSynapse;

class EditHebbianSynTab : public QewExtensibleDialog, public Ui::EditHebbianSynTab
{
    Q_OBJECT

public:
    EditHebbianSynTab(QWidget* parent = 0, const char* name = 0);
    ~EditHebbianSynTab();

    virtual void init( HebbianSynapse * hebb_syn );
    virtual double getLRate( void );

public slots:
    virtual void setLRate( double d );;

protected:
    HebbianSynapse* hebb_synapse;
    double learn_rate;

protected slots:
    virtual void languageChange();

    void saveThis( void );


};

#endif // EDITHEBBIANSYNTAB_H
