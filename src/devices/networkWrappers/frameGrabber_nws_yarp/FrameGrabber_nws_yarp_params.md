 * |   | period         | double  | s              |   0.033       | No       | refresh period (in s) of the broadcasted values through yarp ports               | default 0.03s |
 * |   | name           | string  | -              |   /grabber    | No       | Prefix name of the ports opened by the FrameGrabber_nws_yarp                     | Required suffix like '/rpc' will be added by the device |
 * |   | capabilities   | string  | -              |   COLOR       | No       | two capabilities supported, COLOR and RAW respectively for rgb and raw streaming | - |
 * |   | no_drop        | bool    | -              |   true        | No       | if present, use strict policy for sending data | - |
