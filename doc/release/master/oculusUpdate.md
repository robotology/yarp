oculusUpdate {master}
----------------------

### Devices

#### `ovrheadset`

* Port to the new rendering API and make it focus aware (SDK 1.19 is required).
* Fixed race condition causing randomly frame drops.
* Fixed behaviour of the `CTRL` button. Both `CTRL` buttons can now be pressed,
  only `SHIFT` will decide which eye offsets should be modified.
* Add `P` command to print current settings.
* Fixed `--no-logo` option.
* Fixed `--userpose` option.
