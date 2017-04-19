/*
 * Copyright (C) 2015 iCub Facility, Istituto Italiano di Tecnologia
 * Authors: Ali Paikan <ali.paikan@iit.it>
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */

#ifndef YARP_RTF_YARPTESTASSERTER_H
#define YARP_RTF_YARPTESTASSERTER_H

#include <yarp/rtf/api.h>
#include <yarp/sig/Vector.h>

namespace yarp {
namespace rtf {

/**
 * @brief The yarp::rtf::TestAsserter class is used to throw exception or to
 * report messages/failures to a TestResult.
 */
class YARP_rtf_API TestAsserter
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
     * @param lenght: specifies the lengths of all vectors
     * @return: the result of the comparison
     */
    static bool isApproxEqual(const double *left,
                              const double *right,
                              const double *l_thresholds,
                              const double *h_thresholds,
                              int lenght);

    /**
     * Element-wise compare two vectors to determine if they are approximately
     * equal, according to a threshold. The threshold is assumed to be positive.
     *
     * @param left: left operand
     * @param right: right operand
     * @param thresholds: a vector of thresholds whose elements are used when
     *                    checking equality of corresponding elements of the
     *                    left and right vectors
     * @param lenght: specifies the lengths of all vectors
     * @return: the result of comparison
     */
    static bool isApproxEqual(const double *left,
                              const double *right,
                              const double *thresholds,
                              int lenght);

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


} // namespace rtf
} // namespace yarp

#endif // YARP_RTF_YARPTESTASSERTER_H
