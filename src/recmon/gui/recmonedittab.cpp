#include "recmonedittab.h"

#include <QVariant>
#include <QMessageBox>
#include <QDir>
#include <QFileDialog>
#include "../graphicmonitor.h"
#include "../boxmonitor.h"
#include "../devicerecorder.h"
#include "../recmonconnection.h"
#include "devmondialog.h"
#include "graphmondialog.h"
#include "../netrecmontechnician.h"
#include "../recmonfactory.h"
#include "../../neuraldis/xmloperator.h"
#include "../../neuraldis/neuraldisdoc.h"

/*
 *  Constructs a RecMonEditTab which is a child of 'parent', with the
 *  name 'name'.'
 */

RecMonEditTab::RecMonEditTab(QWidget* parent, const char* name)
    : QewTreeViewDialog(parent, name)
{
    setupUi(this);

    connect(addButton, SIGNAL(clicked()), this, SLOT(addMonitor()));
    connect(addRecordGraphToolButton, SIGNAL(clicked()),
            this, SLOT(addResRec()));
    connect(addRESGraphToolButton, SIGNAL(clicked()),
            this, SLOT(addResMon()));
    connect(delRecordGraphToolButton, SIGNAL(clicked()),
            this, SLOT(delResRec()));
    connect(delRESGraphToolButton, SIGNAL(clicked()), this, SLOT(delResMon()));
    connect(editButton, SIGNAL(clicked()), this, SLOT(editGeneral()));
    connect(editrecButton, SIGNAL(clicked()), this, SLOT(editGeneralRec()));
    connect(everyRecordSpinBox, SIGNAL(valueChanged(int)),
            this, SLOT(setEveryRecord(int)));
    connect(monTreeWidget, SIGNAL(itemDoubleClicked(QTreeWidgetItem*, int )),
            this, SLOT(editGraphItem(QTreeWidgetItem*)));
    connect(recTreeWidget, SIGNAL(itemDoubleClicked(QTreeWidgetItem*, int )),
            this, SLOT(editRecItem(QTreeWidgetItem*)));
    connect(recordCheckBox, SIGNAL(toggled(bool)),
            this, SLOT(setWFileMon(bool)));
    connect(saveFileButton, SIGNAL(clicked()), this, SLOT(saveToFile()));
    connect(weightsCheckBox, SIGNAL(toggled(bool)),
            this, SLOT(setWeightsMon(bool)));
    connect(writeComboBox, SIGNAL(activated(int)),
            this, SLOT(setFileMode(int)));
    connect(loadFileButton, SIGNAL(clicked()), this, SLOT(loadFromFile()));

}

/*
 *  Destroys the object and frees any allocated resources
 */
RecMonEditTab::~RecMonEditTab()
{
    // no need to delete child widgets, Qt does it all for us
}

/*
 *  Sets the strings of the subwidgets using the current
 *  language.
 */
void RecMonEditTab::languageChange()
{
    retranslateUi(this);
}

void RecMonEditTab::init(NetRecMonTechnician* netRMTech)
{

    netRecMonTech = netRMTech;
    netRecMonTech->saveInfo();
//    QString recmonInfoStr;
//    QTextStream stream2(&recmonInfoStr);
//    netRecMonTech->recmonInfo.save(stream2, 4 /*indent*/);
    factory = RecMonFactory::instance();
    xml_operator = XMLOperator::instance();


    //    nomonTreeWidget->setColumnWidth(0, 215);
    //    monTreeWidget->setColumnWidth(0, 215);
    //    norecTreeWidget->setColumnWidth(0, 215);
    //    recTreeWidget->setColumnWidth(0, 215);
    every_tenth_record = 0;
    adv_period = 20;
    sims_dir = ".";
    setFileMode(netRecMonTech->file_mode);
    setWeightsMon(netRecMonTech->is_monitoring);
    setWFileMon(netRecMonTech->is_recording);
    setEveryRecord(netRecMonTech->recordPeriod);

    loadInfo();

    setDevs(netRecMonTech->net_devices);

}

void RecMonEditTab::setSimsDir(const QString& d)
{
    sims_dir = d;
}

bool RecMonEditTab::validateThis(void) {
    if(wfil_mon && ((everyRecordSpinBox->text().toInt()*100)%adv_period))  {
//        QMessageBox::warning(nullptr, tr("Edit monitoring..."),
//                             tr("Some parameters were invalid") + "\n" +
//                             tr("and has been restored to previous values.") +
//                             "\n" + tr("Returning to edition."),  tr("&Return")) ;
        QMessageBox msgBox;
        msgBox.setIcon(QMessageBox::Warning);
        msgBox.setText(tr("Edit monitoring..."));
        msgBox.setInformativeText(tr("Some parameters were invalid") + "\n" + tr("and has been restored to previous values.") + "\n" + tr("Returning to edition."));
        QPushButton *button = msgBox.addButton(tr("Return"), QMessageBox::ActionRole);
        msgBox.exec();
        return false;
    }

    return true;
}

void  RecMonEditTab::saveRecorder(QDomElement& e_rec, int dev_id)
{

    QDomElement e_old_dev, e_old_rec, e_copy;
    QDomElement e_oper_rec = xml_operator->findTag("operation", e_rec);
    QString s_oper_rec = e_oper_rec.text();
    e_rec.removeChild(e_oper_rec);
    if(s_oper_rec == "delete") {
        e_old_dev = xml_operator->findParentTag("dev_identification",
                                                netRecMonTech->recmonInfo, dev_id);
        QDomElement e_old_rec = xml_operator->findTag("Recorder", e_old_dev);
        e_old_dev.removeChild(e_old_rec);
    }
    else if(s_oper_rec == "append") {
        e_copy = e_rec.cloneNode().toElement();
        e_old_dev = xml_operator->findParentTag("dev_identification",
                                                netRecMonTech->recmonInfo, dev_id);
        QDomElement e_old_rec = xml_operator->findTag("Recorder", e_old_dev);
        if(!e_old_rec.isNull())
            e_old_dev.removeChild(e_old_rec);
        e_old_dev.appendChild(e_copy);
    }
    else if(s_oper_rec == "edit") {
        QDomElement e_old_dev =
                xml_operator->findParentTag("dev_identification",
                                            netRecMonTech->recmonInfo, dev_id);
        QDomElement e_old_rec = xml_operator->findTag("Recorder", e_old_dev);
        e_old_dev.appendChild(e_rec);
        e_old_dev.replaceChild(e_rec, e_old_rec);
    }

}

void  RecMonEditTab::saveBoxMonitor(QDomElement& e_box, int dev_id)
{

    QString s_oper_box;
    QDomElement e_old_dev, e_old_box, e_copy;
    QDomElement e_oper_box = xml_operator->findTag("operation", e_box);

    e_old_dev = xml_operator->findParentTag("dev_identification",
                                            netRecMonTech->recmonInfo, dev_id);
    e_old_box = xml_operator->findTag("BoxMonitor", e_old_dev);
    if(!e_oper_box.isNull())
        s_oper_box = e_oper_box.text();
    if(s_oper_box == "delete") {
        e_old_dev.removeChild(e_old_box);
    }
    else if(s_oper_box == "append") {
        if(!e_old_box.isNull())
            e_old_dev.removeChild(e_old_box);
        xml_operator->deleteElementsByTagName("operation", e_box);
        e_copy = e_box.cloneNode().toElement();
        e_old_dev.appendChild(e_copy);
        e_old_dev.insertAfter(e_copy, e_old_dev.firstChild());
    }
    else if(s_oper_box == "edit") {
        xml_operator->deleteElementsByTagName("operation", e_box);
        e_copy = e_box.cloneNode().toElement();
        e_old_dev.appendChild(e_box);
        e_old_dev.replaceChild(e_box, e_old_box);
    }

}

