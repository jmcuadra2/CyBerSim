/***************************************************************************
                          neuralconnection.h  -  description
                             -------------------
    begin                : Thu Dec 2 2004
    copyright            : (C) 2004 by Jose M. Cuadra Troncoso
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

#ifndef NEURALCONNECTION_H
#define NEURALCONNECTION_H

#include <QDomDocument>

#include "../recmon/recmonconnection.h"

/**
  *@author Jose M. Cuadra Troncoso
  */

class NeuralConnection : public RecMonConnection  {

	Q_OBJECT
	
  public:
  	NeuralConnection(unsigned long n_id, bool recmon = false,
                        QObject *parent = 0, const char *name = 0);
  	~NeuralConnection();
  	
    virtual void setWeight(double w) { weight = w ;}
    virtual double getWeight(void) const { return weight ;}

    virtual void setPosWeight(bool yes) { posWeight = yes ;}
    virtual bool getPosWeight(void) const { return posWeight; }
    virtual void setStabWeightMode(int w_stabmode)
    												{stabWeight = w_stabmode ;}
    virtual void setStabWeightVal(double w_stabval)
    												{stabWeightVal = w_stabval ;}
    virtual int getStabWeightMode(void)
    												{return stabWeight ;}
    virtual double getStabWeightVal(void)
    												{return stabWeightVal ;}
    virtual void stabilizeWeight(double ) {};
    const QString connName(bool isCap)
    					{ return isCap ? tr("Neural connection") :
    															tr("neural connection") ;}
    void write(QDomDocument& , QDomElement& ) {};
    virtual bool setParams(const QDomElement& ) = 0;
    void write_constructor(QDomElement& ) {};
    virtual int type(void) const = 0;
    virtual int superType(void) const = 0;
   		
  public slots:
    virtual void addWeight(double w) {weight += w ;}
    void emitToMonitor(void);
    void emitToRecorder(void);

  public:
    enum SuperTypes {SYNAPSE = 1000, RESONATOR = 2000};
    enum weightStabMode {WS_NONE, WS_SYMMETRIC, WS_ASYMMETRIC};
    
  protected:
    virtual void defaultValues(bool to_base = true);    
    virtual void acceptEdit(void) { };
    
  signals:
    void weightDif(double);
 		void changed(void);

	protected:
    double weight;
    bool posWeight;
		int stabWeight;
		double stabWeightVal;	    		
		  	
};

inline void NeuralConnection::emitToMonitor(void)
{

  emit emitMonVal(weight, num_id);

};

inline void NeuralConnection::emitToRecorder(void)
{

  emit emitRecVal(weight, num_id);

};

#endif
