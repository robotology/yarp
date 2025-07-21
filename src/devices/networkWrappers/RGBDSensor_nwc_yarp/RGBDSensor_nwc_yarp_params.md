 * |   | period          | double  | s   |   0.03                     | No    | refresh period (in s) of the broadcasted values through yarp ports               | default 0.03s |
 * |   | localImagePort  | string  | -   |  /RGBD_nwc/Image:o         | Yes   | Full name of the local image streaming port to open | - |
 * |   | localDepthPort  | string  | -   |  /RGBD_nwc/Depth:o         | Yes   | Full name of the local depth streaming port to open | - |
 * |   | remoteImagePort | string  | -   |  /RGBD_nws/Image:o         | Yes   | Full name of the remote image port to connect to | - |
 * |   | remoteDepthPort | string  | -   |  /RGBD_nws/Depth:i         | Yes   | Full name of the remote depth port to connect to | - |
 * |   | localRpcPort    | string  | -   |  /RGBD_nwc/rpc:o           | Yes   | Full name of the local rpc port to open | - |
 * |   | remoteRpcPort   | string  | -   |  /RGBD_nws/rpc:i           | Yes   | Full name of the remote rpc port to connect to | - |
 * |   | ImageCarrier    | string  | -   |   udp                      | No    | Carrier for the image stream | - |
 * |   | DepthCarrier    | string  | -   |   udp                      | No    | Carrier for the depth stream | - |