void  RecMonEditTab::saveDevice(QDomElement& e)
{

    QDomElement e_box = xml_operator->findTag("BoxMonitor", e);
    QDomElement e_rec = xml_operator->findTag("Recorder", e);
    QDomElement e_oper_dev, e_oper_rec;
    QDomElement e_old_dev, e_old_rec;
    QDomElement e_copy;
    int dev_id = xml_operator->findTag("dev_identification", e).
            text().toInt();
    e_oper_dev = xml_operator->findTag("operation", e);
    if(e_oper_dev.isNull())
        return;
    QString s_oper_dev = e_oper_dev.text();
    e.removeChild(e_oper_dev);
    if(s_oper_dev == "delete") {
        e_old_dev =
                xml_operator->findParentTag("dev_identification",
                                            netRecMonTech->recmonInfo, dev_id);
        netRecMonTech->recmonInfo.removeChild(e_old_dev);
        return;
    }
    else if(s_oper_dev == "append") {
        if(!e_box.isNull())
            saveBoxMonitor(e_box, dev_id);
        if(!e_rec.isNull())
            saveRecorder(e_rec, dev_id);
        e_copy = e.cloneNode().toElement();
        netRecMonTech->recmonInfo.appendChild(e_copy);
        xml_operator->insertChildByTagName("dev_identification",
                                           netRecMonTech->recmonInfo, e_copy, false);
        return;
    }
    else if(s_oper_dev == "edit") {
        if(!e_box.isNull())
            saveBoxMonitor(e_box, dev_id);
        if(!e_rec.isNull())
            saveRecorder(e_rec, dev_id);
    }

}

void RecMonEditTab::saveThis(void)
{

    bool change = false;
    if(netRecMonTech->file_mode != getFileMode())
        change = true;
    if(netRecMonTech->is_monitoring != getWeightsMon())
        change = true;
    if(netRecMonTech->is_recording != getWFileMon())
        change = true;
    if(netRecMonTech->recordPeriod != getEveryRecord())
        change = true;
    if(devMon_changed || devRec_changed)
        change = true;
    if(!change)
        return;
    //  cout << doc_info.toString(4);
    //  cout << "#\n#\n#\n#\n";
    writeThis();
    //  cout << netRecMonTech->recmonDoc.toString(4);
    netRecMonTech->connectNet(false);
    netRecMonTech->closeMonitors();
    netRecMonTech->closeRecorders();
    netRecMonTech->construct();
    netRecMonTech->netOK = true;
    netRecMonTech->setNetConnection();
    if(netRecMonTech->is_monitoring && netRecMonTech->monitor_on)
        netRecMonTech->showMonitors();
    else
        netRecMonTech->hideMonitors();
}

void RecMonEditTab::writeThis(void)
{
    QDomNode n = xml_info.firstChild();
    QString xml_infoStr;
    QTextStream stream1(&xml_infoStr);
    xml_info.save(stream1, 4 /*indent*/);
    QString recmonInfoStr;
    QTextStream stream2(&recmonInfoStr);
    netRecMonTech->recmonInfo.save(stream2, 4 /*indent*/);

    QDomElement e_gen =
            netRecMonTech->recmonInfo.firstChild().toElement();
    if(e_gen.tagName() == "general_parameters")
        setGeneralParams(e_gen, getEveryRecord(), getFileMode(),
                         getWeightsMon(), getWFileMon());
    while(!n.isNull()) {
        QDomElement e = n.toElement();
        QString e_infoStr;
        QTextStream stream3(&e_infoStr);
        e.save(stream3, 4 /*indent*/);
        QString tag = e.tagName();
        if(e.tagName() == "Device")
            saveDevice(e);
        n = n.nextSibling();
    }
    recmonInfoStr = "";
    netRecMonTech->recmonInfo.save(stream2, 4 /*indent*/);
}

bool RecMonEditTab::getWeightsMon( void )
{
    return wg_mon;
}

void RecMonEditTab::setWeightsMon(bool w_m )
{
    wg_mon = w_m;
    weightsCheckBox->setChecked(w_m);
}


bool RecMonEditTab::getWFileMon( void )
{
    return wfil_mon;
}

void RecMonEditTab::setWFileMon(bool w_m )
{
    wfil_mon = w_m;
    if(wfil_mon) {
        if(verifyPeriodRec(every_tenth_record)) {
            recordCheckBox->setChecked(wfil_mon);
        }
        else {
            recordCheckBox->setChecked(wfil_mon);
            everyRecordSpinBox->blockSignals(true);
            everyRecordSpinBox->setValue(every_tenth_record);
            everyRecordSpinBox->blockSignals(false);
        }
    } else {
        recordCheckBox->setChecked(wfil_mon);
    }
}

bool RecMonEditTab::isConnMonitored(const QDomElement& e_box, int num_id)
{
    bool ret =
            xml_operator->findTagIdent("graph_id", e_box, num_id).isNull()
            ? false : true;
    return ret;
}

bool RecMonEditTab::isConnRecorded(const QDomElement& e_rec, int num_id)
{

    bool ret =
            xml_operator->findTagIdent("channel_id", e_rec, num_id).isNull()
            ? false : true;
    return ret;
}

void RecMonEditTab::setMonItem(const QDomElement& e_mon,
                               RecMonDevice* dev, QTreeWidgetItem *item_devmon)
{
    QTreeWidgetItem *item_conn, *item_monitor;
    bool ok;
    if(e_mon.isNull())
        return;
    if(e_mon.tagName() != "Monitor")
        return;

    QDomNode n_ident = e_mon.firstChild();
    if(n_ident.isNull())
        return;
    QDomElement e_ident = n_ident.toElement();
    if(e_ident.text().toInt(&ok) < 0)
        return;

    item_monitor = new QTreeWidgetItem(item_devmon);
    QString smon = tr("Monitor") + " " + e_ident.text();
    item_monitor->setText(0, smon);
    QDomNode n_graphs =  n_ident.nextSibling();
    n_graphs = n_graphs.nextSibling();
    if(n_graphs.isNull())
        return;

    QDomElement e_graphs = n_graphs.toElement();

    if(e_graphs.isNull())
        return;
    if(e_graphs.tagName() != "graphs")
        return;

    QList<RecMonConnection*> conns = dev->getRecMonConnections();
    QListIterator<RecMonConnection*> itr(conns);
    RecMonConnection *conn;
    while(itr.hasNext()) {
        conn = itr.next();
        QDomNode n_gr = e_graphs.firstChild();
        while(!n_gr.isNull()) {
            QDomElement e_gr = n_gr.toElement();
            if(e_gr.text().toUInt(&ok) == conn->getNumId()) {
                item_conn = new QTreeWidgetItem(item_monitor);
                item_conn->setText(0, conn->connName(true) + " " +
                                   QString::number(conn->getNumId()).rightJustified(4, '0'));
            }
            n_gr = n_gr.nextSibling();
        }
    }

}

void RecMonEditTab::setBoxItem(const QDomElement& e_box,
                               RecMonDevice* dev, QTreeWidgetItem *item_devnomon)
{
    QTreeWidgetItem *item_devmon, *item_conn;
    item_devmon = 0;
    item_conn  =0;
    QList<RecMonConnection*> conns = dev->getRecMonConnections();
    QListIterator<RecMonConnection*> itr(conns);
    RecMonConnection *conn;

    if(!e_box.isNull()) {
        item_devmon = new QTreeWidgetItem(monTreeWidget);
        item_devmon->setText(0, dev->devName(true) + " " +
                             QString::number(dev->getNumId()).rightJustified(4, '0'));
    }

    while(itr.hasNext()) {
        conn = itr.next();
        if(!e_box.isNull()) {
            if(!isConnMonitored(e_box, conn->getNumId())) {
                item_conn = new QTreeWidgetItem(item_devnomon);
                item_conn->setText(0, conn->connName(true) + " " +
                                   QString::number(conn->getNumId()).rightJustified(4, '0'));
            }
        }
        else {
            item_conn = new QTreeWidgetItem(item_devnomon);
            item_conn->setText(0, conn->connName(true) + " " +
                               QString::number(conn->getNumId()).rightJustified(4, '0'));
        }
    }
    if(e_box.isNull())
        return;
    QDomNode n = e_box.firstChild();

    while(!n.isNull()) {
        QDomElement e_box = n.toElement();
        if(e_box.isNull())
            return;
        if(e_box.tagName() == "Monitors") {
            QDomNode n_mon = e_box.firstChild();
            while(!n_mon.isNull()) {
                QDomElement e_mon = n_mon.toElement();
                setMonItem(e_mon, dev, item_devmon);
                n_mon = n_mon.nextSibling();
            }
        }
        n = n.nextSibling();
    }
}

