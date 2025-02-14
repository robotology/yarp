/*
 * SPDX-FileCopyrightText: 2025-2025 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/dev/ReturnValue.h>
#include <yarp/os/ConnectionReader.h>
#include <yarp/os/ConnectionWriter.h>

using namespace yarp::dev;
using namespace yarp::os;

ReturnValue::ReturnValue()
{
}

#if !YARP_RETURNVALUE_DISABLE_BOOL_INPUT
ReturnValue::ReturnValue(const bool& val)
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
#endif

ReturnValue& ReturnValue::operator && (const ReturnValue& other)
{
    //  THIS IS THE IMPLEMENTATION RULE:
    //     OP1         OP2          RESULT
    //  1) True        True         True
    //  2) FalseType1  FalseType1   FalseType1
    //  3) FalseType1  FalseType2   FalseGeneric
    //  4) True        FalseType1   FalseGeneric
    //  5) FalseType1  True         FalseGeneric

    if (this->value_b == other.value_b)
    {
        //cases 1 and 2
        return *this;
    }

    value_b = return_code::return_value_error_generic;
    return *this;
}

ReturnValue& ReturnValue::operator &= (const ReturnValue& other)
{
    //  THIS IS THE IMPLEMENTATION RULE:
    //     OP1         OP2          RESULT
    //  1) True        True         True
    //  2) FalseType1  FalseType1   FalseType1
    //  3) FalseType1  FalseType2   FalseGeneric
    //  4) True        FalseType1   FalseGeneric
    //  5) FalseType1  True         FalseGeneric

    if (this->value_b == other.value_b)
    {
        //cases 1 and 2
        return *this;
    }

    value_b = return_code::return_value_error_generic;
    return *this;
}

#if !YARP_RETURNVALUE_DISABLE_BOOL_INPUT
ReturnValue& ReturnValue::operator=(const bool& bool_val)
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
#endif

std::string ReturnValue::toString()
{
    switch (value_b)
    {
        case return_code::return_value_ok:
            return std::string("ok");
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
        case return_code::return_value_error_not_ready:
            return std::string("return_value_error_not_ready");
        default:
        case return_code::return_value_uninitialized:
            return std::string("unknown/uninitialized");
    }
}

ReturnValue::operator bool() const
{
    return value_b == return_code::return_value_ok;
}

ReturnValue::ReturnValue(return_code code)
{
    value_b = code;
}

bool ReturnValue::operator == (const return_code& code) const
{
    if (code == this->value_b) return true;
    return false;
}

bool ReturnValue::operator == (const ReturnValue& value) const
{
    if (value == this->value_b) return true;
    return false;
}

bool ReturnValue::read(yarp::os::ConnectionReader& connection)
{
    connection.convertTextMode();
    this->value_b = (ReturnValue::return_code)(connection.expectInt64());
    return true;
}

bool ReturnValue::write(yarp::os::ConnectionWriter& connection) const
{
    connection.convertTextMode();
    connection.appendInt64((int64_t)(this->value_b));
    return true;
}
