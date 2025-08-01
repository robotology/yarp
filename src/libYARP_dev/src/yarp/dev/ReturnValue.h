/*
 * SPDX-FileCopyrightText: 2025-2025 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef ReturnValue_H
#define ReturnValue_H

#include <yarp/dev/api.h>
#include <yarp/os/Log.h>
#include <yarp/os/Portable.h>

#if __cplusplus >= 202002L
#include <source_location>
#endif

#include <string>

// If this macro is enabled (default value=enabled), a bool value cannot be automatically converted to ReturnValue.
// Automatic conversions of this type should be generally avoided, because they make the code less clear.
// It is thus preferable that user assigns a precise value to a ReturnValue.
#define YARP_RETURNVALUE_DISABLE_BOOL_INPUT 1

// If this macro is enabled (default value=disabled), a ReturnValue cannot be automatically converted to bool
// unless explicitly requested using the bool() operator.
// The following macro is used for testing/development purposes only.
// If enabled, it will break backward compatibility with user-application code.
#define YARP_RETURNVALUE_DISABLE_BOOL_OUTPUT 0

namespace yarp::dev {

class YARP_dev_API ReturnValue : public yarp::os::Portable
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
        return_value_error_not_ready = 6, /// Method failed because some initialization is missing
        return_value_uninitialized = 100 /// Default value, should never be explicitly assigned
    };

    private:
    return_code value_b = return_code::return_value_uninitialized;

    public:
    ReturnValue();
    ~ReturnValue() = default;
#if !YARP_RETURNVALUE_DISABLE_BOOL_INPUT
    ReturnValue(const bool& val);
#endif
    ReturnValue(return_code code);
    ReturnValue(const ReturnValue& other) = default;
    ReturnValue& operator && (const ReturnValue& other);
    ReturnValue& operator &= (const ReturnValue& other);
#if !YARP_RETURNVALUE_DISABLE_BOOL_INPUT
    ReturnValue& operator=(const bool& bool_val);
#endif
    bool operator == (const return_code& code) const;
    bool operator == (const ReturnValue& value) const;
    std::string toString();
#if !YARP_RETURNVALUE_DISABLE_BOOL_OUTPUT
    operator bool() const;
#else
    explicit operator bool() const;
#endif

public:
    bool read(yarp::os::ConnectionReader& connection) override;
    bool write(yarp::os::ConnectionWriter& connection) const override;
};

#ifndef SWIG_PREPROCESSOR_SHOULD_SKIP_THIS
#define ReturnValue_ok ReturnValue(yarp::dev::ReturnValue::return_code::return_value_ok)

#if __cplusplus >= 202002L
inline ReturnValue YARP_METHOD_NOT_YET_IMPLEMENTED(const std::source_location& location = std::source_location::current())
{
    yError("Method %s not yet implemented\n", location.function_name());
    return ReturnValue(yarp::dev::ReturnValue::return_code::return_value_error_not_implemented_by_device);
}
inline ReturnValue YARP_METHOD_DEPRECATED(const std::source_location& location = std::source_location::current())
{
    yError("Method %s has been deprecated\n", location.function_name());
    return ReturnValue(yarp::dev::ReturnValue::return_code::return_value_error_deprecated);
}
#else
inline ReturnValue yarp_method_not_implemented(const char* location)
{
    yError("Method %s not yet implemented\n", location);
    return ReturnValue(yarp::dev::ReturnValue::return_code::return_value_error_not_implemented_by_device);
}
#define YARP_METHOD_NOT_YET_IMPLEMENTED() yarp_method_not_implemented(__func__)
inline ReturnValue yarp_method_deprecated(const char* location)
{
    yError("Method %s has been deprecated\n", location);
    return ReturnValue(yarp::dev::ReturnValue::return_code::return_value_error_deprecated);
}
#define YARP_METHOD_DEPRECATED() yarp_method_deprecated(__func__)
#endif
#endif // SWIG_PREPROCESSOR_SHOULD_SKIP_THIS

}

#endif // ReturnValue_H
