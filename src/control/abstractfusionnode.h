//
// C++ Interface: abstractfusionnode
//
// Description:
//
//
// Author: Jose Manuel Cuadra Troncoso <jmcuadra@dia.uned.es>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef ABSTRACTFUSIONNODE_H
#define ABSTRACTFUSIONNODE_H



#include "../neuraldis/measure.hpp"

using namespace std;

/**
  @author Jose Manuel Cuadra Troncoso <jmcuadra@dia.uned.es>
*/
class AbstractFusionNode
{
  public:
    AbstractFusionNode();

    virtual ~AbstractFusionNode();

    virtual void initialize(void);

    vector< Measure <> > getInputMeasures() const { return inputMeasures ; }
    Measure<> result() const { return resultMeasure; }
    void setAuxiliarMeasure(const Measure<>& auxiliarMeasure) { this->auxiliarMeasure = auxiliarMeasure ; }

    void doFusion(void);

  protected:
    virtual void readInformation(void);
    virtual void proceessInformation(void) = 0;
    virtual void writeInformation(void) = 0;
    virtual void initResultMeasure(void) = 0;
    virtual void initAuxiliarMeasure(void) = 0;

  protected:
    vector< AbstractFusionNode* > inputNodesList;
    vector<Measure <> > inputMeasures;
    Measure<> resultMeasure;
    Measure<> auxiliarMeasure; // feedback
//     ??? aprioriKnowledge;

};

#endif
