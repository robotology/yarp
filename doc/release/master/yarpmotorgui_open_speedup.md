yarpmotorgui_open_speedup {#master}
---------------------------------



Branch: [S-Dafarra:yarpmotorgui_speedup](https://github.com/S-Dafarra/yarp/tree/yarpmotorgui_speedup)

#### `yarpmotorgui`

* Improved the initialization time of ``yarpmotorgui`` by:
  * removing useless code
  * using a dummy port to check conflicts with other ``yarpmotorgui`` instances
  * parallelize the opening of the different robot part via ``std::async``

