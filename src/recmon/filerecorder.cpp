//
// C++ Implementation: filerecorder
//
// Description: 
//
//
// Author: Jose Manuel Cuadra Troncoso <jmcuadra@dia.uned.es>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "filerecorder.h"
#include "../neuraldis/xmloperator.h"
#include "../sim/clocksdispatcher.h"

#include <QStringList>
#include <QDateTime>
#include <QFileDialog>
#include <iostream>

#include <qewdialogfactory.h>

FileRecorder::FileRecorder(const QDomElement& e, QObject *parent, const char *name) : QObject(parent)
{

    bool ok;
    type = BASE_RECORDER;
    n_channels = 0;
    init_crono = 0;
    crono = init_crono;
    crono_reset = false;
    really_append = true;
    if(e.tagName() == "constructor") {
        filename = e.attribute("filename");
        num_id = e.attribute("num_id").toInt(&ok);
        setFileMode(e.attribute("file_mode").toInt(&ok));

    }
    dialog = 0;
    recording = false;
}

FileRecorder::FileRecorder(int id, QObject *parent, const char *name)
    : QObject(parent)
{

    type = BASE_RECORDER;
    num_id = id;
    n_channels = 0;
    init_crono = 0;
    crono = init_crono;
    crono_reset = false;
    really_append = true;
    setFileMode(AUTO);
    dialog = 0;
    recording = false;

}

FileRecorder::FileRecorder(int id, const QString &fname, int f_mode,
                           QObject *parent, const char *name)
    : QObject(parent)
{

    type = BASE_RECORDER;
    num_id = id;
    filename = fname;
    n_channels = 0;
    init_crono = 0;
    crono = init_crono;
    crono_reset = false;
    really_append = true;
    setFileMode(f_mode);
    dialog = 0;
    recording = false;

}                                                                                  

FileRecorder::FileRecorder(int id, const QString &fname,
                           const QDomElement& e, int f_mode,
                           QObject *parent, const char *name)
    : QObject(parent)
{

    type = BASE_RECORDER;
    num_id = id;
    filename = fname;
    addChannels(e);
    init_crono = 0;
    crono = init_crono;
    crono_reset = false;
    really_append = true;
    setFileMode(f_mode);
    dialog = 0;
    recording = false;

}  

FileRecorder::FileRecorder(int id, const QString &fname,
                           QVector<int> channels, int f_mode,
                           QObject *parent, const char *name)
    : QObject(parent)
{

    type = BASE_RECORDER;
    num_id = id;
    filename = fname;
    n_channels = channels.count();
    channels_ids.fill(0, n_channels);
    for(int i = 0; i < n_channels; i++)
        channels_ids[i] = channels[i];
    init_crono = 0;
    crono = init_crono;
    crono_reset = false;
    really_append = true;
    setFileMode(f_mode);
    reset(true);
    dialog = 0;
    recording = false;

}

FileRecorder::~FileRecorder()
{
    closeFile();
}

void FileRecorder::reset(bool total)
{

    channels_vals.fill(0.0, n_channels);
    cnt_channels = 0;
    if(total)
        crono = 0;
    else if(crono_reset)
        crono = 0;

}

bool FileRecorder::init(QStringList& data)
{

    bool ret = false;
    bool f_open_ret = false;

    if(filename.isEmpty() || !recording) return ret;
    file_rec.setFileName(filename);
    if(really_append)
        f_open_ret = file_rec.open(QIODevice::WriteOnly | QIODevice::Append);
    else
        f_open_ret = file_rec.open(QIODevice::WriteOnly);
    if(f_open_ret) {
        ts.setDevice(&file_rec);
        if((really_append) && (file_rec.size() > 0))
            ts << "\n\n\n";
        ts << "# " << QDateTime::currentDateTime().
              toString("dd-MM-yyyy hh:mm:ss.zzz") << "\n";
        initRecordText(ts, data);
        initChannelsText(ts);

        ret = true;
    }
    return ret;

}

