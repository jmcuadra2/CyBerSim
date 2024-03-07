/***************************************************************************
                          netrecmontechnician.cpp  -  description
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

#include "netrecmontechnician.h"
#include "recmondevice.h"
#include <QTextStream>
#include <QMdiSubWindow>
#include <QMessageBox>
#include <QDir>
#include <QList>
#include <QVector>

#include "recmonconnection.h"
#include "boxmonitor.h"
#include "graphicmonitor.h"
#include "devicerecorder.h"
#include "recmonfactory.h"
#include "gui/recmonedittab.h"
#include "../neuraldis/neuraldisdoc.h"
#include "../sim/clocksdispatcher.h"
#include "../neuraldis/settings.h"
#include "../neuraldis/xmloperator.h"

NetRecMonTechnician::NetRecMonTechnician(QObject *parent, const char *name) :
                            QObject(parent)
{

    loadOK = false;
    wasFixed = false;
    netOK = false;
    crono = 0;
    file_mode = FileRecorder::AUTO;
    is_monitoring = false;
    is_recording = false;
    recordPeriod = 0;
    monitor_on = false;
    recorder_on = false;
    net_connected = false;
    factory = RecMonFactory::instance();
    xml_operator = XMLOperator::instance();
    net_on = false;
    time_on = false;
    file_changed = false;
    file_really_open = false;

}

NetRecMonTechnician::~NetRecMonTechnician(){

    closeMonitors();
    closeRecorders();

}

void NetRecMonTechnician::construct(void)
{

    loadOK = false;
    bool ok;
    net_connected = false;
    QDomElement e_gen = xml_operator->findTag("general_parameters", recmonInfo);
    if(e_gen.isNull()) {
        badLoad();
        return;
    }
    recordPeriod = e_gen.attribute("recordPeriod").toInt(&ok);
    file_mode = e_gen.attribute("file_mode").toInt(&ok);
    is_monitoring = bool(e_gen.attribute("isMonitoring").toInt(&ok));
    is_recording = bool(e_gen.attribute("isRecording").toInt(&ok));
    QDomElement e_dev = xml_operator->findTag("Device", recmonInfo);
    QDomNode n;

    while(!e_dev.isNull()) {
        constructRecMon(e_dev);
        n = e_dev.nextSibling();
        e_dev = n.toElement();
    }
    loadOK = true;
    saveInfo();

}        

void NetRecMonTechnician::badLoad(void)
{

    closeMonitors();
    closeRecorders();
    loadOK = false;
//    QMessageBox::warning(nullptr, tr("Recorders and monitors load"),
//                         tr("Cannot construct."), tr("&Abort"));
    QMessageBox::warning(nullptr, tr("Recorders and monitors load"),
                         tr("Cannot construct."), QMessageBox::Abort);

}

void NetRecMonTechnician::constructRecMon(QDomElement& e)
{

    FileRecorder* recorder = 0;
    BoxMonitor* monitor = 0;
    QDomNode n = e.firstChild();

    while(!n.isNull()) {
        QDomElement e_rm = n.toElement();
        if(!e_rm.isNull()) {
            if(e_rm.tagName() == "BoxMonitor") {
                monitor = factory->factoryBoxMonitor(e_rm);
                if(monitor) {
                    MonList.append(monitor);
                    monitor->hide();
                    monitor->getMdiSubWindow()->hide();
                }
            }
            else if(e_rm.tagName() == "Recorder") {
                recorder = factory->factoryRecorder(e_rm);
                if(recorder)
                    RecList.append(recorder);
            }
        }
        n = n.nextSibling();
    }

}

void NetRecMonTechnician::setInfo(const QDomElement& e, bool verify_net)
{

    recmonInfo = e;
    if(verify_net) {
        verifyNet();
        loadOK = netOK;
    }
    else {
        loadOK = true;
        netOK = true;
    }

}

void NetRecMonTechnician::setDevices(QList<RecMonDevice*> devs)
{
    net_devices = devs;
}

bool NetRecMonTechnician::isLoadOK(void)
{
    return loadOK;
}

bool NetRecMonTechnician::isNetOK(void)
{
    return netOK;
}

void NetRecMonTechnician::verifyNet()
{

    RecMonDevice* dev;
    bool ok;
    QDomNodeList id_list =
            recmonInfo.elementsByTagName("dev_identification");
    int num_devs = id_list.count();
    for(int i = 0; i < num_devs; i++) {
        QDomNode n = id_list.item(i);
        QDomElement e = n.toElement();
        int n_dev = e.text().toInt(&ok);
        if(!(dev = findDev(n_dev))) {
            QDomNode n_root = e.parentNode();
            n_root.removeChild(e);
            wasFixed = true;
        }
        else
            verifyDev(dev, e);
    }

    if(wasFixed) {
//        if(QMessageBox::warning(nullptr, tr("Recorders and monitors load"),
//                                tr("Some problems happened and were fixed"),
//                                tr("&Ok"), tr("&Abort")) == 0) {
        if(QMessageBox::warning(nullptr, tr("Recorders and monitors load"),
                                 tr("Some problems happened and were fixed"), QMessageBox::Ok | QMessageBox::Abort) == QMessageBox::Ok) {
            netOK = true;
        }
        else {
            netOK = false;
            badLoad();
        }
    }
    else
        netOK = true;
    wasFixed = false;

}

void NetRecMonTechnician::verifyDev(RecMonDevice* dev, QDomElement& e)
{

    bool ok;
    QDomNode n_dev = e.parentNode();
    QDomElement e_dev = n_dev.toElement();
    QDomNodeList gr_id = e_dev.elementsByTagName("graph_id");
    int num_gr = gr_id.count();
    for(int i = 0; i < num_gr; i++) {
        QDomNode n = gr_id.item(i);
        QDomElement e_gr = n.toElement();
        int n_conn = e_gr.text().toInt(&ok);
        if(n_conn > 0) {
            if(!(findConn(dev, n_conn))) {
                QDomNode n_graphs = e_gr.parentNode();
                n_graphs.removeChild(e_gr);
                wasFixed = true;
            }
        }
    }
    QDomNodeList ch_id = e_dev.elementsByTagName("channel_id");
    int num_ch = ch_id.count();
    for(int i = 0; i < num_ch; i++) {
        QDomNode n = ch_id.item(i);
        QDomElement e_ch = n.toElement();
        int n_conn = e_ch.text().toInt(&ok);
        if(n_conn > 0) {
            if(!(findConn(dev, n_conn))) {
                QDomNode n_channels = e_ch.parentNode();
                n_channels.removeChild(e_ch);
                wasFixed = true;
            }
        }
    }
    
}


RecMonDevice* NetRecMonTechnician::findDev(int n_dev)
{

    QListIterator<RecMonDevice*> it(net_devices);
    RecMonDevice* dev = 0;
    while(it.hasNext()) {
        dev = it.next();
        if(int(dev->getNumId()) == n_dev)
            break;
        else
            dev = 0;
    }
    return dev;

}

RecMonConnection* NetRecMonTechnician::findConn(RecMonDevice* dev, int n_conn)
{

    RecMonConnection *conn = 0;
    QList<RecMonConnection *> conn_list = dev->getRecMonConnections();
    QListIterator<RecMonConnection *> it(conn_list);
    while(it.hasNext()) {
        conn = it.next();
        if(int(conn->getNumId()) == n_conn)
            break;
        else
            conn = 0;
    }
    return conn;

}

void NetRecMonTechnician::setNetConnection(void)
{

    if(!loadOK || !netOK) {
//        QMessageBox::warning(nullptr, tr("Recorders and monitors load"),
//                        tr("No net or net not verified"), tr("&Abort"));
        QMessageBox::warning(nullptr, tr("Recorders and monitors load"),
                             tr("No net or net not verified"), QMessageBox::Abort);
        return;
    }

    QListIterator<BoxMonitor *> itb(MonList);
    QListIterator<FileRecorder *> itr(RecList);
    BoxMonitor* box_mon;
    while(itb.hasNext()) {
        box_mon = itb.next();
        RecMonDevice* dev = findDev(box_mon->getNumId());
        if(!dev)
            continue;

        monitored_devices.insert(box_mon->getNumId(), dev);
        QListIterator<GraphicMonitor *> itg(box_mon->getGraphs());
        GraphicMonitor* g_mon;
        while(itg.hasNext()) {
            g_mon = itg.next();
            QVector<int> gr_id = g_mon->idGraphs();
            int cnt_negatives = 0;  // dev graphs
            for(int i = 0; i < gr_id.count(); i++) {
                RecMonConnection* conn = 0;
                if(gr_id[i] > 0)
                    conn = findConn(dev, gr_id[i]);
                else if(gr_id[i] == 0)
                    continue;
                else
                    ++cnt_negatives;
                if(conn)
                    monitored_connections.insert(g_mon, conn);
                else {
                    if(cnt_negatives == 1)
                        monitored_connections.insert(g_mon, (RecMonConnection*) 1);
                }
            }
        }
    }

    FileRecorder* recorder;
    while(itr.hasNext()) {
        recorder = itr.next();
        RecMonDevice* dev = findDev(recorder->getNumId());
        if(!dev)
            continue;

        recorded_devices.insert(recorder->getNumId(), dev);
        QVector<int> ch_id = recorder->idChannels();
        for(int i = 0; i < ch_id.count(); i++) {
            RecMonConnection* conn = 0;
            int cnt_negatives = 0;  // dev channels
            if(ch_id[i] > 0)
                conn = findConn(dev, ch_id[i]);
            else if(ch_id[i] == 0)
                continue;
            else
                ++cnt_negatives;
            if(conn )
                recorded_connections.insert(recorder, conn);
            else {
                if(cnt_negatives == 1)
                    recorded_connections.insert(recorder, (RecMonConnection*) 1);
            }
        }
    }

}        

void NetRecMonTechnician::connectNet(bool on)
{

    if(!loadOK || !netOK) return;
    if(net_on == on) return;
    net_on = on;
    QMapIterator<int, RecMonDevice*> it_mondev(monitored_devices);
    QHashIterator<void*, RecMonConnection*> it_monconn(monitored_connections);
    QMapIterator<int, RecMonDevice*> it_recdev(recorded_devices);
    QHashIterator<void*, RecMonConnection*> it_recconn(recorded_connections);
    RecMonDevice* dev = 0;
    RecMonConnection* conn = 0;
    while(it_mondev.hasNext()) {
        dev = it_mondev.next().value();
        if(on && is_monitoring)
            connect(this, SIGNAL(writeToMonitors(void)),
                    dev, SLOT(emitToMonitor(void)));
        else
            disconnect(this, SIGNAL(writeToMonitors(void)),
                       dev, SLOT(emitToMonitor(void)));
    }
    while(it_monconn.hasNext()) {
        conn = it_monconn.next().value();
        if(conn == (RecMonConnection*)1 || !conn) {
            continue;
        }
        if(on && is_monitoring)
            connect(this, SIGNAL(writeToMonitors(void)),
                    conn, SLOT(emitToMonitor(void)));
        else
            disconnect(this, SIGNAL(writeToMonitors(void)),
                       conn, SLOT(emitToMonitor(void)));
    }
    while(it_recdev.hasNext()) {
        dev = it_recdev.next().value();
        if(on && is_recording)
            connect(this, SIGNAL(writeToRecorders(void)),
                    dev, SLOT(emitToRecorder(void)));
        else
            disconnect(this, SIGNAL(writeToRecorders(void)),
                       dev, SLOT(emitToRecorder(void)));
    }
    while(it_recconn.hasNext()) {
        conn = it_recconn.next().value();
        if(conn == (RecMonConnection*)1 || !conn) {
            continue;
        }
        if(on && is_recording)
            connect(this, SIGNAL(writeToRecorders(void)),
                    conn, SLOT(emitToRecorder(void)));
        else
            disconnect(this, SIGNAL(writeToRecorders(void)),
                       conn, SLOT(emitToRecorder(void)));
    }

    QListIterator<BoxMonitor *> it_b(MonList);
    QListIterator<FileRecorder *> it_r(RecList);
    BoxMonitor* box;
    FileRecorder* rec;
    while(it_b.hasNext()) {
        box = it_b.next();
        dev = monitored_devices[box->getNumId()];
        QListIterator<GraphicMonitor *> it_g(box->getGraphs());
        GraphicMonitor* g_mon;
        while(it_g.hasNext()) {
            g_mon = it_g.next();
            QHashIterator<void*, RecMonConnection*> it_c(monitored_connections);
            int cnt_dev_signals = 0;
            while(it_c.hasNext()) {
                if(it_c.peekNext().key() != g_mon) {
                    it_c.next();
                    continue;
                }
                conn = it_c.next().value();
                if(conn != (RecMonConnection*) 1 && conn) {
                    if(on && is_monitoring)
                        connect(conn, SIGNAL(emitMonVal(double, int)),
                                g_mon, SLOT(paintVals(double, int)));
                    else
                        disconnect(conn, SIGNAL(emitMonVal(double, int)),
                                   g_mon, SLOT(paintVals(double, int)));
                }
                else {
                    if(!cnt_dev_signals) {
                        if(on && is_monitoring)
                            connect(dev, SIGNAL(emitMonVal(double, int)),
                                    g_mon, SLOT(paintVals(double, int)));
                        else
                            disconnect(dev, SIGNAL(emitMonVal(double, int)),
                                       g_mon, SLOT(paintVals(double, int)));
                    }
                    ++cnt_dev_signals;
                }
            }
        }
    }

    while(it_r.hasNext()) {
        rec = it_r.next();
        dev = recorded_devices[rec->getNumId()];
        setRecorderHeader(rec, dev);
        QHashIterator<void*, RecMonConnection*> it_rc(recorded_connections);
        int cnt_dev_signals = 0;
        while(it_rc.hasNext()) {
            if(it_rc.peekNext().key() != rec) {
                it_rc.next();
                continue;
            }
            conn = it_rc.next().value();
            if(conn != (RecMonConnection*) 1 && conn) {
                if(on && is_recording)
                    connect(conn, SIGNAL(emitRecVal(double, int)),
                            rec, SLOT(writeValue(double, int)));
                else
                    disconnect(conn, SIGNAL(emitRecVal(double, int)),
                               rec, SLOT(writeValue(double, int)));
            }
            else {
                if(!cnt_dev_signals) {
                    if(on && is_recording)
                        connect(dev, SIGNAL(emitRecVal(double, int)),
                                rec, SLOT(writeValue(double, int)));
                    else
                        disconnect(dev, SIGNAL(emitRecVal(double, int)),
                                   rec, SLOT(writeValue(double, int)));
                }
                ++cnt_dev_signals;
            }
        }
    }
    net_connected = true;
}

QString NetRecMonTechnician::save(void)
{    
    return  recmonDoc.toString(4);
}

void NetRecMonTechnician::writeGeneralParams(void)
{

    QDomElement tag_par = recmonDoc.createElement( "general_parameters" );
    recmonInfo.appendChild(tag_par);
    tag_par.setAttribute("isMonitoring",
                         QString::number(int(is_monitoring)));
    tag_par.setAttribute("isRecording",
                         QString::number(int(is_recording)));
    tag_par.setAttribute("recordPeriod",
                         QString::number(recordPeriod));
    tag_par.setAttribute("file_mode",
                         QString::number(file_mode));

}  

void NetRecMonTechnician::saveInfo(void)
{

    QListIterator<RecMonDevice *> iti(net_devices);
    RecMonDevice *dev;
    BoxMonitor* box;
    FileRecorder* recorder;
    QDomElement tag_dev;
//    QString xml_infoStr;
//    QTextStream stream1(&xml_infoStr);
//    xml_operator->removeChildren(recmonInfo);
//    recmonInfo.save(stream1, 4 /*indent*/);
    writeGeneralParams();

    while(iti.hasNext()) {
        dev = iti.next();
        box = findBoxMonitor(dev->getNumId());
        recorder = findRecorder(dev->getNumId());
        if(box || recorder) {
            tag_dev = recmonDoc.createElement( "Device" );
            recmonInfo.appendChild(tag_dev);
            tag_dev.appendChild(xml_operator->createTextElement(recmonDoc,
                                                                "dev_identification", QString::number(dev->getNumId())));
        }
        else {
            continue;
        }
        if(box) {
            QDomElement tag_box = recmonDoc.createElement("BoxMonitor");
            tag_dev.appendChild(tag_box);
            box->write(recmonDoc, tag_box);
        }
        if(recorder) {
            QDomElement tag_rec = recmonDoc.createElement( "Recorder" );
            tag_dev.appendChild(tag_rec);
            recorder->write(recmonDoc, tag_rec);
//            xml_infoStr = "";
//            recmonInfo.save(stream1, 4 /*indent*/);
        }
    }
