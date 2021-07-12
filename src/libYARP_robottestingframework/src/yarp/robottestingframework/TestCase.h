/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_ROBOTTESTINGFRAMEWORK_YARPTESTCASE_H
#define YARP_ROBOTTESTINGFRAMEWORK_YARPTESTCASE_H

#include <yarp/robottestingframework/api.h>
#include <yarp/os/Property.h>
#include <robottestingframework/TestCase.h>
#include <string>

namespace yarp {
namespace robottestingframework {

/**
 * @brief The YarpTestCase is a helper class to facilitate loading the tests
 * settings which are developed for YARP.
 *
 * The class simply looks for test configuration file given using "--from"
 * parameter to the test case and loads it into a yarp::os::Property object.
 *
 * If any environment property is given using "testrunner -e" or using
 * \<environment> \</environment> tag within suite XML file, that will be used
 * to updated the properties from the main config file.
 *
 * Please see RobotTestingFramework example folder for how to develop a simple
 * test plugin for YARP.
 */
class YARP_robottestingframework_API TestCase : public ::robottestingframework::TestCase {
public:
    TestCase(std::string name);
    virtual ~TestCase();

    bool setup(int argc, char** argv) override;
    virtual bool setup(yarp::os::Property& property);

private:
    class Private;
    Private * const mPriv;
};

} // namespace robottestingframework
} // namespace yarp

#endif // YARP_ROBOTTESTINGFRAMEWORK_YARPTESTCASE_H
