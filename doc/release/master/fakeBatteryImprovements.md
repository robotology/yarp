fakeBatteryImprovements {#master}
-----------------------

## New Features

### Devices

#### `fakeBattery`

* Added a new port `<name>/control/rpc:i` to change the values reported by the
  fake battery.
* The battery is now charged/discharged depending on the sign of the current.