void RecMonEditTab::setRecItem(const QDomElement& e_rec,
                               RecMonDevice* dev, QTreeWidgetItem *item_devnorec)
{
    QTreeWidgetItem *item_devrec, *item_conn_rec;
    item_devrec = 0;
    item_conn_rec = 0;
    QList<RecMonConnection*> conns = dev->getRecMonConnections();
    QListIterator<RecMonConnection*> itr(conns);
    RecMonConnection* conn;
    if(!e_rec.isNull()) {
        item_devrec = new QTreeWidgetItem(recTreeWidget);
        item_devrec->setText(0, dev->devName(true) + " " +
                             QString::number(dev->getNumId()).rightJustified(4, '0'));
    }
    while(itr.hasNext()) {
        conn = itr.next();
        if(!e_rec.isNull()) {
            if(!isConnRecorded(e_rec, conn->getNumId())) {
                item_conn_rec = new QTreeWidgetItem(item_devnorec);
                item_conn_rec->setText(0, conn->connName(true) + " " +
                                       QString::number(conn->getNumId()).rightJustified(4, '0'));
            }
            else {
                item_conn_rec= new QTreeWidgetItem(item_devrec);
                item_conn_rec->setText(0, conn->connName(true) + " " +
                                       QString::number(conn->getNumId()).rightJustified(4, '0'));
            }
        }
        else {
            item_conn_rec = new QTreeWidgetItem(item_devnorec);
            item_conn_rec->setText(0, conn->connName(true) + " " +
                                   QString::number(conn->getNumId()).rightJustified(4, '0'));
        }
    }
}

void RecMonEditTab::setDevs(QList<RecMonDevice *> dv) {
    RecMonDevice *dev;
    RecMonConnection *conn;
    QList<RecMonConnection*> conns;
    QTreeWidgetItem *item_devnomon, *item_conn_nomon;
    QTreeWidgetItem *item_devnorec, *item_conn_norec;
    devs = dv;
    QListIterator<RecMonDevice*> iti(devs);
    bool has_box;
    bool has_rec;

    monTreeWidget->clear();
    nomonTreeWidget->clear();
    recTreeWidget->clear();
    norecTreeWidget->clear();

//    QString xml_infoStr;
//    QTextStream stream1(&xml_infoStr);

    while(iti.hasNext()) {
        dev = iti.next();
        conns = dev->getRecMonConnections();
        QListIterator<RecMonConnection*> itr(conns);
        item_devnomon = new QTreeWidgetItem(nomonTreeWidget);
        item_devnomon->setText(0, dev->devName(true) + " " +
                               QString::number(dev->getNumId()).rightJustified(4, '0'));
        item_devnorec = new QTreeWidgetItem(norecTreeWidget);
        item_devnorec->setText(0, dev->devName(true) + " " +
                               QString::number(dev->getNumId()).rightJustified(4, '0'));
        QDomNode n = xml_operator->findParentTag("dev_identification",
                                                 netRecMonTech->recmonInfo, dev->getNumId());
        QDomElement e_tag_box, e_tag_rec;
        has_box = false;
        has_rec = false;
        if(!n.isNull()) {
            QDomElement e = n.cloneNode().toElement();
            xml_info.appendChild(e);
//            xml_infoStr = "";
//            xml_info.save(stream1, 4 /*indent*/);
            e_tag_box =  xml_operator->findTag("BoxMonitor", e);
            if(!e_tag_box.isNull()) {
                has_box = true;
                setBoxItem(e_tag_box, dev, item_devnomon);
            }
            e_tag_rec =  xml_operator->findTag("Recorder", e);
            if(!e_tag_rec.isNull()) {
                has_rec = true;
                setRecItem(e_tag_rec, dev, item_devnorec);
            }
//            xml_infoStr = "";
//            xml_info.save(stream1, 4 /*indent*/);
        }
        while(itr.hasNext()) {
            conn = itr.next();
            if(!has_box) {
                item_conn_nomon = new QTreeWidgetItem(item_devnomon);
                item_conn_nomon->setText(0, conn->connName(true) + " " +
                                         QString::number(conn->getNumId()).
                                         rightJustified(4, '0'));
            }
            if(!has_rec) {
                item_conn_norec = new QTreeWidgetItem(item_devnorec);
                item_conn_norec->setText(0, conn->connName(true) + " " +
                                         QString::number(conn->getNumId()).
                                         rightJustified(4, '0'));
            }
        }
        conns.clear();
    }
    devMon_changed = false;
    devRec_changed = false;

//    xml_infoStr = "";
//    xml_info.save(stream1, 4 /*indent*/);
}

void RecMonEditTab::addResMon(void) {
    QTreeWidgetItem *item_nomon, *item_monitor, *item_devnomon, *item_mon;
    QTreeWidgetItem *item_devmon;
    QString s_mon, s_conn, ss_mon;
    QDomElement e_dev, e_box, e_mon,  e_grs, e_gr_id;
    int n_mon, n_conn, n_dev;
    bool devmon_exist = false;
    QList<QTreeWidgetItem *> itemList = nomonTreeWidget->selectedItems();
    if(!itemList.size())
        return;

    item_nomon = itemList.at(0);
    if(!(item_devnomon = item_nomon->parent())) {  // Dev
        item_devnomon = item_nomon;
        item_devmon = monTreeWidget->topLevelItem(0);
        while(item_devmon) {
            if(item_devnomon->text(0) == item_devmon->text(0)) {
                devmon_exist = true;
                break;
            }
            item_devmon = monTreeWidget->itemBelow(item_devmon);
        }
        if(!devmon_exist) {
            item_devmon = new QTreeWidgetItem(monTreeWidget);
            item_devmon->setText(0, item_nomon->text(0));
            if(editDevMonitor(item_devmon->text(0))) {
                nomonTreeWidget->expandItem(item_devnomon);
                monTreeWidget->expandItem(item_devmon);
                monTreeWidget->setCurrentItem(item_devmon);
                monTreeWidget->scrollToItem(item_devmon);
            }
            else
                delete item_devmon;
        }
    }
    else {  // Connection
        QList<QTreeWidgetItem *> itemList = monTreeWidget->selectedItems();
        if(!itemList.size())
            return;
        item_monitor = itemList.at(0);

        if(!(item_devmon = item_monitor->parent())){
            QMessageBox msgBox;
            msgBox.setIcon(QMessageBox::Information);
            msgBox.setText(tr("Select a device to add the connection"));
            msgBox.exec();
            return;
        }
        if(!(item_devmon->text(0) == item_devnomon->text(0))) {
            QMessageBox msgBox;
            msgBox.setIcon(QMessageBox::Information);
            msgBox.setText(tr("The devices have to be the same"));
            msgBox.exec();
            return;
        }

        s_mon = item_monitor->text(0);
        ss_mon = s_mon.section(' ', 0, 0);
        n_mon = s_mon.section(' ', 1, 1).toInt();
        if(ss_mon != tr("Monitor") )
            return;
        if(item_monitor->childCount() >= GraphicMonitor::N_MAX_GRAPHS) {
            GraphicMonitor::nMaxGraphsMessage();
            return;
        }
        s_conn =  item_nomon->text(0);
        n_conn = s_conn.section(' ',  -1).toInt();
        n_dev = item_devmon->text(0).section(' ', -1).toInt();

        e_dev = xml_operator->findParentTag("dev_identification",
                                            xml_info, n_dev);
        e_box = xml_operator->findTag("BoxMonitor", e_dev);
        e_mon = xml_operator->findParentTag("mon_identification", e_box, n_mon);
        e_grs = xml_operator->findTag("graphs", e_mon);

        e_gr_id = xml_operator->createTextElement(doc_info,
                                                  "graph_id", QString::number(n_conn));
        e_grs.appendChild(e_gr_id);
        xml_operator->insertChildByTagName("graph_id", e_grs, e_gr_id);
        item_mon = new QTreeWidgetItem(item_monitor);
        item_mon->setText(0, s_conn);
        monTreeWidget->scrollToItem(item_mon);
        if(nomonTreeWidget->itemBelow(item_nomon)) {
            nomonTreeWidget->setCurrentItem(nomonTreeWidget->itemBelow(item_nomon));
            nomonTreeWidget->scrollToItem(nomonTreeWidget->itemBelow(item_nomon));
        }
        delete item_nomon;
    }
    devMon_changed = true;
}

