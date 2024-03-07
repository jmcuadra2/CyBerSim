#ifndef RECMONEDITTAB_H
#define RECMONEDITTAB_H

#include "ui_recmonedittab.h"
#include "../recmondevice.h"
#include <qewtreeviewdialog.h>
class GraphicMonitor;
class BoxMonitor;
class DeviceRecorder;
class RecMonFactory;
class NetRecMonTechnician;
class RecMonConnection;
class XMLOperator;

class RecMonEditTab : public QewTreeViewDialog, public Ui::RecMonEditTab
{
    Q_OBJECT

public:
    RecMonEditTab(QWidget* parent = 0, const char* name = 0);
    ~RecMonEditTab();

    virtual void init( NetRecMonTechnician * netRMTech );
    virtual bool getWeightsMon( void );
    virtual bool getWFileMon( void );
    virtual bool isConnMonitored( const QDomElement & e_box, int num_id );
    virtual bool isConnRecorded( const QDomElement & e_rec, int num_id );
    virtual bool editDevMonitor( QString s_dev );
    virtual bool editGraphMonitor( QString s_dev, QString s_mon );
    virtual RecMonConnection * findReson( RecMonDevice * dev, QString s_conn );
    virtual RecMonDevice * findDev( QString s_dev );
    virtual bool getDevMonChanged( void );
    virtual bool getDevRecChanged( void );
    virtual bool editDevRec( QString s_dev );
    virtual int getFileMode( void );
    virtual int getEveryRecord( void );
    virtual bool verifyPeriodRec( int period );
    virtual void setGeneralParams( QDomElement & e_gen, int rec_period, int f_mode, bool is_mon, bool is_rec );

public slots:
    virtual void setSimsDir( const QString & d );
    virtual void setWeightsMon( bool w_m );
    virtual void setWFileMon( bool w_m );
    virtual void setMonItem( const QDomElement & e_mon, RecMonDevice * dev,
                             QTreeWidgetItem * item_devmon );
    virtual void setBoxItem( const QDomElement & e_box, RecMonDevice * dev,
                             QTreeWidgetItem * item_devnomon );
    virtual void setRecItem( const QDomElement & e_rec, RecMonDevice * dev,
                             QTreeWidgetItem * item_devnorec );
    virtual void setDevs( QList<RecMonDevice*> dv );
    virtual void addResMon( void );
    virtual void addMonitor( void );
    virtual void delResMon( void );
    virtual void editGraphItem( QTreeWidgetItem * item_mon );
    virtual void editDevItem(QTreeWidgetItem * item_nomon , QTreeWidget *treeWidget);
    virtual void editGeneral( void );
    virtual void addResRec( void );
    virtual void delResRec( void );
    virtual void editRecItem( QTreeWidgetItem * item_rec );
    virtual void editGeneralRec( void );
    virtual void setFileMode( int fm );
    virtual void setEveryRecord( int s );
    virtual void setAdvPeriod( int ap );
    virtual void saveToFile( void );
    virtual void loadFromFile( void );

protected:
    bool validateThis( void );
    virtual void saveRecorder( QDomElement & e_rec, int dev_id );
    virtual void saveBoxMonitor( QDomElement & e_box, int dev_id );
    virtual void saveDevice( QDomElement & e );
    void saveThis( void );
    virtual void writeThis( void );
    QStringList designedCaptions( void );

protected slots:
    virtual void languageChange();
    void getExternVal( const QString & var_name, const QVariant & var_val );
    virtual void loadInfo();

protected:
    RecMonFactory* factory;
    NetRecMonTechnician* netRecMonTech;
    QDomElement xml_info;
    QDomDocument doc_info;
    int adv_period;
    int every_tenth_record;
    QString simname;
    QString netname;
    QString sims_dir;
    bool wfil_mon;
    int conn_id;
    QList<RecMonDevice*> devs;
    bool wg_mon;
    int dev_id;
    bool devMon_changed;
    bool devRec_changed;
    int file_mode;
    XMLOperator* xml_operator;

};

#endif // RECMONEDITTAB_H
