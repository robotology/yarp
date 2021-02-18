//# @author Luca Tricerri <luca.tricerri@iit.it>
#include "../linux/InterfaceForCFunction.h"
#include "../linux/PythonCameraHelper.h"
#include "gtest/gtest.h"
#include "gmock/gmock.h"

using namespace testing;
using ::testing::_;
using ::testing::InvokeArgument;
using ::testing::Matcher;


class InterfaceFoCFunctionMock : public InterfaceForCFunction
{
public:
    MOCK_METHOD(void, open_d, (), (override));
    //MOCK_METHOD(int, open_c, (const char*, int), (override));
};

TEST(UltraPythonTables, 001)
{
    // given
    PythonCameraHelper helper;
    InterfaceFoCFunctionMock interface;
    //EXPECT_CALL(interface, open_c(_, _)).Times(1);

    // when
    helper.openAll();
}