bool FileRecorder::init(void)
{

    bool ret = false;
    bool f_open_ret = false;

    if(filename.isEmpty() || !recording) return ret;
    file_rec.setFileName(filename);
    if(really_append)
        f_open_ret = file_rec.open(QIODevice::WriteOnly | QIODevice::Append);
    else
        f_open_ret = file_rec.open(QIODevice::WriteOnly);
    if(f_open_ret) {
        ts.setDevice(&file_rec);
        if((really_append) && (file_rec.size() > 0))
            ts << "\n\n\n";
        ts << "# " << QDateTime::currentDateTime().
              toString("dd-MM-yyyy hh:mm:ss.zzz") << "\n";
        ts << header;
        initChannelsText(ts);
        ret = true;
    }
    return ret;

}

void FileRecorder::initChannelsText(QTextStream& ts)
{
    int i;
    for(i = 0; i < n_channels - 1; i++) {
        if(channels_ids[i] > 0)
            ts << channels_ids[i] << ", ";
    }
    if(i > 0) {
        if(channels_ids[i] > 0)
            ts << channels_ids[i] << "\n";
    }
    ts << "\n";
}

void FileRecorder::writeValue(double val, int ch_id)
{

    if(!recording) return;
    int r;
    if((r = channels_ids.indexOf(ch_id)) == -1) return;
    channels_vals[r] = val;
    cnt_channels++;
    if(cnt_channels == n_channels) {
        ts << crono/1000.0 << "\t" ;
        for(r = 0; r < n_channels - 1; r++)
            ts << channels_vals[r] << "\t";
        ts << channels_vals[r] << "\n";
        cnt_channels = 0;
    }

}

void FileRecorder::initCrono(int cr)
{

    init_crono = cr;
    crono = init_crono;

}

void FileRecorder::setCronoReset(bool cr_reset)
{

    crono_reset = cr_reset;
    if(file_mode == AUTO)
        really_append = !cr_reset;

}

void FileRecorder::delChannel(int ch_id)
{

    int channel, i;
    if(!n_channels)
        return;
    if((channel = channels_ids.indexOf(ch_id)) == -1) return;
    for(i = channel; i < n_channels - 1; i++)
        channels_ids[i] = channels_ids[i + 1];
    --n_channels;
    channels_ids.resize(n_channels);
    reset();

}

void FileRecorder::figFileHeader(QTextStream &ts, QStringList &data)
{
    ts << "#world_scale " << data[0] << "\n";
    ts << "#width " << data[1] << "\n";
    ts << "#height " << data[2] << "\n";
    ts << "#robot_scale " << data[3] << "\n";

    ts << "#world_xmin " << data[4] << "\n";
    ts << "#world_ymin " << data[5] << "\n";
    ts << "#inverseY " << data[6] << "\n";
    ts << "#toFigUnits " << data[7] << "\n";
}

void FileRecorder::addChannel(int ch_id)
{

    channels_ids.resize(n_channels + 1);
    channels_ids[n_channels] = ch_id;
    ++n_channels;
    reset();

}

void FileRecorder::addChannels(const QDomElement& e)
{

    bool ok;
    QString xml_infoStr;
    QTextStream stream1(&xml_infoStr);
    e.save(stream1, 4 /*indent*/);
    QDomNode n = e.firstChild();
    while( !n.isNull() ) {
        QDomElement e_ch = n.toElement();
        if(e_ch.tagName() == "channel_id")
            addChannel(e_ch.text().toInt(&ok));
        else {
            n = n.nextSibling();
            continue;
        }
        n = n.nextSibling();
    }

}

QVector<int> FileRecorder::idChannels(void)
{
    return channels_ids;
}

void FileRecorder::setFileName(const QString &fname)
{

    filename = fname;
    file_rec.setFileName(filename);

}

