RTF Plugins                                              {#rtf_plugins}
===========

[TOC]

The `rtf-plugins` are dynamic libraries that can be loaded in the `rtf::TestSuite` for using some YARP functionalities.

Compilation and installation
----------------------------

To compile them [Robot Testing Framework](https://github.com/robotology/robot-testing) is required.

Once RTF has been compiled (and eventually installed) the `rtf-plugins` has to be enabled configuring YARP with `YARP_COMPILE_RTF_ADDONS=ON`.
For users that doesn't install on their machines, they have to append `<prefix>/lib/rtf` to `LD_LIBRARY_PATH` (Linux/macOS) and `PATH` 
(Windows) to make sure that RTF loads the plugins.  

Available plugins
-----------------
- `yarpmanager`: plugin that allows you to launch applications inside the `TestSuite` throught `yarpmanager`.
   The xml file of the application is passed throught the parameter `--fixture`.
- `yarpplugin`: plugin that allows you to check the availability of carriers (`--carriers`), portmonitor(`--portmonitors`),
   devices(`--devices`) and plugins (`--plugins`) in the machine on which the test is running.
- `yarpserver`: plugin that allows you to launch a `yarpserver` inside the `TestSuite`.
