#ifndef DEVMONDIALOG_H
#define DEVMONDIALOG_H

#include "ui_devmondialog.h"

class GraphicMonitor;

class DevMonDialog : public QDialog, public Ui::DevMonDialog
{
    Q_OBJECT

public:
    DevMonDialog(QWidget* parent = 0, const char* name = 0, bool modal = false, Qt::WindowFlags fl = Qt::WindowFlags());
    ~DevMonDialog();

    virtual void setSignalNames( const QMap<QString, int> & signals_map );

public slots:
    virtual void init( GraphicMonitor * gmo, GraphicMonitor * gmd, int dec, int mant );
    virtual double getMin( void );
    virtual void setMin( double d );
    virtual double getMax( void );
    virtual void setMax( double d );
    virtual double getMinDiff( void );
    virtual void setMinDiff( double d );
    virtual double getMaxDiff( void );
    virtual void setMaxDiff( double d );
    virtual int getTimeUpd( void );
    virtual void setTimeUpd( int tu );
    virtual int getTLabels( void );
    virtual void setTLabels( int tl );
    virtual int getVLabels( void );
    virtual void setVLabels( int vl );
    virtual int getVDiffLabels( void );
    virtual void setVDiffLabels( int vl );
    virtual void setInfor( const QString in );
    virtual void accept( void );
    virtual void setWindowTitle2( const QString & cap );

protected:
    QListWidget* secondMonList;
    double min;
    double max;
    int tLabels;
    int vLabels;
    int timeupd;
    GraphicMonitor *gmonOut;
    double minDiff;
    double maxDiff;
    int vDiffLabels;
    GraphicMonitor *gmonDiff;
    QListWidget* firstMonList;
    QVector<int> signals_num;
    QString mon1Caption;
    QString mon2Caption;

protected slots:
    virtual void languageChange();

    virtual void setWindowTitle1( const QString & cap );


};

#endif // DEVMONDIALOG_H
