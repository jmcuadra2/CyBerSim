/***************************************************************************
                          codecontrolfactory.cpp  -  description
                             -------------------
    begin                : Sat Apr 30 2005
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

#include "codecontrolfactory.h"
#include "nullsimcontrol.h"
#include "nullnomadsimcontrol.h"
#include "areacenterreactivecontrol.h"
// #include "areacenterslamcontrol.h"
// #include "areacentersegmentedcontrol.h"
#include "areacenterslamsegmentcontrol.h"
#include "areacenterreactivecontrolrightleft.h"
#include "../neuraldis/settings.h"

CodeControlFactory::CodeControlFactory()
{

  null_control = 0;
    
}
CodeControlFactory::~CodeControlFactory(){
}

BaseSimControl* CodeControlFactory::construct(int control_type, const QDomElement& , const QString& )
{

  BaseSimControl* base_control = 0;
  if(control_type == BaseSimControl::NULL_CONTROL) {
    NullSimControl* control = new NullSimControl();
//     control->setControlFileName(control_filename);    
    null_control = control;     
  }
  else if(control_type == BaseSimControl::NULL_NOMAD_CONTROL) {
    NullNomadSimControl* control = new NullNomadSimControl();
//     control->setControlFileName(control_filename);    
    null_control = control;     
  }
  else if(control_type == BaseSimControl::AREA_CENTER_REACTIVE_CONTROL) {
    AreaCenterReactiveControl* control = new AreaCenterReactiveControl();
//     control->setControlFileName(control_filename);
    null_control = control;
    Settings* prog_settings = Settings::instance();
    AbstractWorld* world = prog_settings->getWorld();
    control->setWorld(world);
  }
  else if(control_type == BaseSimControl::AREA_CENTER_SLAM_CONTROL) {
    AreaCenterSLAMControl* control = new AreaCenterSLAMControl();
//     control->setControlFileName(control_filename);
    null_control = control;
    Settings* prog_settings = Settings::instance();
    AbstractWorld* world = prog_settings->getWorld();
    control->setWorld(world);
  }
  else if(control_type == BaseSimControl::AREA_CENTER_SEGMENT_CONTROL) {
    AreaCenterSegmentedControl* control = new AreaCenterSegmentedControl();
//     control->setControlFileName(control_filename);
    null_control = control;
    Settings* prog_settings = Settings::instance();
    AbstractWorld* world = prog_settings->getWorld();
    control->setWorld(world);
  }
  else if(control_type == BaseSimControl::AREA_CENTER_SLAM_SEGMENT_CONTROL) {
    AreaCenterSLAMSegmentControl* control = new AreaCenterSLAMSegmentControl();
//     control->setControlFileName(control_filename);
    null_control = control;
    Settings* prog_settings = Settings::instance();
    AbstractWorld* world = prog_settings->getWorld();
    control->setWorld(world);
  }
  else if(control_type == BaseSimControl::AREA_CENTER_RIGHT_LEFT_REACTIVE_CONTROL) {
    AreaCenterReactiveControlRightLeft* control = new AreaCenterReactiveControlRightLeft();
//     control->setControlFileName(control_filename);
    null_control = control;
    Settings* prog_settings = Settings::instance();
    AbstractWorld* world = prog_settings->getWorld();
    control->setWorld(world);
  }
  
  base_control = null_control;
  return base_control;
  
}

BaseSimControl* CodeControlFactory::construct(void)
{

  BaseSimControl* base_control = 0;
  QDomElement control_classes;
  DictionaryManager* dict_manager = DictionaryManager::instance();    
  NameClass_Map names_list;
  
  bool ret = dict_manager->readFileConf("newControl", "BaseSimControl", control_classes, BaseSimControl::CODE_CONTROL);
  if(ret) {
    dict_manager->getNamesMap(control_classes, names_list);
    int control_type = dict_manager->chooseClass(names_list, QObject::tr("Choose a type of code based control"));   
    if(control_type == BaseSimControl::NULL_CONTROL) {
      NullSimControl* control = new NullSimControl();
//     control->setControlFileName(control_filename);    
      null_control = control;     
    }
    else if(control_type == BaseSimControl::NULL_NOMAD_CONTROL) {
      NullNomadSimControl* control = new NullNomadSimControl();
  //     control->setControlFileName(control_filename);
      null_control = control;
    }
    else if(control_type == BaseSimControl::AREA_CENTER_REACTIVE_CONTROL) {
      AreaCenterReactiveControl* control = new AreaCenterReactiveControl();
  //     control->setControlFileName(control_filename);
      null_control = control;
      Settings* prog_settings = Settings::instance();
      AbstractWorld* world = prog_settings->getWorld();
      control->setWorld(world);
    }
    else if(control_type == BaseSimControl::AREA_CENTER_SLAM_CONTROL) {
      AreaCenterSLAMControl* control = new AreaCenterSLAMControl();
  //     control->setControlFileName(control_filename);
      null_control = control;
      Settings* prog_settings = Settings::instance();
      AbstractWorld* world = prog_settings->getWorld();
      control->setWorld(world);
    }
    else if(control_type == BaseSimControl::AREA_CENTER_SEGMENT_CONTROL) {
      AreaCenterSegmentedControl* control = new AreaCenterSegmentedControl();
  //     control->setControlFileName(control_filename);
      null_control = control;
      Settings* prog_settings = Settings::instance();
      AbstractWorld* world = prog_settings->getWorld();
      control->setWorld(world);
    }
    else if(control_type == BaseSimControl::AREA_CENTER_SLAM_SEGMENT_CONTROL) {
      AreaCenterSLAMSegmentControl* control = new AreaCenterSLAMSegmentControl();
  //     control->setControlFileName(control_filename);
      null_control = control;
      Settings* prog_settings = Settings::instance();
      AbstractWorld* world = prog_settings->getWorld();
      control->setWorld(world);
    }
    else if(control_type == BaseSimControl::AREA_CENTER_RIGHT_LEFT_REACTIVE_CONTROL) {
      AreaCenterReactiveControlRightLeft* control = new AreaCenterReactiveControlRightLeft();
  //     control->setControlFileName(control_filename);
      null_control = control;
      Settings* prog_settings = Settings::instance();
      AbstractWorld* world = prog_settings->getWorld();
      control->setWorld(world);
    }
  }
  base_control = null_control;
  return base_control;     

}
