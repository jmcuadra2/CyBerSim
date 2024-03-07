//
// C++ Implementation: editlinedialog
//
// Description: 
//
//
// Author: Jose Manuel Cuadra Troncoso <jmcuadra@dia.uned.es>, (C) 2009
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "editlinedialog.h"
#include <QVariant>

#include "../../neuraldis/dictionarymanager.h"
#include "../../neuraldis/settings.h"
#include "../../neuraldis/xmloperator.h"
#include "../../robot/abstractsensor.h"

editLineDialog::editLineDialog(QWidget *parent)
    : QewExtensibleDialog(parent)
{
    setupUi(this);

    QObject::connect(w_rand_spin, SIGNAL(valueChanged(double)), this, SLOT(setW_Rand(double)));
    QObject::connect(a_hit_spin, SIGNAL(valueChanged(double)), this, SLOT(setA_Hit(double)));
    QObject::connect(a_max_spin, SIGNAL(valueChanged(double)), this, SLOT(setA_Max(double)));
    QObject::connect(transparencySpinBox, SIGNAL(valueChanged(double)), this, SLOT(setTransparency(double)));
}

editLineDialog::~editLineDialog()
{
}

void editLineDialog::setW_Rand(double d) {
    if(w_rand != d) {
        w_rand = d;
        w_rand_spin->setValue(d);
    }
}

void editLineDialog::setA_Hit(double d) {
    if(a_hit != d) {
        a_hit = d;
        a_hit_spin->setValue(d);
    }
}

void editLineDialog::setA_Max(double d) {
    if(a_max != d) {
        a_max = d;
        a_max_spin->setValue(d);
    }
}

void editLineDialog::setTransparency(double t) {
    if(transparency != t) {
        transparency = t;
        transparencySpinBox->setValue(t);
    }
}

void editLineDialog::init(Material* mat) {
    material = mat;
    setW_Rand(material->getW_rand());
    setA_Hit(material->getA_hit()*NDMath::RAD2GRAD);
    setA_Max(material->getA_max()*NDMath::RAD2GRAD);
    setTransparency(material->getTransparency());

    QDomElement elem_classes;
    DictionaryManager* dict_manager = DictionaryManager::instance();
    NameClass_Map names_list;
    bool ret = dict_manager->readFileConf("", "AbstractSensor", elem_classes,
                                          AbstractSensor::LINEAR_SENSOR);
    if(ret) {
        dict_manager->getNamesMap(elem_classes, names_list);
        //int class_type = dict_manager->chooseClass(names_list, QObject::tr("Choose a type of AbstractSensor"));

        NameClass_Map::Iterator it;
        sensor_type_combo->clear();
        int i = 0;
        for(it =  names_list.begin(); it !=  names_list.end(); ++it) {
            sensor_type_combo->insertItem(i, it.key());
            i++;
        }
        int idx = sensor_type_combo->findText(names_list.key(material->getSensorType()));
        sensor_type_combo->setCurrentIndex(idx);
    } else
        sensor_type_combo->setEnabled(false);
}

int editLineDialog::getSensorType(QString id)
{
    // Devuelve el tipo de sensor seleccionado, 0 en caso de redibir un 'id'
    // que no se encuentre
    int ret = 0;

    QDomElement elem_classes;
    DictionaryManager* dict_manager = DictionaryManager::instance();
    NameClass_Map names_list;
    bool success = dict_manager->readFileConf("", "AbstractSensor", elem_classes, AbstractSensor::LINEAR_SENSOR);
    if(success) {
        dict_manager->getNamesMap(elem_classes, names_list);
        NameClass_Map::Iterator it;
        sensor_type_combo->clear();
        for (it =  names_list.begin(); it !=  names_list.end(); ++it)
            if (it.key() == id)
                ret = it.value();
    }

    return ret;
}

void editLineDialog::saveThis(void) {
    material->setA_max(a_max/NDMath::RAD2GRAD);
    material->setA_hit(a_hit/NDMath::RAD2GRAD);
    material->setW_rand(w_rand);
    material->setTransparency(transparency);
    material->setSensorType(getSensorType(sensor_type_combo->currentText()));
}
