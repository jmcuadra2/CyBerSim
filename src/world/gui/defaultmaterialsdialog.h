//
// C++ Interface: defaultmaterialsdialog
//
// Description:
//
//
// Author: Jose Manuel Cuadra Troncoso <jmcuadra@dia.uned.es>, (C) 2009
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef DEFAULTMATERIALSDIALOG_H
#define DEFAULTMATERIALSDIALOG_H

#include <QDialog>

#include "ui_defaultmaterialsdialog.h"
#include "../material.h"

class Material;

/**
  @author Jose Manuel Cuadra Troncoso <jmcuadra@dia.uned.es>
*/
class DefaultMaterialsDialog : public QDialog, public Ui::DefaultMaterialsDialog
{
  Q_OBJECT
  
  public:
    DefaultMaterialsDialog();

    ~DefaultMaterialsDialog();

    void getXMLNode(void);
    void setMaterials(QList< Material * > materials);
    QList< Material * > getMaterials() const { return materials; }
    QString getIdName() const { return idName; }

  public slots:
    void setID(const QString& idName);    

  protected slots:
    void addMaterial(void);
    void removeMaterial(void);
    void editMaterial(void);

  protected:
    void writeMaterial(Material* material, int row);

  protected:
    QList<Material*> materials;
    QString idName;

};

#endif
