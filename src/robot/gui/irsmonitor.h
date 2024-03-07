#ifndef IRSMONITOR_H
#define IRSMONITOR_H

#include "ui_irsmonitor.h"

class IRSMonitor : public QWidget, public Ui::IRSMonitor
{
    Q_OBJECT

public:
    IRSMonitor(QWidget* parent = 0, const char* name = 0, Qt::WindowFlags fl = Qt::WindowFlags());
    ~IRSMonitor();

    virtual void init( void );
    virtual void setIdOffset( int id_offset );
    virtual void startLCD( int ident );

public slots:
    virtual void setLCD(int val, int ident);
    virtual void setRVel( double rv );
    virtual void setLVel( double lv );
    virtual void setSensorLabel( const QString & label );

signals:
    void lVel(const QString &);
    void lcd1(int);
    void lcd2(int);
    void lcd3(int);
    void lcd4(int);
    void lcd5(int);
    void lcd6(int);
    void lcd7(int);
    void lcd8(int);
    void rVel(const QString &);

protected slots:
    virtual void languageChange();

protected:
    void setLCDColorBackGround(QLCDNumber* lcd, QColor color);

private:
    int ident_offset;

};

#endif // IRSMONITOR_H
