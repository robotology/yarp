//# @author Luca Tricerri <luca.tricerri@iit.it>
#include "../linux/InterfaceForCApi.h"
#include "../linux/UltraPythonCameraHelper.h"
#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "CApiMock.h"

#include <chrono>
#include <thread>

using namespace std::chrono_literals;
using namespace testing;

TEST(UltraPython, setGainOk) {
     // given
    InterfaceFoCApiMock *interface = new InterfaceFoCApiMock();
    UltraPythonCameraHelper helper(interface);

    struct v4l2_control control;
    control.id=V4L2_CID_GAIN;
    control.value=2;
    EXPECT_CALL(*interface, ioctl_control_c(_,VIDIOC_S_CTRL,_)).Times(2);

    //when
    helper.setControl(V4L2_CID_GAIN,2,false);
}

TEST(UltraPython, setGainProof) {
     // given
    InterfaceFoCApiMock *interface = new InterfaceFoCApiMock();
    UltraPythonCameraHelper helper(interface);

    struct v4l2_control control;
    control.id=V4L2_CID_GAIN;
    control.value=155;
    EXPECT_CALL(*interface, ioctl_control_c(_,VIDIOC_S_CTRL,control)).Times(4);

    //when
    helper.setControl(V4L2_CID_GAIN,2,false);
}
 