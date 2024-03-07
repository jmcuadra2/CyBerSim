#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include "ui_settingsdialog.h"

class settingsDialog : public QDialog, public Ui::settingsDialog
{
    Q_OBJECT

public:
    settingsDialog(QWidget* parent = 0, const char* name = 0, bool modal = false, Qt::WindowFlags fl = Qt::WindowFlags());
    ~settingsDialog();

    virtual QString getRobotExtension( void );
    virtual QString getRobotDirectory( void );
    virtual QString getControlExtension( void );
    virtual QString getControlDirectory( void );
    virtual QString getNetExtension( void );
    virtual QString getNetDirectory( void );
    virtual QString getWorldExtension( void );
    virtual QString getWorldDirectory( void );
    virtual QString getDirApp( void );
    virtual QString getWorkingDir( void );
    virtual QString getCommand( void );
    virtual uint getBrowser( void );
    virtual QString getSimsExtension( void );
    virtual QString getSimsDirectory( void );
    virtual void setValues( QString dirA, QString dirWork, QString dir, QString ext, QString w_dir, QString w_ext, uint brow, QString com, QString s_dir, QString s_ext, QString r_dir, QString r_ext, QString c_dir, QString c_ext );
    
    QString fillPath(const QString& path);

public slots:
    virtual void setNetExtension( const QString & ext );
    virtual void setNetDirectory( const QString & dir );
    virtual void setWorldExtension( const QString & ext );
    virtual void setWorldDirectory( const QString & dir );
    virtual void setDirApp( const QString & dir );
    virtual void setWorkingDir( const QString & dir );
    virtual void setCommand( const QString & c );
    virtual void setBrowser( int b );
    virtual void setSimsExtension( const QString & ext );
    virtual void setSimsDirectory( const QString & dir );

    virtual void chooseDirectory( void );
    virtual void chooseWorldDirectory( void );
    virtual void chooseDirApp( void );
    virtual void chooseWorkingDir( void );
    virtual void chooseBrowser( void );
    virtual void chooseSimsDirectory( void );
    void accept( void );
    virtual void setRobotExtension( const QString & ext );
    virtual void setRobotDirectory( const QString & dir );
    virtual void chooseRobotDirectory( void );
    virtual void setControlExtension( const QString & ext );
    virtual void setControlDirectory( const QString & dir );
    virtual void chooseControlDirectory( void );

protected:
    int browser;
    QString dirApp;
    QString workingDir;
    QString net_extension;
    QString net_directory;
    QString command;
    QStringList commands;
    QString world_directory;
    QString world_extension;
    QString sims_directory;
    QString sims_extension;
    QString robot_extension;
    QString robot_directory;
    QString control_directory;
    QString control_extension;

protected slots:
    void languageChange();

};

#endif // SETTINGSDIALOG_H