void FileRecorder::setFileMode(int mode)
{

    file_mode = mode;
    if(file_mode == AUTO)
        really_append = !crono_reset;
    else if(file_mode == APPEND)
        really_append = true;
    else if(file_mode == OVERWRITE)
        really_append = false;
}  

void FileRecorder::setNumId(int n_id)
{

    num_id = n_id;

}

void FileRecorder::closeFile(void)
{
    if(file_rec.isOpen()) {
        file_rec.flush();
        file_rec.close();
    }
}  

void FileRecorder::write(QDomDocument& doc, QDomElement& e)
{

    XMLOperator* xml_operator = XMLOperator::instance();
    QDomElement tag_type = doc.createElement("type");

    e.appendChild(xml_operator->createTextElement(doc,
                                                  "type", QString::number(type)));

    QDomElement tag_cons = doc.createElement( "constructor" );
    e.appendChild(tag_cons);
    tag_cons.setAttribute("num_id", QString::number(num_id));
    tag_cons.setAttribute("num_channels", QString::number(n_channels));
    tag_cons.setAttribute("filename", filename);
    tag_cons.setAttribute("file_mode", QString::number(file_mode));
    QDomElement tag_chann = doc.createElement( "channels" );
    e.appendChild(tag_chann);
    int num_neg = 0;
    std::sort(channels_ids.begin(), channels_ids.end());  // ordenar los negativos por valor absoluto

    int num_channels = channels_ids.count();
    QVector<int> chann_ids(num_channels);
//    for(uint i = 0; i < num_channels; i++) {
//        if(channels_ids[num_channels - 1 -i] < 0) {
//            chann_ids[i] = channels_ids[num_channels - 1 -i];
//            ++ num_neg;
//        }
//        else if(channels_ids[num_channels - 1 -i] > 0)
//            chann_ids[i] = channels_ids[i - num_neg];
//    }
    for(int i = 0; i < num_channels; i++) {
        if(channels_ids[num_channels - 1 -i] < 0) {
            chann_ids[num_neg] = channels_ids[num_channels - 1 - i];
            ++ num_neg;
        }
        else if(channels_ids[num_channels - 1 -i] > 0)
            chann_ids[num_channels - 1 - i] = channels_ids[num_channels - 1 - i];
    }
    
    for(int i = 0; i < num_channels; i++)
        channels_ids[i] = chann_ids[i];

    for(int i = 0; i < num_channels; ++i) {
        tag_chann.appendChild(xml_operator->createTextElement(doc,
                                     "channel_id", QString::number(channels_ids[i])));
    }

}

QewExtensibleDialog* FileRecorder::createDialog(QewExtensibleDialog* parent)
{
     RecordForm* dialog = 0;
    if(!parent) {
        QewDialogFactory *factory = new QewDialogFactory();
        QewExtensibleDialog* parent = factory->createDialog(QewExtensibleDialog::QewSimpleWidget);
        if(!parent->setUpDialog())
            return dialog;
    }
    dialog = new RecordForm(parent);
    if(dialog->setUpDialog()) {
        QString dir = QFileDialog::getExistingDirectory(Q_NULLPTR,
                        tr("Recording directory"), QDir::homePath(),
                        QFileDialog::ShowDirsOnly);
        dialog->setIniDir(dir);
        dialog->init(this);
    }
    else {
        delete dialog;
        dialog = 0;
    }
    return dialog;
}

void FileRecorder::setRecording(bool recording)
{
    this->recording = recording;
}

void FileRecorder::connectToCrono(bool on)
{
    if(recording) {
        ClocksDispatcher* advance_timer = ClocksDispatcher::instance();
        if(on) {
            connect(advance_timer, SIGNAL(Crono(int)), SLOT(Crono(int)));
            connect(advance_timer, SIGNAL(initCrono(int)), SLOT(initCrono(int)));
            connect(advance_timer, SIGNAL(CronoReset(bool)), SLOT(setCronoReset(bool)));
        }
        else
            disconnect(advance_timer, 0, this, 0);
    }
}
