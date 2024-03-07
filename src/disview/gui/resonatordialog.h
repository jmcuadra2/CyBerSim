#ifndef RESONATORDIALOG_H
#define RESONATORDIALOG_H

#include "ui_resonatordialog.h"

class Resonator;

class resonatorDialog : public QDialog, public Ui::resonatorDialog
{
    Q_OBJECT

public:
    resonatorDialog(QWidget* parent = 0, const char* name = 0, bool modal = false,
                    Qt::WindowFlags fl = Qt::WindowFlags());
    ~resonatorDialog();

    virtual void init( Resonator * r, bool mode );

protected:
    virtual int getHalfPeriod( void );
    virtual double getWeight( void );
    virtual bool getWFix( void );
    virtual double getDamp( void );
    virtual bool first2damp();
    virtual bool damp2first();
    virtual void paintEvent(QPaintEvent* e);
    virtual bool isCStimul( void );
//    virtual void paintGraph(void);

protected slots:
    virtual void languageChange();
    virtual void changeXLabels( int c );

    virtual void setWeight( double w );
    virtual void setDamp( double d );
    virtual void setFirstMax( double d );
    virtual void setHalfPeriod( int hp );
    virtual void setWFix( bool wf );
    virtual void setNonN( bool nn );
    virtual void setLink( bool l );
    virtual void setStimulus( int s );
    void accept();
    virtual void setRecMonable( bool recmon );

protected:
    bool wfix;
    bool stimul;
    bool no_init;
    bool link;
//    QVector<double> xn;
//    double x_pixel;
    int hper, prevHper;
    QList<QLabel*> xlabs;
    double damp, prevDamp;
//    double y_pixel;
    double fmax, prevFMax;
    double weight;
    Resonator *res;
    bool nonneg;
    bool rec_mon;

    QButtonGroup* stimulButtonGroup;

};

#endif // RESONATORDIALOG_H
