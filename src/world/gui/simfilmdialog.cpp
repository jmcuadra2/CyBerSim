//
// C++ Implementation: simfilmdialog
//
// Description: 
//
//
// Author: Jose Manuel Cuadra Troncoso <jmcuadra@dia.uned.es>, (C) 2009
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "simfilmdialog.h"
#include "../../neuraldis/settings.h"
#include "../abstractworld.h"

#include <QFileDialog>

SimFilmDialog::SimFilmDialog(QWidget* parent, const char* name)
 : QewSimpleDialog(parent, name)
{
  setupUi(this);
  world = 0;

  connect(resolutionComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(setResolution(const int&)));
//   connect(resolutionComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(setResolution(const AbstractWorld::SequenceResolution&)));
  connect(folderLineEdit, SIGNAL(textChanged(const QString&)), this, SLOT(setSequenceName(const QString&)));
  connect(folderToolButton, SIGNAL(clicked()), this, SLOT(chooseFolderSequence()));
//   connect(formatComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(setFormat(const AbstractWorld::SequenceFormat&)));
  connect(formatComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(setFormat(const int&)));
  connect(embeddedRadioButton, SIGNAL(toggled(bool)), this, SLOT(setEmbedded(bool)));
  
}

SimFilmDialog::~SimFilmDialog()
{
}

void SimFilmDialog::init(AbstractWorld* world)
{
  this->world = world;
  setResolution(world->getSequenceResolution());
  setFormat(world->getSequenceFormat());
  setSequenceName(world->getSequenceName());
  setEmbedded(world->getEmbeddedFullView());
}

// void SimFilmDialog::setResolution(const AbstractWorld::SequenceResolution& resolution)
void SimFilmDialog::setResolution(const int& resolution)
{
  if(this->resolution != resolution) {
    this->resolution = resolution;
    resolutionComboBox->setCurrentIndex(resolution);
  }
}

void SimFilmDialog::setSequenceName(const QString& sequenceName)
{
  if(this->sequenceName != sequenceName) {
    this->sequenceName = sequenceName;
    folderLineEdit->setText(sequenceName);
  }
}

// void SimFilmDialog::setFormat(const AbstractWorld::SequenceFormat& format)
void SimFilmDialog::setFormat(const int& format)
{
  if(this->format != format) {
    this->format = format;
    formatComboBox->setCurrentIndex(format);
  }
}

void SimFilmDialog::chooseFolderSequence(void)
{
  QString dir;
  Settings *progSettings = Settings::instance();
  dir = QFileDialog::getExistingDirectory(this, QObject::tr("Choose a directory for video files"),progSettings->getWorldDirectory(), QFileDialog::ShowDirsOnly);
  if (!(dir == "")) {
    sequenceName = dir;
    folderLineEdit->setText(sequenceName);
  }

}

void SimFilmDialog::setEmbedded(bool embedded)
{
  if(embeddedFullView != embedded) {
    embeddedFullView = embedded;
    embeddedRadioButton->setChecked(embedded);
  }
}

void SimFilmDialog::saveThis(void)
{
  if(world) {
    world->setSequenceResolution((AbstractWorld::SequenceResolution)resolution);
    world->setSequenceFormat((AbstractWorld::SequenceFormat)format);
    world->setSequenceName(sequenceName);
    world->setEmbeddedFullView(embeddedFullView);
  }
}
