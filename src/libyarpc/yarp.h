// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2008 Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the GNU GPL v2.0.
 *
 */

#ifndef YET_ANOTHER_ROBOT_PLATFORM_INC
#define YET_ANOTHER_ROBOT_PLATFORM_INC

#define YARP_DECLARE(rt) rt

#ifdef __cplusplus
extern "C" {
#endif

  typedef struct yarpBottleStruct {
    void *implementation;
  } yarpPort;
  typedef yarpBottle *yarpBottlePtr;

  typedef struct yarpPortStruct {
    void *implementation;
  } yarpPort;
  typedef yarpPort *yarpPortPtr;

  YARP_DECLARE(yarpPortPtr) yarpPortCreate();
  YARP_DECLARE(void) yarpPortDestroy(yarpPortPtr port);
  YARP_DECLARE(int) yarpPortOpen(yarpPortPtr port, const char *name);
  YARP_DECLARE(int) yarpPortClose(yarpPortPtr port);

#ifdef __cplusplus
}
#endif

#endif


