#ifndef EDITISODEVICETAB_H
#define EDITISODEVICETAB_H

#include "ui_editisodevicetab.h"
#include "../../net/isodevice.h"
#include "../../net/netfactory.h"
#include "../../net/resonator.h"
#include <qewextensibledialog.h>

class EditISODeviceTab : public QewExtensibleDialog, public Ui::EditISODeviceTab
{
    Q_OBJECT

public:
    EditISODeviceTab(QWidget* parent = 0, const char* name = 0);
    ~EditISODeviceTab();

    virtual void init(ISODevice * is, bool mod);
    virtual double getLRate(void);
    virtual double damp2first(int hper, double damp, double sampling_period);

public slots:
    virtual void setLRate(double d);
    virtual void writeTableItem(int i);
    virtual void replicate(void);

protected:
    bool validateThis(void);
    void saveThis(void);
    virtual void copyResonator(Resonator * res_from, Resonator * res_to);
    void cleanThis(void);

protected slots:
    virtual void languageChange();

    virtual void editRes();
    virtual void addRes(void);
    virtual void delRes();
    virtual void setNRows(int n);
    virtual void writeTable(void);
    virtual void selectRow(int row);

protected:
    ISODevice *iso;
    QList<Resonator*> resonators;
    int nreson;
    double lrate;
    bool mode;
    NetFactory* factory;
    QList<int> selectedRows;

};

#endif // EDITISODEVICETAB_H
