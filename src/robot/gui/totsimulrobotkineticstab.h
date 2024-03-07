#ifndef TOTSIMULROBOTKINETICSTAB_H
#define TOTSIMULROBOTKINETICSTAB_H

#include "ui_totsimulrobotkineticstab.h"
#include <qewextensibledialog.h>

class TotallySimulatedRobot;

class TotSimulRobotKineticsTab : public QewExtensibleDialog, public Ui::TotSimulRobotKineticsTab
{
    Q_OBJECT

public:
    TotSimulRobotKineticsTab(QWidget* parent = 0, const char* name = 0);
    ~TotSimulRobotKineticsTab();

    virtual void init( TotallySimulatedRobot * rob );
    virtual int getIner( void );
    virtual int getNoise( void );

public slots:
    virtual void setIner( int i );
    virtual void setNoise( int n );

protected:
    void saveThis( void );
    virtual int getSensorMode( void );

protected slots:
    virtual void languageChange();
    virtual void setSensorMode( int sm );

protected:
    bool sensorMode;
    TotallySimulatedRobot* robot;
    int iner;
    int noise;

    QButtonGroup* sensorModeButtonGroup;

};

#endif // TOTSIMULROBOTKINETICSTAB_H
