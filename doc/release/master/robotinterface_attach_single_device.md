robotinterface_attach_single_device {#master}
-----------------------------------

## Libraries

### `robotinterface`

* It is now possible to pass the `device` parameter to the `attach` action,
  without defining `network` or `networks`. For example:

  ```.xml
  <device name="foo_name" type="foo_type">
    <action phase="startup" level="5" type="attach">
      <param name="device"> bar_name </param>
    </action>
    <action phase="shutdown" level="5" type="detach" />
  </device>
  ```
