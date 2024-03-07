//
// C++ Interface: solidobject
//
// Description:
//
//
// Author: Jose Manuel Cuadra Troncoso <jmcuadra@dia.uned.es>, (C) 2009
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef SOLIDOBJECT_H
#define SOLIDOBJECT_H

#include "rtti.h"
#include <QPolygonF>
#include <QString>

class CollisionSolver;

/**
  @author Jose Manuel Cuadra Troncoso <jmcuadra@dia.uned.es>
*/
class SolidObject
{
public:
    SolidObject();

    virtual ~SolidObject();

    CollisionSolver* getCollisionSolver(void) const {
        return solver;
    }

    void editMaterial() {
    }

    virtual void createDialog(void);
    virtual bool editMaterials(void);

    virtual QString getTypeName(void) const = 0;
    virtual QPolygonF getPoints(void) = 0;
    virtual void setPoints(const QPolygonF& polygon) = 0;
    virtual int getRtti(void) const = 0;
    void setCollisionSolver(CollisionSolver* solv) {
        solver = solv;
    }

    void setIdName(const QString& idName) { this->idName = idName; }
    QString getIdName() const { return idName; }


protected:
    virtual bool edit(void) {
        return true;
    }

private:
    CollisionSolver* solver;
    QString idName;

};

#endif
