/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

// This is an automatically generated file.
// It could get re-generated if the ALLOW_IDL_GENERATION flag is on.

#include <SensorRPCData.h>

bool SensorRPCData::read_ThreeAxisGyroscopes(yarp::os::idl::WireReader& reader) {
  {
    ThreeAxisGyroscopes.clear();
    uint32_t _size12;
    yarp::os::idl::WireState _etype15;
    reader.readListBegin(_etype15, _size12);
    ThreeAxisGyroscopes.resize(_size12);
    uint32_t _i16;
    for (_i16 = 0; _i16 < _size12; ++_i16)
    {
      if (!reader.readNested(ThreeAxisGyroscopes[_i16])) {
        reader.fail();
        return false;
      }
    }
    reader.readListEnd();
  }
  return true;
}
bool SensorRPCData::nested_read_ThreeAxisGyroscopes(yarp::os::idl::WireReader& reader) {
  {
    ThreeAxisGyroscopes.clear();
    uint32_t _size17;
    yarp::os::idl::WireState _etype20;
    reader.readListBegin(_etype20, _size17);
    ThreeAxisGyroscopes.resize(_size17);
    uint32_t _i21;
    for (_i21 = 0; _i21 < _size17; ++_i21)
    {
      if (!reader.readNested(ThreeAxisGyroscopes[_i21])) {
        reader.fail();
        return false;
      }
    }
    reader.readListEnd();
  }
  return true;
}
bool SensorRPCData::read_ThreeAxisLinearAccelerometers(yarp::os::idl::WireReader& reader) {
  {
    ThreeAxisLinearAccelerometers.clear();
    uint32_t _size22;
    yarp::os::idl::WireState _etype25;
    reader.readListBegin(_etype25, _size22);
    ThreeAxisLinearAccelerometers.resize(_size22);
    uint32_t _i26;
    for (_i26 = 0; _i26 < _size22; ++_i26)
    {
      if (!reader.readNested(ThreeAxisLinearAccelerometers[_i26])) {
        reader.fail();
        return false;
      }
    }
    reader.readListEnd();
  }
  return true;
}
bool SensorRPCData::nested_read_ThreeAxisLinearAccelerometers(yarp::os::idl::WireReader& reader) {
  {
    ThreeAxisLinearAccelerometers.clear();
    uint32_t _size27;
    yarp::os::idl::WireState _etype30;
    reader.readListBegin(_etype30, _size27);
    ThreeAxisLinearAccelerometers.resize(_size27);
    uint32_t _i31;
    for (_i31 = 0; _i31 < _size27; ++_i31)
    {
      if (!reader.readNested(ThreeAxisLinearAccelerometers[_i31])) {
        reader.fail();
        return false;
      }
    }
    reader.readListEnd();
  }
  return true;
}
bool SensorRPCData::read_ThreeAxisMagnetometers(yarp::os::idl::WireReader& reader) {
  {
    ThreeAxisMagnetometers.clear();
    uint32_t _size32;
    yarp::os::idl::WireState _etype35;
    reader.readListBegin(_etype35, _size32);
    ThreeAxisMagnetometers.resize(_size32);
    uint32_t _i36;
    for (_i36 = 0; _i36 < _size32; ++_i36)
    {
      if (!reader.readNested(ThreeAxisMagnetometers[_i36])) {
        reader.fail();
        return false;
      }
    }
    reader.readListEnd();
  }
  return true;
}
bool SensorRPCData::nested_read_ThreeAxisMagnetometers(yarp::os::idl::WireReader& reader) {
  {
    ThreeAxisMagnetometers.clear();
    uint32_t _size37;
    yarp::os::idl::WireState _etype40;
    reader.readListBegin(_etype40, _size37);
    ThreeAxisMagnetometers.resize(_size37);
    uint32_t _i41;
    for (_i41 = 0; _i41 < _size37; ++_i41)
    {
      if (!reader.readNested(ThreeAxisMagnetometers[_i41])) {
        reader.fail();
        return false;
      }
    }
    reader.readListEnd();
  }
  return true;
}
bool SensorRPCData::read_OrientationSensors(yarp::os::idl::WireReader& reader) {
  {
    OrientationSensors.clear();
    uint32_t _size42;
    yarp::os::idl::WireState _etype45;
    reader.readListBegin(_etype45, _size42);
    OrientationSensors.resize(_size42);
    uint32_t _i46;
    for (_i46 = 0; _i46 < _size42; ++_i46)
    {
      if (!reader.readNested(OrientationSensors[_i46])) {
        reader.fail();
        return false;
      }
    }
    reader.readListEnd();
  }
  return true;
}
bool SensorRPCData::nested_read_OrientationSensors(yarp::os::idl::WireReader& reader) {
  {
    OrientationSensors.clear();
    uint32_t _size47;
    yarp::os::idl::WireState _etype50;
    reader.readListBegin(_etype50, _size47);
    OrientationSensors.resize(_size47);
    uint32_t _i51;
    for (_i51 = 0; _i51 < _size47; ++_i51)
    {
      if (!reader.readNested(OrientationSensors[_i51])) {
        reader.fail();
        return false;
      }
    }
    reader.readListEnd();
  }
  return true;
}
bool SensorRPCData::read_TemperatureSensors(yarp::os::idl::WireReader& reader) {
  {
    TemperatureSensors.clear();
    uint32_t _size52;
    yarp::os::idl::WireState _etype55;
    reader.readListBegin(_etype55, _size52);
    TemperatureSensors.resize(_size52);
    uint32_t _i56;
    for (_i56 = 0; _i56 < _size52; ++_i56)
    {
      if (!reader.readNested(TemperatureSensors[_i56])) {
        reader.fail();
        return false;
      }
    }
    reader.readListEnd();
  }
  return true;
}
bool SensorRPCData::nested_read_TemperatureSensors(yarp::os::idl::WireReader& reader) {
  {
    TemperatureSensors.clear();
    uint32_t _size57;
    yarp::os::idl::WireState _etype60;
    reader.readListBegin(_etype60, _size57);
    TemperatureSensors.resize(_size57);
    uint32_t _i61;
    for (_i61 = 0; _i61 < _size57; ++_i61)
    {
      if (!reader.readNested(TemperatureSensors[_i61])) {
        reader.fail();
        return false;
      }
    }
    reader.readListEnd();
  }
  return true;
}
bool SensorRPCData::read_SixAxisForceTorqueSensors(yarp::os::idl::WireReader& reader) {
  {
    SixAxisForceTorqueSensors.clear();
    uint32_t _size62;
    yarp::os::idl::WireState _etype65;
    reader.readListBegin(_etype65, _size62);
    SixAxisForceTorqueSensors.resize(_size62);
    uint32_t _i66;
    for (_i66 = 0; _i66 < _size62; ++_i66)
    {
      if (!reader.readNested(SixAxisForceTorqueSensors[_i66])) {
        reader.fail();
        return false;
      }
    }
    reader.readListEnd();
  }
  return true;
}
bool SensorRPCData::nested_read_SixAxisForceTorqueSensors(yarp::os::idl::WireReader& reader) {
  {
    SixAxisForceTorqueSensors.clear();
    uint32_t _size67;
    yarp::os::idl::WireState _etype70;
    reader.readListBegin(_etype70, _size67);
    SixAxisForceTorqueSensors.resize(_size67);
    uint32_t _i71;
    for (_i71 = 0; _i71 < _size67; ++_i71)
    {
      if (!reader.readNested(SixAxisForceTorqueSensors[_i71])) {
        reader.fail();
        return false;
      }
    }
    reader.readListEnd();
  }
  return true;
}
bool SensorRPCData::read_ContactLoadCellArrays(yarp::os::idl::WireReader& reader) {
  {
    ContactLoadCellArrays.clear();
    uint32_t _size72;
    yarp::os::idl::WireState _etype75;
    reader.readListBegin(_etype75, _size72);
    ContactLoadCellArrays.resize(_size72);
    uint32_t _i76;
    for (_i76 = 0; _i76 < _size72; ++_i76)
    {
      if (!reader.readNested(ContactLoadCellArrays[_i76])) {
        reader.fail();
        return false;
      }
    }
    reader.readListEnd();
  }
  return true;
}
bool SensorRPCData::nested_read_ContactLoadCellArrays(yarp::os::idl::WireReader& reader) {
  {
    ContactLoadCellArrays.clear();
    uint32_t _size77;
    yarp::os::idl::WireState _etype80;
    reader.readListBegin(_etype80, _size77);
    ContactLoadCellArrays.resize(_size77);
    uint32_t _i81;
    for (_i81 = 0; _i81 < _size77; ++_i81)
    {
      if (!reader.readNested(ContactLoadCellArrays[_i81])) {
        reader.fail();
        return false;
      }
    }
    reader.readListEnd();
  }
  return true;
}
bool SensorRPCData::read_EncoderArrays(yarp::os::idl::WireReader& reader) {
  {
    EncoderArrays.clear();
    uint32_t _size82;
    yarp::os::idl::WireState _etype85;
    reader.readListBegin(_etype85, _size82);
    EncoderArrays.resize(_size82);
    uint32_t _i86;
    for (_i86 = 0; _i86 < _size82; ++_i86)
    {
      if (!reader.readNested(EncoderArrays[_i86])) {
        reader.fail();
        return false;
      }
    }
    reader.readListEnd();
  }
  return true;
}
bool SensorRPCData::nested_read_EncoderArrays(yarp::os::idl::WireReader& reader) {
  {
    EncoderArrays.clear();
    uint32_t _size87;
    yarp::os::idl::WireState _etype90;
    reader.readListBegin(_etype90, _size87);
    EncoderArrays.resize(_size87);
    uint32_t _i91;
    for (_i91 = 0; _i91 < _size87; ++_i91)
    {
      if (!reader.readNested(EncoderArrays[_i91])) {
        reader.fail();
        return false;
      }
    }
    reader.readListEnd();
  }
  return true;
}
bool SensorRPCData::read_SkinPatches(yarp::os::idl::WireReader& reader) {
  {
    SkinPatches.clear();
    uint32_t _size92;
    yarp::os::idl::WireState _etype95;
    reader.readListBegin(_etype95, _size92);
    SkinPatches.resize(_size92);
    uint32_t _i96;
    for (_i96 = 0; _i96 < _size92; ++_i96)
    {
      if (!reader.readNested(SkinPatches[_i96])) {
        reader.fail();
        return false;
      }
    }
    reader.readListEnd();
  }
  return true;
}
bool SensorRPCData::nested_read_SkinPatches(yarp::os::idl::WireReader& reader) {
  {
    SkinPatches.clear();
    uint32_t _size97;
    yarp::os::idl::WireState _etype100;
    reader.readListBegin(_etype100, _size97);
    SkinPatches.resize(_size97);
    uint32_t _i101;
    for (_i101 = 0; _i101 < _size97; ++_i101)
    {
      if (!reader.readNested(SkinPatches[_i101])) {
        reader.fail();
        return false;
      }
    }
    reader.readListEnd();
  }
  return true;
}
bool SensorRPCData::read(yarp::os::idl::WireReader& reader) {
  if (!read_ThreeAxisGyroscopes(reader)) return false;
  if (!read_ThreeAxisLinearAccelerometers(reader)) return false;
  if (!read_ThreeAxisMagnetometers(reader)) return false;
  if (!read_OrientationSensors(reader)) return false;
  if (!read_TemperatureSensors(reader)) return false;
  if (!read_SixAxisForceTorqueSensors(reader)) return false;
  if (!read_ContactLoadCellArrays(reader)) return false;
  if (!read_EncoderArrays(reader)) return false;
  if (!read_SkinPatches(reader)) return false;
  return !reader.isError();
}

