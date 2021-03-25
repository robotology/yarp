# sniffed from the iCub simulator
#   yarpros sniff out /icubSim/cam/left
# suggested values appropriate for 320x240 RGB

int32 v_tag # set to 256 (BOTTLE_TAG_LIST+code)
int32 v_len # set to 4 (elements in list)
int32 v0_tag # set to 9 (BOTTLE_TAG_VOCAB32)
int32 v0 # set to 7627117 (='m'*256^3+'a'*256^2+'t'*256=mat)
int32 v1_tag # set to 9 (BOTTLE_TAG_VOCAB32)
int32 v1 # set to 6449010 (='r'*256^3+'g'*256^2+'b'*256=rgb)
int32 v2_tag # set to 257 (BOTTLE_TAG_LIST+code)
int32[] v2 # suggested length: 5
 # integers seen:  3 230400 8 320 240
int32 v3_tag # set to 12 (BOTTLE_TAG_BLOB)
int8[] v3 # suggested length: 230400
