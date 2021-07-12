/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/os/Value.h>

#include <iostream>
#include <string>

#if !defined(_WIN32)
constexpr static bool colored_output = true;
#else
constexpr static bool colored_output = false;
#endif

#define xGREY    std::string(colored_output ? "\033[01;30m" : "")
#define xRED     std::string(colored_output ? "\033[01;31m" : "")
#define xGREEN   std::string(colored_output ? "\033[01;32m" : "")
#define xYELLOW  std::string(colored_output ? "\033[01;33m" : "")
#define xBLUE    std::string(colored_output ? "\033[01;34m" : "")
#define xMAGENTA std::string(colored_output ? "\033[01;35m" : "")
#define xCYAN    std::string(colored_output ? "\033[01;36m" : "")
#define xWHITE   std::string(colored_output ? "\033[01;37m" : "")
#define xCLEAR   std::string(colored_output ? "\033[00m" : "")

#define GREY(x)    (xGREY    + #x + xCLEAR)
#define RED(x)     (xRED     + #x + xCLEAR)
#define GREEN(x)   (xGREEN   + #x + xCLEAR)
#define YELLOW(x)  (xYELLOW  + #x + xCLEAR)
#define BLUE(x)    (xBLUE    + #x + xCLEAR)
#define MAGENTA(x) (xMAGENTA + #x + xCLEAR)
#define CYAN(x)    (xCYAN    + #x + xCLEAR)
#define WHITE(x)   (xWHITE   + #x + xCLEAR)

# define PRINT_BOOL(x) (x ? GREEN(true) : RED(false))
# define PRINT_IF(cond, x, y) (cond ? xWHITE + std::to_string(x) + xCLEAR : GREY(y))

#define printIsType(T, x) \
{ \
    yarp::os::Value* v = yarp::os::Value::make ## T(0); \
    std::cout << YELLOW(T) << " \t" << xGREY << sizeof(x) * 8 << "-bit" << xCLEAR << "\t\t" \
              << PRINT_BOOL(v->isBool())    << "\t\t" \
              << PRINT_BOOL(v->isInt8())    << "\t\t" \
              << PRINT_BOOL(v->isInt16())   << "\t\t" \
              << PRINT_BOOL(v->isInt32())   << "\t\t" \
              << PRINT_BOOL(v->isInt64())   << "\t\t" \
              << PRINT_BOOL(v->isFloat32()) << "\t\t" \
              << PRINT_BOOL(v->isFloat64()) << "\t\t" \
              << PRINT_BOOL(v->isString())  << "\n"; \
    delete v; \
}

#define printAsType(T, val) \
{ \
    yarp::os::Value* v = yarp::os::Value::make ## T(val); \
    std::cout << YELLOW(T) << " \t" << xGREY << #val << xCLEAR << "\t\t" \
              << PRINT_BOOL(v->asBool()) << "\t\t" \
              << std::to_string(v->asInt8())    << "\t\t" \
              << std::to_string(v->asInt16())   << "\t\t" \
              << std::to_string(v->asInt32())   << "\t\t" \
              << std::to_string(v->asInt64())   << "\t\t" \
              << std::to_string(v->asFloat32()) << "\t" \
              << std::to_string(v->asFloat64()) << "\t" \
              << GREY([) << xWHITE << v->asString() << xCLEAR << GREY(]) << "\t\t" \
              << GREY([) << xWHITE << v->toString() << xCLEAR << GREY(]) << "\n"; \
    delete v; \
}

// Not a real test, just an useful printed table, that allows to check the
// result of all the calls.
int main(int argc, char *argv[])
{

// This method tests all the unsafe methods, therefore the warning is disabled
YARP_WARNING_PUSH
YARP_DISABLE_DEPRECATED_WARNING

    std::cout << "\n\t\t\t\t"
                << CYAN(isBool)    << "\t\t"
                << CYAN(isInt8)    << "\t\t"
                << CYAN(isInt16)   << "\t\t"
                << CYAN(isInt32)   << "\t\t"
                << CYAN(isInt64)   << "\t\t"
                << CYAN(isFloat32) << "\t"
                << CYAN(isFloat64) << "\t"
                << CYAN(isString)  << "\n";

    printIsType(Int8, std::int8_t);
    printIsType(Int16, std::int16_t);
    printIsType(Int32, std::int32_t);
    printIsType(Int64, std::int64_t);
    printIsType(Float32, yarp::conf::float32_t);
    printIsType(Float64, yarp::conf::float64_t);


    std::cout << "\n\t\t\t\t"
                << BLUE(asBool)    << "\t\t"
                << BLUE(asInt8)    << "\t\t"
                << BLUE(asInt16)   << "\t\t"
                << BLUE(asInt32)   << "\t\t"
                << BLUE(asInt64)   << "\t\t"
                << BLUE(asFloat32) << "\t"
                << BLUE(asFloat64) << "\t"
                << BLUE(asString)  << "\t"
                << MAGENTA(toString)  << "\n";

    printAsType(Int8, 8);
    printAsType(Int16, 16);
    printAsType(Int32, 32);
    printAsType(Int64, 64);
    printAsType(Float32, 32.01f);
    printAsType(Float64, 64.01);


    std::cout << "\n\t\t\t\t"
                << BLUE(asBool)    << "\t\t"
                << BLUE(asInt8)    << "\t\t"
                << BLUE(asInt16)   << "\t\t"
                << BLUE(asInt32)   << "\t\t"
                << BLUE(asInt64)   << "\t\t"
                << BLUE(asFloat32) << "\t"
                << BLUE(asFloat64) << "\t"
                << BLUE(asString)  << "\t"
                << MAGENTA(toString)  << "\n";

    printAsType(Int8, 0);
    printAsType(Int16, 0);
    printAsType(Int32, 0);
    printAsType(Int64, 0);
    printAsType(Float32, 0.0f);
    printAsType(Float64, 0.0);

    // Values that might not behave as expected:
    std::cout << "\n\t\t\t\t"
                << BLUE(asBool)    << "\t\t"
                << BLUE(asInt8)    << "\t\t"
                << BLUE(asInt16)   << "\t\t"
                << BLUE(asInt32)   << "\t\t"
                << BLUE(asInt64)   << "\t\t"
                << BLUE(asFloat32) << "\t"
                << BLUE(asFloat64) << "\t"
                << BLUE(asString)  << "\t"
                << MAGENTA(toString)  << "\n";

    printAsType(Int16, 127);
    printAsType(Int16, 128);
    printAsType(Int16, 256);
    printAsType(Int16, 257);
    printAsType(Int32, 32767);
    printAsType(Int32, 32768);
    printAsType(Int32, 65535);
    printAsType(Float32, 31.99f);
    printAsType(Float64, 63.99);

    std::cout << std::flush;
YARP_WARNING_POP

    return 0;
}
