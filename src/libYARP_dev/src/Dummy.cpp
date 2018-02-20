/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 * Copyright (C) 2006-2010 RobotCub Consortium
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

// This is a trivial file.
// When device modules are compiled, and configured to be automatically
// loaded, all device code is moved to the "yarpmod" library in order to
// avoid a circular dependency which can occasionally cause trouble.

extern "C" int libYARP_dev_dummmy() {
    return 1;
}