bool SensorRPCData::read(yarp::os::ConnectionReader& connection) {
  yarp::os::idl::WireReader reader(connection);
  if (!reader.readListHeader(9)) return false;
  return read(reader);
}

bool SensorRPCData::write_ThreeAxisGyroscopes(const yarp::os::idl::WireWriter& writer) const {
  {
    if (!writer.writeListBegin(BOTTLE_TAG_LIST, static_cast<uint32_t>(ThreeAxisGyroscopes.size()))) return false;
    std::vector<SensorMetadata> ::const_iterator _iter102;
    for (_iter102 = ThreeAxisGyroscopes.begin(); _iter102 != ThreeAxisGyroscopes.end(); ++_iter102)
    {
      if (!writer.writeNested((*_iter102))) return false;
    }
    if (!writer.writeListEnd()) return false;
  }
  return true;
}
bool SensorRPCData::nested_write_ThreeAxisGyroscopes(const yarp::os::idl::WireWriter& writer) const {
  {
    if (!writer.writeListBegin(BOTTLE_TAG_LIST, static_cast<uint32_t>(ThreeAxisGyroscopes.size()))) return false;
    std::vector<SensorMetadata> ::const_iterator _iter103;
    for (_iter103 = ThreeAxisGyroscopes.begin(); _iter103 != ThreeAxisGyroscopes.end(); ++_iter103)
    {
      if (!writer.writeNested((*_iter103))) return false;
    }
    if (!writer.writeListEnd()) return false;
  }
  return true;
}
bool SensorRPCData::write_ThreeAxisLinearAccelerometers(const yarp::os::idl::WireWriter& writer) const {
  {
    if (!writer.writeListBegin(BOTTLE_TAG_LIST, static_cast<uint32_t>(ThreeAxisLinearAccelerometers.size()))) return false;
    std::vector<SensorMetadata> ::const_iterator _iter104;
    for (_iter104 = ThreeAxisLinearAccelerometers.begin(); _iter104 != ThreeAxisLinearAccelerometers.end(); ++_iter104)
    {
      if (!writer.writeNested((*_iter104))) return false;
    }
    if (!writer.writeListEnd()) return false;
  }
  return true;
}
bool SensorRPCData::nested_write_ThreeAxisLinearAccelerometers(const yarp::os::idl::WireWriter& writer) const {
  {
    if (!writer.writeListBegin(BOTTLE_TAG_LIST, static_cast<uint32_t>(ThreeAxisLinearAccelerometers.size()))) return false;
    std::vector<SensorMetadata> ::const_iterator _iter105;
    for (_iter105 = ThreeAxisLinearAccelerometers.begin(); _iter105 != ThreeAxisLinearAccelerometers.end(); ++_iter105)
    {
      if (!writer.writeNested((*_iter105))) return false;
    }
    if (!writer.writeListEnd()) return false;
  }
  return true;
}
bool SensorRPCData::write_ThreeAxisMagnetometers(const yarp::os::idl::WireWriter& writer) const {
  {
    if (!writer.writeListBegin(BOTTLE_TAG_LIST, static_cast<uint32_t>(ThreeAxisMagnetometers.size()))) return false;
    std::vector<SensorMetadata> ::const_iterator _iter106;
    for (_iter106 = ThreeAxisMagnetometers.begin(); _iter106 != ThreeAxisMagnetometers.end(); ++_iter106)
    {
      if (!writer.writeNested((*_iter106))) return false;
    }
    if (!writer.writeListEnd()) return false;
  }
  return true;
}
bool SensorRPCData::nested_write_ThreeAxisMagnetometers(const yarp::os::idl::WireWriter& writer) const {
  {
    if (!writer.writeListBegin(BOTTLE_TAG_LIST, static_cast<uint32_t>(ThreeAxisMagnetometers.size()))) return false;
    std::vector<SensorMetadata> ::const_iterator _iter107;
    for (_iter107 = ThreeAxisMagnetometers.begin(); _iter107 != ThreeAxisMagnetometers.end(); ++_iter107)
    {
      if (!writer.writeNested((*_iter107))) return false;
    }
    if (!writer.writeListEnd()) return false;
  }
  return true;
}
bool SensorRPCData::write_OrientationSensors(const yarp::os::idl::WireWriter& writer) const {
  {
    if (!writer.writeListBegin(BOTTLE_TAG_LIST, static_cast<uint32_t>(OrientationSensors.size()))) return false;
    std::vector<SensorMetadata> ::const_iterator _iter108;
    for (_iter108 = OrientationSensors.begin(); _iter108 != OrientationSensors.end(); ++_iter108)
    {
      if (!writer.writeNested((*_iter108))) return false;
    }
    if (!writer.writeListEnd()) return false;
  }
  return true;
}
bool SensorRPCData::nested_write_OrientationSensors(const yarp::os::idl::WireWriter& writer) const {
  {
    if (!writer.writeListBegin(BOTTLE_TAG_LIST, static_cast<uint32_t>(OrientationSensors.size()))) return false;
    std::vector<SensorMetadata> ::const_iterator _iter109;
    for (_iter109 = OrientationSensors.begin(); _iter109 != OrientationSensors.end(); ++_iter109)
    {
      if (!writer.writeNested((*_iter109))) return false;
    }
    if (!writer.writeListEnd()) return false;
  }
  return true;
}
bool SensorRPCData::write_TemperatureSensors(const yarp::os::idl::WireWriter& writer) const {
  {
    if (!writer.writeListBegin(BOTTLE_TAG_LIST, static_cast<uint32_t>(TemperatureSensors.size()))) return false;
    std::vector<SensorMetadata> ::const_iterator _iter110;
    for (_iter110 = TemperatureSensors.begin(); _iter110 != TemperatureSensors.end(); ++_iter110)
    {
      if (!writer.writeNested((*_iter110))) return false;
    }
    if (!writer.writeListEnd()) return false;
  }
  return true;
}
bool SensorRPCData::nested_write_TemperatureSensors(const yarp::os::idl::WireWriter& writer) const {
  {
    if (!writer.writeListBegin(BOTTLE_TAG_LIST, static_cast<uint32_t>(TemperatureSensors.size()))) return false;
    std::vector<SensorMetadata> ::const_iterator _iter111;
    for (_iter111 = TemperatureSensors.begin(); _iter111 != TemperatureSensors.end(); ++_iter111)
    {
      if (!writer.writeNested((*_iter111))) return false;
    }
    if (!writer.writeListEnd()) return false;
  }
  return true;
}
bool SensorRPCData::write_SixAxisForceTorqueSensors(const yarp::os::idl::WireWriter& writer) const {
  {
    if (!writer.writeListBegin(BOTTLE_TAG_LIST, static_cast<uint32_t>(SixAxisForceTorqueSensors.size()))) return false;
    std::vector<SensorMetadata> ::const_iterator _iter112;
    for (_iter112 = SixAxisForceTorqueSensors.begin(); _iter112 != SixAxisForceTorqueSensors.end(); ++_iter112)
    {
      if (!writer.writeNested((*_iter112))) return false;
    }
    if (!writer.writeListEnd()) return false;
  }
  return true;
}
bool SensorRPCData::nested_write_SixAxisForceTorqueSensors(const yarp::os::idl::WireWriter& writer) const {
  {
    if (!writer.writeListBegin(BOTTLE_TAG_LIST, static_cast<uint32_t>(SixAxisForceTorqueSensors.size()))) return false;
    std::vector<SensorMetadata> ::const_iterator _iter113;
    for (_iter113 = SixAxisForceTorqueSensors.begin(); _iter113 != SixAxisForceTorqueSensors.end(); ++_iter113)
    {
      if (!writer.writeNested((*_iter113))) return false;
    }
    if (!writer.writeListEnd()) return false;
  }
  return true;
}
bool SensorRPCData::write_ContactLoadCellArrays(const yarp::os::idl::WireWriter& writer) const {
  {
    if (!writer.writeListBegin(BOTTLE_TAG_LIST, static_cast<uint32_t>(ContactLoadCellArrays.size()))) return false;
    std::vector<SensorMetadata> ::const_iterator _iter114;
    for (_iter114 = ContactLoadCellArrays.begin(); _iter114 != ContactLoadCellArrays.end(); ++_iter114)
    {
      if (!writer.writeNested((*_iter114))) return false;
    }
    if (!writer.writeListEnd()) return false;
  }
  return true;
}
bool SensorRPCData::nested_write_ContactLoadCellArrays(const yarp::os::idl::WireWriter& writer) const {
  {
    if (!writer.writeListBegin(BOTTLE_TAG_LIST, static_cast<uint32_t>(ContactLoadCellArrays.size()))) return false;
    std::vector<SensorMetadata> ::const_iterator _iter115;
    for (_iter115 = ContactLoadCellArrays.begin(); _iter115 != ContactLoadCellArrays.end(); ++_iter115)
    {
      if (!writer.writeNested((*_iter115))) return false;
    }
    if (!writer.writeListEnd()) return false;
  }
  return true;
}
bool SensorRPCData::write_EncoderArrays(const yarp::os::idl::WireWriter& writer) const {
  {
    if (!writer.writeListBegin(BOTTLE_TAG_LIST, static_cast<uint32_t>(EncoderArrays.size()))) return false;
    std::vector<SensorMetadata> ::const_iterator _iter116;
    for (_iter116 = EncoderArrays.begin(); _iter116 != EncoderArrays.end(); ++_iter116)
    {
      if (!writer.writeNested((*_iter116))) return false;
    }
    if (!writer.writeListEnd()) return false;
  }
  return true;
}
bool SensorRPCData::nested_write_EncoderArrays(const yarp::os::idl::WireWriter& writer) const {
  {
    if (!writer.writeListBegin(BOTTLE_TAG_LIST, static_cast<uint32_t>(EncoderArrays.size()))) return false;
    std::vector<SensorMetadata> ::const_iterator _iter117;
    for (_iter117 = EncoderArrays.begin(); _iter117 != EncoderArrays.end(); ++_iter117)
    {
      if (!writer.writeNested((*_iter117))) return false;
    }
    if (!writer.writeListEnd()) return false;
  }
  return true;
}
bool SensorRPCData::write_SkinPatches(const yarp::os::idl::WireWriter& writer) const {
  {
    if (!writer.writeListBegin(BOTTLE_TAG_LIST, static_cast<uint32_t>(SkinPatches.size()))) return false;
    std::vector<SensorMetadata> ::const_iterator _iter118;
    for (_iter118 = SkinPatches.begin(); _iter118 != SkinPatches.end(); ++_iter118)
    {
      if (!writer.writeNested((*_iter118))) return false;
    }
    if (!writer.writeListEnd()) return false;
  }
  return true;
}
bool SensorRPCData::nested_write_SkinPatches(const yarp::os::idl::WireWriter& writer) const {
  {
    if (!writer.writeListBegin(BOTTLE_TAG_LIST, static_cast<uint32_t>(SkinPatches.size()))) return false;
    std::vector<SensorMetadata> ::const_iterator _iter119;
    for (_iter119 = SkinPatches.begin(); _iter119 != SkinPatches.end(); ++_iter119)
    {
      if (!writer.writeNested((*_iter119))) return false;
    }
    if (!writer.writeListEnd()) return false;
  }
  return true;
}
bool SensorRPCData::write(const yarp::os::idl::WireWriter& writer) const {
  if (!write_ThreeAxisGyroscopes(writer)) return false;
  if (!write_ThreeAxisLinearAccelerometers(writer)) return false;
  if (!write_ThreeAxisMagnetometers(writer)) return false;
  if (!write_OrientationSensors(writer)) return false;
  if (!write_TemperatureSensors(writer)) return false;
  if (!write_SixAxisForceTorqueSensors(writer)) return false;
  if (!write_ContactLoadCellArrays(writer)) return false;
  if (!write_EncoderArrays(writer)) return false;
  if (!write_SkinPatches(writer)) return false;
  return !writer.isError();
}