//    xml_infoStr = "";
//    recmonInfo.save(stream1, 4 /*indent*/);
}

bool NetRecMonTechnician::setFile(const QString& f_name)
{

    loadOK = false;
    file_name = f_name;
    clearInfo();
    file_really_open = false;
    if(f_name.trimmed().isEmpty()) {
        QString ext = net_name.section(QDir::toNativeSeparators("/"), -1).section('.', -1);
        file_name = net_name.left(net_name.length() - ext.length()) + "rmon";
        recmonDoc = QDomDocument("monitor_record");
        recmonInfo = recmonDoc.createElement("Monitor_Record");
        recmonDoc.appendChild(recmonInfo);
        writeGeneralParams();
        loadOK = false;
    }
    else {
        NeuralDisDoc* doc_manager = NeuralDisDoc::instance();
        file_name = Settings::instance()->fillPath(file_name,
                                               Settings::instance()->getNetDirectory());
        recmonDoc = doc_manager->loadXML(file_name, "monitor_record",
                                         tr("Net recorders and monitors load"));
        if(!recmonDoc.isNull()) {
            QDomNode n = recmonDoc.firstChild();
            recmonInfo = n.toElement();
            if(!recmonInfo.isNull()) {
                loadOK = true;
                if(file_name != f_name)
                    file_changed = true;
                file_really_open = true;
            }
        }
    }
    return loadOK;

}

