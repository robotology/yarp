yarprun_env_vars {#master}
----------------

### Tools

#### `yarprun`

* The following enviromnent variables are now set in the child processes:
    * `YARP_IS_YARPRUN` is set to `1` for all child processes
    * `YARPRUN_IS_FORWARDING_LOG` is set to `1` when yarprun is forwarding the
      log and to `0` otherwise
