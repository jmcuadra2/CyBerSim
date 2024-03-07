//
// C++ Implementation: solidobject
//
// Description: 
//
//
// Author: Jose Manuel Cuadra Troncoso <jmcuadra@dia.uned.es>, (C) 2009
//
// Copyright: See COPYING file that comes with this distribution
//
//

#include "solidobject.h"
#include "collisionsolver.h"
#include "gui/defaultmaterialsdialog.h"

#include <qewdialogfactory.h>

SolidObject::SolidObject()
{
    solver = 0;
}


SolidObject::~SolidObject()
{
    if(solver)
        delete solver;
}

void SolidObject::createDialog(void) {

    QewDialogFactory *factory = new QewDialogFactory();
    QewExtensibleDialog* main_dialog = factory->createDialog(QewExtensibleDialog::QewTabWidget);
    if (main_dialog->setUpDialog()) {
        // Recorremos cada uno de los materiales de los que está compuesto el objeto
        CollisionSolver* cs = getCollisionSolver();
        QMapIterator<int, Material*> iteratorMaterials(cs->getMaterials());
        while (iteratorMaterials.hasNext()) {
            iteratorMaterials.next();
            Material* m = iteratorMaterials.value();
            main_dialog->addExtensibleChild(m->createDialog(), "Material");
        }
        main_dialog->setWindowTitle("Atributos de los materiales");
        main_dialog->exec();
    }
    else {
        delete main_dialog;
        main_dialog = 0;
    }
}

bool SolidObject::editMaterials(void)
{
    bool ret = false;
    DefaultMaterialsDialog* dialog = new DefaultMaterialsDialog();
    QList<Material*> mats;
    CollisionSolver* cs = getCollisionSolver();
    QMapIterator<int, Material*> iteratorMaterials(cs->getMaterials());
    while (iteratorMaterials.hasNext()) {
        iteratorMaterials.next();
        Material* m = iteratorMaterials.value();
        mats.append(m);
    }
    dialog->setMaterials(mats);
    dialog->setID(idName);
    if(dialog->exec() == QDialog::Accepted) {
        ret = true;
        idName = dialog->getIdName();
        cs->removeMaterials(true);
        mats = dialog->getMaterials();
        for(int i = 0; i < mats.count(); i++)
            cs->addMaterial(mats[i]);
    }
    delete dialog;
    return ret;
}
