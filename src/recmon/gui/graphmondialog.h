#ifndef GRAPHMONDIALOG_H
#define GRAPHMONDIALOG_H

#include "ui_graphmondialog.h"

class GraphicMonitor;

class graphmonDialog : public QDialog, public Ui::graphmonDialog
{
    Q_OBJECT

public:
    graphmonDialog(QWidget* parent = 0, const char* name = 0, bool modal = false, Qt::WindowFlags fl = Qt::WindowFlags());
    ~graphmonDialog();

    virtual void init( GraphicMonitor * gm, int dec, int mant );
    virtual double getMin( void );
    virtual double getMax( void );
    virtual int getTimeUpd( void );
    virtual int getTLabels( void );
    virtual int getVLabels( void );
    virtual int getAccurateRestore( void );
    virtual const QString & getTitulo( void );

public slots:
    virtual void setMin( double d );
    virtual void setMax( double d );
    virtual void setTimeUpd( int tu );
    virtual void setTLabels( int tl );
    virtual void setVLabels( int vl );
    virtual void setAccurateRestore( bool accres );
    virtual void setInfor( const QString in );
    virtual void setTitulo( const QString & cap );

protected:
    double min;
    double max;
    int tLabels;
    int vLabels;
    int timeupd;
    GraphicMonitor *gmon;
    QString titulo;
    bool accurateRestore;

protected slots:
    virtual void languageChange();

    virtual void accept( void );


};

#endif // GRAPHMONDIALOG_H
