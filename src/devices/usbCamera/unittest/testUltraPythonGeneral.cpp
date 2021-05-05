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

#include "../linux/InterfaceForCApi.h"
#include "../linux/UltraPythonCameraHelper.h"
#include "CApiMock.h"

#include "gmock/gmock.h"
#include "gtest/gtest.h"

#include <chrono>
#include <thread>

using namespace std::chrono_literals;
using namespace testing;

TEST(UltraPython, openAll_ok_002) {
  // given
  InterfaceFoCApiMock *interface = new InterfaceFoCApiMock();
  UltraPythonCameraHelper helper(interface);
  EXPECT_CALL(*interface, open_c("/dev/media0", O_RDWR)).Times(1);
  EXPECT_CALL(*interface, open_c("video1", O_RDWR | O_NONBLOCK, 0)).Times(1);
  EXPECT_CALL(*interface, open_c("video2", O_RDWR | O_NONBLOCK, 0)).Times(1);
  EXPECT_CALL(*interface, open_c("video3", O_RDWR | O_NONBLOCK, 0)).Times(1);
  EXPECT_CALL(*interface, open_c("video4", O_RDWR | O_NONBLOCK, 0)).Times(1);
  EXPECT_CALL(*interface, open_c("video5", O_RDWR | O_NONBLOCK, 0)).Times(1);
  EXPECT_CALL(*interface, open_c("video6", O_RDWR | O_NONBLOCK, 0)).Times(1);
  EXPECT_CALL(*interface, open_c("video7", O_RDWR | O_NONBLOCK, 0)).Times(1);
  EXPECT_CALL(*interface, open_c("video8", O_RDWR | O_NONBLOCK, 0)).Times(1);
  EXPECT_CALL(*interface, open_c("video9", O_RDWR | O_NONBLOCK, 0)).Times(1);
  EXPECT_CALL(*interface, udev_new_c()).WillOnce(Return(new udev()));
  struct media_entity_desc info1;
  strcpy(info1.name, "vcap_python output 0");
  struct media_entity_desc info2;
  strcpy(info2.name, "PYTHON1300");
  struct media_entity_desc info3;
  strcpy(info3.name, "v_tpg");
  struct media_entity_desc info4;
  strcpy(info4.name, "v_proc_ss");
  struct media_entity_desc info5;
  strcpy(info5.name, "Packet32");
  struct media_entity_desc info6;
  strcpy(info6.name, "imgfusion");
  struct media_entity_desc info7;
  strcpy(info7.name, "PYTHON1300_RXIF");
  EXPECT_CALL(*interface, ioctl_media_c(_, _, _))
      .WillOnce(DoAll(SetArgReferee<2>(info1), Return(1)))
      .WillOnce(DoAll(SetArgReferee<2>(info2), Return(1)))
      .WillOnce(DoAll(SetArgReferee<2>(info2), Return(1)))
      .WillOnce(DoAll(SetArgReferee<2>(info3), Return(1)))
      .WillOnce(DoAll(SetArgReferee<2>(info4), Return(1)))
      .WillOnce(DoAll(SetArgReferee<2>(info5), Return(1)))
      .WillOnce(DoAll(SetArgReferee<2>(info6), Return(1)))
      .WillOnce(DoAll(SetArgReferee<2>(info7), Return(1)))
      .WillOnce(DoAll(SetArgReferee<2>(info7), Return(1)))
      .WillOnce(Return(-1));

  EXPECT_CALL(*interface, xioctl(_, _, _)).WillRepeatedly(Return(1));
  EXPECT_CALL(*interface, makedev_c(_, _))
      .WillOnce(Return(1))
      .WillOnce(Return(1))
      .WillOnce(Return(1))
      .WillOnce(Return(1))
      .WillOnce(Return(1))
      .WillOnce(Return(1))
      .WillOnce(Return(1))
      .WillOnce(Return(1))
      .WillOnce(Return(1));
  EXPECT_CALL(*interface, udev_device_new_from_devnum_c(_, _, _))
      .WillOnce(Return(new udev_device()))
      .WillOnce(Return(new udev_device()))
      .WillOnce(Return(new udev_device()))
      .WillOnce(Return(new udev_device()))
      .WillOnce(Return(new udev_device()))
      .WillOnce(Return(new udev_device()))
      .WillOnce(Return(new udev_device()))
      .WillOnce(Return(new udev_device()))
      .WillOnce(Return(new udev_device()));
  EXPECT_CALL(*interface, udev_device_get_devnode_c(_))
      .WillOnce(Return("video1"))
      .WillOnce(Return("video2"))
      .WillOnce(Return("video3"))
      .WillOnce(Return("video4"))
      .WillOnce(Return("video5"))
      .WillOnce(Return("video6"))
      .WillOnce(Return("video7"))
      .WillOnce(Return("video8"))
      .WillOnce(Return("video9"));
  EXPECT_CALL(*interface, udev_device_unref_c(_))
      .WillOnce(Return(new udev_device()))
      .WillOnce(Return(new udev_device()))
      .WillOnce(Return(new udev_device()))
      .WillOnce(Return(new udev_device()))
      .WillOnce(Return(new udev_device()))
      .WillOnce(Return(new udev_device()))
      .WillOnce(Return(new udev_device()))
      .WillOnce(Return(new udev_device()))
      .WillOnce(Return(new udev_device()));
  int tmp{1};
  EXPECT_CALL(*interface, mmap_c(_, _, _, _, _, _))
      .WillRepeatedly(Return((void *)&tmp));

  // when
  bool out = helper.openAll();

  // then
  EXPECT_TRUE(out);

  delete interface;
}

