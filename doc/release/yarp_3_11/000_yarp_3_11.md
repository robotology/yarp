YARP <yarp-3.11> (UNRELEASED)                                         {#yarp_3_11}
============================

[TOC]

YARP <yarp-3.11> Release Notes
=============================


A (partial) list of bug fixed and issues resolved in this release can be found
[here](https://github.com/robotology/yarp/issues?q=label%3A%22Fixed+in%3A+YARP+yarp-3.11%22).


Changes
----------------

### Compiler features

The `PUBLIC` and `INTERFACE` values set by `target_compile_features` have been changed back from `cxx_std_20` to `cxx_std_17`, to permit
to continue to compile downstream projects with C++17. This is not a long term decision, and in future the minimum C++ version required
to compile headers could be increased again.
