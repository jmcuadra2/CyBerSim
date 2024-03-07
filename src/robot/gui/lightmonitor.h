#ifndef LIGHTMONITOR_H
#define LIGHTMONITOR_H

#include "ui_lightmonitor.h"

class lightMonitor : public QWidget, public Ui::lightMonitor
{
    Q_OBJECT

public:
    lightMonitor(QWidget* parent = 0, const char* name = 0, Qt::WindowFlags fl = Qt::WindowFlags());
    ~lightMonitor();

    virtual void setIdOffset( int id_off );
    void setSensorLabel( const QString & label );

public slots:
    virtual void init( void );
    virtual void setLCD(int val, int ident);
    virtual void resetBump( void );
    virtual void addBumpDetect( int, int val );
    virtual void initBumpDetect( void );
    virtual void addBumpNoDetect( void );
    virtual void initBumpNoDetect( void );
    virtual void startLCD( int ident );

signals:
    void lcdbumpDetect(int);
    void lcd8(int);
    void lcd7(int);
    void lcd6(int);
    void lcd5(int);
    void lcd4(int);
    void lcd3(int);
    void lcd2(int);
    void lcd1(int);
    void lcdbumpNoDetect(int);

protected:
    void setLCDColorBackGround(QLCDNumber* lcd, QColor color);

protected:
    int n_bump_detect;
    int is_already_bump_detect;
    int n_bump_no_detect;
    int is_already_bump_no_detect;
    int ident_offset;

protected slots:
    virtual void languageChange();

};

#endif // LIGHTMONITOR_H
