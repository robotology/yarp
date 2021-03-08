//# @author Luca Tricerri <luca.tricerri@iit.it>
#include "../linux/InterfaceForCFunction.h"
#include "../linux/PythonCameraHelper.h"
#include "gmock/gmock.h"
#include "gtest/gtest.h"

#include <chrono>
#include <thread>

using namespace std::chrono_literals;
using namespace testing;

// Dummy
struct udev {
  int dummy;
};

struct udev_device {
  int dummy;
};

// Mock
class InterfaceFoCFunctionMock : public InterfaceForCFunction {
public:
  MOCK_METHOD(int, open_c, (const char *, int), (override));
  MOCK_METHOD(int, open_c, (const char *, int, mode_t), (override));
  MOCK_METHOD(struct udev *, udev_new_c, (), (override));
  MOCK_METHOD(int, ioctl_c, (int, int, struct media_entity_desc *), (override));
  MOCK_METHOD(int, xioctl, (int, int, void *), (override));
  MOCK_METHOD(dev_t, makedev_c, (int, int), (override));
  MOCK_METHOD(struct udev_device *, udev_device_new_from_devnum_c,
              (struct udev *, char, dev_t), (override));
  MOCK_METHOD(const char *, udev_device_get_devnode_c, (struct udev_device *),
              (override));
  MOCK_METHOD(struct udev_device *, udev_device_unref_c, (struct udev_device *),
              (override));
  MOCK_METHOD(void *, mmap_c, (void *, size_t, int, int, int, __off_t),
              (override));
};

TEST(UltraPython, openAll_ok_002) {
  // given
  InterfaceFoCFunctionMock *interface = new InterfaceFoCFunctionMock();
  PythonCameraHelper helper(interface);
  EXPECT_CALL(*interface, open_c(_, _)).Times(1);
  EXPECT_CALL(*interface, open_c(_, _, _)).Times(9);
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
  EXPECT_CALL(*interface, ioctl_c(_, _, _))
      .WillOnce(DoAll(SetArgPointee<2>(info1), Return(1)))
      .WillOnce(DoAll(SetArgPointee<2>(info2), Return(1)))
      .WillOnce(DoAll(SetArgPointee<2>(info2), Return(1)))
      .WillOnce(DoAll(SetArgPointee<2>(info3), Return(1)))
      .WillOnce(DoAll(SetArgPointee<2>(info4), Return(1)))
      .WillOnce(DoAll(SetArgPointee<2>(info5), Return(1)))
      .WillOnce(DoAll(SetArgPointee<2>(info6), Return(1)))
      .WillOnce(DoAll(SetArgPointee<2>(info7), Return(1)))
      .WillOnce(DoAll(SetArgPointee<2>(info7), Return(1)))
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
      .WillOnce(Return("c"))
      .WillOnce(Return("c"))
      .WillOnce(Return("c"))
      .WillOnce(Return("c"))
      .WillOnce(Return("c"))
      .WillOnce(Return("c"))
      .WillOnce(Return("c"))
      .WillOnce(Return("c"))
      .WillOnce(Return("c"));
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

TEST(UltraPython, openAll_ko_001) {
  // given
  InterfaceFoCFunctionMock *interface = new InterfaceFoCFunctionMock();
  PythonCameraHelper helper(interface);
  EXPECT_CALL(*interface, open_c(_, _)).Times(1);
  EXPECT_CALL(*interface, open_c(_, _, _)).Times(2);
  EXPECT_CALL(*interface, udev_new_c()).WillOnce(Return(new udev()));
  struct media_entity_desc info1;
  strcpy(info1.name, "vcap_python output 0");
  struct media_entity_desc info2;
  strcpy(info2.name, "PYTHON1300");
  EXPECT_CALL(*interface, ioctl_c(_, _, _))
      .WillOnce(DoAll(SetArgPointee<2>(info1), Return(1)))
      .WillOnce(DoAll(SetArgPointee<2>(info2), Return(1)))
      .WillOnce(Return(-1));
  EXPECT_CALL(*interface, xioctl(_, _, _)).WillRepeatedly(Return(1));
  EXPECT_CALL(*interface, makedev_c(_, _))
      .WillOnce(Return(1))
      .WillOnce(Return(1));
  EXPECT_CALL(*interface, udev_device_new_from_devnum_c(_, _, _))
      .WillOnce(Return(new udev_device()))
      .WillOnce(Return(new udev_device()));
  EXPECT_CALL(*interface, udev_device_get_devnode_c(_))
      .WillOnce(Return("c"))
      .WillOnce(Return("c"));
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
  InterfaceFoCFunctionMock *interface = new InterfaceFoCFunctionMock();
  PythonCameraHelper helper(interface);
  {
    helper.setInjectedLog([](const std::string &str, Severity severity) {
      EXPECT_EQ("::~PythonCameraHelper", str);
      EXPECT_TRUE(severity == Severity::debug);
      return str;
    });

    // when
  }
}
