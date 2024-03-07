//
// C++ Implementation: nomadwindow
//
// Description: 
//
//
// Author: Javier Garcia Misis <>, (C) 2010
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "../nomad/nomadwindow.h"
#include <iostream>

Nomad::Window::Window(QWidget *parent)
    : QWidget(parent)
{
    setupUi(this);
    setAttribute(Qt::WA_DeleteOnClose);
    std::cout << "Creando Nomad Window" << std::endl;
}


Nomad::Window::~Window()
{
    std::cout << "Eliminando Nomad Window" << std::endl;
}

void Nomad::Window::setVelocity(double vel, double turret, double ster){
    //    std::cout << "setVelocity " << vel << std::endl;
    QString  tmp;
    tmp.setNum (vel,'g',4);
    tmp.append(" mm/s");
    lineVel->setText(tmp);

    tmp.setNum (turret,'g',4);
    tmp.append(" deg/s");
    lineTor->setText(tmp);

    tmp.setNum (ster,'g',4);
    tmp.append(" deg/s");
    lineSte->setText(tmp);

}

void Nomad::Window::setPower(double motor, double system){
    
    QString  tmp;
    int value,value2;
    tmp.setNum (motor,'g',4);
    value = (int) (motor * 100);
    lineVMotor->setText(tmp);
    barVMotor->setValue(value);

    //21.6 - 24.0
    if(motor <= 22.3){
        barVMotor->setStyleSheet(QApplication::translate("NomadWindow",
                                                         "selection-background-color: red;", 0));
    }
    else if( motor <= 23.0){
        barVMotor->setStyleSheet(QApplication::translate("NomadWindow",
                                                         "selection-background-color: orange;", 0));
    }
    else{
        barVMotor->setStyleSheet(QApplication::translate("NomadWindow",
                                                         "selection-background-color: green;", 0));
    }

    
    tmp.setNum (system,'g',4);
    lineVCPU->setText(tmp);
    value2 = (int) (system * 100);
    barVCPU->setValue(value2);

    //10.8 - 12.0
    if(system <= 11.2){
        barVCPU->setStyleSheet(QApplication::translate("NomadWindow",
                                                       "selection-background-color: red;", 0));
    }
    else if( system <= 11.5){
        barVCPU->setStyleSheet(QApplication::translate("NomadWindow",
                                                       "selection-background-color: orange;", 0));
    }
    else{
        barVCPU->setStyleSheet(QApplication::translate("NomadWindow",
                                                       "selection-background-color: green;", 0));
    }

}

void Nomad::Window::BotonTalk(){
    emit command("TALK",0);
}

void Nomad::Window::BotonStop(){
    emit command("STOP",0);
}

void Nomad::Window::BotonZR(){
    emit command("ZR",0);
}

void Nomad::Window::Simulado(bool value){
    if(value){
        emit command("TO_SIM",0);
    }else{
        emit command("TO_REAL",0);
    }
}

void Nomad::Window::fixedTurret(bool value){
    emit command("FIXED",value);
}

void Nomad::Window::setReal(bool value){
    robotReal->setChecked(value);
    robotSim->setChecked(!value);
}


void Nomad::Window::keyPressEvent ( QKeyEvent * keyEvent ){
    std::cout << "Nomad::Window::keyPressEvent " << std::endl;
    
}
