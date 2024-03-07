//
// C++ Interface: blobcamerafusionnode
//
// Description: 
//
//
// Author: Jose Manuel Cuadra Troncoso <jmcuadra@dia.uned.es>, (C) 2009
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef BLOBCAMERAFUSIONNODE_H
#define BLOBCAMERAFUSIONNODE_H

#include "inputfusionnode.h"

/**
	@author Jose Manuel Cuadra Troncoso <jmcuadra@dia.uned.es>
*/
class BlobCameraFusionNode : public InputFusionNode
{
public:
    BlobCameraFusionNode(AbstractSensor* sensor);

    ~BlobCameraFusionNode();

    void initialize(void);

  protected:
    void proceessInformation(void);
    void writeInformation(void) {};
    void initResultMeasure(void);
    void initAuxiliarMeasure(void) {};
};

#endif
