/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_RET_VALUE_H
#define YARP_RET_VALUE_H

#include <yarp/dev/api.h>
#include <string>

namespace yarp::dev {

class YARP_dev_API yarp_ret_value
{
    public:
    enum class YARP_dev_API return_code
    {
        return_value_ok = 0,
        return_value_error_generic = 1,
        return_value_error_not_implemented_by_device = 2,
        return_value_error_nws_nwc_communication_error = 3,
        return_value_error_deprecated = 4,
        return_value_error_method_failed = 5,
        return_value_unitialized = 100
    };

    private:
    return_code value_b = return_code::return_value_unitialized;

    public:
    yarp_ret_value();
    yarp_ret_value(const bool& val);
    yarp_ret_value(return_code code);
    yarp_ret_value(const yarp_ret_value& other) = default;
    yarp_ret_value& operator&=(const yarp_ret_value& other);
    yarp_ret_value& operator=(const bool& bool_val);
    bool operator == (const return_code& code) const;
    std::string toString();
    operator bool() const;
};

}

#endif // YARP_RET_VALUE_H