void RecMonEditTab::addMonitor(void) {

    QTreeWidgetItem * item_monitor,  *item_devmon, *item_children;
    QList<QTreeWidgetItem *> itemList = monTreeWidget->selectedItems();
    if(!itemList.size())
        return;

    item_devmon = itemList.at(0);
    if(item_devmon->parent())
        return;

    int n_mon, n_children;
    n_children = item_devmon->childCount();
    if(!n_children) n_mon = 1;
    else {
        item_children = item_devmon->child(0);
        for(int i = 0; i < n_children - 1; i++)
            item_children = monTreeWidget->itemBelow(item_children);
        n_mon = item_children->text(0).section(' ', -1).toInt() + 1;
        if(!n_mon) n_mon = 1;
    }
    QString s_mon = tr("Monitor") + " " +
            QString::number(n_mon).rightJustified(2, '0');
    if(editGraphMonitor(item_devmon->text(0), s_mon)) {
        item_monitor = new QTreeWidgetItem(item_devmon);
        item_monitor->setText(0, s_mon);
        monTreeWidget->setCurrentItem(item_monitor);
        monTreeWidget->scrollToItem(item_monitor);
        devMon_changed = true;
    }
}

void RecMonEditTab::delResMon(void) {
    QTreeWidgetItem * item_nomon, *item_mon, *item_devmon,
            *item_devnomon, *item_monitor;
    int id_mon, id_dev, i;
    QDomElement e_dev, e_mon, e_rec, e_oper, e_oper_dev, e_oper_box;

    QList<QTreeWidgetItem *> itemList = monTreeWidget->selectedItems();
    if(!itemList.size())
        return;
    item_mon = itemList.at(0);
    if(!(item_devmon = item_mon->parent())) {  // Dev
        item_devmon = item_mon;
        QList<QTreeWidgetItem*> itemListF = nomonTreeWidget->findItems(item_devmon->text(0),
                                                                Qt::MatchExactly, 0);
        if(!itemListF.size())
            return;

        item_devnomon = itemListF.at(0);
        item_monitor = item_devmon->child(0);
        while(item_monitor) {
            item_mon = item_monitor->child(0);
            while(item_mon) {
                item_nomon = new QTreeWidgetItem(item_devnomon);
                item_nomon->setText(0, item_mon->text(0));
                item_mon = monTreeWidget->itemBelow(item_mon);
            }
            item_monitor = monTreeWidget->itemBelow(item_monitor);
        }
        id_dev = item_devmon->text(0).section(' ', -1).toInt();
        e_dev = xml_operator->findParentTag("dev_identification", xml_info, id_dev);
//        e_oper_dev = xml_operator->findTag("operation", e_dev);

        e_mon = xml_operator->findTag("BoxMonitor", e_dev);
        e_rec = xml_operator->findTag("Recorder", e_dev);
        bool del_rec = false;
        if(!e_rec.isNull()) {
            e_oper = xml_operator->findTag("operation", e_rec);
            if(e_oper.text() == "delete")
                del_rec = true;
        }
        else
            del_rec = true;
        if(del_rec) {
            e_oper = xml_operator->findTag("operation", e_dev);
            if(!e_oper.isNull())
                e_dev.removeChild(e_oper);
            xml_operator->removeChildren(e_dev, 1);
            e_oper = xml_operator->createTextElement(doc_info,
                                                     "operation", "delete");
            e_dev.appendChild(e_oper);
            e_dev.insertBefore(e_oper, e_dev.firstChild());
        }
        else {
            e_oper = xml_operator->createTextElement(doc_info,
                                                     "operation", "delete");
            xml_operator->removeChildren(e_mon);
            e_mon.appendChild(e_oper);
        }
        delete item_devmon;
    }
    else if(item_mon->text(0).section(' ', 0, 0) == tr("Monitor")) { // Monitor
        item_monitor = item_mon;
        item_devmon = item_monitor->parent();

        id_dev = item_devmon->text(0).section(' ', -1).toInt();

        QList<QTreeWidgetItem*> itemListF = nomonTreeWidget->findItems(item_devmon->text(0),
                                                                Qt::MatchExactly, 0);
        if(!itemListF.size())
            return;

        item_devnomon = itemListF.at(0);
        item_mon = item_monitor->child(0);
        while(item_mon) {
            item_nomon = new QTreeWidgetItem(item_devnomon);
            item_nomon->setText(0, item_mon->text(0));
            item_mon = monTreeWidget->itemBelow(item_mon);
        }
        id_mon = item_monitor->text(0).section(' ', -1).toInt();
        item_mon = item_devmon->child(0);
        while(item_mon) {
            if((i = item_mon->text(0).section(' ', -1).toInt()) > id_mon)
                item_mon->setText(0, tr("Monitor") + " " + QString::number(i - 1).rightJustified(2, '0'));
            item_mon = monTreeWidget->itemBelow(item_mon);
        }
        id_dev = item_devmon->text(0).section(' ', -1).toInt();
        e_dev = xml_operator->findParentTag("dev_identification", xml_info, id_dev);
        xml_operator->deleteParentsByTagIdent("mon_identification", e_dev, id_mon);
        delete item_monitor;
    }
    else {      // Connection
        item_monitor = item_devmon;
        item_devmon = item_monitor->parent();

        QList<QTreeWidgetItem*> itemListF = nomonTreeWidget->findItems(item_devmon->text(0),
                                                                Qt::MatchExactly, 0);
        if(!itemListF.size())
            return;

        item_devnomon = itemListF.at(0);

        item_nomon = new QTreeWidgetItem(item_devnomon);
        item_nomon->setText(0, item_mon->text(0));
        nomonTreeWidget->expandItem(item_devnomon);
        id_mon = item_monitor->text(0).section(' ', -1).toInt();
        if(item_monitor->childCount() == 1) {
            item_mon = item_devmon->child(0);
            while(item_mon) {
                if((i = item_mon->text(0).section(' ', -1).toInt()) > id_mon)
                    item_mon->setText(0, tr("Monitor") + " " + QString::number(i - 1).rightJustified(2, '0'));
                item_mon = monTreeWidget->itemBelow(item_mon);
            }

            id_dev = item_devmon->text(0).section(' ', -1).toInt();
            e_dev = xml_operator->findParentTag("dev_identification", xml_info, id_dev);
            xml_operator->deleteParentsByTagIdent("mon_identification",
                                                  e_dev, id_mon);
            delete item_monitor;
        }
        else {
            id_dev = item_devmon->text(0).section(' ', -1).toInt();
            e_dev = xml_operator->findParentTag("dev_identification", xml_info, id_dev);
            e_mon = xml_operator->findParentTag("mon_identification", e_dev, id_mon);
            QDomElement e_grs = xml_operator->findTag("graphs", e_mon);
            QDomElement e_gr_id = xml_operator->findTagIdent("graph_id", e_grs,
                                                             item_mon->text(0).section(' ', -1).toInt());
            e_grs.removeChild(e_gr_id);
            e_oper = xml_operator->createTextElement(doc_info,
                                                     "operation", "edit");
            xml_operator->deleteElementsByTagName("operation", e_mon);
            e_mon.appendChild(e_oper);
            e_mon.insertBefore(e_oper, e_mon.firstChild());
            delete item_mon;
        }
    }
    e_dev = xml_operator->findParentTag("dev_identification", xml_info, id_dev);
    e_oper_dev = xml_operator->findTag("operation", e_dev);
    if(e_oper_dev.isNull()) {
        e_oper_dev = xml_operator->createTextElement(doc_info,
                                                     "operation", "edit");
        e_dev.appendChild(e_oper_dev);
        e_dev.insertBefore(e_oper_dev, e_dev.firstChild());
    }
    else
        e_oper_dev.setNodeValue("edit");

    e_mon = xml_operator->findTag("BoxMonitor", e_dev);
    e_oper_box = xml_operator->findTag("operation", e_mon);
    if(e_oper_box.isNull()) {
        e_oper_box = xml_operator->createTextElement(doc_info,
                                                     "operation", "edit");
        e_mon.appendChild(e_oper_box);
        e_mon.insertBefore(e_oper_box, e_mon.firstChild());
    }
    else
        e_oper_box.setNodeValue("edit");
    devMon_changed = true;
}

void RecMonEditTab::editGraphItem(QTreeWidgetItem *item_mon) {
    QTreeWidgetItem  *item_devmon;
    if(!item_mon)
        return;
    if(!(item_devmon = item_mon->parent()))
        editDevMonitor(item_mon->text(0));
    else if(item_mon->text(0).section(' ', 0, 0) == tr("Monitor"))
        editGraphMonitor(item_devmon->text(0), item_mon->text(0));
}

