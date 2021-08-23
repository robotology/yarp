Logging with YARP {#yarp_logging}
=================

[TOC]

YARP has an internal mechanism to help you debug your distributed application.

While you can simply use `printf` or `std::cout` methods to debug your
application locally, if you use the functionalities offered, YARP will collect
and show several additional information, including system and network time,
file, line, thread id, etc., and eventually forward them to
[yarplogger](@ref yarplogger).

YARP is also able to detect if it is running on a console or inside
[yarprun](@ref yarprun) and if the application output is forwarded to
[yarplogger](@ref yarplogger) (using the `--log` option), and change its
output accordingly, so that the extra information is forwarded properly.

When the log is forwarded over the network, the logging process opens a yarp port
with the following syntax: `/log/hostname/processname/pid`. 
NOTE: If yarprun is used, hostname is replaced by the name of yarprun server.

## Log Levels

YARP defines 6 levels for log.

| Level       | Value | Stream   | Notes                                                          |
|:-----------:|:-----:|----------|----------------------------------------------------------------|
| `[TRACE]`   | 1     | `stdout` | Generates no binary code in release mode.                      |
| `[DEBUG]`   | 2     | `stdout` | Generates no binary code if `YARP_NO_DEBUG_OUTPUT` is defined. |
| `[INFO]`    | 3     | `stdout` |                                                                |
| `[WARNING]` | 4     | `stderr` |                                                                |
| `[ERROR]`   | 5     | `stderr` |                                                                |
| `[FATAL]`   | 6     | `stderr` | Aborts the execution and prints a backtrace.                   |

For each these levels, YARP offers logging functions.

## Generic Macros

YARP offers some generic macros that do not require any configuration, and will
use the default logging settings.

| Level       | C-Style                          | Stream       |
|:-----------:|----------------------------------|--------------|
| `[TRACE]`   | `yTrace(const char* msg, ...)`   | `yTrace()`   |
| `[DEBUG]`   | `yDebug(const char* msg, ...)`   | `yDebug()`   |
| `[INFO]`    | `yInfo(const char* msg, ...)`    | `yInfo()`    |
| `[WARNING]` | `yWarning(const char* msg, ...)` | `yWarning()` |
| `[ERROR]`   | `yError(const char* msg, ...)`   | `yError()`   |
| `[FATAL]`   | `yFatal(const char* msg, ...)`   | `yFatal()`   |


### C-Style macros

The `yDebug(const char* msg, ...)` macro family is defined in the
`<yarp/os/Log.h>` header.

These macros work in a similar way to the C `printf()` function.
For example:

```{.cpp}
int g = 42;
yDebug("The value of g is %d", g);
```

will print

```
[DEBUG] The value of g is 42
```

Note that the string does not end with `\n`, since YARP will automatically add
it.

### Stream Macros

In order to use the `yDebug()` macro family, you need to include the
`<yarp/os/LogStream.h>` header.

These macros work in a similar way to C++ `std::cout`. For example:

```{.cpp}
int g = 42;
yDebug() << "The value of g is" << g;
```

will print

```
[DEBUG] The value of g is 42
```

Note that there is no need to add a whitespace between the fields since these
are added automatically. Also YARP will automatically add a `\n` at the end of
the stream.


## Components

The output of YARP applications tends to be very messy, especially when using
multiple threads, or several devices at the same time.

Therefore YARP allows to define *Log Components* that can be used to filter the
output and follow the execution flow.

| Level       | Component C-Style                                                     | Component Stream                                |
|:-----------:|-----------------------------------------------------------------------|-------------------------------------------------|
| `[TRACE]`   | `yCTrace(const yarp::os::Logcomponent& comp, const char* msg, ...)`   | `yCTrace(const yarp::os::Logcomponent& comp)`   |
| `[DEBUG]`   | `yCDebug(const yarp::os::Logcomponent& comp, const char* msg, ...)`   | `yCDebug(const yarp::os::Logcomponent& comp)`   |
| `[INFO]`    | `yCInfo(const yarp::os::Logcomponent& comp, const char* msg, ...)`    | `yCInfo(const yarp::os::Logcomponent& comp)`    |
| `[WARNING]` | `yCWarning(const yarp::os::Logcomponent& comp, const char* msg, ...)` | `yCWarning(const yarp::os::Logcomponent& comp)` |
| `[ERROR]`   | `yCError(const yarp::os::Logcomponent& comp, const char* msg, ...)`   | `yCError(const yarp::os::Logcomponent& comp)`   |
| `[FATAL]`   | `yCFatal(const yarp::os::Logcomponent& comp, const char* msg, ...)`   | `yCFatal(const yarp::os::Logcomponent& comp)`   |


### Defining a Logging Component

In order to define a log component, the `YARP_LOG_COMPONENT` macro (defined in
`<yarp/os/LogComponent.h>`) should be used in a `.cpp` file. For example:

```{.cpp}
YARP_LOG_COMPONENT(FOO, "foo.bar")
```

will define a logging component `FOO` that will be printed as `foo.bar`. In
order to use the component in that file, there is no need to do anything else.
If you need to use it in more than one file, you need to forward declare it
somewhere else (e.g. in a `.h` file):

```{.cpp}
YARP_DECLARE_LOG_COMPONENT(FOO)
```

### C-Style Macros

The `yCDebug(const yarp::os::Logcomponent& comp, const char* msg, ...)` macro
family is defined in the `<yarp/os/LogComponent.h>` header.

These macros work in a similar way to the C `printf()` function.
For example (assuming that the `FOO` component is defined as shown before):

```{.cpp}
int g = 42;
yCDebug(FOO, "The value of g is %d", g);
```

will print

```
[DEBUG] |foo.bar| The value of g is 42
```

Like for the `yDebug` family, there is no need to add `\n` at the end of the
string.


### Stream Macros

In order to use the `yCDebug(const yarp::os::Logcomponent& comp)` macro family,
you need to include the `<yarp/os/LogStream.h>` header.

These macros work in a similar way to C++ `std::cout`.
For example (assuming that the `FOO` component is defined as shown before):

```{.cpp}
int g = 42;
yCDebug(FOO) << "The value of g is" << g;
```

will print

```
[DEBUG] |foo.bar| The value of g is 42
```

Like for the `yDebug` family, there is no need to add a whitespace between the
fields, nor `\n` at the end of the stream.


## External timestamp

Users have the possibility of logging information using an external source which
provides a timestamp, by including the timestamp in the macro.
These macros expand on the previous macros by including a external time term.

| Level       | C-Style                                                           | Stream                                      |
|:-----------:|-------------------------------------------------------------------|---------------------------------------------|
| `[TRACE]`   | `yTraceExternalTime(double externaltime, const char* msg, ...)`   | `yTraceExternalTime(double externaltime)`   |
| `[DEBUG]`   | `yDebugExternalTime(double externaltime, const char* msg, ...)`   | `yDebugExternalTime(double externaltime)`   |
| `[INFO]`    | `yInfoExternalTime(double externaltime, const char* msg, ...)`    | `yInfoExternalTime(double externaltime)`    |
| `[WARNING]` | `yWarningExternalTime(double externaltime, const char* msg, ...)` | `yWarningExternalTime(double externaltime)` |
| `[ERROR]`   | `yErrorExternalTime(double externaltime, const char* msg, ...)`   | `yErrorExternalTime(double externaltime)`   |
| `[FATAL]`   | `yFatalExternalTime(double externaltime, const char* msg, ...)`   | `yFatalExternalTime(double externaltime)`   |


| Level       | Component C-Style                                                                                      | Component Stream                                                                 |
|:-----------:|--------------------------------------------------------------------------------------------------------|----------------------------------------------------------------------------------|
| `[TRACE]`   | `yCTraceExternalTime(double externaltime, const yarp::os::Logcomponent& comp, const char* msg, ...)`   | `yCTraceExternalTime(double externaltime, const yarp::os::Logcomponent& comp)`   |
| `[DEBUG]`   | `yCDebugExternalTime(double externaltime, const yarp::os::Logcomponent& comp, const char* msg, ...)`   | `yCDebugExternalTime(double externaltime, const yarp::os::Logcomponent& comp)`   |
| `[INFO]`    | `yCInfoExternalTime(double externaltime, const yarp::os::Logcomponent& comp, const char* msg, ...)`    | `yCInfoExternalTime(double externaltime, const yarp::os::Logcomponent& comp)`    |
| `[WARNING]` | `yCWarningExternalTime(double externaltime, const yarp::os::Logcomponent& comp, const char* msg, ...)` | `yCWarningExternalTime(double externaltime, const yarp::os::Logcomponent& comp)` |
| `[ERROR]`   | `yCErrorExternalTime(double externaltime, const yarp::os::Logcomponent& comp, const char* msg, ...)`   | `yCErrorExternalTime(double externaltime, const yarp::os::Logcomponent& comp)`   |
| `[FATAL]`   | `yCFatalExternalTime(double externaltime, const yarp::os::Logcomponent& comp, const char* msg, ...)`   | `yCFatalExternalTime(double externaltime, const yarp::os::Logcomponent& comp)`   |


## Limited Macros

Sometimes some logging line should be printed only once, or only at most once
in a certain amount of time.
YARP has several utilities to achieve this.


### yDebugOnce() Family

These log lines are printed only once in the execution of the program.

| Level       | C-Style                              | Stream           |
|:-----------:|--------------------------------------|------------------|
| `[TRACE]`   | `yTraceOnce(const char* msg, ...)`   | `yTraceOnce()`   |
| `[DEBUG]`   | `yDebugOnce(const char* msg, ...)`   | `yDebugOnce()`   |
| `[INFO]`    | `yInfoOnce(const char* msg, ...)`    | `yInfoOnce()`    |
| `[WARNING]` | `yWarningOnce(const char* msg, ...)` | `yWarningOnce()` |
| `[ERROR]`   | `yErrorOnce(const char* msg, ...)`   | `yErrorOnce()`   |
| `[FATAL]`   | N/A                                  | N/A              |


| Level       | Component C-Style                                                         | Component Stream                                    |
|:-----------:|---------------------------------------------------------------------------|-----------------------------------------------------|
| `[TRACE]`   | `yCTraceOnce(const yarp::os::Logcomponent& comp, const char* msg, ...)`   | `yCTraceOnce(const yarp::os::Logcomponent& comp)`   |
| `[DEBUG]`   | `yCDebugOnce(const yarp::os::Logcomponent& comp, const char* msg, ...)`   | `yCDebugOnce(const yarp::os::Logcomponent& comp)`   |
| `[INFO]`    | `yCInfoOnce(const yarp::os::Logcomponent& comp, const char* msg, ...)`    | `yCInfoOnce(const yarp::os::Logcomponent& comp)`    |
| `[WARNING]` | `yCWarningOnce(const yarp::os::Logcomponent& comp, const char* msg, ...)` | `yCWarningOnce(const yarp::os::Logcomponent& comp)` |
| `[ERROR]`   | `yCErrorOnce(const yarp::os::Logcomponent& comp, const char* msg, ...)`   | `yCErrorOnce(const yarp::os::Logcomponent& comp)`   |
| `[FATAL]`   | N/A                                                                       | N/A                                                 |

| Level       | C-Style w/ External time                                              | Stream w/ External time                         |
|:-----------:|-----------------------------------------------------------------------|-------------------------------------------------|
| `[TRACE]`   | `yTraceExternalTimeOnce(double externaltime, const char* msg, ...)`   | `yTraceExternalTimeOnce(double externaltime)`   |
| `[DEBUG]`   | `yDebugExternalTimeOnce(double externaltime, const char* msg, ...)`   | `yDebugExternalTimeOnce(double externaltime)`   |
| `[INFO]`    | `yInfoExternalTimeOnce(double externaltime, const char* msg, ...)`    | `yInfoExternalTimeOnce(double externaltime)`    |
| `[WARNING]` | `yWarningExternalTimeOnce(double externaltime, const char* msg, ...)` | `yWarningExternalTimeOnce(double externaltime)` |
| `[ERROR]`   | `yErrorExternalTimeOnce(double externaltime, const char* msg, ...)`   | `yErrorExternalTimeOnce(double externaltime)`   |
| `[FATAL]`   | N/A                                                                   | N/A                                             |


| Level       | Component C-Style w/ External time                                                                         | Component Stream w/ External time                                                    |
|:-----------:|------------------------------------------------------------------------------------------------------------|--------------------------------------------------------------------------------------|
| `[TRACE]`   | `yCTraceExternalTimeOnce(double externaltime, const yarp::os::Logcomponent& comp, const char* msg, ...)`   | `yCTraceExternalTimeOnce(double externaltime, const yarp::os::Logcomponent& comp)`   |
| `[DEBUG]`   | `yCDebugExternalTimeOnce(double externaltime, const yarp::os::Logcomponent& comp, const char* msg, ...)`   | `yCDebugExternalTimeOnce(double externaltime, const yarp::os::Logcomponent& comp)`   |
| `[INFO]`    | `yCInfoExternalTimeOnce(double externaltime, const yarp::os::Logcomponent& comp, const char* msg, ...)`    | `yCInfoExternalTimeOnce(double externaltime, const yarp::os::Logcomponent& comp)`    |
| `[WARNING]` | `yCWarningExternalTimeOnce(double externaltime, const yarp::os::Logcomponent& comp, const char* msg, ...)` | `yCWarningExternalTimeOnce(double externaltime, const yarp::os::Logcomponent& comp)` |
| `[ERROR]`   | `yCErrorExternalTimeOnce(double externaltime, const yarp::os::Logcomponent& comp, const char* msg, ...)`   | `yCErrorExternalTimeOnce(double externaltime, const yarp::os::Logcomponent& comp)`   |
| `[FATAL]`   | N/A                                                                                                        | N/A                                                                                  |


### yDebugThreadOnce() Family

These log lines are printed at most once by every thread during the execution of
the program.

| Level       | C-Style                                    | Stream                 |
|:-----------:|--------------------------------------------|------------------------|
| `[TRACE]`   | `yTraceThreadOnce(const char* msg, ...)`   | `yTraceThreadOnce()`   |
| `[DEBUG]`   | `yDebugThreadOnce(const char* msg, ...)`   | `yDebugThreadOnce()`   |
| `[INFO]`    | `yInfoThreadOnce(const char* msg, ...)`    | `yInfoThreadOnce()`    |
| `[WARNING]` | `yWarningThreadOnce(const char* msg, ...)` | `yWarningThreadOnce()` |
| `[ERROR]`   | `yErrorThreadOnce(const char* msg, ...)`   | `yErrorThreadOnce()`   |
| `[FATAL]`   | N/A                                        | N/A                    |


| Level       | Component C-Style                                                               | Component Stream                                          |
|:-----------:|---------------------------------------------------------------------------------|-----------------------------------------------------------|
| `[TRACE]`   | `yCTraceThreadOnce(const yarp::os::Logcomponent& comp, const char* msg, ...)`   | `yCTraceThreadOnce(const yarp::os::Logcomponent& comp)`   |
| `[DEBUG]`   | `yCDebugThreadOnce(const yarp::os::Logcomponent& comp, const char* msg, ...)`   | `yCDebugThreadOnce(const yarp::os::Logcomponent& comp)`   |
| `[INFO]`    | `yCInfoThreadOnce(const yarp::os::Logcomponent& comp, const char* msg, ...)`    | `yCInfoThreadOnce(const yarp::os::Logcomponent& comp)`    |
| `[WARNING]` | `yCWarningThreadOnce(const yarp::os::Logcomponent& comp, const char* msg, ...)` | `yCWarningThreadOnce(const yarp::os::Logcomponent& comp)` |
| `[ERROR]`   | `yCErrorThreadOnce(const yarp::os::Logcomponent& comp, const char* msg, ...)`   | `yCErrorThreadOnce(const yarp::os::Logcomponent& comp)`   |
| `[FATAL]`   | N/A                                                                             | N/A                                                       |

| Level       | C-Style w/ External time                                                    | Stream w/ External time                               |
|:-----------:|-----------------------------------------------------------------------------|-------------------------------------------------------|
| `[TRACE]`   | `yTraceExternalTimeThreadOnce(double externaltime, const char* msg, ...)`   | `yTraceExternalTimeThreadOnce(double externaltime)`   |
| `[DEBUG]`   | `yDebugExternalTimeThreadOnce(double externaltime, const char* msg, ...)`   | `yDebugExternalTimeThreadOnce(double externaltime)`   |
| `[INFO]`    | `yInfoExternalTimeThreadOnce(double externaltime, const char* msg, ...)`    | `yInfoExternalTimeThreadOnce(double externaltime)`    |
| `[WARNING]` | `yWarningExternalTimeThreadOnce(double externaltime, const char* msg, ...)` | `yWarningExternalTimeThreadOnce(double externaltime)` |
| `[ERROR]`   | `yErrorExternalTimeThreadOnce(double externaltime, const char* msg, ...)`   | `yErrorExternalTimeThreadOnce(double externaltime)`   |
| `[FATAL]`   | N/A                                                                         | N/A                                                   |


| Level       | Component C-Style w/ External time                                                                               | Component Stream w/ External time                                                          |
|:-----------:|------------------------------------------------------------------------------------------------------------------|--------------------------------------------------------------------------------------------|
| `[TRACE]`   | `yCTraceExternalTimeThreadOnce(double externaltime, const yarp::os::Logcomponent& comp, const char* msg, ...)`   | `yCTraceExternalTimeThreadOnce(double externaltime, const yarp::os::Logcomponent& comp)`   |
| `[DEBUG]`   | `yCDebugExternalTimeThreadOnce(double externaltime, const yarp::os::Logcomponent& comp, const char* msg, ...)`   | `yCDebugExternalTimeThreadOnce(double externaltime, const yarp::os::Logcomponent& comp)`   |
| `[INFO]`    | `yCInfoExternalTimeThreadOnce(double externaltime, const yarp::os::Logcomponent& comp, const char* msg, ...)`    | `yCInfoExternalTimeThreadOnce(double externaltime, const yarp::os::Logcomponent& comp)`    |
| `[WARNING]` | `yCWarningExternalTimeThreadOnce(double externaltime, const yarp::os::Logcomponent& comp, const char* msg, ...)` | `yCWarningExternalTimeThreadOnce(double externaltime, const yarp::os::Logcomponent& comp)` |
| `[ERROR]`   | `yCErrorExternalTimeThreadOnce(double externaltime, const yarp::os::Logcomponent& comp, const char* msg, ...)`   | `yCErrorExternalTimeThreadOnce(double externaltime, const yarp::os::Logcomponent& comp)`   |
| `[FATAL]`   | N/A                                                                                                              | N/A                                                                                        |


### yDebugThrottle() Family

These log lines are printed at most once every `period` seconds.

| Level       | C-Style                                                 | Stream                            |
|:-----------:|---------------------------------------------------------|-----------------------------------|
| `[TRACE]`   | `yTraceThrottle(double period, const char* msg, ...)`   | `yTraceThrottle(double period)`   |
| `[DEBUG]`   | `yDebugThrottle(double period, const char* msg, ...)`   | `yDebugThrottle(double period)`   |
| `[INFO]`    | `yInfoThrottle(double period, const char* msg, ...)`    | `yInfoThrottle(double period)`    |
| `[WARNING]` | `yWarningThrottle(double period, const char* msg, ...)` | `yWarningThrottle(double period)` |
| `[ERROR]`   | `yErrorThrottle(double period, const char* msg, ...)`   | `yErrorThrottle(double period)`   |
| `[FATAL]`   | N/A                                                     | N/A                               |


| Level       | Component C-Style                                                                            | Component Stream                                                       |
|:-----------:|----------------------------------------------------------------------------------------------|------------------------------------------------------------------------|
| `[TRACE]`   | `yCTraceThrottle(const yarp::os::Logcomponent& comp, double period, const char* msg, ...)`   | `yCTraceThrottle(const yarp::os::Logcomponent& comp, double period)`   |
| `[DEBUG]`   | `yCDebugThrottle(const yarp::os::Logcomponent& comp, double period, const char* msg, ...)`   | `yCDebugThrottle(const yarp::os::Logcomponent& comp, double period)`   |
| `[INFO]`    | `yCInfoThrottle(const yarp::os::Logcomponent& comp, double period, const char* msg, ...)`    | `yCInfoThrottle(const yarp::os::Logcomponent& comp, double period)`    |
| `[WARNING]` | `yCWarningThrottle(const yarp::os::Logcomponent& comp, double period, const char* msg, ...)` | `yCWarningThrottle(const yarp::os::Logcomponent& comp, double period)` |
| `[ERROR]`   | `yCErrorThrottle(const yarp::os::Logcomponent& comp, double period, const char* msg, ...)`   | `yCErrorThrottle(const yarp::os::Logcomponent& comp, double period)`   |
| `[FATAL]`   | N/A                                                                                          | N/A                                                                    |

| Level       | C-Style w/ External time                                                                 | Stream w/ External time                                            |
|:-----------:|------------------------------------------------------------------------------------------|--------------------------------------------------------------------|
| `[TRACE]`   | `yTraceExternalTimeThrottle(double externaltime, double period, const char* msg, ...)`   | `yTraceExternalTimeThrottle(double externaltime, double period)`   |
| `[DEBUG]`   | `yDebugExternalTimeThrottle(double externaltime, double period, const char* msg, ...)`   | `yDebugExternalTimeThrottle(double externaltime, double period)`   |
| `[INFO]`    | `yInfoExternalTimeThrottle(double externaltime, double period, const char* msg, ...)`    | `yInfoExternalTimeThrottle(double externaltime, double period)`    |
| `[WARNING]` | `yWarningExternalTimeThrottle(double externaltime, double period, const char* msg, ...)` | `yWarningExternalTimeThrottle(double externaltime, double period)` |
| `[ERROR]`   | `yErrorExternalTimeThrottle(double externaltime, double period, const char* msg, ...)`   | `yErrorExternalTimeThrottle(double externaltime, double period)`   |
| `[FATAL]`   | N/A                                                                                      | N/A                                                                |


| Level       | Component C-Style w/ External time                                                                                            | Component Stream w/ External time                                                                       |
|:-----------:|-------------------------------------------------------------------------------------------------------------------------------|---------------------------------------------------------------------------------------------------------|
| `[TRACE]`   | `yCTraceExternalTimeThrottle(double externaltime, const yarp::os::Logcomponent& comp, double period, const char* msg, ...)`   | `yCTraceExternalTimeThrottle(double externaltime, const yarp::os::Logcomponent& comp, double period)`   |
| `[DEBUG]`   | `yCDebugExternalTimeThrottle(double externaltime, const yarp::os::Logcomponent& comp, double period, const char* msg, ...)`   | `yCDebugExternalTimeThrottle(double externaltime, const yarp::os::Logcomponent& comp, double period)`   |
| `[INFO]`    | `yCInfoExternalTimeThrottle(double externaltime, const yarp::os::Logcomponent& comp, double period, const char* msg, ...)`    | `yCInfoExternalTimeThrottle(double externaltime, const yarp::os::Logcomponent& comp, double period)`    |
| `[WARNING]` | `yCWarningExternalTimeThrottle(double externaltime, const yarp::os::Logcomponent& comp, double period, const char* msg, ...)` | `yCWarningExternalTimeThrottle(double externaltime, const yarp::os::Logcomponent& comp, double period)` |
| `[ERROR]`   | `yCErrorExternalTimeThrottle(double externaltime, const yarp::os::Logcomponent& comp, double period, const char* msg, ...)`   | `yCErrorExternalTimeThrottle(double externaltime, const yarp::os::Logcomponent& comp, double period)`   |
| `[FATAL]`   | N/A                                                                                                                           | N/A                                                                                                     |


### yDebugThreadThrottle() Family

These log lines are printed at most once by every thread every `period` seconds.

| Level       | C-Style                                                       | Stream                                  |
|:-----------:|---------------------------------------------------------------|-----------------------------------------|
| `[TRACE]`   | `yTraceThreadThrottle(double period, const char* msg, ...)`   | `yTraceThreadThrottle(double period)`   |
| `[DEBUG]`   | `yDebugThreadThrottle(double period, const char* msg, ...)`   | `yDebugThreadThrottle(double period)`   |
| `[INFO]`    | `yInfoThreadThrottle(double period, const char* msg, ...)`    | `yInfoThreadThrottle(double period)`    |
| `[WARNING]` | `yWarningThreadThrottle(double period, const char* msg, ...)` | `yWarningThreadThrottle(double period)` |
| `[ERROR]`   | `yErrorThreadThrottle(double period, const char* msg, ...)`   | `yErrorThreadThrottle(double period)`   |
| `[FATAL]`   | N/A                                                           | N/A                                     |


| Level       | Component C-Style                                                                                  | Component Stream                                                             |
|:-----------:|----------------------------------------------------------------------------------------------------|------------------------------------------------------------------------------|
| `[TRACE]`   | `yCTraceThreadThrottle(const yarp::os::Logcomponent& comp, double period, const char* msg, ...)`   | `yCTraceThreadThrottle(const yarp::os::Logcomponent& comp, double period)`   |
| `[DEBUG]`   | `yCDebugThreadThrottle(const yarp::os::Logcomponent& comp, double period, const char* msg, ...)`   | `yCDebugThreadThrottle(const yarp::os::Logcomponent& comp, double period)`   |
| `[INFO]`    | `yCInfoThreadThrottle(const yarp::os::Logcomponent& comp, double period, const char* msg, ...)`    | `yCInfoThreadThrottle(const yarp::os::Logcomponent& comp, double period)`    |
| `[WARNING]` | `yCWarningThreadThrottle(const yarp::os::Logcomponent& comp, double period, const char* msg, ...)` | `yCWarningThreadThrottle(const yarp::os::Logcomponent& comp, double period)` |
| `[ERROR]`   | `yCErrorThreadThrottle(const yarp::os::Logcomponent& comp, double period, const char* msg, ...)`   | `yCErrorThreadThrottle(const yarp::os::Logcomponent& comp, double period)`   |
| `[FATAL]`   | N/A                                                                                                | N/A                                                                          |

| Level       | C-Style                                                                                        | Stream                                                                   |
|:-----------:|------------------------------------------------------------------------------------------------|--------------------------------------------------------------------------|
| `[TRACE]`   | `yTraceExternalTimeThreadThrottle(double externaltime, double period, const char* msg, ...)`   | `yTraceExternalTimeThreadThrottle(double externaltime, double period)`   |
| `[DEBUG]`   | `yDebugExternalTimeThreadThrottle(double externaltime, double period, const char* msg, ...)`   | `yDebugExternalTimeThreadThrottle(double externaltime, double period)`   |
| `[INFO]`    | `yInfoExternalTimeThreadThrottle(double externaltime, double period, const char* msg, ...)`    | `yInfoExternalTimeThreadThrottle(double externaltime, double period)`    |
| `[WARNING]` | `yWarningExternalTimeThreadThrottle(double externaltime, double period, const char* msg, ...)` | `yWarningExternalTimeThreadThrottle(double externaltime, double period)` |
| `[ERROR]`   | `yErrorExternalTimeThreadThrottle(double externaltime, double period, const char* msg, ...)`   | `yErrorExternalTimeThreadThrottle(double externaltime, double period)`   |
| `[FATAL]`   | N/A                                                                                            | N/A                                                                      |


| Level       | Component C-Style                                                                                                                   | Component Stream                                                                                              |
|:-----------:|-------------------------------------------------------------------------------------------------------------------------------------|---------------------------------------------------------------------------------------------------------------|
| `[TRACE]`   | `yCTraceExternalTimeThreadThrottle(double externaltime, const yarp::os::Logcomponent& comp, double period, const char* msg, ...)`   | `yCTraceExternalTimeThreadThrottle(double externaltime, const yarp::os::Logcomponent& comp, double period)`   |
| `[DEBUG]`   | `yCDebugExternalTimeThreadThrottle(double externaltime, const yarp::os::Logcomponent& comp, double period, const char* msg, ...)`   | `yCDebugExternalTimeThreadThrottle(double externaltime, const yarp::os::Logcomponent& comp, double period)`   |
| `[INFO]`    | `yCInfoExternalTimeThreadThrottle(double externaltime, const yarp::os::Logcomponent& comp, double period, const char* msg, ...)`    | `yCInfoExternalTimeThreadThrottle(double externaltime, const yarp::os::Logcomponent& comp, double period)`    |
| `[WARNING]` | `yCWarningExternalTimeThreadThrottle(double externaltime, const yarp::os::Logcomponent& comp, double period, const char* msg, ...)` | `yCWarningExternalTimeThreadThrottle(double externaltime, const yarp::os::Logcomponent& comp, double period)` |
| `[ERROR]`   | `yCErrorExternalTimeThreadThrottle(double externaltime, const yarp::os::Logcomponent& comp, double period, const char* msg, ...)`   | `yCErrorExternalTimeThreadThrottle(double externaltime, const yarp::os::Logcomponent& comp, double period)`   |
| `[FATAL]`   | N/A                                                                                                                                 | N/A                                                                                                           |


## Assert

YARP offers some macros that can help programmers find bugs in their programs,
and stop the execution of the program when some unexpected situation happens.

The macros

| Assert               | Component Assert                                          |
|:--------------------:|:---------------------------------------------------------:|
| `yAssert(condition)` | `yCAssert(const yarp::os::Logcomponent& comp, condition)` |

| Assert w/ External time          | Component Assert                                                      |
|:--------------------------------:|:---------------------------------------------------------------------:|
| `yAssertExternalTime(condition)` | `yCAssertExternalTime(const yarp::os::Logcomponent& comp, condition)` |

will check the value of the condition, and throw a fatal error, if the assertion
is not true.

When the code is compiled in release mode, these assertions will just disappear
from the compiled code.


## Configuration

The log behaviour can be configured using build flags, environment variables,
and configuration files.

### Build flags

When building a project, a few build options can change the behavior of the
logging system.

When building in "Release mode" (i.e. `NDEBUG` is defined, and the optimizations
are enabled), the `[TRACE]` level does not produce any binary code.
The `[DEBUG]` level behaves in the same way when `YARP_NO_DEBUG_OUTPUT` is
defined.

This happens only for the projects that are built with these options. If you are
building in "Release mode", but you are linking a library that was built in
"Debug mode", your program will still execute some code for each trace line in
the library.

As already mentioned, the `NDEBUG` option also controls whether the `yAssert`s
are enabled or not.

### Environment variables

The default behaviour for the logger can be configured using specific
environment variables.

`YARP_COLORED_OUTPUT` can be set to `1` to enable colors in the console output
(if supported).

If colors are enabled, `YARP_COMPACT_OUTPUT` can be used to print a single
colored character instead of the log level.

If `YARP_VERBOSE_OUTPUT` is set to `1`, YARP will print several extra
information for each log line.

User can set 'YARP_LOG_PROCESS_LABEL` to specify a string which can help in identifying the
process which is currently broadcasting the log over the network. 
The string will be appended after the process name in the log port and included between
two square brackets, i.e. `/log/hostname/processname[user_label]/pid`. 
A typical use case for this environment variable is when multiple [yarprobotinterface](@ref yarprobotinterface)
and/or [yarpdev](@ref yarpdev) are running simultaneously. In this case, the name log ports are 
difficult to interpret, since the process name alone is not enough to identify them.
In this case the user can set a custom label for each of them to better distinguish them
in the [yarplogger](@ref yarplogger) gui.
`YARP_LOG_PROCESS_LABEL` can be easily set for each individual process, via [yarpmanager](@ref yarpmanager),
using the `environment` section.

`YARP_FORWARD_LOG_ENABLE` can be used to enable the forwarding of the log to
[yarplogger](@ref yarplogger).

By default, YARP prints all log from `[DEBUG]` to `[FATAL]`. If `[DEBUG]` is not
required, it is possible to disable it by setting the `YARP_DEBUG_ENABLE`
environment variable to `0`. It is also possible to enable the `[TRACE]` level
by setting `YARP_TRACE_ENABLE` to `1`.

For `yarp::os` internal logging, the default minimum level is `[INFO]`. It is
possible to change this by using `YARP_QUIET` to change the minimum level to
`[WARNING]`, and `YARP_VERBOSE` to change it to `[DEBUG]`.

Finally, `YARP_DEBUG_LOG_ENABLE` can be enabled to debug the output of the
logger internal component, in case you want to debug the output of your
application.

### Configuration File

FIXME TODO (configuring the log using configuration files is not enabled yet)


## Advanced

### Forwarding output

By default, the logging callback prints the output on the `stdout`/`stderr`.
When a YARP program is started by [yarprun --log](@ref yarprun), this will
forward the output on a YARP port that can be read by
[yarplogger](@ref yarplogger), and displayed.

This behaviour can be obtained also without [yarprun](@ref yarprun), by
setting the `YARP_FORWARD_LOG_ENABLE` environment variable to `1`.
Please note that, while [yarprun](@ref yarprun) is able to forward all the
output of the application (including `printf` and `std::out` output), this
method will forward only the output using YARP log utilities.

At the moment, not all the information gathered is forwarded. It is possible
to enable them by using the following environment variables:

* `YARP_FORWARD_CODEINFO_ENABLE`
* `YARP_FORWARD_HOSTNAME_ENABLE`
* `YARP_FORWARD_PROCESSINFO_ENABLE`
* `YARP_FORWARD_BACKTRACE_ENABLE`

Please note that `yarp::os` internal logging is never forwarded, since this
could cause recursions that will crash the program.


### Custom Logging functions

The default print callback is smart and can detect when it is running inside
[yarprun](@ref yarprun), and if this is forwarding the output.
It is also possible to change the output format.

If you don't like the behaviour, or need to change it for some specific
application, it is possible to change the callback with a custom one using the
`yarp::os::Log::setPrintCallback()` method.
It's also possible to disable output completely by setting it to `nullptr`.

If log forwarding is enabled, it is also possible to change the forwarding
callback with a custom one, using the `yarp::os::Log::setForwardCallback()`
method. This allows, for example, to log everything to a different system.

Inside both the print callback and the forward callback, it is possible to call
YARP default callbacks by calling `yarp::os::Log::defaultPrintCallback()(...)`
and `yarp::os::Log::defaultForwardCallback()(...)`.


### Log Components

It's possible to change the default behaviour for each specific Log Component by
adding some optional parameter to the `YARP_LOG_COMPONENT` macro.
The full syntax is:

```{.cpp}
YARP_LOG_COMPONENT(component,
                   name,
                   minimumPrintLevel = yarp::os::Log::minimumPrintLevel(),
                   minimumForwardLevel = yarp::os::Log::minimumForwardLevel(),
                   printCallback = yarp::os::Log::printCallback(),
                   forwardCallback = yarp::os::Log::forwardCallback())
```

By changing `minimumPrintLevel` it's possible to decide the minimum level that
is printed using the `printCallback`.

By changing `minimumForwardLevel` it's possible to decide the minimum level that
is forwarded using the `forwardCallback`.

By changing the `printCallback`, the output printed on the command line for this
component can be customized as needed. If this is set to `nullptr`, the log
component will not be printed.

By changing the `forwardCallback`, it is possible to change the callback that
will handle forwarding. If this is set to `nullptr`, the log component will not
be forwarded.


### Providing Support for the yDebug() Stream Operator

In order to provide support for yarp logging to your classes, you need to
overload the stream operator (`operator<<`).

This means that you have to declare the `operator<<` function taking `LogStream`
and your class as parameters, somewhere outside your class:

```{.cpp}
yarp::os::LogStream operator<<(yarp::os::LogStream stream, const MyClass& c)
{
    // stream << ...
    return stream;
}
```

In order to access your private class members inside this function you also have
to declare this function as `friend` inside your class declaration:

```{.cpp}
class MyClass
{
    // ...
    friend yarp::os::LogStream operator<<(yarp::os::LogStream stream, const MyClass& c);
}
```

Then you can just log your class in this way:

```{.cpp}
MyClass c
yDebug() << c;
```

This is a full example that shows how to overload and use the stream operator
for a custom class.


```{.cpp}
#include <yarp/os/Network.h>
#include <yarp/os/LogStream.h>

#include <sstream>
#include <iomanip>

class Date
{
    int yr;
    int mo;
    int da;

public:
    Date(int y, int m, int d) : yr(y), mo(m), da(d) {}
    friend yarp::os::LogStream operator<<(yarp::os::LogStream stream, const Date& dt);
};

yarp::os::LogStream operator<<(yarp::os::LogStream stream, const Date& dt)
{
    std::ostringstream oss;
    oss << std::setw(4) << dt.yr << '/' << std::setw(2) << std::setfill('0') << dt.mo << '/' << std::setw(2) << std::setfill('0') << dt.da;
    stream << oss.str();
    return stream;
}

int main()
{
    yarp::os::Network yarp;
    Date today(2020, 06, 17);
    Date tomorrow(2020, 06, 18);
    yDebug() << today << tomorrow;
    return 0;
}
```


### System Clock, Network Clock, Custom Clock and External Time

YARP offers several methods to use the time from an external source, and
therefore it is important to clarify the differences.

The first time that is recorded by the YARP logging system is the **system**
time.
This is the time of the system where YARP is running.

The second time that is recorded by the YARP logging system is the **network**
time.
This is the time of the YARP network, and depends on the clock that is set when
the network is initialized.
There are currently 4 possible way to initialize the network clock (see also
`yarp::os::yarpClockType` and `yarp::os::NetworkBase::yarpClockInit`):
 * Using the default clock (`YARP_CLOCK_DEFAULT`). If the `YARP_CLOCK`
   environment variable is set to a valid YARP port name, then data coming from
   this port will be used as clock, otherwise the system clock is used.
 * Using the system clock (`YARP_CLOCK_SYSTEM`). The system clock is used
   everywhere.
 * Using a network clock (`YARP_CLOCK_NETWORK`). A valid YARP port name must be
   specified when initializing the network, and data coming from
   this port will be used as clock.
 * Using a custom clock (`YARP_CLOCK_CUSTOM`). A class inheriting from
   `yarp::os::Clock` must be specified when initializing the network, and this
   class will be used as clock.
Depending on the clock set when initializing the YARP network,
the `yarp::os::Time::now()` and`yarp::os::Time::delay()` functions will be
return and stop the execution for a different (real) time.

There are cases when an event is generated from an external source (for example
a device with its own clock) and it is important to keep the association between
the event and the timestamp.
This time can be optionally recorded by the YARP logging system as **external**
time.

It is important to understand the difference between setting a custom clock and
passing an external timestamp.
In the first case, the timestamp is requested to the clock when the event is
logged.
In the second case, the timestamp is already available, and is passed to the
logging system, which will store it together with the other data.


## Recommendations

### Log Levels

The user is free to use these levels as he wishes, but we recommend to follow
these guidelines:

* `[TRACE]` - Information useful for the developer. These are normally turned
              off, and might not be compiled in some configurations.
* `[DEBUG]` - Diagnostic information required to debug the application, useful for
              developers, but also for system administrators, etc.
* `[INFO]` - The normal output of the application.
* `[WARNING]` - Anything that can cause troubles, but from which the application
                will recover and continue to run normally.
* `[ERROR]` - An error in the application. The application will continue to run,
              but it might behave in an unexpected way.
* `[FATAL]` - An error that will cause the application to stop.

The `[FATAL]` level, in some cases, can be very dangerous, since it will abort
the execution.

Developers often leave lots of debugging lines commented out in their code.
These lines, after a while, tend to become useless, since they are not updated
when the code is changed, variables renamed or removed, and they just clutter up
the code.
A better practice is to remove them if they are not needed, or eventually to use
`[TRACE]` for the lines that are actually useful for the developers.
Most compilers will just cut any `[TRACE]` line out of the produced code when
building in "Release mode", but they will still interpret the line, ensuring
therefore that the debug line is updated together with the code.


### Component Variable Name

Please be aware that `YARP_LOG_COMPONENT(name)` will generate a method `name`
that can generate some namimg conflicts. If the component is used only in one
compilation unit (i.e. in a `.cpp` file), you should consider defining it
inside an anonymous namespace, i.e.

```{.cpp}
namespace {
YARP_LOG_COMPONENT(FOO, "foo.bar")
}
```


### Component Names

The component name is defined as a string.
Any string is valid, nonethleless we recommend to use a hierarchical naming
scheme, using `.` to separate levels.
For example, all components in YARP are defined as `yarp.*`, and components in
`yarp::os` are defined as `yarp.os.*`.


### Logic inside asserts

Asserts are only compiled into code when `NDEBUG` is not defined.
Do not insert code that should always be executed, since this will not be
compiled in release mode. For example this is ok:

```{.cpp}
yarp::os::Port p;
[[maybe_unused]] bool ret = p.open("...");
yAssert(ret);
```

but this is not ok, since the port will not be open in release mode:

```{.cpp}
yarp::os::Port p;
yAssert(p.open("..."));
```

Note that, in the first example, the `[[maybe_unused]]` C++ attribute can be
used to disable the unused variable warning in release mode, since the `ret`
variable is used only inside the `yAssert`, and therefore "unused".


### Custom Print Callback

While it's technically possible to do anything by replacing the print callback,
including forwarding to a different system, it is recommended to avoid any
network operation inside the callback.

YARP internal components use the logging system, but disable the forwarding,
since this could cause recursion and ultimately crash the program.
Using any YARP port inside the custom print callback could have the same
implications.

Using other networking systems (for example to send the output to some other
logging system) inside the print callback is strongly discouraged.
You should use the forward callback for this kind of tasks.

The custom print callback should be used for example to change the format of
the output, or to log the output to a file.


### Understanding the Implications of Log Forwarding

When log forwarding is demanded to [yarprun](@ref yarprun), the output of the
process is simply piped to another process, which will forward it to the logger.
YARP logging system understands that the output is being forwarded, and changes
the output slightly, to match the format expected by yarplogger.
This should not have a big impact on the process.
Also note that [yarprun](@ref yarprun) will forward all the output of the
application, including `printf`, `std::out`, etc., even though the extra
information will not be attached to these output lines.

On the other hand, enabling the `YARP_FORWARD_LOG_ENABLE` environment variable
will add some extra instructions for every log line, will cause the YARP
application to open an extra port, that will have one or more extra thread,
requires the `Network` to be initialized, etc. This could slow down the
application, therefore you should take this into account before enabling it.


### Limited Output

`yDebugOnce()` and the other macros are useful in some cases, but you should
take into consideration that these macros do not come completely for free,
they use a callback mechanism to decide whether the output should be printed or
not, and therefore some extra code is executed every time, even when there is no
output.
Therefore in some cases there might be a good reason for using alternatives.


### Formatting Output

The stream version of the debugging macros is faster to use and does not
require to know the type of the variable printed, but it offers a very limited
way of formatting the output.
If you require a proper formatting, the C-style macros are recommended.


### Performance

The C-style `yDebug()` macro family is slightly more performant than the stream
version, since the stream version implies object construction, copies, and
destruction. If you care about the performance of some parts of your code, you
should take into consideration the C-style version.

Please note normally the C-style version uses an internal buffer, and therefore
it does not use any dynamic allocation of memory, unless the internal buffer
is not big enough to fit the output.
If you care about performance, you should also ensure that your log output does
not exceed 1024 bytes per log line.
