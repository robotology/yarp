/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

// This is an automatically generated file.
// It could get re-generated if the ALLOW_IDL_GENERATION flag is on.

#include <yarp/os/Wire.h>
#include <yarp/os/idl/WireTypes.h>
#include <firstInterface/PointQuality.h>

namespace yarp { namespace test {


int PointQualityVocab::fromString(const std::string& input) {
  // definitely needs optimizing :-)
  if (input=="UNKNOWN") return (int)UNKNOWN;
  if (input=="GOOD") return (int)GOOD;
  if (input=="BAD") return (int)BAD;
  return -1;
}
std::string PointQualityVocab::toString(int input) const {
  switch((PointQuality)input) {
  case UNKNOWN:
    return "UNKNOWN";
  case GOOD:
    return "GOOD";
  case BAD:
    return "BAD";
  }
  return "";
}
}} // namespace


