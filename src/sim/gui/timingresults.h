#ifndef TIMINGRESULTS_H
#define TIMINGRESULTS_H

#include "ui_timingresults.h"

class ClocksDispatcher;

class TimingResults : public QDialog, public Ui::TimingResults
{
    Q_OBJECT

public:
    TimingResults(QWidget* parent = 0, const char* name = 0, bool modal = false,
                  Qt::WindowFlags fl = Qt::WindowFlags());
    ~TimingResults();

    virtual void write( void );

public slots:
    virtual void init( ClocksDispatcher * dispatch );
    virtual void save( void );

protected:
    ClocksDispatcher* dispatcher;

protected slots:
    virtual void languageChange();

};

#endif // TIMINGRESULTS_H
