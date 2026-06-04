 * |   | period              | double    | s    |   0.01               | No       | Refresh period (in s) of the broadcasted values through yarp ports               | - |
 * |   | name                | string    | -    |   /robotName/joypad  | Yes      | Prefix name of the ports opened by the JoypadControlServer                      | - |
 * |   | use_separate_ports  | bool      | -    |   true               | No       | set it to 1 to use separate ports (buttons, axes, trackballs, hats) and 0 to stream all in one single port | - |
 * |   | profile             | bool      | -    |   false              | No       | print the joypad data for debugging purpose | - |
 * |   | publish_on_event    | bool      | -    |   false              | No       | if true, events, are published when the status changes (on event). Otherwise they are published at the frequency specified through the period parameter| - |
