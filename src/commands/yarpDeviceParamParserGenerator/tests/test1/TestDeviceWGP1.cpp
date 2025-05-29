/*
 * SPDX-FileCopyrightText: 2024-2024 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "TestDeviceWGP1.h"

#include <yarp/os/LogComponent.h>
#include <yarp/os/LogStream.h>

using namespace yarp::os;
using namespace yarp::dev;

TestDeviceWGP1::TestDeviceWGP1()
{
}

TestDeviceWGP1::~TestDeviceWGP1()
{
}

bool TestDeviceWGP1::do_test()
{
    if (m_param_1 != "string")           { yError() << "error m_param_1 "; return false; }
    if (m_param_2 != 1.0)                { yError() << "error m_param_2 "; return false; }
    if (m_param_3 != true)               { yError() << "error m_param_3 "; return false; }
    if (m_param_4 != 1)                  { yError() << "error m_param_4 "; return false; }
    if (m_param_5 != 'a')                { yError() << "error m_param_5 "; return false; }
    if (m_param_6 != 10)                 { yError() << "error m_param_6 "; return false; }
    if (m_param_7 != 1.0)                { yError() << "error m_param_7 "; return false; }
    if (m_group1_param_1 != "def10")     { yError() << "error m_group1_param_1"; return false; }
    if (m_group1_param_2 != 101)         { yError() << "error m_group1_param_2"; return false; }
    if (m_group1_param_3 != true)        { yError() << "error m_group1_param_3"; return false; }
    if (m_group1_param_4 != 1001)        { yError() << "error m_group1_param_4"; return false; }
    if (m_group2_param_1 != "def20")     { yError() << "error m_group2_param_1"; return false; }
    if (m_group2_param_2 != 102)         { yError() << "error m_group2_param_2"; return false; }
    if (m_group2_param_3 != true)        { yError() << "error m_group2_param_3"; return false; }
    if (m_group2_param_4 != 1002)        { yError() << "error m_group2_param_4"; return false; }
    if (m_group3_param_1 != "def1")      { yError() << "error m_group3_param_1"; return false; }
    if (m_group3_param_2 != 1.0)         { yError() << "error m_group3_param_2"; return false; }
    if (m_group3_param_3 != true)        { yError() << "error m_group3_param_3"; return false; }
    if (m_group3_param_4 != 1000)        { yError() << "error m_group3_param_4"; return false; }
    if (m_group4_subgroup1_param_1 != 0) { yError() << "error m_group4_subgroup1_param_1"; return false; }
    if (m_group4_subgroup1_param_2 != 1) { yError() << "error m_group4_subgroup1_param_2"; return false; }
    if (m_group4_subgroup1_param_3 != 2) { yError() << "error m_group4_subgroup1_param_3"; return false; }
    if (m_group4_subgroup1_param_4 != 3) { yError() << "error m_group4_subgroup1_param_4"; return false; }
    if (m_group4_subgroup2_param_1 != 10)  { yError() << "error m_group4_subgroup2_param_1"; return false; }
    if (m_group4_subgroup2_param_2 != 11)  { yError() << "error m_group4_subgroup2_param_2"; return false; }
    if (m_group4_subgroup2_param_3 != 12)  { yError() << "error m_group4_subgroup2_param_3"; return false; }
    if (m_group4_subgroup2_param_4 != 13)  { yError() << "error m_group4_subgroup2_param_4"; return false; }
    if (m_group5_subgroup1_param_1 != 200) { yError() << "error m_group5_subgroup1_param_1"; return false; }
    if (m_group5_subgroup1_param_2 != 210) { yError() << "error m_group5_subgroup1_param_2"; return false; }
    if (m_group5_subgroup1_param_3 != 220) { yError() << "error m_group5_subgroup1_param_3"; return false; }
    if (m_group5_subgroup1_param_4 != 230) { yError() << "error m_group5_subgroup1_param_4"; return false; }
    if (m_group5_subgroup2_param_1 != 300) { yError() << "error m_group5_subgroup2_param_1"; return false; }
    if (m_group5_subgroup2_param_2 != 310) { yError() << "error m_group5_subgroup2_param_2"; return false; }
    if (m_group5_subgroup2_param_3 != 320) { yError() << "error m_group5_subgroup2_param_3"; return false; }
    if (m_group5_subgroup2_param_4 != 330) { yError() << "error m_group5_subgroup2_param_4"; return false; }

    //vec1,2,3 are empty
    {
        if (m_param_vec1.size() != 0) { yError() << "error vec1 size"; return false; }
        if (m_param_vec2.size() != 0) { yError() << "error vec2 size"; return false; }
        if (m_param_vec3.size() != 0) { yError() << "error vec3 size"; return false; }
    }

    {
        if (m_param_vec4.size() != 3) { yError() << "error vec4 size"; return false; }
        if (m_param_vec5.size() != 3) { yError() << "error vec5 size"; return false; }
        if (m_param_vec6.size() != 3) { yError() << "error vec6 size"; return false; }

        if (m_param_vec4[0] != 1 ||
            m_param_vec4[1] != 2 ||
            m_param_vec4[2] != 3) {
            yError() << "error vec4 content"; return false;    }

        if (m_param_vec5[0] != 1 ||
            m_param_vec5[1] != 2 ||
            m_param_vec5[2] != 3)    {
            yError() << "error vec5 content"; return false;    }

        if (m_param_vec6[0] != "sa1" ||
            m_param_vec6[1] != "sa2" ||
            m_param_vec6[2] != "sa3")    {
            yError() << "error vec5 content"; return false;    }
    }

    {
        if (m_param_vec7.size() != 3) { yError() << "error vec7 size"; return false; }
        if (m_param_vec8.size() != 3) { yError() << "error vec8 size"; return false; }
        if (m_param_vec9.size() != 3) { yError() << "error vec9 size"; return false; }

        if (m_param_vec7[0] != 1 ||
            m_param_vec7[1] != 2 ||
            m_param_vec7[2] != 3) {
            yError() << "error vec7 content"; return false;
        }

        if (m_param_vec8[0] != 1.0 ||
            m_param_vec8[1] != 2.0 ||
            m_param_vec8[2] != 3.0) {
            yError() << "error vec8 content"; return false;
        }

        if (m_param_vec9[0] != "sa1" ||
            m_param_vec9[1] != "sa2" ||
            m_param_vec9[2] != "sa3") {
            yError() << "error vec9 content"; return false;
        }
    }

    return true;
}

bool TestDeviceWGP1::open(yarp::os::Searchable &config)
{
    bool ret = parseParams(config);
    if (!ret) { yError() << "parseParams() failed"; return false; }

    ret = do_test();
    if (!ret) { yError() << "do_test() failed"; return false; }

    return true;
}

bool TestDeviceWGP1::close()
{
    return true;
}
