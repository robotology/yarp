/*
 * SPDX-FileCopyrightText: 2023-2023 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/dev/ReturnValue.h>

using namespace yarp::dev;

yarp_ret_value::yarp_ret_value()
{
}

yarp_ret_value::yarp_ret_value(const bool& val)
{
    if (val)
    {
        value_b = return_code::return_value_ok;
    }
    else
    {
        value_b = return_code::return_value_error_generic;
    }
}

/*/yarp_ret_value::yarp_ret_value(const yarp_ret_value& other)
{
    this->value_b=other.value_b;
}*/

yarp_ret_value& yarp_ret_value::operator&=(const yarp_ret_value& other)
{
    if (other.operator bool() == true)
    {
        return *this;
    }
    else
    {
        value_b = return_code::return_value_error_generic;
    }
    return *this;
}

yarp_ret_value& yarp_ret_value::operator=(const bool& bool_val)
{
    if (bool_val)
    {
        value_b = return_code::return_value_ok;
    }
    else
    {
        value_b = return_code::return_value_error_generic;
    }
    return *this;
}

std::string yarp_ret_value::toString()
{
    switch (value_b)
    {
        case return_code::return_value_ok:
            return std::string("ok");
        case return_code::return_value_unitialized:
            return std::string("return_value_unitialized");
        case return_code::return_value_error_deprecated:
            return std::string("return_value_error_deprecated");
        case return_code::return_value_error_generic:
            return std::string("return_value_error_generic");
        case return_code::return_value_error_method_failed:
            return std::string("return_value_error_method_fail");
        case return_code::return_value_error_not_implemented_by_device:
            return std::string("return_value_error_not_implemented_by_device");
        case return_code::return_value_error_nws_nwc_communication_error:
            return std::string("return_value_error_nws_nwc_communication_error");
        default:
            return std::string("unknown");
    }
}

yarp_ret_value::operator bool() const
{
    return value_b == return_code::return_value_ok;
}

yarp_ret_value::yarp_ret_value(return_code code)
{
    value_b = code;
}

bool yarp_ret_value::operator == (const return_code& code) const
{
    if (code == this->value_b) return true;
    return false;
}
