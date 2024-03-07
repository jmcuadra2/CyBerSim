#include <stdio.h>
#include <stdlib.h>

#include "Aria.h"
#include "ArNetworking.h"
#include "ArSystemStatus.h"
#include "Arnl.h"


#include "../aria-tools/arserverextinforobot.h"
#include "../aria-tools/arservermodereactive.h"


/** Main function */
int main(int argc, char *argv[])
{
  // Initialize location of Aria, Arnl and their args.
  Aria::init();
  Arnl::init();
  
  // log to a file
  //ArLog::init(ArLog::File, ArLog::Normal, "log.txt", true, true);
  //ArLog::init(ArLog::File, ArLog::Verbose);
 
  // To get CPU usage and wireless information from Linux
  ArSystemStatus::runRefreshThread();

  // The robot object
  ArRobot robot;

  // Our server
  ArServerBase server;
  
  // Parse the command line arguments.
  ArArgumentParser parser(&argc, argv);

  // Set up our simpleConnector
  ArSimpleConnector simpleConnector(&parser);

  // Set up our simpleOpener
  ArServerSimpleOpener simpleOpener(&parser);

  // Set up our client for the central server
  ArClientSwitchManager clientSwitch(&server, &parser);
    
  // Load default arguments for this computer (from /etc/Aria.args, environment
  // variables, and other places)
  parser.loadDefaultArguments();

  // set up a gyro
  ArAnalogGyro gyro(&robot);
  
  // Parse arguments for the simple connector.
  if (!Aria::parseArgs() || !parser.checkHelpAndWarnUnparsed())
  {
    ArLog::log(ArLog::Normal, "\nUsage: %s -map mapfilename\n", argv[0]);
    Aria::logOptions();
    Aria::exit(1);
  }


#ifndef SONARNL
  // The laser object
  ArSick sick(361, 181);

  // Add the laser to the robot
  robot.addRangeDevice(&sick);


#endif // ifndef SONARNL

  // Sonar, must be added to the robot, used by teleoperation and wander to
  // detect obstacles
  ArSonarDevice sonarDev;

  // Add the sonar to the robot
  robot.addRangeDevice(&sonarDev);
  
  // Set up where we'll look for files
  char fileDir[1024];
  ArUtil::addDirectories(fileDir, sizeof(fileDir), Aria::getDirectory(), 
       "examples");
  
  // Set up the map, this will look for files in the examples
  // directory (unless the file name starts with a /, \, or .
  // You can take out the 'fileDir' argument to look in the current directory
  // instead
  ArMap arMap(fileDir);
  // set it up to ignore empty file names (otherwise the parseFile
  // on the config will fail)
  arMap.setIgnoreEmptyFileName(true);

  // add our logging to the config
  ArLog::addToConfig(Aria::getConfig());
  
  // First open the server up
  if (!simpleOpener.open(&server, fileDir, 240))
  {
    if (simpleOpener.wasUserFileBad())
      ArLog::log(ArLog::Normal, "Bad user file");
    else
      ArLog::log(ArLog::Normal, "Could not open server port");
    exit(2);
  }

  // Connect the robot
  if (!simpleConnector.connectRobot(&robot))
  {
    ArLog::log(ArLog::Normal, "Could not connect to robot... exiting");
    Aria::exit(3);
  }

  // Set up a class that'll put the movement parameters into the config stuff
  ArRobotConfig robotConfig(&robot);
  robotConfig.addAnalogGyro(&gyro);

  robot.enableMotors();
  robot.clearDirectMotion();

  // if we are connected to a simulator, reset it to its start position
  robot.comInt(ArCommands::RESETSIMTOORIGIN, 1);

#ifndef SONARNL
  // Set up the laser before handing it to the robot.
  simpleConnector.setupLaser(&sick);
#endif // ifndef SONARNL


  // Start the robot thread.
  robot.runAsync(true);

  // Start the laser thread.
  sick.runAsync();

  robot.com2Bytes(31, 11, 0); 
  ArUtil::sleep(1000);
  robot.com2Bytes(31, 11, 1);

  // Connect the laser
  if (!sick.blockingConnect())
  {
    ArLog::log(ArLog::Normal, "Couldn't connect to sick, exiting");
    Aria::exit(4);
  }

  ArUtil::sleep(300);

  // Add additional range devices to the robot and path planning task.
  // IRs
  robot.lock();

  // Bumpers.
  ArBumpers bumpers;
  robot.addRangeDevice(&bumpers);
 
  // Create objects that add network services:
  
  // Drawing in the map display:
  ArServerInfoDrawings drawings(&server);
  drawings.addRobotsRangeDevices(&robot);

  ArServerHandlerCommands commands(&server);

  // These provide various kinds of information to the client:
  ArServerInfoRobot serverInfoRobot(&server, &robot);
//   ArServerInfoSensor serverInfoSensor(&server, &robot);
  ArServerExtInfoRobot serverExtInfoRobot(&server, &robot);

  // Provide the map to the client (and related controls):
  // This uses both lines and points now, since everything except
  // sonar localization uses both (path planning with sonar still uses both)
  ArServerHandlerMap serverMap(&server, &arMap);

  

  // Add some simple (custom) commands for testing and debugging:
  ArServerSimpleComUC uCCommands(&commands, &robot);                   // Send any command to the microcontroller
  ArServerSimpleComMovementLogging loggingCommands(&commands, &robot); // configure logging
  ArServerSimpleComGyro gyroCommands(&commands, &robot, &gyro);        // monitor the gyro
  ArServerSimpleComLogRobotConfig configCommands(&commands, &robot);   // trigger logging of the robot config parameters
  ArServerSimpleServerCommands serverCommands(&commands, &server);     // monitor networking behavior (track packets sent etc.)

  // Mode To stop and remain stopped:
  ArServerModeStop modeStop(&server, &robot);

  ArSonarAutoDisabler sonarAutoDisabler(&robot);

  // Teleoperation modes To drive by keyboard, joystick, etc:
  ArServerModeRatioDrive modeRatioDrive(&server, &robot);  // New, improved mode
  ArServerModeDrive modeDrive(&server, &robot);            // Older mode for compatability

  // Drive mode's configuration and custom (simple) commands:
  modeRatioDrive.addToConfig(Aria::getConfig(), "Teleop settings");
  modeDrive.addControlCommands(&commands);
  modeRatioDrive.addControlCommands(&commands);

  // Wander mode (also prevent wandering if lost):
//   ArServerModeWander modeWander(&server, &robot);
//   SensoryTask sensorySyncTask(&robot, &drawings);
//   MyServerModeWander modeWander(&server, &robot, &sensorySyncTask);

 ArServerModeReactive modeReactiveWander(&server, &robot);

//   ArServerModeWander modeWander(&server, &robot);
//   ArActionGroup* wanderAction = modeWander.getActionGroup();
//   std::list< ArAction * > listActions = *(wanderAction->getActionList());
//   std::list<ArAction *>::iterator it;
//   for (it = listActions.begin(); it != listActions.end(); it++) {
//     const char* acName = (*it)->getName();
//     if(strcmp(acName,"Constant Velocity")==0) {
//       int priority = 50;
//       int vel = 500;
//       wanderAction->remAction((*it));
//       wanderAction->addAction(new ArActionConstantVelocity("Constant Velocity", vel), priority);
//     }
//   }

  // This provides a small table of interesting information for the client
  // to display to the operator:
  ArServerInfoStrings stringInfo(&server);
  Aria::getInfoGroup()->addAddStringCallback(stringInfo.getAddStringFunctor());

  Aria::getInfoGroup()->addStringInt(
    "Laser Packet Count", 10, 
    new ArRetFunctorC<int, ArSick>(&sick, 
           &ArSick::getSickPacCount));

  Aria::getInfoGroup()->addStringInt(
    "Motor Packet Count", 10, 
    new ArConstRetFunctorC<int, ArRobot>(&robot, 
                 &ArRobot::getMotorPacCount));

  // Make Stop mode the default (If current mode deactivates without entering
  // a new mode, then Stop Mode will be selected)
  modeStop.addAsDefaultMode();

#ifdef WIN32
  // these server file things don't work under windows yet
  ArLog::log(ArLog::Normal, "Note, file upload/download services are not implemented for Windows; not enabling them.");
#else
  // This block will allow you to set up where you get and put files
  // to/from, just comment them out if you don't want this to happen
  // /*
  ArServerFileLister fileLister(&server, fileDir);
  ArServerFileToClient fileToClient(&server, fileDir);
  ArServerFileFromClient fileFromClient(&server, fileDir, "/tmp");
  ArServerDeleteFileOnServer deleteFileOnServer(&server, fileDir);
  // */
#endif


  // Create the service that allows the client to monitor the communication 
  // between the robot and the client.
  //
  ArServerHandlerCommMonitor handlerCommMonitor(&server);

  // Create service that allows client to change configuration parameters in ArConfig 
  ArServerHandlerConfig handlerConfig(&server, Aria::getConfig(),
              Arnl::getTypicalDefaultParamFileName(),
              Aria::getDirectory());


  
  // Read in parameter files.
  Aria::getConfig()->useArgumentParser(&parser);
  if (!Aria::getConfig()->parseFile(Arnl::getTypicalParamFileName()))
  {
    ArLog::log(ArLog::Normal, "Trouble loading configuration file, exiting");
    Aria::exit(5);
  }

  // Warn about unknown params.
  if (!simpleOpener.checkAndLog() || !parser.checkHelpAndWarnUnparsed())
  {
    ArLog::log(ArLog::Normal, "\nUsage: %s -map mapfilename\n", argv[0]);
    simpleConnector.logOptions();
    simpleOpener.logOptions();
    Aria::exit(6);
  }

  if (arMap.getFileName() == NULL || strlen(arMap.getFileName()) <= 0)
  {
    ArLog::log(ArLog::Normal, "");
    ArLog::log(ArLog::Normal, "### No map file is set up ###");
  }

  /* Finally, sensorySyncTaskt ready to run the robot: */
  robot.setDirectMotionPrecedenceTime(0);

  robot.unlock();
  
  // Let the client switch manager spin off into its own thread
  clientSwitch.runAsync();

  // Now let it spin off in its own thread
  server.runAsync();

  // Add a key handler (mostly so that on windows you can exit by pressing
  // escape.) This key handler, however, prevents this program from
  // running in the background (e.g. as a system daemon or run from 
  // the shell with "&") -- it will lock up trying to read the keys; 
  // remove this if you wish to be able to run this program in the background.
  ArKeyHandler *keyHandler;
  if ((keyHandler = Aria::getKeyHandler()) == NULL)
  {
    keyHandler = new ArKeyHandler;
    Aria::setKeyHandler(keyHandler);
    robot.lock();
    robot.attachKeyHandler(keyHandler);
    robot.unlock();
    printf("To exit, press escape.\n");
  }
  
  modeReactiveWander.activate();

  robot.waitForRunExit();
  Aria::exit(0);
}
