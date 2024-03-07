/***************************************************************************
 *   Copyright (C) 2004 by jose manuel                                     *
 *   jose@pcjose                                                           *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#include <QFile>
#include <QDir>
#include <QMessageBox>

#include "../neuraldis/settings.h"
#include "world.h"
#include "worldfactory.h"
#include "ndariaworld.h"
#include "worldview.h"

WorldFactory::WorldFactory()
{
}

WorldFactory::~WorldFactory()
{
}

AbstractWorld* WorldFactory::createWorld(const QString &worldname)
{

    AbstractWorld* world = nullptr;
    QFile file( worldname );
    QString line;
    Settings* settings = Settings::instance();
    if ( !file.open( QIODevice::ReadOnly ) ) {
//        QMessageBox::critical(nullptr, Settings::tr("Load world"),
//                              Settings::tr("World file doesn't exist"), Settings::tr("&Cancel"));
        QMessageBox::critical(nullptr, Settings::tr("Load world"),
                              Settings::tr("World file doesn't exist"), QMessageBox::Cancel);
        return world;
    }

    QTextStream ts( &file );
    line = ts.readLine();
    while(line.contains("#") && !ts.atEnd()) {
        if(line.contains("#FIG 3.2")) {
            World* fig_world = new World(settings->getWorkspace());
            if(fig_world->load(ts))
                world = fig_world;
            else
                delete fig_world;
            break;
        }
        else
            line = ts.readLine();
    }
    if(line.contains("2D-Map")) {
        NDAriaWorld* nda_world = new NDAriaWorld(settings->getWorkspace());
        if(nda_world->load(ts))
            world = nda_world;
        else
            delete nda_world;
    }
    else if(line.contains("!DOCTYPE CyberSim_world")) {
        NDAriaWorld* nda_world = new NDAriaWorld(settings->getWorkspace());
        file.close();
        if(nda_world->readXML(worldname))
            world = nda_world;
        else
            delete nda_world;
    }
    if(world) {
        world->setFileName(worldname);
        settings->setWorld(world);
        world->view()->setWindowTitle(world->view()->windowTitle().section(":", 0, 0) + ": " + worldname.section(QDir::toNativeSeparators("/"), -1));
    }
    else
//        QMessageBox::critical(nullptr, Settings::tr("Load world"), Settings::tr("Unknown format or corrupted world file"), Settings::tr("&Cancel"));
                QMessageBox::critical(nullptr, Settings::tr("Load world"), Settings::tr("Unknown format or corrupted world file"), QMessageBox::Cancel);
    return world;
}


