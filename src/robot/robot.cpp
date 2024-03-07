/***************************************************************************
                          robot.cpp  -  description
                             -------------------
    begin                : lun dic 8 2003
    copyright            : (C) 2003 by Jose M. Cuadra Troncoso
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

#include <QFile>
#include <QMessageBox>
#include <QEvent>
#include <qewdialogfactory.h>
#include <QPixmap>
#include <QMouseEvent>
#include <QTextStream>

#include "robot.h"
#include "robotposition.h"
#include "sensorsset.h"
#include "differentialmotor.h"
#include "../neuraldis/settings.h"
#include "gui/robotkineticstab.h"
#include "../neuraldis/ndmath.h"
#include "../neuraldis/xmloperator.h"
#include "../neuraldis/neuraldisdoc.h"
#include "../world/abstractworld.h"

Robot::Robot(int n_id, QObject *parent, QString name_ )
    : QObject(parent)
{

    num_id = n_id;
    robot_scale = 2.0;
    setObjectName(name_);
    full_name = QString(objectName()) + "_" + QString::number(num_id);
    status_ = false;
    is_bump = false;
    is_crash = false;

    x = 100;
    y = 100;

    diamtr = 30;
    rvel = 0.0;
    lvel = 0.0;
    init_right_vel = 0.0;
    init_left_vel = 0.0;
    old_right_vel = 0.0;
    old_left_vel = 0.0;
    init_rot = 0.0;
    //base   inerl = 0.0;
    //   inerr = 0.0;
    contact = false;
    prev_contact = false;
    sensors_visible = true;

    motors_output = QVector<double>(4);
    motors_output.fill(0.0);
    sensorsSet = 0;

    robot_sprite = 0;
    saveKinetics = false;
    robot_position = 0;
    is_recording = false;
    is_monitoring = false;
    file_changed = false;
    vlin = 0;
    vrad = 0;
    advance_period = REAL_TIME_ADV_PERIOD;
    max_vel = 100;
    rot = 0;
    actualSensorsList = 0;
    sensors_visible = false;
    //   head_increment = 0;
    //   vel_increment = 0;
    driving = false;
    paused  = false;

    mainMotor = 0;
    //   odomX = 0;
    //   odomY = 0;
    //   exactOdomX = 0;
    //   exactOdomY = 0;
    velUnitsConversion = 1.0;
    realRobot = false; // MobileSim
    prevX = 0;
    prevY = 0;
    prevHead = 0;

    // Para Aria
    xInit = 0.0;
    yInit = 0.0;
    rotInit = 0.0;
}


Robot::~Robot()
{
    if(robot_sprite)
        delete robot_sprite;
    if(sensorsSet)
        delete sensorsSet;
    if(robot_position)
        delete robot_position;
    
    QHashIterator<QString, AbstractMotor*> itm(motors);
    while (itm.hasNext()) {
        itm.next();
        delete itm.value();
    }

}

void Robot::setIntrinsicVars(const QDomElement& e_size)
{

    bool ok;
    robot_scale = e_size.attribute("robot_scale").toDouble(&ok);
    real_diamtr = e_size.attribute("robot_diameter").toInt(&ok);
    double real_wheelsSep = e_size.attribute("wheels_separation").toDouble(&ok);
    if(!ok) real_wheelsSep  = -1;
    setRealWheelsSep(real_wheelsSep);
    diamtr = NDMath::roundInt(real_diamtr/robot_scale);
    if(diamtr < 10) diamtr = 10;

}

void Robot::setCanvas(QGraphicsScene* w_canvas)
{

    robot_sprite->setX(0);
    robot_sprite->setY(0);
    robot_sprite->diameter = NDMath::roundInt(diamtr);
    robot_sprite->wheelsSep = wheelsSep;
    robot_sprite->rotation = rot;

    robot_position = new RobotPosition(robot_sprite);
    connect(robot_sprite, SIGNAL(outWorld()), this, SIGNAL(outWorld()));
    robot_sprite->robot_scale = robot_scale;
    w_canvas->addItem(robot_sprite);

}

void Robot::setRobotSprite(RobotSprite* robot_sprite)
{
    this->robot_sprite = robot_sprite;
}

void Robot::setX(double x_)
{

    double x_sprite = world->xTransform(x_);
    double mov_x = x_sprite - robot_sprite->x();
    robot_sprite->setX(x_sprite);
    sensorsSet->moveBy(mov_x, 0.0);
    x = x_;// inicial
}

void Robot::setY(double y_)
{

    double y_sprite = world->yTransform(y_);
    double mov_y = y_sprite - robot_sprite->y();
    robot_sprite->setY(y_sprite);
    sensorsSet->moveBy(0.0, mov_y);
    y = y_; // inicial
}

void Robot::setXY(double x_, double y_)
{

    double x_sprite = world->xTransform(x_);
    double y_sprite = world->yTransform(y_);
    double mov_x = x_sprite - robot_sprite->x();
    double mov_y = y_sprite - robot_sprite->y();
    robot_sprite->setX(x_sprite);
    robot_sprite->setY(y_sprite);
    sensorsSet->moveBy(mov_x, mov_y);

}

void Robot::setMaxVel(double m_vel) 
{
    if(m_vel <= 0) {
//        QMessageBox::information(nullptr, tr("Robot MaxVel"), tr("Robot MaxVel = 0") + "\n" + tr("Leaving previous value"), tr("&Ok"));
                QMessageBox::information(nullptr, tr("Robot MaxVel"), tr("Robot MaxVel = 0") + "\n" + tr("Leaving previous value"), QMessageBox::Ok);
    }
    max_vel = m_vel;
    if(mainMotor)
        mainMotor->setMaxVel(max_vel);
    if(robot_sprite)
        robot_sprite->setMaxVel(max_vel);
    
}

void Robot::setVel2(double r_vel, double l_vel)
{

    rvel = r_vel;
    lvel = l_vel;
    emit rVel(rvel);
    emit lVel(lvel);

}

void Robot::setOldVel(double r_vel, double l_vel)
{

    old_right_vel = r_vel;
    old_left_vel = l_vel;

}

void Robot::setRealWheelsSep(double realWheelsSep) 
{ 
    this->realWheelsSep = realWheelsSep/10; // mm a cm
    wheelsSep = realWheelsSep/robot_scale; // para RobotSprite en pixels
}


bool Robot::walking(void)
{

    return status_;

}

void Robot::setStatus(bool on)
{

    status_ = on;

}

void Robot::rotation(double rot_)
{

    while(rot_ < 0.0) rot_ += 360.0;
    while(rot_ > 360.0) rot_ -= 360.0;
    init_rot = rot_;
    rot_ = world->headingTransform(init_rot);
    robot_sprite->setRotation(rot_);
    sensorsSet->rotate();

}

void Robot::crash(void)
{

    is_crash = false;
    is_bump = false;
    prev_contact = false;
    contact = false;

}

bool Robot::isCrash(void)
{
    return is_crash;
}

double Robot::getRotInit() const
{
    return rotInit;
}

void Robot::updateOdometries(bool paintRaw, bool paintSlam)
{
    double odomX = mainMotor->getRawOdometry().getPosition().at(0);
    double  odomY = mainMotor->getRawOdometry().getPosition().at(1);
    rawOdometry.setCoordinates(odomX, odomY, mainMotor->getRawOdometry().getOrientation().at(0));

    double slamOdomX = mainMotor->getSlamOdometry().getPosition().at(0);
    double slamOdomY = mainMotor->getSlamOdometry().getPosition().at(1);
    slamOdometry.setCoordinates(slamOdomX, slamOdomY, mainMotor->getSlamOdometry().getOrientation().at(0));

    if(paintRaw) {
        QPointF p_raw = QPointF(odomX, odomY);
        NDMath::selfRotateRad(p_raw, /*-world->inverseHeadingTransform(1)* */
                              (init_rot + rotInit)/NDMath::RAD2GRAD, QPointF());
        robot_sprite->updateRawOdometry(world->xTransform(p_raw.x() + x + xInit),
                                        world->yTransform(p_raw.y() + y + yInit));
        //        std::cout << "updateRawOdometry " << p_raw.x() + x + xInit << " " << p_raw.y() + y + yInit << endl;
    }
    if(paintSlam) {
        QPointF p_slam = QPointF(slamOdomX, slamOdomY);
        NDMath::selfRotateRad(p_slam, /*-world->inverseHeadingTransform(1)* */
                              (init_rot + rotInit)/NDMath::RAD2GRAD, QPointF());
        robot_sprite->updateSlamOdometry(world->xTransform(p_slam.x() + x + xInit),
                                         world->yTransform(p_slam.y() + y + yInit));
    }

}

