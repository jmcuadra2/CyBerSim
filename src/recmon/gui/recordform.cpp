//
// C++ Implementation: recordform
//
// Description: 
//
//
// Author: Jose Manuel Cuadra Troncoso <jmcuadra@dia.uned.es>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//

#include <QMessageBox>
#include <QFileDialog>
#include <QDir>

#include "recordform.h"
#include "../filerecorder.h"

RecordForm::RecordForm(QWidget* parent, const char* name) :
                            QewExtensibleDialog(parent, name)
{
    setupUi(this);

    id = "Proceso";

    connect(recordCheckBox, SIGNAL(toggled(bool)), this, SLOT(setFileRec(bool)));
    connect(fileNameButton, SIGNAL(clicked()), this, SLOT(chooseFile()));
    connect(fileLineEdit, SIGNAL(textChanged(QString)), this, SLOT(setFile(QString)));
}


RecordForm::~RecordForm()
{
}

void RecordForm::init(FileRecorder* recorder)
{
    this->recorder = recorder;
    fileRec = recorder->getRecording();
    recordCheckBox->setChecked(fileRec);
    setFile(recorder->getFileName());

}

bool RecordForm::getFileRec( void )
{
    return fileRec;
}

void RecordForm::setFileRec(bool fr )
{
    if(fileRec != fr) {
        fileRec = fr;
        recordCheckBox->setChecked(fileRec);

        if(fileRec && fileLineEdit->text().trimmed().isEmpty())
            chooseFile();

        saveThis(); // provisional
    }
}


QString RecordForm::getFile( void)
{
    return fileName;
}

void  RecordForm::setFile(QString const& f)
{
    if(fileName == f) return;

    if(f.trimmed().isEmpty()) {
        if(fileName.trimmed().isEmpty()) {
            fileName = ini_dir + tr("recording");
            if(fileRec) {
//                QMessageBox::information(nullptr, tr("Edit recording..."),
//                                tr("File name for recording is empty.") + "\n" + tr("Restoring previous or default name."), tr("&Return"));
                QMessageBox msgBox;
                msgBox.setIcon(QMessageBox::Information);
                msgBox.setText(tr("Edit recording..."));
                msgBox.setInformativeText(tr("File name for recording is empty.") + "\n" + tr("Restoring previous or default name."));
                QPushButton *button = msgBox.addButton(tr("Return"), QMessageBox::ActionRole);
                msgBox.exec();
            }
        }
    }
    else fileName = f;
    fileLineEdit->setText(fileName);

    saveThis(); // provisional
}

void RecordForm::setIniDir(QString const& d)
{
    ini_dir = d;
}

void RecordForm::chooseFile(void )
{
    QString file;
    QFileDialog* fd;
    fd = new QFileDialog();
    fd->setWindowTitle(id + " " + tr("recording file"));
    fd -> setDirectory(fileName);
    QStringList filters;
    filters << tr("Data") + " (*.dat)" << tr("Any files") + " (*)";
    fd -> setNameFilters(filters);
    fd -> setFileMode(QFileDialog::AnyFile);
    if ( fd->exec() == QDialog::Accepted ) {
        QStringList fileNames = fd->selectedFiles();
        if(fileNames.size() == 0) {
            delete fd;
            return;
        }
        file = fileNames.at(0);
        if(file.section(QDir::toNativeSeparators("/"), -1).indexOf(".") == -1)
            file += ".dat";
        setFile(file);
    }
    else {
        if(file.trimmed().isEmpty())
            setFile(file);
    }
    delete fd;
}

void RecordForm::saveThis(void)
{
    recorder->setRecording(fileRec);
    recorder->setFileName(fileName);
}
