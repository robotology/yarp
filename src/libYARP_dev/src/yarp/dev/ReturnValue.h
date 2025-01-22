/*
 * SPDX-FileCopyrightText: 2025-2025 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_RET_VALUE_H
#define YARP_RET_VALUE_H

#include <yarp/dev/api.h>
#include <yarp/os/Log.h>
#include <yarp/os/Portable.h>
#include <source_location>
#include <string>

// The following macro is used for testing/development purposes only, but it must be generally not enabled.
// If enabled, a bool value cannot be automatically converted to yarp_ret_value
// In this way, the developer can check if some old devices/interfaces do unwanted automatic
// conversions and fix them.
 #define DISABLE_BOOL_INPUT

// The following macro is used for testing/development purposes only, but it must be generally not enabled.
// If enabled, a yarp_ret_value cannot be automatically converted to bool unless explicitly requested
// using the bool() operator.
// If enabled, it will break backward compatibility with user-application code.
// #define DISABLE_BOOL_OUTPUT

namespace yarp::dev {

class YARP_dev_API yarp_ret_value : public yarp::os::Portable
{
    public:
    enum class YARP_dev_API return_code
    {
        return_value_ok = 0, /// Method was successfully executed
        return_value_error_generic = 1,
        return_value_error_not_implemented_by_device = 2, /// Method is not (yet) implemented
        return_value_error_nws_nwc_communication_error = 3, /// Command answer lost during network transmission. Status unknown.
        return_value_error_deprecated = 4, /// Method is deprecated
        return_value_error_method_failed = 5, /// Method failed due to invalid internal status/invalid request
        return_value_unitialized = 100 /// Default value, should never be explicitly assigned
    };

    private:
    return_code value_b = return_code::return_value_unitialized;

    public:
    yarp_ret_value();
    ~yarp_ret_value() = default;
#ifndef DISABLE_BOOL_INPUT
    yarp_ret_value(const bool& val);
#endif
    yarp_ret_value(return_code code);
    yarp_ret_value(const yarp_ret_value& other) = default;
    yarp_ret_value& operator && (const yarp_ret_value& other);
    yarp_ret_value& operator &= (const yarp_ret_value& other);
#ifndef DISABLE_BOOL_INPUT
    yarp_ret_value& operator=(const bool& bool_val);
#endif
    bool operator == (const return_code& code) const;
    std::string toString();
#ifndef DISABLE_BOOL_OUTPUT
    operator bool() const;
#else
    explicit operator bool const();
#endif

public:
    bool read(yarp::os::ConnectionReader& connection) override;
    bool write(yarp::os::ConnectionWriter& connection) const override;
};

#define yarp_ret_value_ok yarp_ret_value(yarp::dev::yarp_ret_value::return_code::return_value_ok)

#if __cplusplus >= 202002L
inline yarp_ret_value YARP_METHOD_NOT_YET_IMPLEMENTED(const std::source_location& location = std::source_location::current())
{
    yError("Method %s not yet implemented\n", location.function_name());
    return yarp_ret_value(yarp::dev::yarp_ret_value::return_code::return_value_error_not_implemented_by_device);
}
inline yarp_ret_value YARP_METHOD_DEPRECATED(const std::source_location& location = std::source_location::current())
{
    yError("Method %s has been deprecated\n", location.function_name());
    return yarp_ret_value(yarp::dev::yarp_ret_value::return_code::return_value_error_deprecated);
}
#else
inline yarp_ret_value yarp_method_not_implemented(const char* location)
{
    yError("Method %s not yet implemented\n", location);
    return yarp_ret_value(yarp::dev::yarp_ret_value::return_code::return_value_error_not_implemented_by_device);
}
#define YARP_METHOD_NOT_YET_IMPLEMENTED() yarp_method_not_implemented(__func__)
inline yarp_ret_value yarp_method_deprecated(const char* location)
{
    yError("Method %s has been deprecated\n", location);
    return yarp_ret_value(yarp::dev::yarp_ret_value::return_code::return_value_error_deprecated);
}
#define YARP_METHOD_DEPRECATED() yarp_method_deprecated(__func__)
#endif


}

#endif // YARP_RET_VALUE_H