void Robot::sensorsVisible(bool on)
{

    if(sensors_visible == on) return;
    sensors_visible = on;
    robot_sprite->setVisible(on);
    sensorsSet->setVisible(on);

}

void Robot::viewSensors(bool show)
{
    sensorsVisible(true);
    sensorsSet->showField(show);
}

void Robot::readSensors(void)
{
    sensorsSet->sample();
}

void Robot::initSensors(void)
{
    sensorsSet->initSensors();
}

void Robot::worldCleared(void)
{
    sensorsSet->worldCleared();
}

void Robot::worldLoaded(void)
{
    emit sendRadius(diamtr/2.0);
}

void Robot::addMotor(const QString& motor_id, AbstractMotor* motor)
{
    if(!mainMotor)
        cerr << "Warning: Robot::addMotor() Robot hasn't main motor yet" << endl;
    else if(!motors.contains(motor_id))
        motors.insert(motor_id, motor);
    else
        cerr << "Warning: Robot::addMotor() Motor already added" << endl;
}

AbstractMotor* Robot::getMainMotor(void)
{
    return mainMotor;
}

void Robot::save(void)
{

    QFile file_robot;
    QTextStream ts;
    XMLOperator* xml_operator = XMLOperator::instance();

    QDomDocument doc("robot");
    QDomElement root = doc.createElement("Robot");
    doc.appendChild(root);

    root.appendChild(xml_operator->createTextElement(doc,
                                                     "robot_type", type()));

    QDomElement tag_cons = doc.createElement("constructor");
    root.appendChild(tag_cons);
    tag_cons.setAttribute("num_id", QString::number(num_id));
    tag_cons.setAttribute("name", objectName());

    QDomElement tag_intr = doc.createElement("Intrinsic_vars");
    root.appendChild(tag_intr);

    QDomElement tag_size = doc.createElement("size");
    tag_intr.appendChild(tag_size);
    tag_size.setAttribute("robot_diameter", QString::number(real_diamtr));
    tag_size.setAttribute("robot_scale", QString::number(robot_scale));
    tag_size.setAttribute("wheels_separation", QString::number(realWheelsSep * 10)); // cm  to mm

    save_special(tag_size);

    QDomElement tag_img = doc.createElement("Images");
    tag_intr.appendChild(tag_img);

    QString relativeName = robot_image_file;
    tag_img.appendChild(xml_operator->createTextElement(doc,
                    "robot_image_file", relativeName.remove(Settings::instance()->getAppDirectory()
                                                            + QDir::separator())));

    sensorsSet->writeSensors(doc, root);

    NeuralDisDoc* doc_manager = NeuralDisDoc::instance();
    QString ret_name = doc_manager->saveDocument(file_name, doc.toString(4),
                       tr("Robot file saving"), "robot", tr("Robot file saving"), true);
    if(!ret_name.isEmpty())
        file_name = ret_name;
    
}

