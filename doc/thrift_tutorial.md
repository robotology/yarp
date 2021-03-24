Thrift IDL in YARP: advanced tutorial                         {#thrift_tutorial}
=====================================

[TOC]

This tutorial shows how to use the Apache Thrift Interface Definition Language
to serialize data sent over YARP ports and define interfaces for RPC-based
services in YARP Modules.


Introduction                                            {#thrift_tutorial_intro}
------------

Apache Thrift allows to define data types and service interfaces in a simple
definition file.
Taking that file as input, a compiler generates source code which can be used by
different client modules and a server.

Language Reference                                   {#thrift_tutorial_language}
------------------

The following is a summary of the Thrift language reference, with corresponding
mapping to YARP (C++) code.
Most of it was adapted from
 * http://thrift.apache.org/
 * http://diwakergupta.github.com/thrift-missing-guide/

The **Thrift type system** consists of pre-defined base types, user-defined
structs, container types, and service definitions.


### Base Types                                 {#thrift_tutorial_subs_basetypes}

\arg `bool`: A boolean value (true or false), one byte; mapped to `bool`
\arg `i8`: A 8-bit signed integer; mapped to `int8_t`
\arg `i16`: A 16-bit signed integer; mapped to `int16_t`
\arg `i32`: A 32-bit signed integer; mapped to `int32_t`
\arg `i64`: A 64-bit signed integer; mapped to `int64_t`
\arg `double`: A 64-bit floating point number; mapped to `float64_t`
\arg `string`: Encoding agnostic text or binary string; mapped to `std::string`

Note that Thrift does not support unsigned integers, but YARP thrift generator
is able to generate unsigned types using annotated typedefs (see
\ref thrift_tutorial_subs_typedef)


### Containers                                {#thrift_tutorial_subs_containers}

\arg `list<t1>`: An ordered list of elements of type t1. May contain duplicates. Mapped to `std::vector`.
\arg `set<t1>`: An unordered set of unique elements of type t1. Mapped to `std::set<t1>`.
\arg `map<t1, t2>`: A map of strictly unique keys of type t1 to values of type t2. Mapped to `std::map<t1, t2>`.

Types used in containers many be any valid Thrift type excluding services.


### Structs                                      {#thrift_tutorial_subs_structs}

Structs are the basic building blocks in a Thrift IDL.
A struct is composed of fields; each field has a unique, positive integer
identifier, a type, a name and an optional default value.
Example:

~~~{.thrift}
struct PointD {
  1: i32 x;
  2: i32 y;
  3: i32 z;
}

struct PointDLists{
  1: string name ="pointLists";
  2: list<PointD> firstList;
  3: list<PointD> secondList;
}
~~~

Note that structs may contain other structs, and that multiple structs can be
defined and referred to within the same Thrift file.

Structs translate to C++ classes that inherit from the
`yarp::os::idl::WirePortable` class.
For each struct, a .h and a .cpp file are created, which contain a definition of
the class and an implementation of the default constructor and of the read/write
methods of the `yarp::os::idl::WirePortable` interface.

In case a certain structure should be translated to an existing YARP type, this
can be declared with `yarp.name` and, if needed, `yarp.includefile` annotations:

~~~{.thrift}
struct Vector
{
  1: list<double> content;
} (
  yarp.name = "yarp::sig::Vector"
  yarp.includefile="yarp/sig/Vector.h"
)
~~~

All structs inside structs are usually serialized as a flat struct when
serialized to a WireWriter (e.g. in RPC calls to thrift services).
In some cases, this is not desired (e.g. when writing a thrift file to interact
with an existing RPC server that uses nested structs). In this case it is
possible to annotate the struct using the `yarp.nested` annotation, for example:

~~~{.thrift}
    struct Foo
    {
        1: double foo1;
        2: double foo2;
    }

    struct Bar
    {
        1: double bar1;
        2: Foo ( yarp.nested = "true" );
    }
~~~

In this case, the `Bar` struct will be serialized as `bar1 (foo1 foo2)`, instead
of as a flat struct `bar1 foo1 foo2`.


### Typedefs                                     {#thrift_tutorial_subs_typedef}

Thrift supports C/C++ style typedefs.

~~~{.thrift}
typedef PointD Point3D
~~~

Note that there is no trailing semi-colon, and that not only base types but also
structs can be used in typedefs.
If any typedef or constant value (see \ref thrift_tutorial_subs_const) is
defined, a `<thriftFileName>_common.h` file is generated, which contains all
typedefs and constants;
this file is automatically included by all the other generated files.

Typedefs to base types can be used in order to use unsigned types, vocabs, and
a few other simple types. For example:

~~~{.thrift}
typedef i32 ( yarp.type = "yarp::conf::vocab32_t" ) vocab
typedef i8 ( yarp.type = "std::uint8_t" ) ui8
typedef i16 ( yarp.type = "std::uint16_t" ) ui16
typedef i32 ( yarp.type = "std::uint32_t" ) ui32
typedef i64 ( yarp.type = "std::uint64_t" ) ui64
typedef i32 ( yarp.type = "size_t" ) size_t
typedef double ( yarp.type = "yarp::conf::float32_t" ) float32
typedef double ( yarp.type = "yarp::conf::float64_t" ) float64

struct TestAnnotatedTypes
{
  1: vocab a_vocab,
  2: ui8 a_ui8,
  3: ui16 a_ui16,
  4: ui32 a_ui32,
  5: ui64 a_ui64,
  6: float32 a_float32,
  7: float64 a_float64,
  8: size_t a_size;
}
~~~

Note that `size_t` in YARP uses 32 bit integers for retro-compatibility, and to
save some bandwidth.
Also note that `float64` here is not actually useful, since it can be replaced
with double, and it is defined only for coherence.

### Constants                                      {#thrift_tutorial_subs_const}

Thrift lets you define constants for use across languages.
Complex types and structs are specified using JSON notation.

~~~{.thrift}
const i32 ANSWER = 42
~~~

Note that semi-colon is optional; hex values are valid here.
If any typedef (see \ref thrift_tutorial_subs_typedef) or constant value is
defined, a `<thriftFileName>_common.h` file is generated, which contains all
typedefs and constants;
this file is automatically included by all other generated files.


### Enums                                          {#thrift_tutorial_subs_enums}

Enums are specified C-style.
Compiler assigns default values starting at 0, but specific integral values (in
the range of positive 32-bit integers) can be specified for constants.
Hex values are also acceptable.

~~~{.thrift}
enum PointQuality
{
  UNKNOWN = 0,
  GOOD = 1,
  BAD = 2
}

struct PointWithQuality
{
  1: PointD point;
  2: PointQuality quality= PointQuality.UNKNOWN;
}
~~~

Note that there is no trailing semi-colon, and that the fully qualified name of
the constant must be used when assigning default values.
For each enum, a `.h` and a `.cpp` file are created, which contain the
definition of the enum and a helper class that handles number/string conversion
for the enum elements.


### Namespaces                                 {#thrift_tutorial_subs_namespace}

Namespaces in Thrift are akin to namespaces in C++ or packages in Java: they
offer a convenient way of organizing (or isolating) your code.
Namespaces may also be used to prevent name clashes between type definitions.
Thrift allows you to customize the namespace behavior on a per-language basis.
YARP example:

~~~{.thrift}
namespace yarp yarp.test
~~~

means that all the code in the generated files will be included in

~~~{.cpp}
namespace yarp {
namespace test {
 ... all
 ... code
}
}
~~~


### Includes                                     {#thrift_tutorial_subs_include}

It is often useful to split up Thrift definitions in separate files to ease
maintenance, enable reuse and improve modularity/organization.
Thrift allows files to include other Thrift files.
Included files are looked up in the current directory and by searching relative
to the path from which the `yarp_idl_to_dir` macro is executed (see
\ref thrift_tutorial_codegen ).

~~~{.thrift}
include "firstInterface/PointD.thrift"
~~~

Included objects are accessed using the name of the Thrift file as a prefix (see
example in \ref thrift_tutorial_subs_services).
In generated files, the needed header files generated from the `PointD.thrift`
file will be included with the same inclusion prefix (in this case,
`firstInterface`).



### Services                                    {#thrift_tutorial_subs_services}

Service definitions are semantically equivalent to defining an interface (or a
pure virtual abstract class) in object-oriented programming.
The Thrift compiler generates fully functional client and server stubs that
implement the communication routine for the interface.
Services contain a collection of method definitions.

~~~{.thrift}
include "firstInterface/PointD.thrift"
...
service Demo {
  i32 get_answer();
  i32 add_one(1:i32 x = 0);
  i32 double_down(1: i32 x);
  PointD.PointD add_point(1: PointD.PointD x, 2: PointD.PointD y);
}
~~~

A method definition has a return type and arguments, like C code.
Note that argument lists are specified using the exact same syntax as field
lists in structs.
Return types can be primitive types or structs; the `oneway` modifier can
precede a `void` return type to indicate that the client only requests that the
server execute the function, but does not wait for an acknowlegment that the
execution has completed (asynchronous processing).
Default values can be provided for tail arguments; clients can avoid providing
values for those parameters, which is especially useful when sending RPC calls
via command line, as will be shown in section
\ref thrift_tutorial_completex.

Services support inheritance: a service may optionally inherit from another
service using the `extends` keyword.

~~~{.thrift}
service DemoExtended extends Demo {
  Point3D multiply_point (1: Point3D x, 2:double factor)
}
~~~

For each service, a `.h` and a `.cpp` file are created, which contain the
definition of the interface as a class derived from `yarp::os::Wire`.
The implementation of the `read` method to receive commands over a YARP port is
provided, as well as the implementation of the command transmission over YARP
for function calls performed by a client.
The description of how to use this generated code to create server and client
modules is provided in sections \ref thrift_tutorial_server and
\ref thrift_tutorial_client respectively.


### Comments                                    {#thrift_tutorial_subs_comments}

Thrift supports shell-style, C-style multi-line as well as single-line Java/C++
style comments.

~~~{.thrift}
# This is a valid comment.

/*
 * This is a multi-line comment.
 * Just like in C.
 */

// C++/Java style single-line comments work just as well.
~~~


### Code generation                                   {#thrift_tutorial_codegen}

Generation of code for a Thrift definition file `PointD.thrift` in the
`firstInterface` directory can be automatically performed by CMake calling the
`yarp_idl_to_dir` macro:

~~~{.cmake}
yarp_idl_to_dir(INPUT_FILES firstInterface/PointD.thrift
                OUTPUT_DIR <desired_output_dir>)
~~~

The macro defines a CMake "advanced" option, `ALLOW_IDL_GENERATION`, which is by
default set to `OFF` if there is already generated code in the desired output
directory.
Code generation occurs at CMake-configure time only when this option is enabled,
otherwise it is assumed that code has already been generated and/or committed.

Upon execution of the macro, the code is generated by the `yarpidl_thrift`
compiler and copied into the `<desired_output_dir>`.
In particular, `.h` files get copied in the `include` subdirectory, while `.cpp`
files go into the `src` subdirectory.
The directory structure inside these subdirectories replicates the one of the
definition file: since `PointD.thrift` is in the `firstInterface` directory,
`.h` files will go to the `<desired_output_dir>/include/firstInterface/` folder,
and .cpp files will go to `<desired_output_dir`>/src/firstInterface/` folder.

You can ask for a list of generated source and header files to be placed in
variables for you to refer to later:

~~~{.cmake}
yarp_idl_to_dir(INPUT_FILES firstInterface/PointD.thrift
                OUTPUT_DIR <desired_output_dir>
                SOURCES_VAR <source_variable_name>
                HEADERS_VAR <header_variable_name>)
~~~

You can also get a list of paths to include:

~~~{.cmake}
yarp_idl_to_dir(INPUT_FILES firstInterface/PointD.thrift
                OUTPUT_DIR <desired_output_dir>
                SOURCES_VAR <source_variable_name>
                HEADERS_VAR <header_variable_name>
                INCLUDE_DIRS_VAR <include_dirs_variable_name>)
~~~

Typical usage of these variables would be something like this:

~~~{.cmake}
yarp_idl_to_dir(INPUT_FILES firstInterface/PointD.thrift
                OUTPUT_DIR <desired_output_dir>
                SOURCES_VAR sources
                HEADERS_VAR headers
                INCLUDE_DIRS_VAR include_dirs)
add_executable(test_program)
target_sources(test_program PRIVATE test_program.cpp
                                    ${sources}
                                    ${headers})
target_include_directories(test_program PRIVATE ${include_dirs})
~~~


Server implementation                                  {#thrift_tutorial_server}
---------------------

The purpose of a server is to listen for commands on a YARP port, execute the
method that each command refers to, and send back the reply.
With Thrift, a server is created from a `service` interface class (generated as
in section \ref thrift_tutorial_subs_services), creating an object that
implements the methods of that interface, and attaching it to a YARP port.

~~~{.cpp}
#include <iostream>
#include <yarp/os/all.h>
#include <secondInterface/Demo.h>


class DemoServer : public yarp::test::Demo
{
public:
    int32_t get_answer() override
    {
        std::cout<<"Server:get_answer called" <<std::endl;
        return ANSWER;
    }
    int32_t add_one(const int32_t x = 0) override
    {
        std::cout<<"Server::add_one called with "<< x <<std::endl;
        return x+1;
    }
    int32_t double_down(const int32_t x) override
    {
        std::cout<<"Server::double_down called with "<< x <<std::endl;
        return x*2;
    }
    ::yarp::test::PointD add_point(const  ::yarp::test::PointD& x, const  ::yarp::test::PointD& y) override
    {
        std::cout<<"Server::add_point called"<<std::endl;
        ::yarp::test::PointD z;
        z.x = x.x + y.x;
        z.y = x.y + y.y;
        z.z = x.z + y.z;
        return z;
    }
};

int main(int argc, char *argv[])
{
    yarp::os::Network yarp;

    DemoServer demoServer;
    yarp::os::Port port;
    demoServer.yarp().attachAsServer(port);
    if (!port.open("/demoServer")) {
        return 1;
    }

    while (true)
    {
        printf("Server running happily\n");
        yarp::os::Time::delay(10);
    }
    port.close();
    return 0;
}
~~~

An altenative solution is to create a YARP module that implements the service
interface:

~~~{.cpp}
#include <iostream>
#include <yarp/os/all.h>
#include <secondInterface/Demo.h>


class DemoServerModule : public yarp::test::Demo, public yarp::os::RFModule
{
public:
  // Thrift Interface Implementation

  // <snip> see above

  // RFModule implementation
  yarp::os::Port cmdPort;

  bool attach(yarp::os::Port &source)
  {
      return this->yarp().attachAsServer(source);
  }
  bool configure( yarp::os::ResourceFinder &rf )
    {
        std::string moduleName = rf.check("name",
                yarp::os::Value("demoServerModule"),
                "module name (string)").asString().c_str();
        setName(moduleName.c_str());

        std::string slash="/";

        attach(cmdPort);

        std::string cmdPortName= "/";
        cmdPortName+= getName();
        cmdPortName += "/cmd";
        if (!cmdPort.open(cmdPortName.c_str())) {
            std::cout << getName() << ": Unable to open port " << cmdPortName << std::endl;
            return false;
        }
        return true;
    }
  bool updateModule()
  {
      return true;
  }
  bool close()
  {
      cmdPort.close();
      return true;
  }
};

int main(int argc, char *argv[])
{
    yarp::os::Network yarp;
    if (!yarp.checkNetwork())
    {
        std::cout<<"Error: yarp server does not seem available"<<std::endl;
        return -1;
    }

    yarp::os::ResourceFinder rf;
    rf.configure(argc, argv);

    DemoServerModule demoMod;

    if (!demoMod.configure(rf)) {
        return -1;
    }

    return demoMod.runModule();
}
~~~

Client Use                                             {#thrift_tutorial_client}
----------

Clients can invoke a remote procedure on the server by simply declaring the
interface and attaching it to a YARP port connected to the server.

Simple example:

~~~{.cpp}
#include <iostream>
#include <yarp/os/all.h>
#include <secondInterface/Demo.h>

using namespace yarp::test;
using namespace yarp::os;

int main(int argc, char *argv[])
{
    Property config;
    config.fromCommand(argc,argv);

    Network yarp;
    Port client_port;

    std::string servername= config.find("server").asString().c_str();
    client_port.open("/demo/client");
    if (!yarp.connect("/demo/client",servername.c_str()))
    {
        std::cout << "Error! Could not connect to server " << servername << std::endl;
        return -1;
    }

    Demo demo;
    demo.yarp().attachAsClient(client_port);

    PointD point;
    point.x = 0;
    point.y = 0;
    point.z = 0;
    PointD offset;
    offset.x = 1;
    offset.y = 2;
    offset.z = 3;

    std::cout << "== get_answer ==" << std::endl;
    int answer=demo.get_answer();
    std::cout << answer << std::endl;

    std::cout<<"== add_one =="<<std::endl;
    answer = demo.add_one(answer);
    std::cout << answer << std::endl;

    std::cout<<"== double_down =="<<std::endl;
    answer = demo.double_down(answer);
    std::cout << answer << std::endl;

    std::cout<<"== add_point =="<<std::endl;
    point = demo.add_point(point,offset);
    std::cout<<("== done! ==\n");

    return 0;
}
~~~

Complete example                                    {#thrift_tutorial_completex}
----------------

A complete example of Thrift code generation and server/client creation with
CMake is available in `example/idl/thrift/`

The server can be launched from command line (assuming a `yarpserver` is
running):

~~~{.sh}
cd <build_directory>
userImpl/DemoServer
~~~

~~~
yarp: Port /demoServer active at tcp://10.xxx.xx.xx:10002
~~~

From another terminal, the communication on the server port can be eavesdropped
with this command:

~~~{.sh}
yarp read /log tcp+log.in://demoServer
~~~

From yet another terminal, the client can be run with the following command:

~~~{.sh}
cd <build_directory>
userImpl/DemoClient --server /demoServer
~~~

~~~
yarp: Port /demo/client active at tcp://10.xxx.xx.xx:10004
yarp: Sending output from /demo/client to /demoServer using tcp
== get_answer ==
42
== add_one ==
43
== double_down ==
86
== add_point ==
== done! ==
yarp: Removing output from /demo/client to /demoServer
~~~

Note that RPC calls can also be sent to the server from command line:
~~~{.sh}
yarp rpc /demoServer
~~~

~~~
get answer
Response: 42
get_answer
Response: 42
add one 42
Response: 43
double down 43
Response: 86
add point 1 2 3 4 5 6
Response: 5 7 9
add one
1
~~~
