/*
 * Copyright (C) 2006-2021 Istituto Italiano di Tecnologia (IIT)
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301 USA
 */

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
  buf.bytesused = 10000;
  EXPECT_CALL(*interface, select_c(_, _, nullptr, nullptr, _))
      .WillOnce(Return(1));
  EXPECT_CALL(*interface, xioctl_v4l2(_, VIDIOC_DQBUF, _))
      .WillOnce(DoAll(SetArgPointee<2>(buf), Return(1)));
  EXPECT_CALL(*interface, xioctl(_, VIDIOC_QBUF, _)).WillOnce(Return(1));

  // when
  unsigned char yarpbuffer[10000000];
  bool out = helper.step(yarpbuffer);

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
  buf.bytesused = 10000;
  EXPECT_CALL(*interface, select_c(_, _, nullptr, nullptr, _))
      .WillOnce(Return(1));
  EXPECT_CALL(*interface, xioctl_v4l2(_, VIDIOC_DQBUF, _))
      .WillOnce(DoAll(SetArgPointee<2>(buf), Return(1)));
  EXPECT_CALL(*interface, xioctl(_, VIDIOC_QBUF, _)).Times(0);

  // when
  unsigned char yarpbuffer[10000000];
  bool out = helper.step(yarpbuffer);

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
  unsigned char yarpbuffer[10000000];
  bool out = helper.step(yarpbuffer);

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
  unsigned char yarpbuffer[10000000];
  bool out = helper.step(yarpbuffer);

  // then
  EXPECT_FALSE(out);

  delete interface;
}
