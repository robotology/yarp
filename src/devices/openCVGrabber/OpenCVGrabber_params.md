 * | Group    | Name                  | Type    | Units    | Default Value       | Required     | Description                                                       | Notes |
 * |:--------:|:---------------------:|:-------:|:--------:|:-------------------:|:------------:|:-----------------------------------------------------------------:|:-----:|
 * |          | movie                 | string  | -        | -                   | No           | if present, read an .avi file instead of opening a camera         |       |
 * |          | loop                  | bool    | -        | false               | No           | if true, and movie parameter is set, enable the loop playback of the file |       |
 * |          | camera                | int     | -        | 0                   | No           | Id of the camera hardware device                              |       |
 * |          | framerate             | double  | -        | -1                  | No           | Framerate. Default value obtained by the hardware             |       |
 * |          | width                 | int     | -        | 0                   | No           | Width of the frame. Default value obtained by the hardware    |       |
 * |          | height                | int     | -        | 0                   | No           | Height of the frame. Default value obtained by the hardware   |       |
 * |          | flip_x                | bool    | -        | false               | No           | Flip along the x axis          |       |
 * |          | flip_y                | bool    | -        | false               | No           | flip along the y axis          |       |
 * |          | transpose             | bool    | -        | false               | No           | Rotate the image by 90 degrees |       |
