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
 
#include <Aria.h> 
 
#include "aractiongroupreactive.h"

ArActionGroupReactive::ArActionGroupReactive(ArRobot *robot) : ArActionGroup(robot) 
{

  addAction(new ArActionBumpers, 100);


// // Solo para wander Aria, quitar cuanto antes
//   int avoidFrontDist = 600; // valor por defecto
//   int avoidVel  = 200; // valor por defecto
//   int avoidTurnAmt = 15; // valor por defecto
//   
//   int forwardVel = 600;  
//   addAction(new ArActionBumpers, 100);
//   addAction(new ArActionStallRecover, 90);
// 
//   addAction(new ArActionAvoidFront("Avoid Front", avoidFrontDist, avoidVel, avoidTurnAmt), 80);
//   addAction(new ArActionConstantVelocity("Constant Velocity", forwardVel), 50);
//   addAction(new ArActionAvoidSide(), 70);

}


ArActionGroupReactive::~ArActionGroupReactive()
{
  std::list<ArAction *>::iterator it;
  removeActions();
  for (it = myActions.begin(); it != myActions.end(); ++it)
    delete (*it);
}
