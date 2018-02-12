/*
 * Copyright (C) 2017 Istituto Italiano di Tecnologia (IIT)
 * Authors: Francesco Romano <francesco.romano@iit.it>
 *          Claudio Fantacci <claudio.fantacci@iit.it>
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */


#ifndef YARP_OS_MACOSAPI_H
#define YARP_OS_MACOSAPI_H

void* disableAppNap();
void restoreAppNap(void *activityInfo);


#endif /* end of include guard: YARP_OS_MACOSAPI_H */
