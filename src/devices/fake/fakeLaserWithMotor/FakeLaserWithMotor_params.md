* |  | test                | string |   -   | use_pattern           | Yes      | Choose the modality   | It can be one of the following: no_obstacles, use_pattern, use_mapfile, use_constant, use_square_trap |
* |  | localization_port   | string |   -   | /fakeLaser/location:i | No       | Full name of the port to which device connects to receive the localization data   |  |
* |  | localization_server | string |   -   | /localizationServer   | No       | Full name of the port to which device connects to receive the localization data   |  |
* |  | localization_client | string |   -   | /fakeLaser/localizationClient | No       | Full name of the local transformClient opened by the device | It cannot be used togheter if localization_port parameter is set |
* |  | localization_device | string |   -   | localization2DClient  | No       | Type of localization device, e.g. localization2DClient, localization2D_nwc_yarp | It cannot be used togheter if localization_port parameter is set |
* | MAP_MODE | map_file            | string |   -   |       -               | No       | Full path to a .map file   | Mandatory if --test use_mapfile option has been set |
* | MAP_MODE | map_context         | string |   -   |       -               | No       | Full path to a .map file   | Mandatory if --test use_mapfile option has been set |
* |  | clip_max            | double |   m   | 3.5                   | No       | Maximum detectable distance for an obstacle | - |
* |  | clip_min            | double |   m   | 0.1                   | No       | Minimum detectable distance for an obstacle | - |
* | GENERAL    | period    | double |   s   | 0.02                  | 0        | Thread period                               | - |
* | CONSTANT_MODE | const_distance |double | m | 1                  | 0        | Default const distance for mode use_constant | - |
