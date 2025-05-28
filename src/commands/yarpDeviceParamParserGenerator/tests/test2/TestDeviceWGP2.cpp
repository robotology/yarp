/*
 * SPDX-FileCopyrightText: 2024-2024 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "TestDeviceWGP2.h"

#include <yarp/os/LogComponent.h>
#include <yarp/os/LogStream.h>

using namespace yarp::os;
using namespace yarp::dev;

TestDeviceWGP2::TestDeviceWGP2()
{
}

TestDeviceWGP2::~TestDeviceWGP2()
{
}

bool TestDeviceWGP2::do_test()
{
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

        if (m_param_vec8[0] != 1 ||
            m_param_vec8[1] != 2 ||
            m_param_vec8[2] != 3) {
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

bool TestDeviceWGP2::open(yarp::os::Searchable &config)
{
    bool ret = parseParams(config);
    if (!ret) { yError() << "parseParams() failed"; return false; }

    ret = do_test();
    if (!ret) { yError() << "do_test() failed"; return false; }

    return true;
}

bool TestDeviceWGP2::close()
{
    return true;
}
