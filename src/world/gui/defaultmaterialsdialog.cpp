//
// C++ Implementation: defaultmaterialsdialog
//
// Description: 
//
//
// Author: Jose Manuel Cuadra Troncoso <jmcuadra@dia.uned.es>, (C) 2009
//
// Copyright: See COPYING file that comes with this distribution
//
//

#include "defaultmaterialsdialog.h"
#include "../../neuraldis/ndmath.h" 

DefaultMaterialsDialog::DefaultMaterialsDialog()
 : QDialog()
{
    setupUi(this);
    connect(addButton, SIGNAL(clicked()), this, SLOT(addMaterial()));
    connect(removeButton, SIGNAL(clicked()), this, SLOT(removeMaterial()));
    connect(editButton, SIGNAL(clicked()), this, SLOT(editMaterial()));
    connect(materialsTable, SIGNAL(cellDoubleClicked ( int, int)), this, SLOT(editMaterial()));
    connect(IDLineEdit, SIGNAL(textChanged (const QString &)), this, SLOT(setID(const QString &)));

    materialsTable->resizeColumnsToContents();
}


DefaultMaterialsDialog::~DefaultMaterialsDialog()
{

}

void DefaultMaterialsDialog::setID(const QString& idName) {
  if(this->idName != idName) {
    this->idName = idName;
    IDLineEdit->setText(idName);
  }
}

void DefaultMaterialsDialog::setMaterials(QList< Material * > materials)
{
//   this->materials = materials;
  for(int i = 0; i < materials.count(); i++) {
    Material* material = new Material(*materials.at(i));
    this->materials.append(material);
  }
  materialsTable->clearContents();
  materialsTable->setRowCount(0);
  for(int i = 0; i < materials.count(); i++)
    writeMaterial(materials.at(i), i);
}

void DefaultMaterialsDialog::addMaterial(void)
{
  Material* material = new Material();
  if(material->edit()) {
    int row = materialsTable->rowCount();
    writeMaterial(material, row);
    materials.append(material);
  }
  else
    delete material;

}

void DefaultMaterialsDialog::removeMaterial(void)
{
  int row = materialsTable->currentRow();
  if(row >= 0 & row < materials.size()) {  
    materialsTable->removeRow(row);
    delete materials.takeAt(row);
  }
}

void DefaultMaterialsDialog::editMaterial(void)
{
  int row = materialsTable->currentRow();
  if(row >= 0 & row < materials.size()) {
    if(materials.at(row)->edit())
      writeMaterial(materials.at(row), row);
  }
}

void DefaultMaterialsDialog::writeMaterial(Material* material, int row)
{
  bool newMat = (row == materialsTable->rowCount());
  QTableWidgetItem* item;
  if(newMat) {
    materialsTable->setRowCount(row + 1);
    
    item = new QTableWidgetItem(QString::number(material->getSensorType()));
    materialsTable->setItem(row, 0, item);

    item = new QTableWidgetItem(QString::number(material->getA_hit()*NDMath::RAD2GRAD));
    materialsTable->setItem(row, 1, item);

    item = new QTableWidgetItem(QString::number(material->getA_max()*NDMath::RAD2GRAD));
    materialsTable->setItem(row, 2, item);

    item = new QTableWidgetItem(QString::number(material->getTransparency()));
    materialsTable->setItem(row, 3, item);

    item = new QTableWidgetItem(QString::number(material->getW_rand()));
    materialsTable->setItem(row, 4, item);
  }
  else {
    item = materialsTable->item(row, 0);
    item->setText(QString::number(material->getSensorType()));

    item = materialsTable->item(row, 1);
    item->setText(QString::number(material->getA_hit()*NDMath::RAD2GRAD));

    item = materialsTable->item(row, 2);
    item->setText(QString::number(material->getA_max()*NDMath::RAD2GRAD));

    item = materialsTable->item(row, 3);
    item->setText(QString::number(material->getTransparency()));

    item = materialsTable->item(row, 4);
    item->setText(QString::number(material->getW_rand()));
  }
  materialsTable->resizeColumnsToContents();
}
