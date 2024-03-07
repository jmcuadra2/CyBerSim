/***************************************************************************
                          isoinputsynapse.h  -  description
                             -------------------
    begin                : Sun Jun 19 2005
    copyright            : (C) 2005 by Jose M. Cuadra Troncoso
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

#ifndef ISOINPUTSYNAPSE_H
#define ISOINPUTSYNAPSE_H

#include "synapse.h"

class ISODevice;
class Neuron;

/**
  *@author Jose M. Cuadra Troncoso
  */
  
class ISOInputSynapse : public Synapse  {

  Q_OBJECT
  public:
    ISOInputSynapse(unsigned long n_i, bool recmon = false,
                        QObject *parent = 0, const char *name = 0);
	  ISOInputSynapse(unsigned long n_i, bool isCondStim, uchar n_color,
                            double w, ulong To_, bool recmon = false,
                            QObject *parent = 0, const char *name = 0);
    ~ISOInputSynapse();

    bool setParams(const QDomElement& e);
    bool setPointerTo(Neuron* neuron_to);
    bool getIsCS(void);
    void setIsCS(bool isCondStim);
    void write_constructor(QDomElement& e);
    int type(void) const;
    const QString connName(bool isCap);
    bool validStim(bool isCondStim);
//     bool edit(bool mode = true, bool added = false);

  protected:
    void defaultValues(bool to_base = true); 
    QewExtensibleDialog* createDialog(bool mode, bool added);   

  protected:
    ISODevice *iso_to;
    bool isCS;
    bool firstSetPointerTo;
};

inline bool ISOInputSynapse::getIsCS(void)
{

  return isCS;
};

#endif
