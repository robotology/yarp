/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_COMPANION_COMPANION_H
#define YARP_COMPANION_COMPANION_H

#include <yarp/companion/api.h>

namespace yarp {
namespace companion {

/**
 * The standard main method for the YARP companion utility.
 * @param argc Argument count
 * @param argv Command line arguments
 * @return 0 on success, non-zero on failure
 */
YARP_companion_API
int main(int argc, char *argv[]);

} // namespace companion
} // namespace yarp


#endif // YARP_COMPANION_IMPL_COMPANION_H
