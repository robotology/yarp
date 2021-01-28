/*
 * Copyright (C) 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * Copyright (C) 2006-2010 RobotCub Consortium
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
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
