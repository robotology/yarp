/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

/*
//! [Demo.thrift Old]
service Demo
{
  i32 get_answer();
  bool set_answer(1:i32 rightAnswer)
}
//! [Demo.thrift Old]
*/

//! [Demo.thrift New]
service Demo
{
  i32 get_answer();
  bool set_answer(1:i32 rightAnswer)
} (
    yarp.monitor = "true"
)
//! [Demo.thrift New]