bool SensorRPCData::write(yarp::os::ConnectionWriter& connection) const {
  yarp::os::idl::WireWriter writer(connection);
  if (!writer.writeListHeader(9)) return false;
  return write(writer);
}
bool SensorRPCData::Editor::write(yarp::os::ConnectionWriter& connection) const {
  if (!isValid()) return false;
  yarp::os::idl::WireWriter writer(connection);
  if (!writer.writeListHeader(dirty_count+1)) return false;
  if (!writer.writeString("patch")) return false;
  if (is_dirty_ThreeAxisGyroscopes) {
    if (!writer.writeListHeader(3)) return false;
    if (!writer.writeString("set")) return false;
    if (!writer.writeString("ThreeAxisGyroscopes")) return false;
    if (!obj->nested_write_ThreeAxisGyroscopes(writer)) return false;
  }
  if (is_dirty_ThreeAxisLinearAccelerometers) {
    if (!writer.writeListHeader(3)) return false;
    if (!writer.writeString("set")) return false;
    if (!writer.writeString("ThreeAxisLinearAccelerometers")) return false;
    if (!obj->nested_write_ThreeAxisLinearAccelerometers(writer)) return false;
  }
  if (is_dirty_ThreeAxisMagnetometers) {
    if (!writer.writeListHeader(3)) return false;
    if (!writer.writeString("set")) return false;
    if (!writer.writeString("ThreeAxisMagnetometers")) return false;
    if (!obj->nested_write_ThreeAxisMagnetometers(writer)) return false;
  }
  if (is_dirty_OrientationSensors) {
    if (!writer.writeListHeader(3)) return false;
    if (!writer.writeString("set")) return false;
    if (!writer.writeString("OrientationSensors")) return false;
    if (!obj->nested_write_OrientationSensors(writer)) return false;
  }
  if (is_dirty_TemperatureSensors) {
    if (!writer.writeListHeader(3)) return false;
    if (!writer.writeString("set")) return false;
    if (!writer.writeString("TemperatureSensors")) return false;
    if (!obj->nested_write_TemperatureSensors(writer)) return false;
  }
  if (is_dirty_SixAxisForceTorqueSensors) {
    if (!writer.writeListHeader(3)) return false;
    if (!writer.writeString("set")) return false;
    if (!writer.writeString("SixAxisForceTorqueSensors")) return false;
    if (!obj->nested_write_SixAxisForceTorqueSensors(writer)) return false;
  }
  if (is_dirty_ContactLoadCellArrays) {
    if (!writer.writeListHeader(3)) return false;
    if (!writer.writeString("set")) return false;
    if (!writer.writeString("ContactLoadCellArrays")) return false;
    if (!obj->nested_write_ContactLoadCellArrays(writer)) return false;
  }
  if (is_dirty_EncoderArrays) {
    if (!writer.writeListHeader(3)) return false;
    if (!writer.writeString("set")) return false;
    if (!writer.writeString("EncoderArrays")) return false;
    if (!obj->nested_write_EncoderArrays(writer)) return false;
  }
  if (is_dirty_SkinPatches) {
    if (!writer.writeListHeader(3)) return false;
    if (!writer.writeString("set")) return false;
    if (!writer.writeString("SkinPatches")) return false;
    if (!obj->nested_write_SkinPatches(writer)) return false;
  }
  return !writer.isError();
}
bool SensorRPCData::Editor::read(yarp::os::ConnectionReader& connection) {
  if (!isValid()) return false;
  yarp::os::idl::WireReader reader(connection);
  reader.expectAccept();
  if (!reader.readListHeader()) return false;
  int len = reader.getLength();
  if (len==0) {
    yarp::os::idl::WireWriter writer(reader);
    if (writer.isNull()) return true;
    if (!writer.writeListHeader(1)) return false;
    writer.writeString("send: 'help' or 'patch (param1 val1) (param2 val2)'");
    return true;
  }
  std::string tag;
  if (!reader.readString(tag)) return false;
  if (tag=="help") {
    yarp::os::idl::WireWriter writer(reader);
    if (writer.isNull()) return true;
    if (!writer.writeListHeader(2)) return false;
    if (!writer.writeTag("many",1, 0)) return false;
    if (reader.getLength()>0) {
      std::string field;
      if (!reader.readString(field)) return false;
      if (field=="ThreeAxisGyroscopes") {
        if (!writer.writeListHeader(1)) return false;
        if (!writer.writeString("std::vector<SensorMetadata>  ThreeAxisGyroscopes")) return false;
      }
      if (field=="ThreeAxisLinearAccelerometers") {
        if (!writer.writeListHeader(1)) return false;
        if (!writer.writeString("std::vector<SensorMetadata>  ThreeAxisLinearAccelerometers")) return false;
      }
      if (field=="ThreeAxisMagnetometers") {
        if (!writer.writeListHeader(1)) return false;
        if (!writer.writeString("std::vector<SensorMetadata>  ThreeAxisMagnetometers")) return false;
      }
      if (field=="OrientationSensors") {
        if (!writer.writeListHeader(1)) return false;
        if (!writer.writeString("std::vector<SensorMetadata>  OrientationSensors")) return false;
      }
      if (field=="TemperatureSensors") {
        if (!writer.writeListHeader(1)) return false;
        if (!writer.writeString("std::vector<SensorMetadata>  TemperatureSensors")) return false;
      }
      if (field=="SixAxisForceTorqueSensors") {
        if (!writer.writeListHeader(1)) return false;
        if (!writer.writeString("std::vector<SensorMetadata>  SixAxisForceTorqueSensors")) return false;
      }
      if (field=="ContactLoadCellArrays") {
        if (!writer.writeListHeader(1)) return false;
        if (!writer.writeString("std::vector<SensorMetadata>  ContactLoadCellArrays")) return false;
      }
      if (field=="EncoderArrays") {
        if (!writer.writeListHeader(1)) return false;
        if (!writer.writeString("std::vector<SensorMetadata>  EncoderArrays")) return false;
      }
      if (field=="SkinPatches") {
        if (!writer.writeListHeader(1)) return false;
        if (!writer.writeString("std::vector<SensorMetadata>  SkinPatches")) return false;
      }
    }
    if (!writer.writeListHeader(10)) return false;
    writer.writeString("*** Available fields:");
    writer.writeString("ThreeAxisGyroscopes");
    writer.writeString("ThreeAxisLinearAccelerometers");
    writer.writeString("ThreeAxisMagnetometers");
    writer.writeString("OrientationSensors");
    writer.writeString("TemperatureSensors");
    writer.writeString("SixAxisForceTorqueSensors");
    writer.writeString("ContactLoadCellArrays");
    writer.writeString("EncoderArrays");
    writer.writeString("SkinPatches");
    return true;
  }
  bool nested = true;
  bool have_act = false;
  if (tag!="patch") {
    if ((len-1)%2 != 0) return false;
    len = 1 + ((len-1)/2);
    nested = false;
    have_act = true;
  }
  for (int i=1; i<len; i++) {
    if (nested && !reader.readListHeader(3)) return false;
    std::string act;
    std::string key;
    if (have_act) {
      act = tag;
    } else {
      if (!reader.readString(act)) return false;
    }
    if (!reader.readString(key)) return false;
    // inefficient code follows, bug paulfitz to improve it
    if (key == "ThreeAxisGyroscopes") {
      will_set_ThreeAxisGyroscopes();
      if (!obj->nested_read_ThreeAxisGyroscopes(reader)) return false;
      did_set_ThreeAxisGyroscopes();
    } else if (key == "ThreeAxisLinearAccelerometers") {
      will_set_ThreeAxisLinearAccelerometers();
      if (!obj->nested_read_ThreeAxisLinearAccelerometers(reader)) return false;
      did_set_ThreeAxisLinearAccelerometers();
    } else if (key == "ThreeAxisMagnetometers") {
      will_set_ThreeAxisMagnetometers();
      if (!obj->nested_read_ThreeAxisMagnetometers(reader)) return false;
      did_set_ThreeAxisMagnetometers();
    } else if (key == "OrientationSensors") {
      will_set_OrientationSensors();
      if (!obj->nested_read_OrientationSensors(reader)) return false;
      did_set_OrientationSensors();
    } else if (key == "TemperatureSensors") {
      will_set_TemperatureSensors();
      if (!obj->nested_read_TemperatureSensors(reader)) return false;
      did_set_TemperatureSensors();
    } else if (key == "SixAxisForceTorqueSensors") {
      will_set_SixAxisForceTorqueSensors();
      if (!obj->nested_read_SixAxisForceTorqueSensors(reader)) return false;
      did_set_SixAxisForceTorqueSensors();
    } else if (key == "ContactLoadCellArrays") {
      will_set_ContactLoadCellArrays();
      if (!obj->nested_read_ContactLoadCellArrays(reader)) return false;
      did_set_ContactLoadCellArrays();
    } else if (key == "EncoderArrays") {
      will_set_EncoderArrays();
      if (!obj->nested_read_EncoderArrays(reader)) return false;
      did_set_EncoderArrays();
    } else if (key == "SkinPatches") {
      will_set_SkinPatches();
      if (!obj->nested_read_SkinPatches(reader)) return false;
      did_set_SkinPatches();
    } else {
      // would be useful to have a fallback here
    }
  }
  reader.accept();
  yarp::os::idl::WireWriter writer(reader);
  if (writer.isNull()) return true;
  writer.writeListHeader(1);
  writer.writeVocab(VOCAB2('o','k'));
  return true;
}

std::string SensorRPCData::toString() const {
  yarp::os::Bottle b;
  b.read(*this);
  return b.toString();
}
