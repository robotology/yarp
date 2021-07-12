/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_ROBOTTESTINGFRAMEWORK_YARPTESTASSERTER_H
#define YARP_ROBOTTESTINGFRAMEWORK_YARPTESTASSERTER_H

#include <yarp/robottestingframework/api.h>
#include <yarp/sig/Vector.h>

namespace yarp {
namespace robottestingframework {

/**
 * @brief The yarp::robottestingframework::TestAsserter class is used to throw
 * exception or to report messages/failures to a TestResult.
 */
class YARP_robottestingframework_API TestAsserter
{
public:
    explicit TestAsserter();
    virtual ~TestAsserter();

    /**
     * Element-wise compare two vectors to determine if they are approximately
     * equal, according to asymmetric thresholds that specify lower and upper
     * bound.
     *
     * Both thresholds are assumed to be positive.
     * @param left: left operand
     * @param right: right operand
     * @param l_thresholds: a vector of thresholds whose elements are used when
     *                      checking equality of corresponding elements of the
     *                      left and right vectors. It specifies the low bound
     *                      of the interval.
     * @param h_thresholds: a vector of thresholds whose elements are used when
     *                      checking equality of corresponding elements of the
     *                      left and right vectors. It specifies the upper bound
     *                      of the interval.
     * @param length: specifies the lengths of all vectors
     * @return: the result of the comparison
     */
    static bool isApproxEqual(const double *left,
                              const double *right,
                              const double *l_thresholds,
                              const double *h_thresholds,
                              int length);

    /**
     * Element-wise compare two vectors to determine if they are approximately
     * equal, according to a threshold. The threshold is assumed to be positive.
     *
     * @param left: left operand
     * @param right: right operand
     * @param thresholds: a vector of thresholds whose elements are used when
     *                    checking equality of corresponding elements of the
     *                    left and right vectors
     * @param length: specifies the lengths of all vectors
     * @return: the result of comparison
     */
    static bool isApproxEqual(const double *left,
                              const double *right,
                              const double *thresholds,
                              int length);

    /**
     * Element-wise compare two vectors to determine if they are approximately
     * equal, according to a threshold. The threshold is assumed to be positive.
     *
     * @param left: left operand
     * @param right: right operand
     * @param thresholds: a vector of thresholds whose elements are used when
     *                    checking equality of corresponding elements of the
     *                    left and right vectors
     * @return: the result of comparison
     */
    static bool isApproxEqual(const yarp::sig::Vector &left,
                              const yarp::sig::Vector &right,
                              const yarp::sig::Vector &thresholds);

    /**
     * Compare two scalars to determine if they are approximately equal,
     * according to asymmetric thresholds that specify upper and lower bound.
     *
     * @param left: left operand
     * @param right: right operand
     * @param l_th: lower bound of interval for comparison
     * @param h_th: upper bound of interval for comparison
     * @return: the result of comparison
     */
    static bool isApproxEqual(double left,
                              double right,
                              double l_th,
                              double h_th);

private:
    class Private;
    Private * const mPriv;
};


} // namespace robottestingframework
} // namespace yarp

#endif // YARP_ROBOTTESTINGFRAMEWORK_YARPTESTASSERTER_H
