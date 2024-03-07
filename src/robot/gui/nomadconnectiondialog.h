//
// C++ Interface: nomadconnectiondialog
//
// Description: 
//
//
// Author: Javier Garcia Misis <>, (C) 2010
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef NOMADCONNECTIONDIALOG_H
#define NOMADCONNECTIONDIALOG_H

#include "ui_NomadConnectionDialog.h"

/**
	@author Javier Garcia Misis
*/
class NomadConnectionDialog : public QDialog, public Ui::NomadConnectionDialog
{
    Q_OBJECT
public:
    NomadConnectionDialog(QWidget* parent = 0, const char* name = 0, bool modal = true, Qt::WindowFlags fl = Qt::WindowFlags());

    ~NomadConnectionDialog();

    virtual QString getHost();
    virtual QString getPort();
    virtual int getID();
    virtual bool isDirectConnection();
    virtual bool createNewRobot();
    virtual bool connectToReal();

public slots:
    virtual void ConnectionServer( bool on );
    virtual void setHost( const QString & host );
    virtual void setPort( const QString & port );

private:

    QString  hostServer;
    QString  hostRobot;
    QString  portServer;
    QString  portRobot;
};

#endif
