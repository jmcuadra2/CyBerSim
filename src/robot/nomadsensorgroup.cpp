//
// C++ Implementation: nomadsensorgroup
//
// Description: 
//
//
// Author: Javier Garcia Misis <>, (C) 2010
//
// Copyright: See COPYING file that comes with this distribution
//
//

#include "sensorfactory.h"
#include "robotposition.h"
#include "../world/abstractworld.h"
#include "../neuraldis/settings.h"
#include "../neuraldis/ndmath.h"
#include "../neuraldis/xmloperator.h"
#include "nomadsensorgroup.h"
#include "nomadsensor.h"

NomadSensorGroup::NomadSensorGroup(const QDomElement& e, void * client): SensorGroup(e)
{
    client_ = client;
    std::cout << "Creando NomadSensorGroup" << std::endl;
}


NomadSensorGroup::NomadSensorGroup(int sens_type, int sup_type, void * client) :
    SensorGroup(sens_type, sup_type)
{
    client_ = client;
    std::cout << "Creando NomadSensorGroup" << std::endl;
}


NomadSensorGroup::~NomadSensorGroup()
{
}


bool NomadSensorGroup::createSensors()
{
    QVector<double> sx(num_sensors);
    QVector<double> sy(num_sensors);
    SensorFactory* factory = SensorFactory::instance();
    Settings* settings = Settings::instance();
    AbstractWorld* world = settings->getWorld();
    NomadSensor* sen;
    
    double maxRaduis = 0;
    
    for (uint i = 0; i < num_sensors; i++) {
        QPointF p_org = NDMath::polarToCartesian(radii[i], rotations[i]);
        sx[i] = p_org.x();
        sy[i] = p_org.y();
        maxRaduis = maxRaduis < radii[i] ? radii[i] : maxRaduis;
    }

    for (uint i = 0; i < num_sensors; i++) {
        if (polar_mode) {
            sen = factory->factoryNomadSensor(sensor_type,
                                              identifications[i], radii[i], group_idents[i],
                                              start_angles[i], end_angles[i]);
            if (sen) {
                sen->initialize();
                rotations[i] = sen->getRotation();
                orientations[i] = sen->getOrientation();
            }
        }
        else {
            sen = factory->factoryNomadSensor(sensor_type,
                                              identifications[i], group_idents[i], radii[i],
                                              rotations[i], orientations[i]);
            if (sen)
                sen->initialize();
        }
        if (!sen) continue;

        sen->setConnector((Nomad::Connector *) client_);

        if (sensor_max_val >= 0)
            sen->setSensorMaxVal(sensor_max_val);
        if (sensor_wrong_val >= 0)
            sen->setSensorWrongVal(sensor_wrong_val);
        if (sensor_critic_val >= 0)
            sen->setSensorCriticVal(sensor_critic_val);
        sen->setRobotPosition(robot_position);
        sen->setWorldScale(world->getWorldScale());
        sen->createSensorLine(world, drawing_type, drawing_color, drawing_color2);

        sensors.append(sen);
    }

    return true;
}

void NomadSensorGroup::scaleSensorField(const double& scale)
{
}

void NomadSensorGroup::write_special(QDomDocument& doc, QDomElement& e)
{
}