BoxMonitor* NetRecMonTechnician::findBoxMonitor(int n_id)
{

    QListIterator<BoxMonitor *> it(MonList);
    BoxMonitor *box_mon = 0, *box_mon_aux = 0;
    while(it.hasNext()) {
        box_mon_aux = it.next();
        if(box_mon_aux->getNumId() == n_id) {
            box_mon = box_mon_aux;
            break;
        }
    }
    return box_mon;

}

FileRecorder* NetRecMonTechnician::findRecorder(int n_id)
{

    QListIterator<FileRecorder *> it(RecList);
    FileRecorder *recorder = 0, *recorder_aux = 0;
    while(it.hasNext()) {
        recorder_aux = it.next();
        if(recorder_aux->getNumId() == n_id) {
            recorder = recorder_aux;
            break;
        }
    }
    return recorder;

}

void NetRecMonTechnician::doMonitor(void)
{

    if(monitor_on)
        emit writeToMonitors();
    if(recorder_on) {
        if(recordPeriod) {
            if(!(crono % (100*recordPeriod)))
                emit writeToRecorders();
        } else
            emit writeToRecorders();
    }
    
}

void NetRecMonTechnician::connectToTime(bool on)
{

    if(time_on == on) return;
    ClocksDispatcher* advance_timer = ClocksDispatcher::instance();
    time_on = on;
    if(on)
        connect(advance_timer, SIGNAL(Crono(int)), this, SLOT(setCrono(int)));
    else
        disconnect(advance_timer, SIGNAL(Crono(int)), this, SLOT(setCrono(int)));

}  

