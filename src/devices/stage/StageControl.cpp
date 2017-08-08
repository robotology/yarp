/*
 * Copyright (C) 2007 RobotCub Consortium
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#include "StageControl.h"

#include <cstdio>
#include <cstdlib>
#include <cstring>

#include <yarp/os/Time.h>

using namespace yarp::dev;
using namespace yarp::os;

bool StageControl::open(yarp::os::Searchable& config) {
  ConstString worldFile = config.check("world",Value(""),
                                       "stage world file").asString();
  ConstString robotName = config.check("robot",Value(""),
                                       "robot name").asString();
  if (worldFile=="") {
    printf("Please specify a world file\n");
    return false;
  }
  if (robotName=="") {
    printf("Please specify a robot name\n");
    return false;
  }

  int argc = 3;
  char *argv[] = { "fake",
                   (char*)worldFile.c_str(),
                   (char*)robotName.c_str()
  };

  // initialize libstage
  stg_init( argc, argv );

  world = stg_world_create_from_file( argv[1] );

  char* robotname = argv[2];

  // generate the name of the laser attached to the robot
  char lasername[64];
  snprintf( lasername, 63, "%s.laser:0", robotname );

  char sonarname[64];
  snprintf( sonarname, 63, "%s.ranger:0", robotname );

  position = stg_world_model_name_lookup( world, robotname );
  laser = stg_world_model_name_lookup( world, lasername );

  // subscribe to the laser - starts it collecting data
  stg_model_subscribe( laser );
  stg_model_subscribe( position);

  stg_model_print( position, "Subscribed to model" );
  stg_model_print( laser, "Subscribed to model" );

  printf( "Starting world clock..." ); fflush(stdout);
  // start the clock
  stg_world_start( world );
  puts( "done" );

  // really should close too :-)

  stg_world_set_interval_real( world, 0 );

  start();

  return true;
}


bool StageControl::close() {
  stop();
  mutex.wait();
  if (world!=NULL) {
    stg_world_destroy( world );
    world = NULL;
  }
  mutex.post();
  return true;
}


bool StageControl::velocityMove(const double *v) {
  mutex.wait();
  printf("Velocity move...\n");
  stg_position_cmd_t cmd;
  memset(&cmd,0,sizeof(cmd));
  cmd.x = v[0];
  cmd.y = v[1];
  cmd.a = v[2];
  for (int i=0; i<3; i++) {
    setpoint[i] = v[i];
  }

  stg_model_set_cmd( position, &cmd, sizeof(cmd));
  mutex.post();

  return true;
}



// how to do a velocity command:
// echo "([vmos]) (1.0 2.0 3.0)" | yarp write /junk /controlboard/command:i


void StageControl::run() {
  while (!isStopping()) {
    mutex.wait();
    int result = stg_world_update( world,0 );
    mutex.post();
    printf("Tick...\n");
    if (result!=0) break;
    SystemClock::delaySystem(0.05);
  }
  mutex.wait();
  if (world!=NULL) {
    stg_world_destroy( world );
    world = NULL;
  }
  mutex.post();
}