void RecMonEditTab::editDevItem(QTreeWidgetItem *item_nomon, QTreeWidget* treeWidget)
{
    RecMonDevice *dev;
    RecMonConnection *conn;
    QTreeWidgetItem  *item_devnomon;
    if(!item_nomon)
        return;
    if((item_devnomon = item_nomon->parent())) {
        if(!(dev = findDev(item_devnomon->text(0))))
            return;
        if(!(conn = findReson(dev, item_nomon->text(0))))
            return;
        conn->edit(false);
    }
    else {
        if(!(dev = findDev(item_nomon->text(0))))
            return;
        dev->edit(false);
    }
    treeWidget->expandItem(item_nomon);
}

bool RecMonEditTab::editDevMonitor(QString s_dev) {
    bool ret = false;
    int n_dev;
    RecMonDevice *dev;
    dev = findDev(s_dev);
    n_dev = s_dev.section(' ', -1).toInt();
    QString infor = "Hola\nsoy infor";
    GraphicMonitor *gmon_01 = 0, *gmon_02 = 0;
    BoxMonitor* box = 0;
    QDomElement e_box, e_mon;
    QDomText text_dev_oper, text_box_oper;
    QDomElement e_dev_oper, e_box_oper;
    QString s_old_dev_oper, s_old_box_oper;
    QString s_dev_oper, s_box_oper;

    QDomElement e_dev = xml_operator->findParentTag("dev_identification",
                                                    xml_info, n_dev);

    if(!e_dev.isNull()) {
        e_dev_oper = xml_operator->findTag("operation", e_dev);
        if(!e_dev_oper.isNull()) {
            s_dev_oper = e_dev_oper.text();
            s_old_dev_oper = s_dev_oper;
            e_dev.removeChild(e_dev_oper);
        }
        else
            s_dev_oper = "edit";
        e_box = xml_operator->findTag("BoxMonitor", e_dev);
        if(!e_box.isNull()) {
            e_box_oper = xml_operator->findTag("operation", e_box);

            if(!e_box_oper.isNull()) {
                s_box_oper = e_box_oper.text();
                s_old_box_oper = s_box_oper;
                if(s_box_oper == "delete")
                    s_box_oper = "append";
                e_box.removeChild(e_box_oper).toElement();
            }
            else
                s_box_oper = "edit";
        }
        else
            s_box_oper = "append";
    }
    else {
        s_dev_oper = "append";
        s_box_oper = "append";
    }

    if(s_box_oper == "append") {
        box = new BoxMonitor(n_dev, dev->devName(true) + " " +
                             QString::number(n_dev), false, 0);
        box->hide();
        gmon_02 = new GraphicMonitor(60, 7, 0.0, 1.0, 6, tr("Output"),
                                     &box->fore_color, &box->back_color, box, "-2");
        box->gmons.append(gmon_02);
        gmon_01 = new GraphicMonitor(60, 7, 0.0, 1.0, 6, tr("Output_first_difference"),
                                     &box->fore_color, &box->back_color, box, "-1");
        box->gmons.append(gmon_01);
    }
    else if(s_box_oper == "edit") {
        box = factory->factoryBoxMonitor(e_box);
        box->hide();
        QDomElement e_gmon02 = xml_operator->
                findParentTag("mon_identification", e_box, -2);
        if(e_gmon02.isNull()) {
            gmon_02 = new GraphicMonitor(60, 7, 0.0, 1.0, 6, tr("Output"),
                                         &box->fore_color, &box->back_color, box, "-2");
            box->gmons.prepend(gmon_02);
        }
        else {
            gmon_02 = box->getGraphs().at(0);
        }
        QDomElement e_gmon01 = xml_operator->
                findParentTag("mon_identification", e_box, -1);
        if(e_gmon01.isNull()) {
            gmon_01 = new GraphicMonitor(60, 7, 0.0, 1.0, 6, tr("Output_first_difference"),
                                         &box->fore_color, &box->back_color, box, "-1");
            if(box->getGraphs().count())
                box->getGraphs().insert(1, gmon_01);
            else
                box->getGraphs().append(gmon_01);
        }
        else {
            if(box->getGraphs().count())
                gmon_01 = box->getGraphs().at(1);
            else
                gmon_01 = box->getGraphs().at(0);
        }
    }

    DevMonDialog *dialog = new DevMonDialog(0, 0, false);
    dialog->init(gmon_02, gmon_01, 3, 4);
    dialog->setInfor(infor);
    dialog->setSignalNames(dev->signalNames());
    if(dialog->exec() == QDialog::Accepted) {
        e_dev_oper = xml_operator->createTextElement(doc_info,
                                                     "operation", s_dev_oper);
        if(s_old_dev_oper == "delete")
            xml_info.removeChild(e_dev);
        if(s_dev_oper == "append") {
            e_dev = doc_info.createElement("Device");
            e_dev.appendChild(xml_operator->createTextElement(doc_info,
                                                              "dev_identification", QString::number(box->getNumId())));
            xml_operator->insertChildByTagName("dev_identification",
                                               xml_info, e_dev, false);
        }
        //    cout << doc_info.toString(4);
        if(!e_box.isNull()) {
            e_box_oper = xml_operator->findTag("operation", e_box);
            if(e_box_oper.isNull())
                s_box_oper = "edit";
            else
                s_box_oper = e_box_oper.text();
            e_dev.removeChild(e_box);
            //      e_box.clear();
        }
        else
            s_box_oper = "append";
        e_box = doc_info.createElement("BoxMonitor");
        e_dev.appendChild(e_box);

        e_box_oper = xml_operator-> createTextElement(doc_info,
                                                      "operation", s_box_oper);
        e_box.appendChild(e_box_oper);
        //    cout << doc_info.toString(4);
        box->write(doc_info, e_box);
        QDomElement e_mon1 = xml_operator->
                findParentTag("mon_identification", e_box, -2);
        QDomElement e_mon1_oper = xml_operator->createTextElement(doc_info,
                                                                  "operation", s_box_oper);
        e_mon1.appendChild(e_mon1_oper);
        e_mon1.insertBefore(e_mon1_oper, e_mon1.firstChild());

        QDomElement e_mon2 = xml_operator->findParentTag("mon_identification",
                                                         e_box, -1);
        QDomElement e_mon2_oper = xml_operator->createTextElement(doc_info,
                                                                  "operation", s_box_oper);
        e_mon2.appendChild(e_mon2_oper);
        e_mon2.insertBefore(e_mon2_oper, e_mon2.firstChild());

        e_dev.insertAfter(e_box, e_dev.firstChild());
        e_dev.appendChild(e_dev_oper);
        e_dev.insertBefore(e_dev_oper, e_dev.firstChild());
        //    cout << doc_info.toString(4);
        delete box;
        devMon_changed = true;
        ret = true;
    }
    else {
        if(!s_old_dev_oper.isEmpty()) {
            e_dev_oper = xml_operator->createTextElement(doc_info,
                                                         "operation", s_old_dev_oper);
            e_dev.appendChild(e_dev_oper);
            e_dev.insertBefore(e_dev_oper, e_dev.firstChild());
        }
        else if(!s_old_box_oper.isEmpty()) {
            e_box_oper = xml_operator->createTextElement(doc_info,
                                                         "operation", s_old_box_oper);
            e_box.appendChild(e_box_oper);
            e_box.insertBefore(e_box_oper, e_box.firstChild());
        }
        delete box;
    }
    return ret;

}

