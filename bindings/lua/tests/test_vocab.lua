#!/usr/bin/lua

-- Copyright: (C) 2018 Juan G Victores
-- Author: Juan G Victores
-- Copy Policy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT

require("yarp")

function test_vocab()
  vocab = yarp.Vocab.encode("abcd")
  assert("number" == type(vocab))
  assert(1684234849 == vocab)
  assert("abcd" == yarp.Vocab.decode(vocab))
end

function test_vocab_pixel_types_enum()
  assert("number" == type(yarp.VOCAB_PIXEL_RGB))
  assert(6449010 == yarp.VOCAB_PIXEL_RGB) -- VOCAB3
  assert(761423730 == yarp.VOCAB_PIXEL_RGB_SIGNED) -- VOCAB4 with '-' 
  assert(909209453 == yarp.VOCAB_PIXEL_MONO16) -- VOCAB4 with '1' and '6'
end

test_vocab()
test_vocab_pixel_types_enum()

