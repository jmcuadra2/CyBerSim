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
#ifndef ABSTRACTFUNCTION_H
#define ABSTRACTFUNCTION_H

#include "ndmath.h"

/**
@author jose manuel
*/

class AbstractFunction{

  friend class NDMath;
  protected:
    AbstractFunction();  
  public:

    virtual ~AbstractFunction();

    virtual double value(double val, double val2=0) = 0;
    
    public:
      enum FunctionTypes {FUNC_IDENTITY, FUNC_SIGMOIDE, FUNC_PERCEPTRON};

};

class IdentityFunction : public AbstractFunction{

  friend class NDMath;
  protected:
    IdentityFunction();  
  public:
    ~IdentityFunction();

    double value(double val, double /*val2=0*/) {return val > 1.0 ? 1.0 : val ; };

};

class SigmoidalFunction : public AbstractFunction{

  friend class NDMath;
  protected:
    SigmoidalFunction();
  public:
    ~SigmoidalFunction();

    double value(double val, double val2=0);

};

class PerceptronFunction : public AbstractFunction{

  friend class NDMath;
  protected:
    PerceptronFunction();  
  public:
    ~PerceptronFunction();

    double value(double, double ) {return 1.0 ; };

};

// nuevas funciones de entrada y de salida convertidas en objetos

class InputFunction : public AbstractFunction{

	friend class NDMath;
	protected:
		InputFunction();  
	public:
		~InputFunction();
	
		double value(double val1, double val2);

};

class OutputFunction : public AbstractFunction{

	friend class NDMath;
	protected:
		OutputFunction();  
	public:
		~OutputFunction();
		// double value (double val) { return val; };
		double value(double val1, double val2);

};
#endif
