#ifndef ISOCONTROLTAB_H
#define ISOCONTROLTAB_H

#include "ui_ISOcontroltab.h"
#include <qewextensibledialog.h>

class NetSimControl;

class ISOControlTab : public QewExtensibleDialog, public Ui::ISOControlTab
{
    Q_OBJECT

public:
    ISOControlTab(QWidget* parent = 0, const char* name = 0);
    ~ISOControlTab();

    virtual void init( NetSimControl * ctrl );

protected slots:
    virtual void setWeightStabValue( double d );
    virtual void setWeightStabMode( int ws );
    virtual void setLearnRate( double d );
    virtual void setLearnRateOver( bool b );
    virtual void setPosWeights( bool ps );
    virtual void setInitWeightsValue( double d );
    virtual void setInitWeightsMode( bool b );
    virtual void setInstantPropagation( int inst_prop );
    virtual void saveToFile( void );
    virtual void setWeightDecay( double w_dec );

protected:
    virtual double getWeightStabValue( void );
    virtual int getWeightStabMode( void );
    virtual double getLearnRate( void );
    virtual bool getLearnRateOver( void );
    virtual bool getPosWeights( void );
    virtual double getInitWeightsValue( void );
    virtual bool getInitWeightsMode( void );
    virtual int getInstantPropagation( void );
    virtual double getWeightDecay( void );

protected:
    bool pos_weights;
    bool learnRateOver;
    double learnRateValue;
    int weightStabMode;
    double weightStabValue;
    NetSimControl * control;
    double initWeightsValue;
    bool initWeightsMode;
    int instant_prop;
    double w_decay;

    QButtonGroup* propButtonGroup;
    QButtonGroup* weightStabButtonGroup;

    bool validateThis( void );
    void saveThis( void );
    void cleanThis( void );

protected slots:
    virtual void languageChange();

};

#endif // ISOCONTROLTAB_H
