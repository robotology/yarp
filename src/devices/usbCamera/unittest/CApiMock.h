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

#pragma once

#include "../linux/InterfaceForCApi.h"
#include "gmock/gmock.h"
#include "gtest/gtest.h"

// Dummy
struct udev {
  int dummy;
};

struct udev_device {
  int dummy;
};

// Mock
class InterfaceFoCApiMock : public InterfaceForCApi {
public:
  MOCK_METHOD(int, open_c, (const char *, int), (override));
  MOCK_METHOD(int, open_c, (const char *, int, mode_t), (override));
  MOCK_METHOD(struct udev *, udev_new_c, (), (override));
  MOCK_METHOD(int, ioctl_media_c, (int, int, struct media_entity_desc &),
              (override));
  MOCK_METHOD(int, ioctl_control_c, (int, int, struct v4l2_control &),
              (override));
  MOCK_METHOD(int, ioctl_query_c, (int, int, struct v4l2_queryctrl &),
              (override));
  MOCK_METHOD(int, xioctl, (int, int, void *), (override));
  MOCK_METHOD(int, xioctl_v4l2, (int, int, struct v4l2_buffer *), (override));

  MOCK_METHOD(dev_t, makedev_c, (int, int), (override));
  MOCK_METHOD(struct udev_device *, udev_device_new_from_devnum_c,
              (struct udev *, char, dev_t), (override));
  MOCK_METHOD(const char *, udev_device_get_devnode_c, (struct udev_device *),
              (override));
  MOCK_METHOD(struct udev_device *, udev_device_unref_c, (struct udev_device *),
              (override));
  MOCK_METHOD(void *, mmap_c, (void *, size_t, int, int, int, __off_t),
              (override));
  MOCK_METHOD(int, select_c,
              (int, fd_set *, fd_set *, fd_set *, struct timeval *),
              (override));
  MOCK_METHOD(void *, memcpy_c, (void *, const void *, size_t), (override));
};

// Equal operator
inline bool operator==(const struct v4l2_control &left,
                       const struct v4l2_control &right) {
  if (left.id != right.id)
    return false;
  if (left.value != right.value)
    return false;
  return true;
}
