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
      .WillOnce(Return(1)).WillOnce(Return(1));
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

  struct v4l2_control control1;
  control1.id = V4L2_CID_BRIGHTNESS;
  control1.value = 0.5;
  EXPECT_CALL(*interface, ioctl_query_c(_, _, _))
      .WillOnce(Return(1)).WillOnce(Return(1));
  EXPECT_CALL(*interface, ioctl_control_c(_, VIDIOC_S_CTRL, control1)).Times(2);

  // when
  bool res = helper.setControl(V4L2_CID_BRIGHTNESS, 0.5, false);

  // then
  EXPECT_TRUE(res);

  delete interface;
}