bool RecMonEditTab::editGraphMonitor(QString s_dev, QString s_mon) {
    bool ret = false;
    QString infor = "";
    int n_mon;
    GraphicMonitor *gmon;
    BoxMonitor *box;
    QDomElement e_dev, e_dev_id;
    QDomElement e_box, e_mons;
    QDomElement e_mon;
    QDomElement e_oper, e_dev_oper, e_box_oper;
    QDomText text_oper, text_dev_oper, text_box_oper;
    QDomElement e_null;
    QString s_oper, s_old_oper;
    int n_dev = s_dev.section(' ', -1).toInt();
    e_dev_id = xml_operator->findTagIdent("dev_identification", xml_info, n_dev);
    e_dev = xml_operator->findParentTag("dev_identification", xml_info, n_dev);
    n_mon = s_mon.section(' ', -1).toInt();
    //  cout << doc_info.toString(4);
    if(e_dev.isNull())
        return ret;
    e_box = xml_operator->findTag("BoxMonitor", e_dev);
    if(e_box.isNull())
        return ret;
    box = factory->factoryBoxMonitor(e_box);
    if(!box)
        return ret;
    box->hide();
    e_mon = xml_operator->findParentTag("mon_identification", e_dev, n_mon);

    if(!e_mon.isNull()) {
        e_oper = xml_operator->findTag("operation", e_mon);
        if(!e_oper.isNull()) {
            s_oper = e_oper.text();
            s_old_oper = s_oper;
            e_dev.removeChild(e_oper);
            if(s_oper == "delete")
                s_oper = "append";
            e_mon.removeChild(e_oper).toElement();
        }
        else
            s_oper = "edit";
    }
    else
        s_oper = "append";
    e_mons = xml_operator->findTag("Monitors", e_box);
    if(s_old_oper == "delete")
        e_mons.removeChild(e_mon);
    if(s_oper == "append") {
        gmon = new GraphicMonitor(e_null, box, QString::number(n_mon).toLatin1().data());
        box->addMon(gmon);
    }
    else
        gmon = factory->factoryGraphicMonitor(e_mon, box);

    graphmonDialog *dialog = new graphmonDialog(0, 0, false);
    dialog->init(gmon, 3, 4);
    dialog->setInfor(infor);
    if(dialog->exec() == graphmonDialog::Accepted) {
        e_dev_oper = xml_operator->findTag("operation", e_dev);
        if(e_dev_oper.isNull()) {
            e_dev_oper = xml_operator->createTextElement(doc_info,
                                                         "operation", "edit");
            e_dev.appendChild(e_dev_oper);
            e_dev.insertBefore(e_dev_oper, e_dev.firstChild());
        }
        e_box_oper = xml_operator->findTag("operation", e_box);
        if(e_box_oper.isNull()) {
            e_box_oper = xml_operator->createTextElement(doc_info,
                                                         "operation", "edit");
            e_box.appendChild(e_box_oper);
            e_box.insertBefore(e_box_oper, e_box.firstChild());
        }
        //    cout << doc_info.toString(4);
        e_oper = xml_operator->createTextElement(doc_info,
                                                 "operation", s_oper);
        if(s_oper == "append") {
            if(e_mons.isNull()) {
                e_mons =  doc_info.createElement("Monitors");
                e_box.appendChild(e_mons);
                e_box.insertAfter(e_mons, e_box.firstChild());
            }
            e_mon = doc_info.createElement("Monitor");
            e_mon.appendChild(e_oper);
            //     "mon_identification" se escribe en gmon->write()
            xml_operator->insertChildByTagName("mon_identification",
                                               e_mons, e_mon, false);
        }
        else if(s_oper == "edit") {
            xml_operator->removeChildren(e_mon);
            //      cout << doc_info.toString(4);
            e_mon.appendChild(e_oper);
        }
        gmon->write(doc_info, e_mon);
        devMon_changed = true;
        ret = true;
    }
    else {
        if(!s_old_oper.isEmpty()) {
            e_oper = xml_operator->createTextElement(doc_info,
                                                     "operation", s_old_oper);
            e_mon.appendChild(e_oper);
            e_mon.insertBefore(e_oper, e_mon.firstChild());
        }
    }
    //  cout << doc_info.toString(4);
    delete dialog;
    delete box;
    return ret;
}

RecMonConnection * RecMonEditTab::findReson(RecMonDevice *dev, QString s_conn) {
    RecMonConnection *conn = 0;
    uint  n_conn;
    bool ok;
    s_conn = s_conn.section(' ', -1);
    n_conn = s_conn.toUInt(&ok);
    if(dev && n_conn >0) {
        QList<RecMonConnection*> conns;
        conns = dev->getRecMonConnections();
        QListIterator<RecMonConnection*> it(conns);
        while(it.hasNext()) {
            conn = it.next();
            if(conn->getNumId() == n_conn)
                break;
        }

    }
    return conn;
}

RecMonDevice * RecMonEditTab::findDev(QString s_dev) {
    RecMonDevice *dev = 0;
    int n_dev;
    s_dev = s_dev.section(' ', -1);
    n_dev = s_dev.toInt();
    QListIterator<RecMonDevice*> it(devs);
    while(it.hasNext()) {
        dev = it.next();
        if(dev->getNumId() == uint(n_dev))
            break;
    }
    return dev;
}

bool RecMonEditTab::getDevMonChanged(void) {
    return devMon_changed;
}

bool RecMonEditTab::getDevRecChanged(void) {
    return devRec_changed;
}

void RecMonEditTab::editGeneral(void) {
    QList<QTreeWidgetItem *> itemList = monTreeWidget->selectedItems();
    QList<QTreeWidgetItem *> itemListNo = nomonTreeWidget->selectedItems();
    if(itemList.size()) {
        QTreeWidgetItem *item = itemList.at(0);
        editGraphItem(item);
    }
    else if(itemListNo.size()) {
        QTreeWidgetItem *item = itemListNo.at(0);
        editDevItem(item, nomonTreeWidget);
    }
}

void RecMonEditTab::addResRec(void) {
    QTreeWidgetItem *item_norec, *item_devnorec, *item_rec, *item_devrec;
    QString s_conn;
    int n_dev;
    bool devrec_exist = false;

    QList<QTreeWidgetItem *> itemList = norecTreeWidget->selectedItems();
    if(!itemList.size())
        return;

    item_norec = itemList.at(0);
    if(!(item_devnorec = item_norec->parent())) {  // Dev
        n_dev = item_norec->text(0).section(' ', -1).toInt();
        item_devnorec = item_norec;
        item_devrec = recTreeWidget->topLevelItem(0);
        while(item_devrec) {
            if(item_devnorec->text(0) == item_devrec->text(0)) {
                devrec_exist = true;
                break;
            }
            item_devrec = recTreeWidget->itemBelow(item_devrec);
        }
        if(!devrec_exist) {
            item_devrec = new QTreeWidgetItem(recTreeWidget);
            item_devrec->setText(0, item_norec->text(0));
            if(editDevRec(item_devrec->text(0))) {
                norecTreeWidget->expandItem(item_devnorec);
                norecTreeWidget->expandItem(item_devrec);
                recTreeWidget->setCurrentItem(item_devrec);
                recTreeWidget->scrollToItem(item_devrec);
                devRec_changed = true;
            }
            else
                delete item_devrec;
        }
    }
    else {  // Connection
        QList<QTreeWidgetItem *> itemList = recTreeWidget->selectedItems();
        if(!itemList.size()) {
            QMessageBox msgBox;
            msgBox.setIcon(QMessageBox::Information);
            msgBox.setText(tr("Select a device to add the connection"));
            msgBox.exec();
            return;
        }
        item_devrec = itemList.at(0);
        if(item_devrec->parent()) {
            QMessageBox msgBox;
            msgBox.setIcon(QMessageBox::Information);
            msgBox.setText(tr("Select a device to add the connection"));
            msgBox.exec();
            return;
        }
        if(!(item_devrec->text(0) == item_devnorec->text(0))) {
            QMessageBox msgBox;
            msgBox.setIcon(QMessageBox::Information);
            msgBox.setText(tr("The devices have to be the same"));
            msgBox.exec();
            return;
        }

        s_conn =  item_norec->text(0);
        item_rec = new QTreeWidgetItem(item_devrec);
        item_rec->setText(0, s_conn);
        recTreeWidget->scrollToItem(item_rec);
        if(norecTreeWidget->itemBelow(item_norec)) {
            norecTreeWidget->setCurrentItem(norecTreeWidget->itemBelow(item_norec), true);
            norecTreeWidget->scrollToItem(norecTreeWidget->itemBelow(item_norec));
        }

        int id_dev = item_devrec->text(0).section(' ', -1).toInt();
        int id_conn = item_rec->text(0).section(' ', -1).toInt();
        QDomElement e_dev = xml_operator->findParentTag("dev_identification",
                                                        xml_info, id_dev);
        QDomElement e_rec = xml_operator->findTag("Recorder", e_dev);
        QDomElement e_chs = xml_operator->findTag("channels", e_rec);
        QDomElement e_ch_id = xml_operator->createTextElement(doc_info,
                                                              "channel_id", QString::number(id_conn));
        e_chs.appendChild(e_ch_id);
        xml_operator->insertChildByTagName("channel_id", e_chs, e_ch_id);
        QDomElement e_dev_oper = xml_operator->findTag("operation", e_dev);
        devRec_changed = true;
        if(e_dev_oper.isNull()) {
            e_dev_oper = xml_operator->createTextElement(doc_info,
                                                         "operation", "edit");
            e_dev.appendChild(e_dev_oper);
            e_dev.insertBefore(e_dev_oper, e_dev.firstChild());
        }
        else
            e_dev_oper.setNodeValue("edit");
        QDomElement e_rec_oper = xml_operator->findTag("operation", e_rec);
        if(e_rec_oper.isNull()) {
            e_rec_oper = xml_operator->createTextElement(doc_info,
                                                         "operation", "edit");
            e_rec.appendChild(e_rec_oper);
            e_rec.insertBefore(e_rec_oper, e_rec.firstChild());
        }
        else
            e_rec_oper.setNodeValue("edit");

        delete item_norec;
    }
    return;
}

