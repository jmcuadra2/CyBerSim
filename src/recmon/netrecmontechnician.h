/***************************************************************************
                          netrecmontechnician.h  -  description
                             -------------------
    begin                : Fri Apr 15 2005
    copyright            : (C) 2005 by Jose M. Cuadra Troncoso
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

#ifndef NETRECMONTECHNICIAN_H
#define NETRECMONTECHNICIAN_H

#include <QDomDocument>
#include <QObject>
#include <qewextensibledialog.h>

#include <QList>

class RecMonDevice;
class RecMonConnection;
class BoxMonitor;
class FileRecorder;
class RecMonFactory;
class QWidget;
class XMLOperator;

/**
  *@author Jose M. Cuadra Troncoso
  */

class NetRecMonTechnician : public QObject {

    Q_OBJECT

    friend class RecMonEditTab;

public:
    NetRecMonTechnician(QObject *parent = 0, const char *name = 0);
    ~NetRecMonTechnician();

    virtual void construct(void);
    virtual void setInfo(const QDomElement& e, bool verify_net = false);
    virtual void setDevices(QList<RecMonDevice *> devs);
    virtual bool isLoadOK(void);
    virtual bool isNetOK(void);
    virtual void verifyNet(void);
    virtual void setNetConnection(void);
    virtual QString save(void);
    virtual void doMonitor(void);
    virtual bool setFile(const QString& f_name);
    virtual void startMonitor(bool on);
    virtual void startRecorder(bool on, bool crono_reset);
    virtual void resetMonitors(void);

    QList<QewExtensibleDialog*> tabsForEdit(QewExtensibleDialog* parent);

    virtual void setNetName(QString net_n)
    {net_name = net_n ; };
    virtual void setControlFileName(QString control_filename)
    {
        this->control_filename = control_filename ;
    }

    virtual void setRecorderHeader(FileRecorder* recorder, RecMonDevice* dev);
    virtual void write(QDomDocument& doc, QDomElement& e);
    virtual void connectToTime(bool on);
    bool fileChanged(void)  {return file_changed ;}
    void setFileChanged(bool f_changed) { file_changed = f_changed ;}

public slots:
    virtual void setCrono(int cr);
    
protected:
    virtual void badLoad(void);
    FileRecorder* findRecorder(int n_id);
    BoxMonitor* findBoxMonitor(int n_id);
    RecMonDevice* findDev(int n_dev);
    RecMonConnection* findConn(RecMonDevice* dev, int n_conn);
    virtual void verifyDev(RecMonDevice* dev, QDomElement& e);
    virtual void saveInfo(void);
    virtual void writeGeneralParams(void);
    void showMonitors(void);
    void hideMonitors(void);
    void closeMonitors(void);
    void closeRecorders(void);
    virtual void connectNet(bool on);
    virtual void constructRecMon(QDomElement& e);
    virtual void clearInfo(void);

signals:
    void writeToMonitors(void);
    void writeToRecorders(void);

protected:
    QDomDocument recmonDoc;
    QDomElement recmonInfo;
    QList<BoxMonitor *> MonList;
    QList<FileRecorder *> RecList;
    QList<RecMonDevice *> net_devices;
    QMap<int, RecMonDevice*> monitored_devices;

    QHash<void*, RecMonConnection*> monitored_connections;
    QMap<int, RecMonDevice*> recorded_devices;
    QHash<void*, RecMonConnection*> recorded_connections;
    bool loadOK, wasFixed, netOK, net_connected;
    QString file_name;
    int file_mode;
    int crono;
    bool monitor_on, recorder_on, net_on, time_on;
    int recordPeriod;
    bool is_monitoring, is_recording;
    RecMonFactory* factory;
    XMLOperator* xml_operator;
    QString net_name;
    QString control_filename;
    bool file_changed;
    bool file_really_open;
    
};    

inline void NetRecMonTechnician::setCrono(int cr)
{
    crono = cr ;
}

#endif
