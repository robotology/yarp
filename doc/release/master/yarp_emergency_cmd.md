yarp_emergency_cmd {#master}
------------------

## Tools

### `yarp`

* Added `emergency` sub-command
  This command calls an emergency meeting and try to determine the impostor
  port.
  Each open port gets a vote, and the port who receives more votes is ejected
  from YARP.
