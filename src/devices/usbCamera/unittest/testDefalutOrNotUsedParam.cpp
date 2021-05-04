/*
 * Copyright (C) 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */
//# @author Luca Tricerri <luca.tricerri@iit.it>

#include "../linux/UltraPythonCameraHelper.h"
#include "CApiMock.h"
#include "gmock/gmock.h"
#include "gtest/gtest.h"

#include <chrono>
#include <linux/v4l2-controls.h>
#include <thread>

using namespace std::chrono_literals;
using namespace testing;

TEST(UltraPython, notusedparams_ok) {
  // given
  InterfaceFoCApiMock *interface = new InterfaceFoCApiMock();
  UltraPythonCameraHelper helper(interface);

  // when
  bool force = helper.getForceFormatProperty();
  bool crop = helper.getCropEnabledProperty();
  bool autoCtrl = helper.hasAutoControl(1);
  bool honor = helper.getHonorFps();
  double step = helper.getStepPeriod();

  // then
  EXPECT_FALSE(crop);
  EXPECT_TRUE(force);
  EXPECT_FALSE(autoCtrl);
  EXPECT_FALSE(honor);
  EXPECT_EQ(40,step);

  delete interface;
}

TEST(UltraPython, hasControls_ok) {
  // given
  InterfaceFoCApiMock *interface = new InterfaceFoCApiMock();
  UltraPythonCameraHelper helper(interface);

  // when
  bool ret1 = helper.hasControl(V4L2_CID_GAIN);
  bool ret2 = helper.hasControl(V4L2_CID_BRIGHTNESS);
  bool ret3 = helper.hasControl(UltraPythonCameraHelper::V4L2_ANALOGGAIN_ULTRA_PYTHON);
  bool ret4 = helper.hasControl(UltraPythonCameraHelper::V4L2_EXTTRIGGGER_ULTRA_PYTHON);
  bool ret5 = helper.hasControl(UltraPythonCameraHelper::V4L2_REDBALANCE_ULTRA_PYTHON);
  bool ret6 = helper.hasControl(UltraPythonCameraHelper::V4L2_DEADTIME_ULTRA_PYTHON);
  bool ret7 = helper.hasControl(UltraPythonCameraHelper::V4L2_EXPOSURE_ULTRA_PYTHON);

  // then
  EXPECT_TRUE(ret1);
  EXPECT_TRUE(ret2);
  EXPECT_TRUE(ret3);
  EXPECT_TRUE(ret4);
  EXPECT_TRUE(ret5);
  EXPECT_TRUE(ret6);
  EXPECT_TRUE(ret7);

  delete interface;
}