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

TEST(UltraPython, step_base_ok) {
  // given
  InterfaceFoCApiMock *interface = new InterfaceFoCApiMock();
  UltraPythonCameraHelper helper(interface);

  struct v4l2_buffer buf;
  buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
  buf.memory = V4L2_MEMORY_MMAP;
  buf.index = 1;
  buf.flags = 0;
  buf.sequence = 0;
  buf.bytesused=10000;
  EXPECT_CALL(*interface, select_c(_, _, nullptr, nullptr, _))
      .WillOnce(Return(1));
  EXPECT_CALL(*interface, xioctl_v4l2(_, VIDIOC_DQBUF, _)).WillOnce(DoAll(SetArgPointee<2>(buf), Return(1)));
  EXPECT_CALL(*interface, xioctl(_, VIDIOC_QBUF, _)).WillOnce(Return(1));
  
  // when
  bool out = helper.step();

  // then
  EXPECT_TRUE(out);

  delete interface;
}

TEST(UltraPython, step_base_error1) {
  // given
  InterfaceFoCApiMock *interface = new InterfaceFoCApiMock();
  UltraPythonCameraHelper helper(interface);

  struct v4l2_buffer buf;
  buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
  buf.memory = V4L2_MEMORY_MMAP;
  buf.index = 1;
  buf.flags = V4L2_BUF_FLAG_ERROR;
  buf.sequence = 0;
  buf.bytesused=10000;
  EXPECT_CALL(*interface, select_c(_, _, nullptr, nullptr, _))
      .WillOnce(Return(1));
  EXPECT_CALL(*interface, xioctl_v4l2(_, VIDIOC_DQBUF, _)).WillOnce(DoAll(SetArgPointee<2>(buf), Return(1)));
  EXPECT_CALL(*interface, xioctl(_, VIDIOC_QBUF, _)).Times(0);
  
  // when
  bool out = helper.step();

  // then
  EXPECT_FALSE(out);

  delete interface;
}

TEST(UltraPython, step_base_timeout_ko) {
  // given
  InterfaceFoCApiMock *interface = new InterfaceFoCApiMock();
  UltraPythonCameraHelper helper(interface);

  EXPECT_CALL(*interface, select_c(_, _, nullptr, nullptr, _))
      .WillOnce(Return(0));

  // when
  bool out = helper.step();

  // then
  EXPECT_FALSE(out);

  delete interface;
}

TEST(UltraPython, step_base_error_ko) {
  // given
  InterfaceFoCApiMock *interface = new InterfaceFoCApiMock();
  UltraPythonCameraHelper helper(interface);

  EXPECT_CALL(*interface, select_c(_, _, nullptr, nullptr, _))
      .WillOnce(Return(-1));

  // when
  bool out = helper.step();

  // then
  EXPECT_FALSE(out);

  delete interface;
}
