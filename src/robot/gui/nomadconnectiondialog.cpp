//
// C++ Implementation: nomadconnectiondialog
//
// Description: 
//
//
// Author: Javier Garcia Misis <>, (C) 2010
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "nomadconnectiondialog.h"

#include <QVariant>
#include <QDir>

#include "../../neuraldis/settings.h"

NomadConnectionDialog::NomadConnectionDialog(QWidget* parent, const char* name, bool modal, Qt::WindowFlags fl)
    : QDialog(parent, fl)
{
    setupUi(this);
    setObjectName(name);
    setModal(modal);

    hostServer = "localhost";
    hostRobot = "192.168.1.11";

    portServer = "7019";
    portRobot = "65001";

    connect(buttonOk, SIGNAL(clicked()), this, SLOT(accept()));
    connect(buttonCancel, SIGNAL(clicked()), this, SLOT(reject()));
    connect(hostLineEdit, SIGNAL(textChanged(QString)), this, SLOT(setHost(QString)));
    connect(portLineEdit, SIGNAL(textChanged(QString)), this, SLOT(setPort(QString)));

}


NomadConnectionDialog::~NomadConnectionDialog()
{
}

void NomadConnectionDialog::ConnectionServer( bool on )
{
    if(on){
        hostLineEdit->setText(hostServer);
        portLineEdit->setText(portServer);
        ConnectionData->setTitle(QApplication::translate("NomadConnectionDialog",
                              "Server Connection Data", 0));
    }
    else{
        hostLineEdit->setText(hostRobot);
        portLineEdit->setText(portRobot);
        ConnectionData->setTitle(QApplication::translate("NomadConnectionDialog",
                              "Robot Connection Data", 0));
    }

}

void NomadConnectionDialog::setHost(const QString & host)
{
    if (nserverRadioButton->isChecked ()){
        hostServer = host;
    }
    else{
        hostRobot = host;
    }
}

void NomadConnectionDialog::setPort(const QString & port)
{
    if (nserverRadioButton->isChecked ()){
        portServer = port;
    }
    else{
        portRobot = port;
    }
}

QString NomadConnectionDialog::getHost(){
    return hostLineEdit->text();
}

QString NomadConnectionDialog::getPort(){
    return portLineEdit ->text();
}

int NomadConnectionDialog::getID(){
    return idSpinBox->value ();
}

bool NomadConnectionDialog::createNewRobot(){
    return newCheckBox->isChecked ();
}

bool NomadConnectionDialog::isDirectConnection(){
    return directRadioButton->isChecked ();
}

bool NomadConnectionDialog::connectToReal(){
    return toRealCheckBox->isChecked ();
}
