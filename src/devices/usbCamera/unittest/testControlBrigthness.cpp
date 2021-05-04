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

TEST(UltraPython, setBrithness_absolute_ok) {
  // given
  InterfaceFoCApiMock *interface = new InterfaceFoCApiMock();
  UltraPythonCameraHelper helper(interface);
  helper.setStepPeriod(100);

  struct v4l2_control control1;
  control1.id = V4L2_CID_BRIGHTNESS;
  control1.value = 20;
  EXPECT_CALL(*interface, ioctl_query_c(_, _, _))
      .WillOnce(Return(1))
      .WillOnce(Return(1));
  EXPECT_CALL(*interface, ioctl_control_c(_, VIDIOC_S_CTRL, control1)).Times(2);

  // when
  bool res = helper.setControl(V4L2_CID_BRIGHTNESS, 20, true);

  // then
  EXPECT_TRUE(res);

  delete interface;
}

TEST(UltraPython, setBrithness_relative_ok) {
  // given
  InterfaceFoCApiMock *interface = new InterfaceFoCApiMock();
  UltraPythonCameraHelper helper(interface);
  helper.setStepPeriod(100);

  struct v4l2_queryctrl queryctrl;
  queryctrl.maximum = 0;
  queryctrl.minimum = 100;
  struct v4l2_control control1;
  control1.id = V4L2_CID_BRIGHTNESS;
  control1.value = 50;
  EXPECT_CALL(*interface, ioctl_query_c(_, VIDIOC_QUERYCTRL, _))
      .WillOnce(DoAll(SetArgReferee<2>(queryctrl), Return(1)))
      .WillOnce(DoAll(SetArgReferee<2>(queryctrl), Return(1)));
  EXPECT_CALL(*interface, ioctl_control_c(_, VIDIOC_S_CTRL, control1)).Times(2);

  // when
  bool res = helper.setControl(V4L2_CID_BRIGHTNESS, 0.50, false);

  // then
  EXPECT_TRUE(res);

  delete interface;
}