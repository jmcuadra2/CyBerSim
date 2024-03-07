#ifndef ROBOTCONNECTIONDIALOG_H
#define ROBOTCONNECTIONDIALOG_H

#include "ui_RobotConnectionDialog.h"

class RobotConnectionDialog : public QDialog, public Ui::RobotConnectionDialog
{
    Q_OBJECT

public:
    RobotConnectionDialog(QWidget* parent = 0, const char* name = 0, bool modal = true, Qt::WindowFlags fl = Qt::WindowFlags());
    ~RobotConnectionDialog();

    virtual QString getUser();
    virtual QString getHost();
    virtual QString getPassword();
    virtual QString getTerminal();
    virtual QString getServidor();
    virtual QString getMapa();
    virtual QString getRemotePort();
    virtual QString getUserRobot();
    virtual QString getDirRobot();
    virtual bool isRealRobot();

public slots:
    virtual void setUser( const QString & user );
    virtual void setHost( const QString & host );
    virtual void setPassword( const QString & password );
    virtual void setTerminal( const QString & terminal );
    virtual void setServidor( const QString & servidor );
    virtual void setMapa( const QString & mapa );
    virtual void setRemotePort( const QString & remotePort );
    virtual void setUserRobot( const QString & userRobot );
    virtual void setDirRobot( const QString & dirRobot );
    
    virtual void activarServidorPC( bool on );
    virtual void setFileServidor();
    virtual void setFileMapa();


protected slots:
    virtual void languageChange();

private:
    void init();

protected:
    QString server_file;
    QString map_file;
    QString dirRobot;
    QString userRobot;
    QString remotePort;
    QString programServer;
    QString mapa;
    QString password;
    QString user;
    QString host;
    QString terminal;
    QButtonGroup* modeButtonGroup;

};

#endif // ROBOTCONNECTIONDIALOG_H
