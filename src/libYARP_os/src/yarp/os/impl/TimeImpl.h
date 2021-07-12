/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_OS_IMPL_TIMEIMPL_H
#define YARP_OS_IMPL_TIMEIMPL_H

namespace yarp {
namespace os {
namespace impl {
namespace Time {

void removeClock();

/**
 * For OS where it makes sense sets the scheduler to be called more often.
 * This sets the scheduler to be run to the maximum possible rate based
 * on the capability of the hardware.
 * Specifically, on Microsoft Windows, high resolution scheduling is
 * used.
 *
 * @warning According to https://msdn.microsoft.com/en-us/library/vs/alm/dd757624(v=vs.85).aspx
 *          timeBeginPeriod (called in startTurboBoost) affects a global Windows
 *          setting and should be matched with a call to timeEndPeriod (called
 *          in endTurboBoost).
 */
void startTurboBoost();
void endTurboBoost();

} // namespace Time
} // namespace impl
} // namespace os
} // namespace yarp

#endif // YARP_OS_IMPL_TIMEIMPL_H