void NetRecMonTechnician::startMonitor(bool on)
{

    if(!is_monitoring)
        on = false;
    if(monitor_on == on)
        return;

    ClocksDispatcher* advance_timer = ClocksDispatcher::instance();
    if(on)
        showMonitors();
    else
        hideMonitors();

    QListIterator<BoxMonitor *> itb(MonList);
    BoxMonitor* box_mon;
    GraphicMonitor* gmon;
    while (itb.hasNext()) {
        box_mon = itb.next();
        QListIterator<GraphicMonitor *> itm(box_mon->gmons);
        while(itm.hasNext()) {
            gmon = itm.next();
            if(on) {
                connect(advance_timer, SIGNAL(Crono(int)),
                        gmon, SLOT(Crono(int)));
                connect(advance_timer, SIGNAL(initCrono(int)),
                        gmon, SLOT(initCrono(int)));
                connect(advance_timer, SIGNAL(CronoReset(bool)),
                        gmon, SLOT(setCronoReset(bool)));
            }
            else {
                disconnect(advance_timer, SIGNAL(Crono(int)),
                           gmon, SLOT(Crono(int)));
                disconnect(advance_timer, SIGNAL(initCrono(int)),
                           gmon, SLOT(initCrono(int)));
                disconnect(advance_timer, SIGNAL(CronoReset(bool)),
                           gmon, SLOT(setCronoReset(bool)));
            }
            gmon->reset();
        }
    }

    connectNet(on);
    monitor_on = on;

}

