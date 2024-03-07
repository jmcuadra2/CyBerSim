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
#include "abstractfunction.h"

AbstractFunction::AbstractFunction()
{
}

AbstractFunction::~AbstractFunction()
{
}

IdentityFunction::IdentityFunction() : AbstractFunction()
{
}

IdentityFunction::~IdentityFunction()
{
}

SigmoidalFunction::SigmoidalFunction() : AbstractFunction()
{
}

SigmoidalFunction::~SigmoidalFunction()
{
}

double SigmoidalFunction::value(double val, double /*val2*/)
{
  return 1.0/(1.0 + exp(-5*val + 5));
}

PerceptronFunction::PerceptronFunction() : AbstractFunction()
{
}

PerceptronFunction::~PerceptronFunction()
{
}

InputFunction::InputFunction() : AbstractFunction()
{
}

InputFunction::~InputFunction()
{
}
double InputFunction::value(double val1, double val2)
{
	return val1+val2;
}

OutputFunction::OutputFunction() : AbstractFunction()
{
}

OutputFunction::~OutputFunction()
{
}

double OutputFunction::value(double val1, double val2)
{
	return val1+val2;
}

