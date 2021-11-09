robotinterface_dryrun {#master}
---------------------

### `robotinterface`

* Added `--dryrun` option to test the xml file without actually opening devices.
* Added `reverse-shutdown-action-order` attribute for the `robot` tag.
  This reverses the order of actions in shutdown and interrupt phase, making it
  easier to write the actions when multiple attach and detach are involved.
* All `robot` tag parameters are now passed to all devices.
