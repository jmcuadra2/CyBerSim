/***************************************************************************
                          settings.h  -  description
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

#ifndef SETTINGS_H
#define SETTINGS_H

#include <QObject>
#include <QWidget>
#include "xmloperator.h"
#include <QGraphicsScene>
#include <QList>
#include <QMdiArea>

class QMdiArea;
class NeuralDisView;
class NeuralNetwork;
class AbstractWorld;

/**
  *@author Jose M. Cuadra Troncoso
  */

class Settings: public QObject
{

    Q_OBJECT

protected:
    Settings();
public:
    ~Settings();

    static Settings* instance();

    virtual void editSettings(void);
    virtual void saveSettings(void);
    QString getNetDirectory(void);
    QString getNetExtension(void);
    QString getWorldDirectory(void);
    QString getWorldExtension(void);
    QString getRobotDirectory(void);
    QString getRobotExtension(void);
    QString getControlDirectory(void);
    QString getControlExtension(void);
    QString getSimsDirectory(void);
    QString getSimsExtension(void);
    QString getAppDirectory(void) ;
    QString getWorkingDirectory(void) ;
    QString getHelpCommand(void) ;
    void setHelpCommand(QString) ;
    void setHelpBrowser(uint) ;
    bool ok_settings(void);

    QMdiArea* getWorkspace(void);
    void setWorkspace(QMdiArea* p_work);

    QList<QGraphicsScene *> *getCanvasList(void);
    void setCanvasList(QList<QGraphicsScene *> * pCanvasList);

    QList<QWidget *> * getWindowList(void);
    void setWindowList(QList<QWidget *> * w_list);
    
    NeuralDisView* getNetView(void);
    void setNetView(NeuralDisView* netview);
    NeuralNetwork* getNet(void);
    void setNet(NeuralNetwork* net_);
    void deleteNet(void);
    void deleteNetView(void);
    AbstractWorld* getWorld(void);
    void setWorld(AbstractWorld* wrl);
    QString fillPath(const QString& path, QString pre_path = "");
    QString relativeWorkingPath(const QString& path);

private:
    static Settings* settings;
    QString appDirectory;
    QString workingDirectory;
    QString netDirectory;
    QString netExtension;
    QString worldDirectory;
    QString worldExtension;
    QString robotDirectory;
    QString robotExtension;
    QString controlDirectory;
    QString controlExtension;
    QString simsDirectory;
    QString simsExtension;
    uint browser;
    QString helpCommand;
    bool ok_;

    QMdiArea* pWorkspace;

    QList<QGraphicsScene *> *pCanvasList;
    QList<QWidget *> *pWindowList;
    NeuralDisView* net_view;
    NeuralNetwork* net;
    AbstractWorld* world;

};

#endif
