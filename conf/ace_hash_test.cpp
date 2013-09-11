// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright: (C) 2013 iCub Facility
 * Author: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */

#include <string>
#include <ace/Hash_Map_Manager.h>
#include <ace/Null_Mutex.h>
#include <ace/Functor_String.h>

int main(int argc, char *argv[]) {
    ACE_Hash_Map_Manager<std::string,std::string,ACE_Null_Mutex> my_map;
    ACE_Hash_Map_Entry<std::string,std::string> *it = NULL;
    my_map.find("hello",it);
    return 0;
}
