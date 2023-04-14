/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

typedef i32 ( yarp.type = "yarp::conf::vocab32_t" ) vocab
typedef i8 ( yarp.type = "std::uint8_t" ) ui8
typedef i16 ( yarp.type = "std::uint16_t" ) ui16
typedef i32 ( yarp.type = "std::uint32_t" ) ui32
typedef i64 ( yarp.type = "std::uint64_t" ) ui64
typedef i32 ( yarp.type = "size_t" ) size_t
typedef double ( yarp.type = "yarp::conf::float32_t" ) float32
typedef double ( yarp.type = "yarp::conf::float64_t" ) float64


struct TestAnnotatedTypes
{
  1: vocab a_vocab,
  2: ui8 a_ui8,
  3: ui16 a_ui16,
  4: ui32 a_ui32,
  5: ui64 a_ui64,
  6: float32 a_float32,
  7: float64 a_float64,
  8: size_t a_size;
}
