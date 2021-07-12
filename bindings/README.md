```
SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
SPDX-License-Identifier: BSD-3-Clause
```

# SWIG-YARP interfaces

This directory is for producing SWIG interfaces to YARP.

This lets you run YARP from Java, Python, Perl, Tcl, C#, Ruby, Matlab.

Note that while we try to preserve YARP as close to its original form
as possible, some classes and methods had to be changed because
of issues with templates and pointers.  So the API is slightly
different from the original.  The two ways to find out about the API
are:

- Compile the java or python interfaces, and then run javadoc
   or pydoc to build a browsable form of the API.

- Read yarp.i.  This is a bit complicated, but powerful because you'll
   learn how to fix things if you run into problems.  See www.swig.org
   for documentation on the syntax of this file.


**Note**: all the `cmake` and `make` commands are referring to the **main** YARP build directory.

## Java INTERFACE

To create the Java interface to YARP from linux
(the process on windows is analogous using cmake):

```
  ccmake .
```

Enable the `CREATE_JAVA` option, then

```
make
```

If you install, you end up with a shared library called `yarp_java` in
`${CMAKE_INSTALL_PREFIX}/lib/jni`
and a `yarp.jar` file (actually it is a symbolic link to a versioned `.jar` in the same folder)
in `${CMAKE_INSTALL_PREFIX}/share/yarp/java`
where `${CMAKE_INSTALL_PREFIX}` is the path to your YARP installation.
An additional library, called `yarp_matlab_java.jar` is created, containing utilities to load YARP
classes inside MATLAB.

You should add `${CMAKE_INSTALL_PREFIX}/share/yarp/java` to your Java ClassPath and
`${CMAKE_INSTALL_PREFIX}/lib/jni` to the `java.library.path` Java setting.

Remember in your Java code, before calling any YARP classes to load the JNI library (usually in a `static` block):

```java
static {
    try {
        System.loadLibrary("yarp_java");
    } catch (UnsatisfiedLinkError ule) {
        ule.printStackTrace();
    } catch (SecurityException se) {
        se.printStackTrace();
    } catch (NullPointerException npe) {
        npe.printStackTrace();
    }
}
```

All YARP classes are located in the `yarp` package.

### Cross-compiling options

You might need to enable cross-compiling options, that is, supporting JAVA versions older than your JDK version.
For example, you want to distribute the built .jar files, or use them with the default JDK of MATLAB (which may be older than your system JDK).

Cross compilation works differently in JAVA 9 with respect to previous versions

#### JAVA 9
Starting with JAVA 9, cross compilation is easier, and we cross-compile to JAVA 7 by default.
If you want to change the target version, change the CMake variable (in the advanced options)
`JAVA_RELEASE_VERSION` to your desired version. Note that not all the versions are supported.
See `javac --help` (`--release` option) for more information.

If you want to disable cross-compiling, simply set an empty string to `JAVA_RELEASE_VERSION`.

#### JAVA 1.6 to 1.8

Cross compilation in JAVA previous to 9 is more complex and it is managed by a pair of options: `source` and `target`.
By default, we set both variables to coincide with the loaded JDK, i.e. we disable cross-compilation.
The same behaviour can be obtained by setting to empty strings the following CMake variables (both in the advanced options):
`JAVA_SOURCE_VERSION` and `JAVA_TARGET_VERSION`.

Note that to have a correct cross compilation you have to specify additional options to `javac`.
Unfortunately we cannot configure them automatically, and you have to manually specify them.
These options are `-bootclasspath` (pointing to the correct `rt.jar` file) and `-extdirs`.
You can add them by using the CMake variable (in the advanced options) `JAVA_FLAGS`.


## MATLAB INTERFACE

**Note**: native MATLAB support in swig is currently not ready upstream.
Currently we are supporting MATLAB through Java, but in the future, expect a native
MATLAB support.
Experimental support of native MATLAB is implemented in https://github.com/robotology-playground/yarp-matlab-bindings.

Matlab can run java code, so we access YARP via Java.  See the
"Java interface" section above.

See also:
  http://wiki.icub.org/wiki/Calling_yarp_from_Matlab
for latest tips.

Additionally to the instructions to generate Java bindings, see the following:
Configure your MATLAB classpath and jni loader to find the generated libraries.
See [MATLAB documentation](https://it.mathworks.com/help/matlab/matlab_external/java-class-path.html)

The JAVA package for the MATLAB utility classes is `yarp.matlab`.


## PYTHON, PERL, TCL, RUBY INTERFACES

Run:

```
ccmake .
```

And set `CREATE_PERL` and/or `CREATE_PYTHON` and/or `CREATE_TCL` to true.
Run:

```
make
```

You should now have libraries called

- libyarp.so
- and/or _yarp.so
- and/or cyarp.so
- and/or yarp.so

Or on windows, the equivalent DLLs.

Now try to run `perl/examples/example.pl` (Perl) and/or `python/examples/example.py` (Python) and/or `tcl/examples/example.tcl` (TCL) and/or `ruby/examples/example.rb` (Ruby).

For TCL, on windows, look at `example.tcl` and uncomment the line referring
to a .dll, and comment the line referring to a .so

Make sure all DLLs or SOs needed for YARP are available (for example, by
copying them to the current directory).


## CSHARP INTERFACE

Run:

```
ccmake .
```

And set `CREATE_CSHARP` to true.
Run:

```
make
```

You should now have `libyarp.so` or `yarp.so` (on Linux, presumably a .dll
on windows) and a whole lot of .cs files

On Linux, now either do something like one of:
```
setenv LD_LIBRARY_PATH $PWD:$LD_LIBRARY_PATH
export LD_LIBRARY_PATH=$PWD:$LD_LIBRARY_PATH
```
On Windows, put all this stuff in your path.

There used be some issues that are now fixed; solution preserved
just in case they recur

```
OLD>  The .cs files are not quite correct, you need to run:
OLD>  ./csharp-fix.pl
OLD> to fix them up (problems with multiple inheritance mismatch)
OLD> You need the "mcs" compiler for this script to work.  If you're
OLD> on windows, it might be easier to borrow a linux laptop for this
OLD> step.  Alternatively, manually fix the problems (there are cases
OLD> where "override" is specified rather than "virtual").
OLD>
OLD> At the time of writing, there are also a few errors related to the
OLD> PidVector class.  One solution is to just remove that and all
OLD> references to it.
```

Now go into `csharp/examples` folder and try:

```
mcs -out:example.exe *.cs
```

or whatever your compiler is (the mcs compiler is in the debian/ubuntu
"mono-mcs" package).  If it works, then:

```
./example.exe
```

should do something yarpy.

NOTE:
Different compilers need the library to be called different things.
If you have yarp.so, try copying it to libyarp.so, or vice versa,
if you run into trouble.
