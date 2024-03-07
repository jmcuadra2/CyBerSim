/***************************************************************************
                          settings.cpp  -  description
                             -------------------
    begin                : sábado dic 14 2002
    copyright            : (C) 2002 by Jose M. Cuadra Troncoso
    email                : jmcuadra@dia.uned.es
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <QFile>
#include <QtGlobal>
#include <QDir>
#include <QMessageBox>
#include <QStringList>
#include <QTextStream>

#include "settings.h"
#include "gui/settingsdialog.h"
#include "../disview/neuraldisview.h"
#include "../net/neuralnetwork.h"
#include "../world/abstractworld.h"

Settings* Settings::settings = 0;

Settings* Settings::instance(void)
{

    if(settings == 0)
        settings = new Settings();
    return settings;

}

Settings::Settings()
{

    QString sbrow, line;
    QStringList dat;

    appDirectory = QDir::homePath() + QDir::toNativeSeparators("/");
    workingDirectory = appDirectory + "examples";
    netDirectory = QDir::toNativeSeparators("/") + "nets" + QDir::toNativeSeparators("/");
    worldDirectory = QDir::toNativeSeparators("/") + "worlds" + QDir::toNativeSeparators("/");
    simsDirectory = QDir::toNativeSeparators("/") + "simulations" + QDir::toNativeSeparators("/");
    robotDirectory = QDir::toNativeSeparators("/") + "robots" + QDir::toNativeSeparators("/");
    controlDirectory = QDir::toNativeSeparators("/") + "controls" + QDir::toNativeSeparators("/");
    netExtension = "*.net";
    worldExtension = "*.fig";
    simsExtension = "*.sim";
    robotExtension = "*.robot";
    controlExtension = "*.ctrl";
    browser = 0;
    helpCommand = "CyBeRSim";
    ok_ = false;
    net = 0;
    net_view = 0;
    world = 0;

    QFile file( QDir::homePath() +
                QDir::toNativeSeparators("/.cybersimrc" ));

    if ( file.open( QIODevice::ReadOnly ) ) {
        QTextStream ts( &file );
        appDirectory = ts.readLine();
        workingDirectory = ts.readLine();
        netDirectory = ts.readLine();
        netExtension = ts.readLine();
        worldDirectory = ts.readLine();
        worldExtension = ts.readLine();
        simsDirectory = ts.readLine();
        simsExtension = ts.readLine();
        robotDirectory = ts.readLine();
        robotExtension = ts.readLine();
        controlDirectory = ts.readLine();
        controlExtension = ts.readLine();
        sbrow = ts.readLine();
        browser = sbrow.toUInt();
        helpCommand = ts.readLine();
        ok_ = true;
    } else  {
        editSettings();
        if ( file.open( QIODevice::ReadOnly ) )
            ok_ = true;
        else
//            QMessageBox::critical(nullptr, "CyBeRSim",
//                            tr("The application hasn't been configured.\n\t\tQuit"), tr("&Ok"));
            QMessageBox::critical(nullptr, "CyBeRSim",
                            tr("The application hasn't been configured.\n\t\tQuit"), QMessageBox::Ok);
    }
    file.close();

}

Settings::~Settings()
{}

void Settings::editSettings()
{

    settingsDialog *dialog = new settingsDialog(0, 0, false);

    dialog->setValues(appDirectory, workingDirectory, netDirectory, netExtension,
                      worldDirectory,worldExtension, browser,
                      helpCommand, simsDirectory, simsExtension, robotDirectory, robotExtension, controlDirectory, controlExtension);
    if ( dialog->exec() == settingsDialog::Accepted ) {
        appDirectory = dialog->getDirApp();
        workingDirectory = dialog->getWorkingDir();
        netDirectory = dialog->getNetDirectory();
        netExtension = dialog->getNetExtension();
        worldDirectory = dialog->getWorldDirectory();
        worldExtension = dialog->getWorldExtension();
        simsDirectory = dialog->getSimsDirectory();
        simsExtension = dialog->getSimsExtension();
        robotDirectory = dialog->getRobotDirectory();
        robotExtension = dialog->getRobotExtension();
        controlDirectory = dialog->getControlDirectory();
        controlExtension = dialog->getControlExtension();
        browser = dialog->getBrowser();
        helpCommand = dialog->getCommand();
        saveSettings();
    }
    delete dialog;
}

void Settings::saveSettings()
{

    QFile file(QDir::homePath() +
               QDir::toNativeSeparators("/.cybersimrc"));
    QTextStream ts( &file );
    if ( file.open( QIODevice::WriteOnly ) )
        ts << appDirectory << "\n" << workingDirectory << "\n"
           << netDirectory << "\n"
           << netExtension << "\n" << worldDirectory << "\n"
           << worldExtension << "\n" << simsDirectory << "\n"
           << simsExtension << "\n" << robotDirectory << "\n"
           << robotExtension << "\n" << controlDirectory << "\n"
           << controlExtension << "\n"
           << browser << "\n" << helpCommand << "\n";
    
}

QString Settings::getNetDirectory(void)
{

    return fillPath(netDirectory);

}

QString Settings::getNetExtension(void)
{

    return netExtension;

}

QString Settings::getWorldDirectory(void)
{

    return fillPath(worldDirectory);

}

QString Settings::getWorldExtension(void)
{

    return worldExtension;

}

QString Settings::getSimsDirectory(void)
{

    return fillPath(simsDirectory);

}

QString Settings::getSimsExtension(void)
{

    return simsExtension;

}


QString Settings::getRobotDirectory(void)
{

    return fillPath(robotDirectory);

}

QString Settings::getRobotExtension(void)
{

    return robotExtension;

}

QString Settings::getControlDirectory(void)
{

    return fillPath(controlDirectory);

}

QString Settings::getControlExtension(void)
{

    return controlExtension;

}


QString Settings::getAppDirectory(void)
{

    return appDirectory;

}

QString Settings::getWorkingDirectory(void)
{

    return workingDirectory;
}

QString Settings::getHelpCommand(void)
{

    return helpCommand;

}

void Settings::setHelpCommand(QString c)
{

    helpCommand = c;

}
void Settings::setHelpBrowser(uint b)
{

    browser = b;

}

bool Settings::ok_settings(void)
{

    return ok_;

}

QMdiArea* Settings::getWorkspace(void)
{

    return pWorkspace;

}

QList<QWidget *> * Settings::getWindowList(void)
{

    return pWindowList;

}

void Settings::setWorkspace(QMdiArea* p_work)
{

    pWorkspace = p_work;

}

void Settings::setWindowList(QList<QWidget *> * w_list)
{

    pWindowList = w_list;

}


QList<QGraphicsScene *> * Settings::getCanvasList(void)
{
    return pCanvasList;
}

void Settings::setCanvasList (QList<QGraphicsScene *> *pCanvasList)
{
    this->pCanvasList = pCanvasList;
}


NeuralDisView* Settings::getNetView(void)
{
    return net_view;
}

void Settings::setNetView(NeuralDisView* netview)
{
    net_view = netview;
}                    

NeuralNetwork* Settings::getNet(void)
{
    return net;
}

void Settings::setNet(NeuralNetwork* net_)
{

    net = net_;

}

void Settings::deleteNet(void)
{

    if(net) {
        delete net;
        net = 0;
    }

}

void Settings::deleteNetView(void)
{

    if(net_view) {
        pWorkspace->removeSubWindow(net_view);
        delete net_view;
        net_view = 0;
    }

}

AbstractWorld* Settings::getWorld(void)
{
    return world;
}

void Settings::setWorld(AbstractWorld* wrl)
{
    world = wrl;
}

QString Settings::fillPath(const QString& path, QString pre_path)
{
    QString fullpath = path;
    bool is_relative = false;
#if defined(Q_OS_UNIX)
    is_relative = (path.left(1) != "/");
#elif defined(Q_OS_WIN)
    is_relative = !(path.contains(":\\") || path.contains(":/"));
#endif
    if(is_relative) {
        if(pre_path.trimmed().isEmpty())
            pre_path  =  workingDirectory;
        fullpath = QDir::cleanPath(pre_path + QDir::separator() + fullpath);

    }
    return fullpath;
}

QString Settings::relativeWorkingPath(const QString& path) 
{
    QString s = path;
    return s.remove(workingDirectory);

}
