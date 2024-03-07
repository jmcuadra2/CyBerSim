#ifndef INPUTISOSYNTAB_H
#define INPUTISOSYNTAB_H

#include "ui_inputisosyntab.h"
#include <qewextensibledialog.h>

class ISOInputSynapse;

class InputISOSynTab : public QewExtensibleDialog, public Ui::InputISOSynTab
{
    Q_OBJECT

public:
    InputISOSynTab(QWidget* parent = 0, const char* name = 0);
    ~InputISOSynTab();

    virtual void init( ISOInputSynapse * iso_syn );
    virtual bool getIsCS( void );

public slots:
    virtual void setIsCS( int is_cs );

protected:
    void saveThis( void );

protected slots:
    virtual void languageChange();

protected:
    ISOInputSynapse* iso_synapse;
    bool isCS;

    QButtonGroup* stimButtonGroup;

};

#endif // INPUTISOSYNTAB_H