void Robot::write(QDomDocument& doc, QDomElement& e)
{

    if(file_changed) {
//        if(QMessageBox::information(nullptr,tr("Robot save"), tr("Robot definitions have changed.") + "\n" + tr("Save ?"), tr("&Ok"), tr("&Cancel")) == 0)
          if(QMessageBox::information(nullptr,tr("Robot save"), tr("Robot definitions have changed.") + "\n" + tr("Save ?"), QMessageBox::Ok | QMessageBox::Cancel) == QMessageBox::Ok)
            save();
        file_changed = false;
    }

    NeuralDisDoc* doc_manager = NeuralDisDoc::instance();
    XMLOperator* xml_operator = XMLOperator::instance();

    QString relativeName = file_name;
    e.appendChild(xml_operator->createTextElement(doc,
              "robot_file", relativeName.remove(Settings::instance()->getRobotDirectory() + QDir::separator())));

    QDomElement tag_kinet, old_tag_kinet;
    if(!saveKinetics) {
        QDomNode root = doc_manager->getXMLSim().firstChild();
        old_tag_kinet = xml_operator->findTag("Kinetics", root.toElement(), false);
        if(old_tag_kinet.isNull())
            tag_kinet = doc.createElement("Kinetics");
        else
            tag_kinet = old_tag_kinet.cloneNode().toElement();
    }
    else
        tag_kinet = doc.createElement("Kinetics");
    if(saveKinetics || old_tag_kinet.isNull()) {
        double x_robot;
        double y_robot;
        if(robot_sprite) {
            x_robot = world->inverseXTransform(robot_sprite->x());
            y_robot = world->inverseYTransform(robot_sprite->y());
        }
        else {
            x_robot = x;
            y_robot = y;
        }
        tag_kinet.appendChild(xml_operator->createTextElement(doc, "x_robot", QString::number(x_robot)));
        tag_kinet.appendChild(xml_operator->createTextElement(doc, "y_robot", QString::number(y_robot)));
        tag_kinet.appendChild(xml_operator->createTextElement(doc, "init_rotation", QString::number(init_rot)));
        tag_kinet.appendChild(xml_operator->createTextElement(doc,  "init_right_vel", QString::number(init_right_vel)));
        tag_kinet.appendChild(xml_operator->createTextElement(doc,  "init_left_vel", QString::number(init_left_vel)));
        tag_kinet.appendChild(xml_operator->createTextElement(doc, "max_vel", QString::number(max_vel)));
        tag_kinet.appendChild(xml_operator->createTextElement(doc, "robot_advance_period", QString::number(advance_period)));
    }

    e.appendChild(tag_kinet);

    if(saveKinetics || old_tag_kinet.isNull())
        write_special(doc, e);

}

