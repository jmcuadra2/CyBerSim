//
// C++ Interface: filerecorder
//
// Description: 
//
//
// Author: Jose Manuel Cuadra Troncoso <jmcuadra@dia.uned.es>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef FILERECORDER_H
#define FILERECORDER_H

#include <QObject>
#include <QFile>
#include <QDomDocument>
#include <QTextStream>

#include "gui/recordform.h"

/**
  @author Jose Manuel Cuadra Troncoso <jmcuadra@dia.uned.es>
*/
class FileRecorder : public QObject
{
    Q_OBJECT

public:
    FileRecorder(const QDomElement& e, QObject *parent = 0, const char *name = 0);
    FileRecorder(int id, QObject *parent = 0, const char *name = 0);
    FileRecorder(int id, const QString &fname, const QDomElement& e, int f_mode = AUTO,
                 QObject *parent = 0, const char *name = 0);
    FileRecorder(int id, const QString &fname, QVector<int> channels, int f_mode = AUTO,
                 QObject *parent = 0, const char *name = 0);
    FileRecorder(int id, const QString &fname, int f_mode = AUTO,
                 QObject *parent = 0, const char *name = 0);
    ~FileRecorder();
    void addChannel(int ch_id);
    void reset(bool total = false);
    bool init(QStringList& data);
    bool init(void);
    QVector<int> idChannels(void);
    void setFileName(const QString &fname);
    const QString& getFileName(void) {return filename ; }
    void setFileMode(int mode);
    int getFileMode(void) { return file_mode ; }
    virtual void write(QDomDocument& doc, QDomElement& e);
    void addChannels(const QDomElement& e);
    void closeFile(void);
    int getNChannels(void) { return n_channels ; }
    int getNumId(void) { return num_id ; }
    void setNumId(int n_id);
    virtual void setHeader(QString const& head) { header = head ; }
    QString getHeader(void) const { return header ; }

    virtual QewExtensibleDialog* createDialog(QewExtensibleDialog* parent);
    bool getRecording() const { return recording ; }
    void setRecording(bool recording);
    void connectToCrono(bool on);

public slots:
    virtual void writeValue(double val ,int ch_id);
    void Crono(int cr);
    void initCrono(int cr);
    void setCronoReset(bool cr_reset);
    void delChannel(int ch_id);

protected:
    virtual void initRecordText(QTextStream& ts, QStringList& data) = 0;
    virtual void initChannelsText(QTextStream& ts);
    void figFileHeader(QTextStream& ts, QStringList& data);
    
public:
    enum DevFileModes {AUTO, APPEND, OVERWRITE};
    enum RecorderTypes{BASE_RECORDER = 0, NEURON_RECORDER = 1, ROBOT_RECORDER = 2,
                       SENSORS_RECORDER = 3, ACCONTROL_RECORDER = 4};

protected:
    int num_id;
    int type;
    QString filename;
    QVector<int> channels_ids;
    QVector<double> channels_vals;
    int n_channels, cnt_channels, crono, init_crono;
    QTextStream ts;
    QFile file_rec;
    int file_mode;
    bool really_append;
    bool crono_reset;
    QString header;

    RecordForm* dialog;
    bool recording;

};

inline void FileRecorder::Crono(int cr)
{
    crono = cr;
}

#endif
