portnumber {#master}
----------

* Added the `YARP_PORTNUMBER<XXX>` environment variable.
  This allows to change the port number on which the corresponding port is
  opened by setting the `YARP_PORTNUMBER<XXX>` environment variable to the desired
  port number of the port.
  For example: `YARP_PORTNUMBER_read=20050 yarp read /read` will open a port named
  `/read` on the `20050` port.
  Port numbers (if present) are applied before prefixes specified with
  `YARP_PORT_PREFIX` and renames specified with `YARP_RENAME` (if present).
  **WARNING**, if the same port is opened with the same name but different port
  number by two processes, this might lead to port stealing by the latest
  process executed.

