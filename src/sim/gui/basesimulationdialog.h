#ifndef BASESIMULATIONDIALOG_H
#define BASESIMULATIONDIALOG_H

#include "ui_basesimulationdialog.h"
#include <qewtreeviewdialog.h>

class GraphicMonitor;
class BaseSimulation;

class baseSimulationDialog : public QewTreeViewDialog, public Ui::baseSimulationDialog
{
    Q_OBJECT

public:
    baseSimulationDialog(QWidget* parent = 0, const char* name = 0);
    ~baseSimulationDialog();

    virtual void init( BaseSimulation * simul );
    virtual int getSimSpeed( void );
    virtual int getTypeSimSpeed( void );
    virtual int getScrUpdate( void );
    virtual bool getClockReset( void );
    virtual int getTypeScrUpdate( void );
    virtual int getDuration( void );

public slots:
    virtual void setSimsDir( const QString & d );
    virtual int getAdvPeriod( void );
    virtual void setAdvPeriod( int ap );
    virtual void setSimSpeed( int s );
    virtual void setTypeSimSpeed( int s );
    virtual void setScrUpdate( int s );
    virtual void setClockReset( int c );
    virtual void setTypeScrUpdate( int s );
    virtual void setIniDir( const QString & d );
    virtual void setDuration( const QTime & d );

protected:
    bool validateThis( void );
    virtual bool verifyPeriodScrUpd( int period );
    void saveThis( void );
    QStringList designedCaptions( void );

protected slots:
    virtual void languageChange();

protected:
    int every_tenth;
    BaseSimulation *sim;
    bool clockReset;
    int type_scr_upd;
    int type_sim_speed;
    int noise;
    int advance_period;
    QString ini_dir;
    int sim_speed;
    int iner;
    QString simulation_dir;
    QString sims_ext;
    QString simname;
    QTime duration;

    QButtonGroup* simsButtonGroup;
    QButtonGroup* clockResetButtonGroup;
    QButtonGroup* supButtonGroup;

};

#endif // BASESIMULATIONDIALOG_H
