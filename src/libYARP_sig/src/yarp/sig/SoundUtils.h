/*
 * SPDX-FileCopyrightText: 2026-2026 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_SIG_SOUNDUTILS_H
#define YARP_SIG_SOUNDUTILS_H

#include <utility> // std::pair
#include <yarp/sig/Sound.h>

namespace yarp::sig::utils {

/**
 * @brief Split vertically an image in two images of the same size.
 * @param[in] inImg image to be vertically split.
 * @param[out] outImgL left half of inImg.
 * @param[out] outImgR right half of inImg.
 * @note The input image must have same height, double width of the output images and same pixel type.
 * @return true on success, false otherwise.
 */
bool YARP_sig_API makeTone(Sound& outSound, double duration_s=1.0, size_t channels=1, size_t sampleRate=16000);

} // namespace yarp::sig::utils


#endif // YARP_SIG_SOUNDUTILS_H
