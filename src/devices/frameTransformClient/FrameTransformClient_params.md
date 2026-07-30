* |  | custom_config_from            | string     | -   |                       | No   | xml configuration file name |  |
* |  | custom_config_context         | string     | -   |                       | No   | xml configuration file context |  |
* |  | config_name                   | string     | -   |  ftc_local_only.xml   | No   | The name of the xml file containing the needed client configuration. if used, the name will be appended to config_xml/ | it is mutually exclusive with custom_config_from, custom_config_context options |
* |  | local_rpc                     | string     | -   |  /ftClient/rpc        | No   | Full name of the client rpc port | - |
* |  | FrameTransform_verbose_debug  | bool       | -   |  false                | No   | Enable debug prints | - |
* |  | period                        | double     | s   |  0.10                 | No   | thread period  | - |