// Missing main device
TEST(UltraPython, openAll_ko_000) {
  // given
  InterfaceFoCApiMock *interface = new InterfaceFoCApiMock();
  UltraPythonCameraHelper helper(interface);
  EXPECT_CALL(*interface, open_c("/dev/media0", O_RDWR)).WillOnce(Return(-1));

  // when
  bool out = helper.openAll();

  // then
  EXPECT_FALSE(out);

  delete interface;
}

// Missing sub device
TEST(UltraPython, openAll_ko_001) {
  // given
  InterfaceFoCApiMock *interface = new InterfaceFoCApiMock();
  UltraPythonCameraHelper helper(interface);
  EXPECT_CALL(*interface, open_c("/dev/media0", O_RDWR)).Times(1);
  EXPECT_CALL(*interface, udev_new_c()).WillOnce(Return(new udev()));
  EXPECT_CALL(*interface, open_c("video1", O_RDWR | O_NONBLOCK, 0)).Times(1);
  EXPECT_CALL(*interface, open_c("video2", O_RDWR | O_NONBLOCK, 0)).Times(1);

  struct media_entity_desc info1;
  strcpy(info1.name, "vcap_python output 0");
  struct media_entity_desc info2;
  strcpy(info2.name, "PYTHON1300");
  EXPECT_CALL(*interface, ioctl_media_c(_, _, _))
      .WillOnce(DoAll(SetArgReferee<2>(info1), Return(1)))
      .WillOnce(DoAll(SetArgReferee<2>(info2), Return(1)))
      .WillOnce(Return(-1));
  EXPECT_CALL(*interface, xioctl(_, _, _)).WillRepeatedly(Return(1));
  EXPECT_CALL(*interface, makedev_c(_, _))
      .WillOnce(Return(1))
      .WillOnce(Return(1));
  EXPECT_CALL(*interface, udev_device_new_from_devnum_c(_, _, _))
      .WillOnce(Return(new udev_device()))
      .WillOnce(Return(new udev_device()));
  EXPECT_CALL(*interface, udev_device_get_devnode_c(_))
      .WillOnce(Return("video1"))
      .WillOnce(Return("video2"));
  EXPECT_CALL(*interface, udev_device_unref_c(_))
      .WillOnce(Return(new udev_device()))
      .WillOnce(Return(new udev_device()));
  int tmp{1};
  EXPECT_CALL(*interface, mmap_c(_, _, _, _, _, _))
      .WillRepeatedly(Return((void *)&tmp));

  // when
  bool out = helper.openAll();

  // then
  EXPECT_FALSE(out);

  delete interface;
}

TEST(UltraPython, log_ok_001) {
  // given
  InterfaceFoCApiMock *interface = new InterfaceFoCApiMock();
  UltraPythonCameraHelper helper(interface);
  {
    helper.setInjectedLog([](const std::string &str, Severity severity) {
      EXPECT_EQ("::~UltraPythonCameraHelper", str);
      EXPECT_TRUE(severity == Severity::debug);
      return str;
    });

    // when

    delete interface;
  }
}