void NetRecMonTechnician::startRecorder(bool on, bool crono_reset)
{

    if(!is_recording)
        on = false;
    if(recorder_on == on)
        return;

    QListIterator<FileRecorder *> itr(RecList);
    FileRecorder* recorder;
    while (itr.hasNext()) {
        recorder = itr.next();
        recorder->connectToCrono(is_recording);
        if(on)  {
            recorder->setCronoReset(crono_reset);
            recorder->init();
        } else {
            recorder->closeFile();
        }
    }

    connectNet(on);
    recorder_on = on;

}


void NetRecMonTechnician::resetMonitors(void)
{

    QListIterator<BoxMonitor *> it(MonList);
    BoxMonitor *box;
    while(it.hasNext()) {
        box = it.next();
        box->reset();
    }

}

void NetRecMonTechnician::closeMonitors(void)
{

    QListIterator<BoxMonitor *> it(MonList);
    BoxMonitor *box;
    while(it.hasNext()) {
        box = it.next();
        Settings::instance()->getWorkspace()->removeSubWindow(box);
        delete box;
    }
    MonList.clear();
    monitored_devices.clear();
    monitored_connections.clear();

}

void NetRecMonTechnician::closeRecorders(void)
{

    QListIterator<FileRecorder *> it(RecList);
    FileRecorder *devrec;
    while(it.hasNext()) {
        devrec = it.next();
        delete devrec;
    }
    RecList.clear();
    recorded_devices.clear();
    recorded_connections.clear();

}

