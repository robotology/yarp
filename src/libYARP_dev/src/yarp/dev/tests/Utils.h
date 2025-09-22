/*
 * SPDX-FileCopyrightText: 2025-2025 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef UTILS_H
#define UTILS_H

#include <vector>
#include <yarp/dev/api.h>

namespace yarp::dev::tests
{
    bool YARP_dev_API vectors_equal(const std::vector<double>& a, const std::vector<double>& b);
    bool YARP_dev_API vectors_equal(const std::vector<int>& a, const std::vector<int>& b);
    bool YARP_dev_API vectors_equal(const size_t siz, const double* a, const double* b);
    bool YARP_dev_API vectors_equal(const size_t siz, const int* a, const int* b);
    void YARP_dev_API set_vector_crescent(std::vector<double>& a, double val);
    void YARP_dev_API set_vector_crescent(const size_t siz, double* a, double val);
    void YARP_dev_API set_vector_crescent(const size_t siz, int* a, int val);
}

#endif
