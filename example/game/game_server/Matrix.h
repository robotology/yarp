/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 * Copyright (C) 2006-2010 RobotCub Consortium
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#ifndef MATRIX_H
#define MATRIX_H

#include <ace/Hash_Map_Manager.h>
#include <ace/Null_Mutex.h>
#include <assert.h>

#include <fstream>
//using namespace std;

#include "ID.h"

// just for windows
#define for if (1) for

class hline : public ACE_Hash_Map_Manager<ID,ID,ACE_Null_Mutex> {
public:
    hline() {
    }
    hline(const hline& alt) {
    }
};

//typedef ACE_Hash_Map_Manager<ID,ID,ACE_Null_Mutex> hline;

typedef ACE_Hash_Map_Manager<ID,ID,ACE_Null_Mutex> hid;
typedef ACE_Hash_Map_Iterator<ID,ID,ACE_Null_Mutex> hid_iterator;

typedef ACE_Hash_Map_Manager<ID,hline,ACE_Null_Mutex> hmatrix;
typedef ACE_Hash_Map_Entry<ID,hline> hmatrix_entry;
typedef ACE_Hash_Map_Iterator<ID,hline,ACE_Null_Mutex> hmatrix_iterator;

class Matrix {
private:
    hmatrix matrix;
public:
    ID get(ID x, ID y) {
        hmatrix_entry *entry;
        if (0==matrix.find(y,entry)) {
            ID result;
            if (0==entry->int_id_.find(x,result)) {
                return result;
            }
        }
        return ID(0);
    }

    void set(ID x, ID y, ID val) {
        hmatrix_entry *entry;
        if (matrix.find(y,entry)!=0) {
            matrix.bind(y,hline());
            int find_again = matrix.find(y,entry);
            assert(find_again==0);
        }
        entry->int_id_.rebind(x,val);
    }

    void save() {
        std::ofstream fout("/tmp/index.txt");
        hid ids;
        for (hmatrix_iterator it=matrix.begin(); it!=matrix.end(); it++) {
            ID y = (*it).ext_id_;
            hid& line = (*it).int_id_;
            for (hid_iterator it2=line.begin(); it2!=line.end(); it2++) {
                ID x = (*it2).ext_id_;
                ID v = (*it2).int_id_;
                printf("(%ld,%ld) = %ld\n",x.asInt(),y.asInt(),v.asInt());
                fout << "piece " << x.asInt() << " " << y.asInt() << " " << 
                    v.asInt() << std::endl;
                ids.rebind(v,v);
            }
        }
        for (hid_iterator it=ids.begin(); it!=ids.end(); it++) {
            ID v = (*it).ext_id_;
            printf("need state for id %ld\n", v.asInt());

            char fname[256];
            sprintf(fname, "/tmp/%ld.txt", v.asInt());

            fout << "state " << v.asInt() << " " << fname << std::endl;

            std::ofstream fout2(fname);
            fout2 << "id " << v.asInt() << std::endl;
        }
    }
};



#endif


