/*
 * SPDX-FileCopyrightText: 2025-2025 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "Utils.h"

#include <memory>
#include <numeric>

namespace yarp::dev::tests
{
    // Utility function to compare two int vectors for equality
    bool YARP_dev_API vectors_equal(const std::vector<double>& a, const std::vector<double>& b)
    {
        return a.size() == b.size() && std::equal(a.begin(), a.end(), b.begin());
    }

    bool YARP_dev_API vectors_equal(const std::vector<int>& a, const std::vector<int>& b)
    {
        return a.size() == b.size() && std::equal(a.begin(), a.end(), b.begin());
    }

    bool YARP_dev_API vectors_equal(const size_t siz, const double* a, const double* b)
    {
        for (size_t i = 0; i < siz; i++)
        {
            if (a[i] != b[i]) return false;
        }
        return true;
    }

    bool YARP_dev_API vectors_equal(const size_t siz, const int* a, const int* b)
    {
        for (size_t i = 0; i < siz; i++)
        {
            if (a[i] != b[i]) return false;
        }
        return true;
    }

    void YARP_dev_API set_vector_crescent(std::vector<double>& a, double val)
    {
        std::iota(a.begin(), a.end(), val);
    }

    void YARP_dev_API set_vector_crescent(const size_t siz, double* a, double val)
    {
        for (size_t i = 0; i < siz; i++)
        {
            a[i] = val;
            val = val + 1.0;
        }
    }

    void YARP_dev_API set_vector_crescent(const size_t siz, int* a, int val)
    {
        for (size_t i = 0; i < siz; i++)
        {
            a[i] = val;
            val = val + 1;
        }
    }
}
