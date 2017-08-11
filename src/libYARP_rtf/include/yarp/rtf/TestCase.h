/*
 * Copyright (C) 2015 Istituto Italiano di Tecnologia (IIT)
 * Authors: Ali Paikan <ali.paikan@iit.it>
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */

#ifndef YARP_RTF_YARPTESTCASE_H
#define YARP_RTF_YARPTESTCASE_H

#include <yarp/rtf/api.h>
#include <yarp/os/Property.h>
#include <rtf/TestCase.h>
#include <string>

namespace yarp {
namespace rtf {

/**
 * @brief The YarpTestCase is a helper class to facilitate loading the tests
 * settings which are developed for YARP.
 *
 * The class simply looks for test configuration file given using "--from"
 * paramter to the test case and loads it into a yarp::os::Property object.
 *
 * If any environment property is given using "testrunner -e" or using
 * \<environment> \</environment> tag within suit XML file, that will be used to
 * updated the properties from the main config file.
 *
 * Please see RTF example folder for how to develop a simple test plugin for
 * YARP.
 */
class YARP_rtf_API TestCase : public RTF::TestCase {
public:
    TestCase(std::string name);
    virtual ~TestCase();

    bool setup(int argc, char** argv) override;
    virtual bool setup(yarp::os::Property& property);

private:
    class Private;
    Private * const mPriv;
};

} // namespace rtf
} // namespace yarp

#endif // YARP_RTF_YARPTESTCASE_H
