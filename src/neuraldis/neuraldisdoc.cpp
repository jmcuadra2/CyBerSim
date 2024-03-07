/***************************************************************************
                          neuraldisdoc.cpp  -  description
                             -------------------
    begin                : mar oct 15 04:16:08 CEST 2002
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
#include <QDomDocument>
#include <QDir>
#include <QFileDialog>
#include <QMessageBox>
#include <QTextStream>

#include "neuraldisdoc.h"
#include "../neuraldis/settings.h"

NeuralDisDoc* NeuralDisDoc::docs_manager = 0;

NeuralDisDoc* NeuralDisDoc::instance(void)
{

    if(!docs_manager)
        docs_manager = new NeuralDisDoc();
    return docs_manager;

}  

NeuralDisDoc::NeuralDisDoc()
{
    modified = false;
    texto = "";
    fileName = tr("Untitled.net");
    settings = Settings::instance();

}

NeuralDisDoc::~NeuralDisDoc()
{

}

void NeuralDisDoc::newDoc(void)
{

    texto = "";
    fileName = tr("Untitled.net") ;
    netDoc.clear();
    //   QDomDocument doc("neuralnet");
    //   QDomElement e_net = doc.createElement("net");
    //   doc.appendChild(e_net);
    //   netDoc = doc;
    //   modified = true;
    emit documentName(fileName);
    //   emit documentOpen(netDoc.toString());
    //   emit documentChanged(netDoc.toString());

}

bool NeuralDisDoc::save(void)
{

    QFile file( fileName );
    if ( file.open( QIODevice::WriteOnly ) ) {
        QTextStream ts( &file );
        ts << texto;
        modified = false;
        return true;
    } else  {
        return false;
    }
}

bool NeuralDisDoc::saveAs(const QString &filename_)
{
    QString old_filename = fileName;
    QFile file(settings->fillPath(filename_, settings->getNetDirectory()));
    if (!file.exists())  {
        fileName = filename_ ;
        if (save())  {
            emit documentName(fileName);
            return true;
        } else  {
            fileName = old_filename;
            return false;
        }
    } else  {
//        if(QMessageBox::information(nullptr,tr("save(Doc)"),
//                            tr("File already exists\n\tSave?"), tr("&Ok"), tr("&Cancel")) == 1)
        if(QMessageBox::information(nullptr,tr("save(Doc)"),
                                     tr("File already exists\n\tSave?"), QMessageBox::Ok | QMessageBox::Cancel) == QMessageBox::Cancel)
            return false;
        else {
            fileName = filename_;
            if (save()) {
                emit documentName(fileName);
                return true;
            } else  {
                fileName = old_filename;
                return false;
            }
        }
    }
}

bool NeuralDisDoc::load(QString &filename)
{

    QString type_file = "Nets";
    QString extension = "net";
    if(filename.isEmpty() || filename.right(1) == QDir::toNativeSeparators("/")) {
        filename = settings->fillPath(filename, settings->getNetDirectory());
        getFileDialog(filename, "NeuralDisDoc", type_file, extension);
    }
    else
        filename = settings->fillPath(filename, settings->getNetDirectory());
    QDomDocument doc = loadXML(filename, "neuralnet", tr("loadNet"), type_file, extension);
    if(!doc.isNull()) {
        netDoc.clear();
        netDoc = doc;
        fileName = filename;
        emit documentOpen(netDoc.toString());
        emit documentChanged(netDoc.toString());
        emit documentName(fileName);
        return true;
    }
    else
        return false;

}

bool NeuralDisDoc::loadSim(QString &file_sim_name)
{

    QString type_file = "Simulations";
    QString extension = "sim";
    QDomDocument doc = loadXML(file_sim_name, "neuraldis_simulation", tr("loadSim"), type_file, extension);
    if(!doc.isNull()) {
        simDoc.clear();
        simDoc = doc;
        return true;
    }
    else
        return false;

}


bool NeuralDisDoc::load_xml(QString& file_name,
                            QDomDocument& doc, const QString& doc_name, const QString& sender)
{

    bool to_continue = false;
    QString errorMsg;
    int errorLine = 0;
    int errorCol = 0;
    QFile file(settings->fillPath(file_name));
    if( file.open(QIODevice::ReadOnly)) {
        QTextStream ts( &file );
        QString xml_text = "";
        xml_text = ts.readAll();
        if(!doc.setContent(xml_text, &errorMsg, &errorLine, &errorCol)) {
//            to_continue = bool(QMessageBox::warning(nullptr, sender, errorMsg + "\n" +
//                                                tr("Line:") + " " + QString::number(errorLine) + " " + tr("Column:") + " " + QString::number(errorCol) + "\n" + tr("Try again ?"), tr("&No"), tr("&Yes")));
            to_continue = QMessageBox::warning(nullptr, sender, errorMsg + "\n" +
                                                                         tr("Line:") + " " + QString::number(errorLine) + " " + tr("Column:") + " " + QString::number(errorCol) + "\n" + tr("Try again ?"), QMessageBox::No | QMessageBox::Yes) == QMessageBox::Yes;
            doc.clear();
            return to_continue;
        }
        QDomDocumentType doc_type = doc.doctype();
        if(doc_type.isNull()) {
//            to_continue = bool(QMessageBox::warning(nullptr, sender,
//                                                tr("File is not in XML format") + "\n" + tr("Try again ?") , tr("&No"), tr("&Yes")));
            to_continue = QMessageBox::warning(nullptr, sender,
                                                    tr("File is not in XML format") + "\n" + tr("Try again ?"), QMessageBox::No | QMessageBox::Yes) == QMessageBox::Yes;
            doc.clear();
            return to_continue;
        }
        if(!doc_name.isEmpty()) {
            if(doc_type.name() != doc_name) {
//                to_continue = bool(QMessageBox::warning(nullptr, sender,
//                                                    tr("XML file format is not") + " " + doc_name + "\n" + tr("Try again ?") , tr("&No"), tr("&Yes")));
                to_continue = QMessageBox::warning(nullptr, sender,
                                                tr("XML file format is not") + " " + doc_name + "\n" + tr("Try again ?"), QMessageBox::No | QMessageBox::Yes) == QMessageBox::Yes;
                doc.clear();
                return to_continue;
            }
        }
        return to_continue;
    }
    else {
        to_continue = noFileDialog(sender);
        doc.clear();
        return to_continue;
    }

}

bool NeuralDisDoc::noFileDialog(const QString& sender)
{

//    bool ret = bool(QMessageBox::warning(nullptr, sender, "\t\t" + tr("Cannot open file.") + "   " + tr("Try again ?") + "\t\t" , tr("&Yes"), tr("&No")));
        bool ret = QMessageBox::warning(nullptr, sender, "\t\t" + tr("Cannot open file.") + "   " + tr("Try again ?") + "\t\t", QMessageBox::Yes | QMessageBox::No) == QMessageBox::No;
    return !ret;
}

bool NeuralDisDoc::getFileDialog(QString& file_name, const QString& sender, QString type_file, QString extension)
{
    bool ret = false;
    if(extension.isEmpty())
        extension = file_name.section(QDir::toNativeSeparators("/"), -1)
                .section(".", -1);
    if(type_file.isEmpty())
        type_file = extension;

    QFileDialog* fd = new QFileDialog();
    fd->setDirectory(settings->fillPath(file_name));
    fd->setWindowTitle(sender);
    QStringList filters;
    if(extension.isEmpty())
        filters << tr("Any files") << " (*)";
    else
        filters << type_file + " (*." + extension + ")" << tr("Any files") + " (*)";
    fd->setNameFilters(filters);
    fd -> setFileMode(QFileDialog::ExistingFile);
    if ( fd->exec() == QDialog::Accepted ) {
        QStringList fileNames = fd->selectedFiles();
        if(fileNames.size() > 0) {
            file_name = fileNames.at(0);
            ret = true;
        }
    }
    delete fd;
    return ret;
}


QDomDocument NeuralDisDoc::loadXML(QString& file_name,
                                   const QString& doc_name, const QString& sender, QString type_file, QString extension)
{

    bool to_continue = true;
    QDomDocument doc(doc_name);
    if(type_file.isEmpty())
        type_file = doc_name;
    if(!needChangeName(file_name)) {
        to_continue = load_xml(file_name, doc, doc_name, sender);
        if(!doc.isNull())
            to_continue = false;
    }
    while(to_continue) {
        to_continue = getFileDialog(file_name, sender, type_file, extension);
        if(to_continue) {
            to_continue = load_xml(file_name, doc, doc_name, sender);
            if(!doc.isNull())
                to_continue = false;
        }
        else {
            to_continue = false;
            doc.clear();
        }
    }

    return doc;
}

bool NeuralDisDoc::isModified(void) const
{
    return modified;
}

void NeuralDisDoc::setModified(bool mod)
{
    modified = mod;
}

QString NeuralDisDoc::getFileName(void)
{
    return fileName;
}

void NeuralDisDoc::textoChange(QString texto_)
{
    texto = texto_ ;
    modified = true;
    emit documentChanged(texto);
}

void NeuralDisDoc::initView(void)
{

    emit documentChanged(texto);

}

const QString NeuralDisDoc::saveDocument(QString& file_name, QString content, const QString& doc_type, const QString& extension, const QString& sender, bool change_name)
{

    //  QString file_name;
    bool ret = false;
    QFile file;
    QFileDialog* fd;
    QTextStream ts;
    if(change_name) {
        QString f_ext = file_name.section(QDir::toNativeSeparators("/"), -1).section('.', -1);
        if(f_ext.isEmpty()) //&& file_name.right(1) != QDir::toNativeSeparators("/"))
            file_name += "." + extension;
        fd = new QFileDialog();
        fd->setWindowTitle(sender);
//        fd -> setDirectory(file_name);
        fd->selectFile(file_name);
        fd->setAcceptMode(QFileDialog::AcceptSave);
        QStringList filters;
        filters << doc_type + " (*." + extension + ")" << tr("Any files") + " (*)";
        fd->setNameFilters(filters);
        fd->setFileMode(QFileDialog::AnyFile);
        if (fd->exec() == QDialog::Accepted) {
            QStringList fileNames = fd->selectedFiles();
            if(fileNames.size() == 0) {
                delete fd;
                return "";
            }
            file_name = fileNames.at(0);
            if(file_name.section(QDir::toNativeSeparators("/"), -1).indexOf(".") == -1)
                file_name += "." + extension;
            ret = true;
        }
        delete fd;
    }
    else
        ret = true;
    if(ret) {
        file.setFileName(file_name);
        if(file.open(QIODevice::WriteOnly))
            ts.setDevice(&file);
        else {
//            QMessageBox::critical(nullptr, tr("Save document"),
//                                  tr("Couldn't open file for saving"), tr("&Cancel"));
            QMessageBox::critical(nullptr, tr("Save document"),
                                  tr("Couldn't open file for saving"), QMessageBox::Cancel);
            ret = false;
        }
    }
    if(ret) {
        ts << content;
        file.close();
        modified = false;
        return file_name;
    }
    else
        return "";

}

void NeuralDisDoc::netClosed(void)
{

    if(modified)
        saveNet();
    netDoc.clear();
    texto = "";
    modified = false;

}

bool NeuralDisDoc::saveNet(void)
{

    bool  ret = false;
    QString  ret_name;
    QString extension = settings->getNetExtension().section('.', -1);
    //ret_name = saveDocument(fileName, netDoc.toString(4), "neuralnet", extension, tr("Saving neural net"), needChangeName(fileName));
    ret_name = saveDocument(fileName, texto, "neuralnet", extension, tr("Saving neural net"), needChangeName(fileName));
    if(!ret_name.isEmpty()) {
        modified = false;
        ret = true;
    }
    return ret;
    
}

bool NeuralDisDoc::testXML(void)
{

    QString file_xml_name;
    QDomDocument test_doc;
    QFileDialog* fd;
    bool ret = false;
    fd = new QFileDialog();
    fd -> setDirectory(QDir::current());
    QStringList filters;
    filters << tr("Any files") + " (*)";
    fd->setNameFilters(filters);
    fd -> setFileMode(QFileDialog::ExistingFile);

    if ( fd->exec() == QDialog::Accepted ) {
        QStringList fileNames = fd->selectedFiles();
        if(fileNames.size() == 0) {
            delete fd;
            return ret;
        }
        file_xml_name = fileNames.at(0);
        test_doc = loadXML(file_xml_name , "", "Test XML");
        ret = !test_doc.isNull();
        if(ret)
//            QMessageBox::information(nullptr,tr("Test XML"),
//                                     tr("XML file is well structured"), tr("&Ok"));
            QMessageBox::information(nullptr,tr("Test XML"),
                                     tr("XML file is well structured"), QMessageBox::Ok);
    }
    delete fd;
    return ret;

}

bool NeuralDisDoc::needChangeName(const QString& file_name)
{
    bool ret = file_name.trimmed().isEmpty() ||
                    file_name.contains("Untitled", Qt::CaseInsensitive)  ||
                    file_name.contains(tr("Untitled"), Qt::CaseInsensitive); // || isRelativePath(file_name);
    return ret;
}