void Robot::initRecord(void)
{

    emit velMonitor(vlin, -1);
    emit rotMonitor(vrad, -1);
    emit sendToRecorder(robot_sprite->x()*robot_scale/10.0, 1); // 10mm = 1cm
    emit sendToRecorder(robot_sprite->y()*robot_scale/10.0, 2);
    emit sendToRecorder(robot_sprite->getRotation(), 3);
    emit sendToRecorder(vlin, 4);
    emit sendToRecorder(vrad, 5);
    emit sendToRecorder(0, 6); // RobotSprite::add_rotation
    emit sendToRecorder(0, 7); // RobotSprite::forw

}    

void Robot::recordRobot(double x, double y, double rot,
                        double , double , double vrot, bool forw)
{

    if(is_monitoring) {
        emit velMonitor(vlin, -1);
        emit rotMonitor(vrot, -1);
    }
    if(is_recording) {
        emit sendToRecorder(world->inverseXTransform(x), 1);
        emit sendToRecorder(world->inverseXTransform(y), 2);
        emit sendToRecorder(world->inverseHeadingTransform(rot), 3);
        emit sendToRecorder(vlin, 4);
        emit sendToRecorder(vrot, 5);
        emit sendToRecorder(0, 6); // RobotSprite::add_rotation
        emit sendToRecorder(forw, 7); // RobotSprite::forw
    }

}

QString Robot::fullName(void)
{
    return full_name;
}

void Robot::initMotors(void)
{
    double rv = max_vel == 0 ? 0 : init_right_vel/max_vel;
    double lv = max_vel == 0 ? 0 : init_left_vel/max_vel;

    if(!driving) {
        mainMotor->setInitVel1(lv);
        mainMotor->setInitVel2(rv);
    }
    else {
        mainMotor->setInitVel1(0);
        mainMotor->setInitVel2(0);
    }

}

void Robot::getMouseEvent(QMouseEvent* e) 
{
    if(status_ && !paused) {
        e->ignore();
        return;
    }
    if(e->type() == QEvent::MouseButtonPress) {
        if(e->button() == Qt::RightButton) {
            QPolygonF r = robot_sprite->mapToScene(robot_sprite->boundingRect());
            if(r.containsPoint(e->pos(), Qt::OddEvenFill))
                editKinetics();
        }
        else
            e->ignore();
    }
    else
        e->ignore();

}

void Robot::getKeyEvent(QKeyEvent* event, bool key_on) 
{
    if(!driving)
        return;
    emit sendKeyEvent(event, key_on);
}

void Robot::setDriving(bool on)
{ 
    if(driving != on) {
        driving = on;
        emit sendDrived(on);
        robot_sprite->drived = on;
    }
}

bool Robot::editKinetics(void)
{

    bool ret = false;

    QewDialogFactory* factory = new QewDialogFactory();
    QewExtensibleDialog* dialog = factory->createDialog(QewExtensibleDialog::QewTabWidget);
    if(dialog->setUpDialog()) {
        QList<QewExtensibleDialog*> add_dialogs = createKineticsDialog(dialog);
        QList<QewExtensibleDialog*> it(add_dialogs);
        for(int j = 0; j < it.size(); j++)
            dialog->addExtensibleChild(it.value(j), "");
        dialog->setWindowTitle("Robot kinetics");
        if(dialog) {
            if(dialog->exec() == QewExtensibleDialog::Accepted)
                ret = true;
            delete dialog;
        }
    }
    else
        delete dialog;
    return ret;

}

QList<QewExtensibleDialog*> Robot::createKineticsDialog(QewExtensibleDialog* parent)
{

    RobotKineticsTab* robkin_tab = 0;
    QList<QewExtensibleDialog*> add_dialogs;
    if(parent) {
        robkin_tab = new RobotKineticsTab(parent);
        if(robkin_tab->setUpDialog()) {
            robkin_tab->init((Robot*) this);
            add_dialogs.append(robkin_tab);
        }
        else
            delete robkin_tab;
    }
    return add_dialogs;
}

void Robot::setWorld(AbstractWorld* robot_world)
{
    world = robot_world;
    connect(getSprite(), SIGNAL(intPosition(int, int)), world, SLOT(robotPosition(int, int)));
}

void Robot::setAdvancePeriod(int ap)
{
    advance_period = ap;
    if(mainMotor)
        mainMotor->setAdvancePeriod(advance_period);
}
