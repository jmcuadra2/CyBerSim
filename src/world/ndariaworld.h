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
#ifndef NDARIAWORLD_H
#define NDARIAWORLD_H

#include <QTransform>

#include "abstractworld.h"

/**
@author jose manuel
*/
class NDAriaWorld : public AbstractWorld
{

    Q_OBJECT

    friend class WorldFactory;

protected:
    NDAriaWorld(QMdiArea* w_work = 0, QObject* parent = 0, const char* name = 0);

public:
    ~NDAriaWorld();
    
    double xTransform(const double& x) { return x/world_scale; }
    double yTransform(const double& y) { return y_max_position + y_min_position - y/world_scale ; }
    
    double headingTransform(const double& head)  { return -head ; }
    double inverseXTransform(const double& x) { return x *world_scale ; }
    double inverseYTransform(const double& y)
    {
        return (-y + y_max_position + y_min_position)*world_scale;
    }
    double inverseHeadingTransform(const double& head) { return -head ; }
    
    double xRobotTransform(const double& x) {
        return x /world_scale ;
    } // del mundo real al canvas

    double yRobotTransform(const double& y) {
        return -y/world_scale ;
    }

    double headingRobotTransform(const double& head) {
        return -head ;
    }

    double inverseXRobotTransform(const double& x) {
        return x *world_scale ;
    } // del canvas al mundo real

    double inverseYRobotTransform(const double& y) {
        return -y*world_scale ;
    }

    double inverseHeadingRobotTransform(const double& head) {
        return -head ;
    }

    void exportToFig(QTextStream& ts, bool changeCoordinates);
    int toFigUnits(void) const { return 45 ; }

protected:
    bool load(QTextStream& ts);
    bool readFirstLines(QString& extra_line, QTextStream& ts);
    bool readCairns(QString& extra_line, QTextStream& ts);
    bool readLinesData(QString& extra_line, QTextStream& ts);
    bool readPointsData(QString& extra_line, QTextStream& ts);

    void drawHeadingLine(const int& x, const int& y, const double& ang,
                         const bool& black = false);

};

#endif
