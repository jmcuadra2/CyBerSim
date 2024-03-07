/***************************************************************************
                          main.cpp  -  description
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

#include <QApplication>
#include <QFont>
#include <QString>
#include <QTranslator>
#include <QStringList>

#include "settings.h"
#include "gui/neuraldisapp.h"

int main(int argc, char *argv[])
{
  QApplication app(argc, argv);
  QTranslator translator;
  // set the location where your .qm files are in load() below as the last parameter instead of "."
  // for development, use "/" to use the english original as
  // .qm files are stored in the base project directory.

//  if (translator.load(QLocale(), QLatin1String("CyberSim"), QLatin1String("_"),
//                                    QLatin1String(":/translations")))
  if (translator.load(QLocale(), QLatin1String("cybersim"), QLatin1String("."), QLatin1String(":/translations"), ".qm"))
      app.installTranslator(&translator);

  Settings *prog_settings = Settings::instance();

  if(prog_settings->ok_settings()) {
    NeuralDisApp *neuraldis = new NeuralDisApp();
  	neuraldis->init(prog_settings);
    neuraldis->setGeometry(50, 50, 900, 550);
    neuraldis->showMaximized();

    return app.exec();
  }
  else {
    return 1;
  }
}


