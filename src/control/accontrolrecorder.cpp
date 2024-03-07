//
// C++ Implementation: accontrolrecorder
//
// Description: 
//
//
// Author: Jose Manuel Cuadra Troncoso <jmcuadra@dia.uned.es>, (C) 2009
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "accontrolrecorder.h"

ACControlRecorder::ACControlRecorder(int id, const QString& fname, int f_mode,
                                     QObject* parent, const char* name) :
    FileRecorder(id, fname, f_mode, parent, name)
{
    type = ACCONTROL_RECORDER;
}


ACControlRecorder::ACControlRecorder(int id, const QString& fname, QVector< int > channels,
                                     int f_mode, QObject* parent, const char* name) :
    FileRecorder(id, fname, channels, f_mode, parent, name)
{
    type = ACCONTROL_RECORDER;
}


ACControlRecorder::ACControlRecorder(int id, const QString& fname, const QDomElement& e,
                                     int f_mode, QObject* parent, const char* name) :
    FileRecorder(id, fname, e, f_mode, parent, name)
{
    type = ACCONTROL_RECORDER;
}


ACControlRecorder::ACControlRecorder(int id, QObject* parent, const char* name) :
    FileRecorder(id, parent, name)
{
    type = ACCONTROL_RECORDER;
}


ACControlRecorder::ACControlRecorder(const QDomElement& e, QObject* parent, const char* name) :
    FileRecorder(e, parent, name)
{
    type = ACCONTROL_RECORDER;
}

ACControlRecorder::ACControlRecorder(const QString &fname) : FileRecorder(1, fname, APPEND)
{

    type = ACCONTROL_RECORDER;
    for(int i = 0; i < 7; i++)   // 7 provisional
        addChannel(i + 1);
    reset();
    recording = true;  //  provisional
    
}


ACControlRecorder::~ACControlRecorder()
{
}

void ACControlRecorder::initRecordText(QTextStream& ts, QStringList& data)
{
    figFileHeader(ts, data);

    ts << "#AC\n";
    ts << "# " + tr("T(s)") << "\t" << tr("X(cm)") << "\t" << tr("Y(cm)")
       << "\t" << tr("Start_advance_angle") << "\t" << tr("End_advance_angle")
       << "\t" << tr("Advance_status") << "\t"<< tr("Pan")
       << "\t" << tr("Blob Area") << "\t" << "\n" << "#\t0, ";// 0 para la columa T(s)
}

