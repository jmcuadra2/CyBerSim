//
// C++ Interface: material
//
// Description: 
//
//
// Author: Jose Manuel Cuadra Troncoso <jmcuadra@dia.uned.es>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef MATERIAL_H
#define MATERIAL_H

#include <QDomDocument>
#include "gui/editlinedialog.h"
/**
	@author Jose Manuel Cuadra Troncoso <jmcuadra@dia.uned.es>
*/

class Material{

  public:
    Material();
    Material(int theSensorType, double a_hit, double a_max, double w_rand, double transparency);

    ~Material();
    double randomize(double z_star, double incidence_angle, double lambda, double sigma, double w_short, int max_val);
    double randomBounce(double z_star, double incidence_angle, int max_val);
    void setParams(const QDomElement& e);
    void getParams(QDomElement& e);

    QewExtensibleDialog* createDialog();
    bool edit(void);
    
    void setSensorType(int newSensorType) { sensorType = newSensorType; }

    int getSensorType() const { return sensorType; }

    void setA_hit(double a_hit) { this->a_hit = a_hit; }

    double getA_hit() const { return a_hit; }

    void setA_max(double a_max) { this->a_max = a_max; }

    double getA_max() const { return a_max; }

    void setW_rand(double w_rand) { this->w_rand = w_rand; }

    double getW_rand() const { return w_rand; }

    void setTransparency(double transparency) { this->transparency = transparency; }
  
    double getTransparency() const { return transparency; }

    bool isTransparent(void) { return transparency >0.9 ; }

    static Material* factoryDefaultMaterial(int sensorType, bool real);
  

  protected:
    double a_hit, a_max;
    double w_rand;
    int sensorType;
    double transparency;
};

#endif
