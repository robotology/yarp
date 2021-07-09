/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
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
                printf("(%ld,%ld) = %ld\n",x.asInt32(),y.asInt32(),v.asInt32());
                fout << "piece " << x.asInt32() << " " << y.asInt32() << " " <<
                    v.asInt32() << std::endl;
                ids.rebind(v,v);
            }
        }
        for (hid_iterator it=ids.begin(); it!=ids.end(); it++) {
            ID v = (*it).ext_id_;
            printf("need state for id %ld\n", v.asInt32());

            char fname[256];
            sprintf(fname, "/tmp/%ld.txt", v.asInt32());

            fout << "state " << v.asInt32() << " " << fname << std::endl;

            std::ofstream fout2(fname);
            fout2 << "id " << v.asInt32() << std::endl;
        }
    }
};



#endif
