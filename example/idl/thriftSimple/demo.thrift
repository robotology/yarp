/*
 * Copyright (C) 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

service Demo {
  i32 get_answer();
  bool set_answer(1:i32 rightAnswer)
  i32 add_one(1:i32 x);
  bool start();
  bool stop();
  bool is_running();
}
