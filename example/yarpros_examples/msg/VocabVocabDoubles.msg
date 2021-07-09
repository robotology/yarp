# SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
# SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
# SPDX-License-Identifier: BSD-3-Clause

# The port we are communicating with can accept many kinds of commands.
# We set up a header here to make it clear we want to do the equivalent of:
#   [set] [poss] (1.0 2.0 3.0 4.0)
# Read as "set-plural-positions to 1.0 2.0 3.0 4.0"

int32 list_tag          # set to 256    (BOTTLE_TAG_LIST)
int32 list_len          # set to 3      (3 elements in list)
int32 vocab1_tag        # set to 9      (BOTTLE_TAG_VOCAB32)
int32 vocab1_val        # set to (256^2)*'s' + 256*'e' + 't'
int32 vocab2_tag        # set to 9      (BOTTLE_TAG_VOCAB32)
int32 vocab2_val        # set to (256^3)*'p' + (256^2)*'o' + 256*'s' + 's'
int32 setpoints_tag     # set to 256+10 (BOTTLE_TAG_LIST+BOTTLE_TAG_FLOAT64)

float64[] setpoints
