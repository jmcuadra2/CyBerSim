/***************************************************************************
 *   Copyright (C) 2009 by Jose Manuel Cuadra Troncoso   *
 *   jmcuadra@dia.uned.es   *
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
#include "pmscan.h"

PMScan::PMScan() : m_r(PM_L_POINTS), m_x(PM_L_POINTS), m_y(PM_L_POINTS), m_bad(PM_L_POINTS), m_seg(PM_L_POINTS)
{
    m_rx = 0;   //robot odometry pos
    m_ry = 0;   //robot odometry pos
    m_th = 0;   //robot orientation
}


PMScan::~PMScan()
{
}

void PMScan::setSegmentsEnds(QVector<pair<int, int> > segmentsEnds)
{ 
  this->segmentsEnds = segmentsEnds;
  segmentsSizes.resize(segmentsEnds.size());
  for(int i = 0; i < segmentsEnds.size(); i++) {
    segmentsSizes[i] = segmentsEnds.at(i).second - segmentsEnds.at(i).first + 1;
  }
  
}