* |  | testxml_from                  | string     | -   |                       | No   | xml configuration file name | NB: FOR TEST ONLY. |
* |  | testxml_context               | string     | -   |                       | No   | xml configuration file context | NB: FOR TEST ONLY. |
* |  | filexml_option                | string     | -   |  fts_yarp_only.xml    | No   | The name of the xml file containing the needed configuration. if used, the name will be appended to config_xml/ | it is mutually exclusive with testxml_from, testxml_context options |
* |  | local_rpc                     | string     | -   |  /ftServer/rpc        | No   | Full name of the rpc port | - |
* |  | FrameTransform_verbose_debug  | bool       | -   |  false                | No   | Enable debug prints    | - |
* |  | period                        | double     | s   |  0.10                 | No   | thread period  | - |
