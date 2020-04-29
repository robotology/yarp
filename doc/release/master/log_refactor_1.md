log_refactor_1 {#master}
--------------

### Libraries

#### `os`

##### `Log`

* Add `LogComponent` and component macros
  * A log component can be declared using the `YARP_LOG_COMPONENT()` macro (in
    a `.cpp` file) and eventually forward declared using
    ` YARP_DECLARE_LOG_COMPONENT()`.
  * the component can then be used in the new macros:
    - `yCTrace`
    - `yCDebug`
    - `yCInfo`
    - `yCWarning`
    - `yCError`
    - `yCTrace`
  * It is possible to set custom printing and forwarding macros for each
    component, and it is possible to enable and disable specific levels for each
    component.
  * At the moment it is not yet possible to change the defaults set in the code,
    but this will be enabled in the future

* The output forwarded is now a property and contains several useful
  information including component and system/network time, file, line,
  function. The following EXPERIMENTAL environment variables can be
  enabled to enable forwarding of some extra information:
  - `YARP_FORWARD_CODEINFO_ENABLE`
  - `YARP_FORWARD_HOSTNAME_ENABLE`
  - `YARP_FORWARD_PROCESSINFO_ENABLE`
  - `YARP_FORWARD_BACKTRACE_ENABLE`
  These environment variables could be removed in the future.

* The logger is now able to detect if it is running in yarprun and
  eventually change the output format.

* Logging is now protected by a mutex to avoid garbled output.

* The `YARP_DEBUG_LOG_ENABLE` environment variable can be set to debug
  the log of the application.

* When color is enabled, the loglevel is represented by a single colored
  character.
