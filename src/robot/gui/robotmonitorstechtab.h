#ifndef ROBOTMONITORSTECHTAB_H
#define ROBOTMONITORSTECHTAB_H

#include "ui_robotmonitorstechtab.h"
#include <qewextensibledialog.h>

class RobotMonitorsTechnician;
class GraphicMonitor;
class Robot;

class RobotMonitorsTechTab : public QewExtensibleDialog, public Ui::RobotMonitorsTechTab
{
    Q_OBJECT

public:
    RobotMonitorsTechTab ( QWidget* parent = 0, const char* name = 0 );
    ~RobotMonitorsTechTab();

    virtual void init ( RobotMonitorsTechnician * m_Tech );
    virtual bool getVFileRec ( void );
    virtual QString getVFile ( void );
    virtual bool getVMon ( void );
    virtual int getViewType ( void );
    virtual int getTimeUpd ( void );
    virtual int getTLabels ( void );
    virtual int getVLinLabels ( void );
    virtual int getVRadLabels ( void );
    virtual double getMinVelLin ( void );
    virtual double getMaxVelLin ( void );
    virtual double getMinVelRad ( void );
    virtual double getMaxVelRad ( void );
    virtual void readVars ( void );

public slots:
    virtual void setVFileRec ( bool v_fr );
    virtual void setVFile ( const QString & f );
    virtual void setIniDir ( const QString & d );
    virtual void chooseVFile ( void );
    virtual void setVMon ( bool v_m );
    virtual void setViewType ( int vt );
    virtual void setTimeUpd ( int tu );
    virtual void setTLabels ( int tl );
    virtual void setVLinLabels ( int vl );
    virtual void setVRadLabels ( int vr );
    virtual void setMinVelLin ( double d );
    virtual void setMaxVelLin ( double d );
    virtual void setMinVelRad ( double d );
    virtual void setMaxVelRad ( double d );
    virtual void saveToFile ( void );
    virtual void loadFromFile ( void );

protected slots:
    void getExternVal(const QString& var_name, const QVariant& var_val);

protected:
    double maxvel_lin;
    double maxvel_rad;
    bool v_filerec;
    bool v_mon;
    int view_type;
    int timeupd;
    double minvel_lin;
    double minvel_rad;
    QString vFile;
    int tLabels;
    int vLinLabels;
    int vRadLabels;
    RobotMonitorsTechnician* monsTech;
    QString ini_dir;

    bool validateThis ( void );
    void saveThis ( void );
    virtual void writeThis ( void );

protected slots:
    virtual void languageChange();

};

#endif // ROBOTMONITORSTECHTAB_H