void RecMonEditTab::delResRec(void) {
    QTreeWidgetItem *item_norec, *item_rec, *item_devrec, *item_devnorec;
    int id_dev;
    QDomElement e_dev, e_oper_dev, e_rec, e_oper;

    QList<QTreeWidgetItem *> itemList = recTreeWidget->selectedItems();
    if(!itemList.size())
        return;
    item_rec = itemList.at(0);   
    if(!(item_devrec = item_rec->parent())) {  // Dev
        item_devrec = item_rec;
        id_dev = item_devrec->text(0).section(' ', -1).toInt();
        e_dev = xml_operator->findParentTag("dev_identification",
                                            xml_info, id_dev);
        QList<QTreeWidgetItem*> itemListF = norecTreeWidget->findItems(item_devrec->text(0),
                                                                Qt::MatchExactly, 0);
        if(!itemListF.size())
            return;

        item_devnorec = itemListF.at(0);
        item_rec = item_devrec->child(0);
        while(item_rec) {
            item_norec = new QTreeWidgetItem(item_devnorec);
            item_norec->setText(0, item_rec->text(0));
            item_rec = recTreeWidget->itemBelow(item_rec);
        }

        e_rec = xml_operator->findTag("Recorder", e_dev);
        e_oper = xml_operator->findTag("Recorder", e_rec);
        if(e_oper.isNull()) {
            e_oper = xml_operator->createTextElement(doc_info,
                                                 "operation", "delete");
            e_rec.appendChild(e_oper);
            e_rec.insertBefore(e_oper, e_rec.firstChild());
        }
        else
            e_oper.setNodeValue("delete");


        delete item_devrec;
    }
    else {      // Connection
        QList<QTreeWidgetItem*> itemListF = norecTreeWidget->findItems(item_devrec->text(0),
                                                                Qt::MatchExactly, 0);
        if(!itemListF.size())
            return;

        item_devnorec = itemListF.at(0);
        item_norec = new QTreeWidgetItem(item_devnorec);
        item_norec->setText(0, item_rec->text(0));
        norecTreeWidget->expandItem(item_devnorec);

        id_dev = item_devrec->text(0).section(' ', -1).toInt();
        e_dev = xml_operator->findParentTag("dev_identification", xml_info, id_dev);
        e_rec = xml_operator->findTag("Recorder", e_dev);
        QDomElement e_chs = xml_operator->findTag("channels", e_rec);
        QDomElement e_ch_id = xml_operator->findTagIdent("channel_id", e_chs,
                                                         item_rec->text(0).section(' ', -1).toInt());
        e_chs.removeChild(e_ch_id);

        e_oper = xml_operator->findTag("Recorder", e_rec);
        if(e_oper.isNull()) {
            e_oper = xml_operator->createTextElement(doc_info,
                                                 "operation", "delete");
            e_rec.appendChild(e_oper);
            e_rec.insertBefore(e_oper, e_rec.firstChild());
        }
        else
            e_oper.setNodeValue("delete");

        e_oper = xml_operator->createTextElement(doc_info,
                                                 "operation", "edit");
//        e_rec.appendChild(e_oper);
//        e_rec.insertBefore(e_oper, e_rec.firstChild());

        delete item_rec;

    }
    e_dev = xml_operator->findParentTag("dev_identification", xml_info, id_dev);
    e_oper_dev = xml_operator->findTag("operation", e_dev);
    if(e_oper_dev.isNull()) {
        e_oper_dev = xml_operator->createTextElement(doc_info,
                                                     "operation", "edit");
        e_dev.appendChild(e_oper_dev);
        e_dev.insertBefore(e_oper_dev, e_dev.firstChild());
    }
    else {
        e_oper_dev.setNodeValue("edit");
    }
    devRec_changed = true;
}


void RecMonEditTab::editRecItem(QTreeWidgetItem *item_rec) {
    QTreeWidgetItem  *item_devrec;
    if(!item_rec)
        return;
    if(!(item_devrec = item_rec->parent()))
        editDevRec(item_rec->text(0));
}

bool RecMonEditTab::editDevRec(QString s_dev) {

    RecMonDevice *dev;
    FileRecorder *devrec;
    bool ret = false;
    int  n_dev;
    QDomElement e_rec, e_id, e_old_oper, e_oper, e_dev, e_dev_oper;
    QDomElement e_cons;
    QDomText text_id, text_dev_id, text_dev_oper, text_oper ;
    QString s_old_dev_oper, s_dev_oper, s_old_oper, s_oper;
    QString f_name;
    bool name_yet = false;

    n_dev = s_dev.section(' ', -1).toInt();

    if(!(dev = findDev(s_dev)))
        return ret;


    e_dev = xml_operator->findParentTag("dev_identification",
                                        xml_info, n_dev);
    if(!e_dev.isNull()) {
        e_rec = xml_operator->findTag("Recorder", e_dev);
        if(!e_rec.isNull()) {
            e_cons = xml_operator->findTag("constructor", e_rec);
            if(!e_cons.isNull()) {
                f_name = e_cons.attribute("filename");
                name_yet = true;
            }
        }
    }

    if(!name_yet) {
        QString ext = netRecMonTech->file_name.
                section(QDir::toNativeSeparators("/"), -1).
                section('.', 0, 0 );
        f_name = netRecMonTech->file_name;
        f_name = f_name.left(f_name.length() - ext.length() - 1 ) + "_" +
                dev->devName(false) +
                s_dev.section(' ', -1).rightJustified(4, '0') + ".rec";
    }

    QFileDialog *fd = new QFileDialog();
    fd->setDirectory(f_name);
    QStringList filters;
    filters << tr("Recording") + " (*.rec)" << tr("Any files") + " (*)";
    fd->setNameFilters(filters);
    fd->setFileMode(QFileDialog::AnyFile);
    if ( fd->exec() == QDialog::Accepted ) {
        QStringList fileNames = fd->selectedFiles();
        if(fileNames.size() == 0) {
            delete fd;
            return ret;
        }
        f_name = fileNames.at(0);
        ret = true;
        if(!e_dev.isNull()) {  // append
            e_oper = xml_operator->findTag("operation", e_rec);
            if(!e_oper.isNull())
                e_old_oper = e_rec.removeChild(e_oper).toElement();
            if(!e_rec.isNull() && e_old_oper.text() != "delete")
                devrec = factory->factoryRecorder(e_rec);
            else
                devrec = new DeviceRecorder(n_dev);
        }
        else
            devrec = new DeviceRecorder(n_dev);

        if(fd->selectedNameFilter() == tr("Recording") + " " +  "(*.rec)") {
            if(f_name.section(QDir::toNativeSeparators("/"), -1).indexOf(".") == -1)
                f_name += ".rec";
        }
        devrec->setFileName(f_name);

        if(e_dev.isNull()) {
            e_dev = doc_info.createElement("Device");
            xml_operator->insertChildByTagName("dev_identification",
                                               xml_info, e_dev, false);

            e_dev_oper = xml_operator->findTag("operation", e_dev);
            if(e_dev_oper.isNull()) {
                e_dev_oper = xml_operator->createTextElement(doc_info,
                                                             "operation", "append");
                e_dev.appendChild(e_dev_oper);
                e_dev.insertBefore(e_dev_oper, e_dev.firstChild());
            }
            else {
                e_dev_oper.setNodeValue("append");
            }

//            e_dev_oper = xml_operator->createTextElement(doc_info,
//                                                         "operation", "append");
//            e_dev.appendChild(e_dev_oper);
//            e_dev.insertBefore(e_dev_oper, e_dev.firstChild());
            e_id = xml_operator->createTextElement(doc_info, "dev_identification",
                                                   QString::number(devrec->getNumId()));
            e_dev.appendChild(e_id);
            e_dev.insertAfter(e_id, e_dev.lastChild());
        }
        else {
            e_dev_oper = xml_operator->findTag("operation", e_dev);
            if(e_dev_oper.isNull()) {
                e_dev_oper = xml_operator->createTextElement(doc_info,
                                                             "operation", "edit");
                e_dev.appendChild(e_dev_oper);
                e_dev.insertBefore(e_dev_oper, e_dev.firstChild());
            }
            else {
                s_dev_oper = e_dev_oper.text();
                s_old_dev_oper = s_dev_oper;
                if(s_dev_oper == "delete") {
                    text_dev_oper = doc_info.createTextNode("append");
                    e_dev_oper.appendChild(text_dev_oper);
                    e_dev.appendChild(e_dev_oper);
                }
            }
        }
        if(!e_rec.isNull()) {
            e_oper = xml_operator->findTag("operation", e_rec);
            if(e_oper.isNull())
                s_oper = "edit";
            else
                s_oper = e_oper.text();
            e_dev.removeChild(e_rec);
        }
        else
            s_oper = "append";
        s_old_oper = s_oper;
        e_rec = doc_info.createElement("Recorder");
        e_dev.appendChild(e_rec);
        e_dev.insertAfter(e_rec, e_dev.lastChild());

        e_oper = xml_operator->createTextElement(doc_info,
                                                 "operation", s_oper);
        e_rec.appendChild(e_oper);
        devrec->write(doc_info, e_rec);
        delete devrec;
        
        devRec_changed = true;
    }
    else {
        if(!s_old_dev_oper.isEmpty()) {
            e_dev_oper = xml_operator->createTextElement(doc_info,
                                                         "operation", s_old_dev_oper);
            e_dev.appendChild(e_dev_oper);
            e_dev.insertBefore(e_dev_oper, e_dev.firstChild());
        }
        else if(!s_old_oper.isEmpty()) {
            e_oper = xml_operator->createTextElement(doc_info,
                                                     "operation", s_old_oper);
            e_rec.appendChild(e_oper);
            e_rec.insertBefore(e_oper, e_rec.firstChild());
        }
    }
    //  cout << doc_info.toString(2);
    delete fd;
    return ret;

}

