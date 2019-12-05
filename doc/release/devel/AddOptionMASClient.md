AddOptionMASClient {#devel}
------------------

### dev

#### multipleanalogsensorsclient

- Added the option `externalConnection`(false by default). This option disables
  the automatic connect to the `rpc:o` and `measures:o` of the respective
  `multipleanalogsensorsserver`. This implies that with this configuration the
  metadata is missing, then the device lacks of some informations such as the
  sensor name, the frame name etc. On the other hand it allows to
  connect/disconnect the client to the port `measures:o` of the server after
  being opened, and to work also when the `rpc:o` port is missing (e.g with the
  `yarpdataplayer`).
