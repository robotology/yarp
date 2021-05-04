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

TEST(UltraPython, setExposition_absolute_ok) {
  // given
  InterfaceFoCApiMock *interface = new InterfaceFoCApiMock();
  UltraPythonCameraHelper helper(interface);
  helper.setStepPeriod(100);

  struct v4l2_control control1;
  control1.id = UltraPythonCameraHelper::V4L2_EXPOSURE_ULTRA_PYTHON;
  control1.value = 20;
  EXPECT_CALL(*interface, ioctl_query_c(_, _, _)).WillOnce(Return(1));
  EXPECT_CALL(*interface, ioctl_control_c(_, VIDIOC_S_CTRL, control1)).Times(1);

  // when
  bool res = helper.setControl(V4L2_CID_EXPOSURE, 20, true);

  // then
  EXPECT_TRUE(res);

  delete interface;
}
TEST(UltraPython, setExposition_absolute_honoryes_ok) {
  // given
  InterfaceFoCApiMock *interface = new InterfaceFoCApiMock();
  UltraPythonCameraHelper helper(interface);
  helper.setStepPeriod(90);
  helper.setHonorFps(true);

  struct v4l2_control control1;
  control1.id = UltraPythonCameraHelper::V4L2_EXPOSURE_ULTRA_PYTHON;
  control1.value = 20;
  EXPECT_CALL(*interface, ioctl_query_c(_, _, _)).WillOnce(Return(1));
  EXPECT_CALL(*interface, ioctl_control_c(_, VIDIOC_S_CTRL, control1)).Times(1);

  // when
  bool res = helper.setControl(V4L2_CID_EXPOSURE, 20, true);

  // then
  EXPECT_TRUE(res);

  delete interface;
}

TEST(UltraPython, setExposition_absolute_honoryes_ko) {
  // given
  InterfaceFoCApiMock *interface = new InterfaceFoCApiMock();
  UltraPythonCameraHelper helper(interface);
  helper.setStepPeriod(20);
  helper.setHonorFps(true);

  struct v4l2_control control1;
  control1.id = UltraPythonCameraHelper::V4L2_EXPOSURE_ULTRA_PYTHON;
  control1.value = 20;
  EXPECT_CALL(*interface, ioctl_query_c(_, _, _)).WillOnce(Return(1));
  EXPECT_CALL(*interface, ioctl_control_c(_, VIDIOC_S_CTRL, control1)).Times(0);

  // when
  bool res = helper.setControl(V4L2_CID_EXPOSURE, 20, true);

  // then
  EXPECT_FALSE(res);

  delete interface;
}

TEST(UltraPython, setExposition_absolute_honorno_ok) {
  // given
  InterfaceFoCApiMock *interface = new InterfaceFoCApiMock();
  UltraPythonCameraHelper helper(interface);
  helper.setStepPeriod(20);
  helper.setHonorFps(false);

  struct v4l2_control control1;
  control1.id = UltraPythonCameraHelper::V4L2_EXPOSURE_ULTRA_PYTHON;
  control1.value = 20;
  EXPECT_CALL(*interface, ioctl_query_c(_, _, _)).WillOnce(Return(1));
  EXPECT_CALL(*interface, ioctl_control_c(_, VIDIOC_S_CTRL, control1)).Times(1);

  // when
  bool res = helper.setControl(V4L2_CID_EXPOSURE, 20, true);

  // then
  EXPECT_TRUE(res);

  delete interface;
}

TEST(UltraPython, setExposition_absolute_negative) {
  // given
  InterfaceFoCApiMock *interface = new InterfaceFoCApiMock();
  UltraPythonCameraHelper helper(interface);
  helper.setStepPeriod(100);

  struct v4l2_control control1;
  control1.id = UltraPythonCameraHelper::V4L2_EXPOSURE_ULTRA_PYTHON;
  control1.value = -20;
  EXPECT_CALL(*interface, ioctl_query_c(_, _, _)).Times(0);
  EXPECT_CALL(*interface, ioctl_control_c(_, VIDIOC_S_CTRL, control1)).Times(0);

  // when
  bool res = helper.setControl(V4L2_CID_EXPOSURE, -20, true);

  // then
  EXPECT_FALSE(res);

  delete interface;
}

TEST(UltraPython, setExposition_normalized_ok) {
  // given
  InterfaceFoCApiMock *interface = new InterfaceFoCApiMock();
  UltraPythonCameraHelper helper(interface);
  helper.setStepPeriod(100);

  struct v4l2_control control1;
  control1.id = UltraPythonCameraHelper::V4L2_EXPOSURE_ULTRA_PYTHON;
  control1.value = 57;
  EXPECT_CALL(*interface, ioctl_query_c(_, _, _)).WillOnce(Return(1));
  EXPECT_CALL(*interface, ioctl_control_c(_, VIDIOC_S_CTRL, control1)).Times(1);

  // when
  bool res = helper.setControl(V4L2_CID_EXPOSURE, 0.5, false);

  // then
  EXPECT_TRUE(res);

  delete interface;
}

TEST(UltraPython, setExposition_normalized_honoryes_ok) {
  // given
  InterfaceFoCApiMock *interface = new InterfaceFoCApiMock();
  UltraPythonCameraHelper helper(interface);
  helper.setStepPeriod(90);
  helper.setHonorFps(true);

  struct v4l2_control control1;
  control1.id = UltraPythonCameraHelper::V4L2_EXPOSURE_ULTRA_PYTHON;
  control1.value = 57;
  EXPECT_CALL(*interface, ioctl_query_c(_, _, _)).WillOnce(Return(1));
  EXPECT_CALL(*interface, ioctl_control_c(_, VIDIOC_S_CTRL, control1)).Times(1);

  // when
  bool res = helper.setControl(V4L2_CID_EXPOSURE, 0.5, false);

  // then
  EXPECT_TRUE(res);

  delete interface;
}

TEST(UltraPython, setExposition_normalized_honoryes_k0) {
  // given
  InterfaceFoCApiMock *interface = new InterfaceFoCApiMock();
  UltraPythonCameraHelper helper(interface);
  helper.setStepPeriod(30);
  helper.setHonorFps(true);

  struct v4l2_control control1;
  control1.id = UltraPythonCameraHelper::V4L2_EXPOSURE_ULTRA_PYTHON;
  control1.value = 57;
  EXPECT_CALL(*interface, ioctl_query_c(_, _, _)).WillOnce(Return(1));
  EXPECT_CALL(*interface, ioctl_control_c(_, VIDIOC_S_CTRL, control1)).Times(0);

  // when
  bool res = helper.setControl(V4L2_CID_EXPOSURE, 0.5, false);

  // then
  EXPECT_FALSE(res);

  delete interface;
}

TEST(UltraPython, setExposition_normalized_negative) {
  // given
  InterfaceFoCApiMock *interface = new InterfaceFoCApiMock();
  UltraPythonCameraHelper helper(interface);
  helper.setStepPeriod(100);

  struct v4l2_control control1;
  control1.id = UltraPythonCameraHelper::V4L2_EXPOSURE_ULTRA_PYTHON;
  control1.value = -57;
  EXPECT_CALL(*interface, ioctl_query_c(_, _, _)).Times(0);
  EXPECT_CALL(*interface, ioctl_control_c(_, VIDIOC_S_CTRL, control1)).Times(0);

  // when
  bool res = helper.setControl(V4L2_CID_EXPOSURE, -0.5, false);

  // then
  EXPECT_FALSE(res);

  delete interface;
}