void RecMonEditTab::editGeneralRec(void) {

    QList<QTreeWidgetItem *> itemList = recTreeWidget->selectedItems();
    QList<QTreeWidgetItem *> itemListNo = norecTreeWidget->selectedItems();
    if(itemList.size()) {
        QTreeWidgetItem *item = itemList.at(0);
        editGraphItem(item);
    }
    else if(itemListNo.size()) {
        QTreeWidgetItem *item = itemListNo.at(0);
        editDevItem(item, norecTreeWidget);
    }
}

int RecMonEditTab::getFileMode(void)
{
    return file_mode;
}


void  RecMonEditTab::setFileMode(int fm)
{
    file_mode = fm;
    writeComboBox->setCurrentIndex(fm);
}

int RecMonEditTab::getEveryRecord( void)
{
    return everyRecordSpinBox->cleanText().toInt();
}

void  RecMonEditTab::setEveryRecord(int s)
{
    if(!verifyPeriodRec(s))
        recordCheckBox->toggle();
    everyRecordSpinBox->setValue(every_tenth_record);
}

bool RecMonEditTab::verifyPeriodRec(int period)
{
    int r, q;
    bool ret = false;
    r = (100*period) % adv_period;
    q = (100*period) / adv_period;
    if(!r) {
        every_tenth_record = period;
        ret = true;
    }
    else {
        if(wfil_mon) {
//            QMessageBox::information(nullptr, tr("Edit recording..."),
//                                     tr("Record period") +" (" + QString::number(period) + ") " + tr("multiplied by 100") +
//                                     "\n" + tr("must be multiple of advance period") +" (" +
//                                     QString::number(adv_period)  + ").\n\n" +
//                                     tr("Recording canceled."),
//                                     tr("&Return"));
            QMessageBox msgBox;
            msgBox.setIcon(QMessageBox::Question);
            msgBox.setText(tr("Edit recording..."));
            msgBox.setInformativeText(tr("Record period") +" (" + QString::number(period) + ") " + tr("multiplied by 100") +
                                      "\n" + tr("must be multiple of advance period") +" (" +
                                      QString::number(adv_period)  + ").\n\n" +
                                      tr("Recording canceled."));
            QPushButton *button = msgBox.addButton(tr("Return"), QMessageBox::ActionRole);
            msgBox.exec();
            wfil_mon = false;
            every_tenth_record = 0;
        }
        else {
            every_tenth_record = period;
            ret = true;
        }
    }
    return ret;
}

void  RecMonEditTab::setAdvPeriod(int ap)
{
    adv_period = ap;
    if(wfil_mon)
        setEveryRecord(every_tenth_record);
}

void RecMonEditTab::setGeneralParams(QDomElement& e_gen, int rec_period,
                                     int f_mode, bool is_mon, bool is_rec)
{
    e_gen.setAttribute("recordPeriod",
                       QString::number(rec_period));
    e_gen.setAttribute("file_mode", QString::number(f_mode));
    e_gen.setAttribute("isMonitoring",
                       QString::number(is_mon));
    e_gen.setAttribute("isRecording",
                       QString::number(is_rec));
}

void RecMonEditTab::saveToFile(void)
{
    if(validateThis()) {
//        if(QMessageBox::information(nullptr, tr("Saving monitors and recorders"),
//                                    tr("This operation cannot be cancelled"),
//                                    tr("&Continue"), tr("Abort")) == 1)
    if(QMessageBox::information(nullptr, tr("Saving monitors and recorders"), tr("This operation cannot be cancelled"), QMessageBox::Save | QMessageBox::Abort) == QMessageBox::Abort)
            return;
        QString xml_infoStr;
        QTextStream stream1(&xml_infoStr);
        xml_info.save(stream1, 4 /*indent*/);
        writeThis();
        NeuralDisDoc* doc_op = NeuralDisDoc::instance();
        QString ret_name = doc_op->saveDocument(netRecMonTech->file_name,  netRecMonTech->recmonDoc.toString(4), tr("Devices monitoring and recording"), "rmon", tr("Saving net monitors and recorders"), true);
        if(!ret_name.isEmpty()) {
            netRecMonTech->file_name = ret_name;
            xml_operator->deleteElementsByTagName("Device", xml_info);
        }
        xml_infoStr = "";
        xml_info.save(stream1, 4 /*indent*/);
    }

}


void RecMonEditTab::getExternVal( const QString & var_name, const QVariant & var_val )
{
    if(var_name == "simulation_dir")
        setSimsDir(var_val.toString());
    else if(var_name == "advance_period")
        setAdvPeriod(var_val.toInt());
}

QStringList RecMonEditTab::designedCaptions( void )
{
    QStringList cap_list;
    cap_list << tr("Monitoring");
    cap_list << tr("Recording");
    return cap_list;
}

void RecMonEditTab::loadFromFile( void )
{
    
    if( netRecMonTech->setFile(tr("Untitled") + "." + tr("rmon"))) {
        netRecMonTech->construct();
        if((netRecMonTech->isLoadOK())) {
            netRecMonTech->setNetConnection();
            loadInfo();
            setDevs(netRecMonTech->net_devices);
        }
    }
}

void RecMonEditTab::loadInfo()
{
    doc_info = QDomDocument("edit_monitor_record");
    QDomNode n = netRecMonTech->recmonInfo.cloneNode(false);
    if(n.isNull())
        xml_info = doc_info.createElement("Monitor_Record");
    else
        xml_info = n.toElement();
    QDomElement e_gen = doc_info.createElement("general_parameters");
    setGeneralParams(e_gen, getEveryRecord(), getFileMode(),
                     getWeightsMon(), getWFileMon());
    xml_info.appendChild(e_gen);
    doc_info.appendChild(xml_info);
}
