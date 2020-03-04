timerfix {master}
--------

### Libraries

#### `os`

* Fixed `Timer` in monothread mode taking 100% of the CPU.
* Fixed `Timer`s in separate threads not updating their settings.
