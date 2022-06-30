Robot Testing Framework (and plugins)                 {#robottestingframework_plugins}
===============================

[TOC]

`Robot Testing Framework` (http://robotology.github.io/robot-testing-framework/index.html) is a generic and multi-platform testing framework for the test driven development (TDD) which is initially designed for the robotic systems. 
The framework provides functionalities for developing and running unit tests in a language and middleware independent manner. 

The test cases are developed as independent plug-ins (i.e., using scripting languages or built as dynamically loadable libraries) to be loaded and executed by an automated test runner. 
Moreover, a fixture manager prepares the setup (e.g., running robot interfaces, simulator) and actively monitors that all the requirements for running the tests are satisfied during the execution of the tests. 
These functionalities along with other facilities such as the test result collector, result formatter and remote interface allow for rapid development of test units to cover different levels of system testing.

The official documentation of `Robot Testing Framework` can be found at: http://robotology.github.io/robot-testing-framework/docs/index.html

The `robottestingframework-plugins` are dynamic libraries that can be loaded in
the `robottestingframework::TestSuite` for using some YARP functionalities.

Compilation and installation
----------------------------

To compile them
[Robot Testing Framework](https://github.com/robotology/robot-testing-framework)
is required.

Once RobotTestingFramework has been compiled (and eventually installed) the
`robottestingframework-plugins` has to be enabled configuring YARP with
`YARP_COMPILE_RobotTestingFramework_ADDONS=ON`.
For users that doesn't install on their machines, they have to append
`<prefix>/lib/robottestingframework` to `LD_LIBRARY_PATH` (Linux/macOS) and
`PATH` (Windows) to make sure that RobotTestingFramework loads the plugins.

Available plugins
-----------------
- `yarpmanager`: plugin that allows you to launch applications inside the
  `TestSuite` throught `yarpmanager`.
   The xml file of the application is passed throught the parameter `--fixture`.
- `yarpplugin`: plugin that allows you to check the availability of carriers
  (`--carriers`), portmonitor(`--portmonitors`), devices(`--devices`) and
  plugins (`--plugins`) in the machine on which the test is running.
- `yarpserver`: plugin that allows you to launch a `yarpserver` inside the
  `TestSuite`.