void NetRecMonTechnician::showMonitors(void)
{

    QListIterator<BoxMonitor *> it(MonList);
    BoxMonitor *box;
    while(it.hasNext()) {
        box = it.next();
        box->showNormal();
        box->getMdiSubWindow()->showNormal();
    }


}
void NetRecMonTechnician::hideMonitors(void)
{

    QListIterator<BoxMonitor *> it(MonList);
    BoxMonitor *box;
    while(it.hasNext()) {
        box = it.next();
        box->hide();
        box->getMdiSubWindow()->hide();
    }

}

void NetRecMonTechnician::setRecorderHeader(FileRecorder* recorder,
                                            RecMonDevice* dev)
{

    QString header;
    QTextStream ts(&header, QIODevice::WriteOnly);
    RecMonConnection * conn;
    QList<RecMonConnection*> conns = dev->getRecMonConnections();
    QListIterator<RecMonConnection*> it(conns);

    if(!it.hasNext())
        return;

    conn = it.next();

    ts << "# " + tr("Net name:") << " : " << net_name << "\n";
    ts << "# " + tr("Control file name:") << " : "  << control_filename << "\n";
    ts << "# " << dev->devName(true) << " "  << dev->getNumId() << " : "  ;

    QVector<int> rec_dev_channels = recorder->idChannels();
    QString s_chann = "";
    uint n_chann = rec_dev_channels.count();
    for(uint i = 0; i < n_chann; i++) {
        if(rec_dev_channels[i] > 0) continue;
        s_chann += dev->signalNumName(rec_dev_channels[i]);
        if(i < n_chann - 2)
            s_chann += ", ";
    }
    ts << s_chann << "\n";
    if(conn)
        ts << "# " << conn->connName(true) << " : ";
    recorder->setHeader(header);
    
}  

QList<QewExtensibleDialog*> NetRecMonTechnician::tabsForEdit(QewExtensibleDialog* parent)
{

    QList<QewExtensibleDialog*> add_dialogs;
    RecMonEditTab* recmon_tab = new RecMonEditTab(parent);
    if(recmon_tab->setUpDialog()) {
        add_dialogs.append(recmon_tab);
        recmon_tab->init(this);
    }
    else
        delete recmon_tab;

    return add_dialogs;

}

void NetRecMonTechnician::write(QDomDocument& doc, QDomElement& e)
{
    QString ret_name = file_really_open ? file_name : "";
    e.appendChild(xml_operator->createTextElement(doc, "monitors_file", ret_name.remove(Settings::instance()->getNetDirectory())));

}

void NetRecMonTechnician::clearInfo(void)
{
    recmonInfo.clear();
    recmonDoc.clear();
    MonList.clear();
    RecList.clear();
    monitored_devices.clear();
    monitored_connections.clear();
    recorded_devices.clear();
    recorded_connections.clear();
}
