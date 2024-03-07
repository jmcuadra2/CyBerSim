//
// C++ Implementation: linegroup
//
// Description:
//
//
// Author: Jose Manuel Cuadra Troncoso <jmcuadra@dia.uned.es>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "linegroup.h"
#include "sensorfactory.h"
#include "robotposition.h"
#include "../world/abstractworld.h"
#include "../neuraldis/settings.h"
#include "../neuraldis/ndmath.h"
#include "../neuraldis/xmloperator.h"

LineGroup::LineGroup(const QDomElement& e): SensorGroup(e)
{
    //// Aqui se leen los parametros probabilisticos

    bool ok;
    QDomElement e_prob = xml_operator->findTag("probability_params", e);
    if (e_prob.isNull())
        initProbabilityParams();
    else {
        idealSensor = (bool)e_prob.attribute("ideal").toInt(&ok);
        sigma = e_prob.attribute("sigma").toDouble(&ok);
    }

    QDomElement e_show = xml_operator->findTag("show_beam", e);
    if (e_show.isNull())
        showBeam = false;
    else
        showBeam = (bool)e_show.text().toInt();

    QDomElement e_beam_color = xml_operator->findTag("beam_color", e);
    if (!e_beam_color.isNull())
        beam_color = e_beam_color.text();
}

LineGroup::LineGroup(int sens_type, int sup_type): SensorGroup(sens_type, sup_type)
{
    initProbabilityParams();
}

LineGroup::~LineGroup()
{

}

void LineGroup::initProbabilityParams(void)
{
    idealSensor = true;
    sigma = 0;
}

bool LineGroup::hasScalableField(void)
{
    return SensorGroup::hasScalableField();
}

QString LineGroup::getGroupName() const
{
    return SensorGroup::getGroupName();
}

bool LineGroup::createSensors(void)
{

    SensorFactory* factory = SensorFactory::instance();
    Settings* settings = Settings::instance();
    AbstractWorld* world = settings->getWorld();

    double maxRaduis = 0;

    for (uint i = 0; i < num_sensors; i++) {
        maxRaduis = maxRaduis < radii[i] ? radii[i] : maxRaduis;
    }
    LinearSensor* sen;
    for (uint i = 0; i < num_sensors; i++) {
        if (polar_mode) {
            sen = factory->factoryLinearSensor(sensor_type,
                               identifications[i], radii[i], group_idents[i],
                               x_orgs[i], y_orgs[i], start_angles[i], end_angles[i]);
            if (sen) {
                sen->initialize();
                rotations[i] = sen->getRotation();
                orientations[i] = sen->getOrientation();
            }
        }
        else {
            sen = factory->factoryLinearSensor(sensor_type,
                                identifications[i], group_idents[i], radii[i],
                                rotations[i], orientations[i]);
            if (sen)
                sen->initialize();
        }
        if (!sen)
            continue;

        if (sensor_max_val >= 0)
            sen->setSensorMaxVal(sensor_max_val);
        if (sensor_wrong_val >= 0)
            sen->setSensorWrongVal(sensor_wrong_val);
        if (sensor_critic_val >= 0)
            sen->setSensorCriticVal(sensor_critic_val);
        sen->setRobotPosition(robot_position);
        sen->setWorldScale(world->getWorldScale());
        sen->setProbabilisticParams(sigma);
        QList<SensorLine*> line = sen->createSensorLines(world, drawing_type,
                                drawing_color, drawing_color2, beam_color,
                                sen->getBeamResolution(), showBeam);

        sen->setIdealSensor(idealSensor);

        linear_sensors.append(sen);
        sensors.append(sen);
    }

    if(num_sensors) {
        int rectSide = int(sensor_max_val/world->getWorldScale() + 1.1*maxRaduis + 0.5);

        boundRect.setRect(int(x_center + 0.5) - rectSide,
                          int(y_center + 0.5) - rectSide, 2*rectSide, 2*rectSide);
    }

    return true;
}


const QString& LineGroup::sourceName(void)
{
    return SensorGroup::sourceName();
}

QList< AbstractSensor*>& LineGroup::getSensors(void)
{
    return SensorGroup::getSensors();
}

void LineGroup::addSources(QList<RadialFieldSource*> sources)
{
    SensorGroup::addSources(sources);
}

void LineGroup::readSensorsPosId(const QDomElement& e)
{
    SensorGroup::readSensorsPosId(e);
}

void LineGroup::sample(void)
{
    Settings* settings = Settings::instance();
    AbstractWorld* world = settings->getWorld();

    //Ahora viene lo del rectangulo del alcance para elegir las líneas del mundo
    world->setSensorsBoundingRect(boundRect);

    SensorGroup::sample();
}

void LineGroup::scaleSensorField(const double& scale)
{
}

void LineGroup::setInRobotPosition(void)
{

    QListIterator<LinearSensor*> it(linear_sensors);
    LinearSensor* sen;
    while (it.hasNext()) {
        sen = it.next();
        sen->setPosition(robot_position->x(), robot_position->y(),
                         robot_position->rotation());
    }
    if(!boundRect.isNull()) {
        //     boundRect->move(robot_position->x() - boundRect->width()/2, robot_position->y() - boundRect->height()/2);
    }

}

void LineGroup::setRobotPosition(RobotPosition* rob_pos)
{
    SensorGroup::setRobotPosition(rob_pos);
}

void LineGroup::worldCleared(void)
{
    SensorGroup::worldCleared();
}

void LineGroup::writeSensors(QDomDocument& doc, QDomElement& e)
{
    SensorGroup::writeSensors(doc, e);
}

void LineGroup::write_special(QDomDocument& doc, QDomElement& e)
{
}

void LineGroup::setGroupName(const QString& group_name)
{
    SensorGroup::setGroupName(group_name);
}

void LineGroup::setScalableField(bool has)
{
    SensorGroup::setScalableField(has);
}

void LineGroup::writeSensorsDefs(QDomDocument& doc, QDomElement& e)
{
    SensorGroup::writeSensorsDefs(doc, e);
}

void LineGroup::showSensors(bool on)
{
    SensorGroup::showField(on);
}

void LineGroup::moveBy(double dx, double dy, double ang)
{
    if(!boundRect.isNull())
        boundRect.translate(dx, dy);
    SensorGroup::moveBy(dx, dy, ang);
}

void LineGroup::rotate(void)
{
    if(!boundRect.isNull())
        boundRect.moveTo(NDMath::roundInt(robot_position->x() - boundRect.width()/2), NDMath::roundInt(robot_position->y() - boundRect.height()/2));
    SensorGroup::rotate();
}

void LineGroup::advance(int stage)
{
    if(!boundRect.isNull())
        boundRect.moveTo(NDMath::roundInt(robot_position->x() - boundRect.width()/2), NDMath::roundInt(robot_position->y() - boundRect.height()/2));
    SensorGroup::advance(stage);
}

void LineGroup::setVisible(bool on)
{
    SensorGroup::setVisible(on);
}
