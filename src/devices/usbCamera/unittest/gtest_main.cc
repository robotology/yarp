//# @author Luca Tricerri <luca.tricerri@iit.it> 

#include "gtest/gtest.h"

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);

    ::testing::GTEST_FLAG(filter) = "UltraPython.setGainOk*";
    //::testing::GTEST_FLAG(filter) = "Test.Test_003";
    //::testing::GTEST_FLAG(filter) = "Test.Test_004";
    return RUN_ALL_TESTS();
}