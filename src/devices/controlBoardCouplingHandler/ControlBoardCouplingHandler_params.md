| Group name      | Parameter name     | Type            | Units   | Default Value  | Required | Description                                    | Notes                |
|:---------------:|:------------------:|:---------------:|:-------:|:--------------:|:--------:|:----------------------------------------------:|:--------------------:|
|                 | axesNames          | vector<string>  | -       | -              |  Yes     | Names of the actuated axes                     |                      |
|                 | jointNames         | vector<string>  | -       | -              |  Yes     | Names of the physical joints                   |                      |
| LIMITS          |     -              | string          | -       | -              |  Yes     | string containing the physical joint limits    |                      |
| LIMITS          | jntPosMin          | vector<double>  | -       | -              |  Yes     | Phyisical joints' position minimum             |                      |
| LIMITS          | jntPosMax          | vector<double>  | -       | -              |  Yes     | Phyisical joints' position maximum             |                      |
| COUPLING        |     -              | string          | -       | -              |  Yes     | The string containing the coupling description |                      |
| COUPLING        | device             | string          | -       | -              |  Yes     | Name of the device that handles the coupling   |                      |
| COUPLING        | actuatedAxesNames  | vector<string>  | -       | -              |  Yes     | Names of the actuated axes                     |                      |
| COUPLING        | actuatedAxesPosMin | vector<string>  | -       | -              |  Yes     | Actuated axes' position minimum                |                      |
| COUPLING        | actuatedAxesPosMax | vector<string>  | -       | -              |  Yes     | Actuated axes' position maximum                |                      |
| COUPLING_PARAMS |     -              | string          | -       | -              |  Yes     | The string containing the coupling params      |                      |
