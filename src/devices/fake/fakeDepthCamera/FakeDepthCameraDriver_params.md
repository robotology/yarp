  * |      |  rgb_h                  | double  | pixels  |   480.0        | No      | rgb_h       |  |
  * |      |  rgb_w                  | double  | pixels  |   640.0        | No      | rgb_w       |  |
  * |      |  dep_h                  | double  | pixels  |   480.0        | No      | dep_h       | probably it should be identical to rgb_h |
  * |      |  dep_w                  | double  | pixels  |   640.0        | No      | dep_w       | probably it should be identical to rgb_w |
  * |      |  accuracy               | double  | m       |   0.001        | No      | accuracy    |  |
  * |      |  rgb_Vfov               | double  | deg     |   50.0         | No      | rgb_Vfov    |  |
  * |      |  rgb_Hfov               | double  | deg     |   36.0         | No      | rgb_Hfov    |  |
  * |      |  dep_Vfov               | double  | deg     |   50.0         | No      | dep_Vfov    |  |
  * |      |  dep_Hfov               | double  | deg     |   36.0         | No      | dep_Hfov    |  |
  * |      |  dep_near               | double  | m       |   0.2          | No      | dep_near    |  |
  * |      |  dep_far                | double  | m       |   6.0          | No      | dep_far     |  |
  * |      |  test_distance_constant | double  | m       |   2.0          | No      | if set != 0, it will set all the pixels of the generated depth to constant distance of x meters  | if set == 0, it will fill the generated image to a gradient pattern |
