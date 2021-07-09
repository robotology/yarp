/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef DMATRIX_H
#define DMATRIX_H


#include <ace/Hash_Map_Manager.h>
#include <ace/Null_Mutex.h>
#include <assert.h>

#include <fstream>
//using namespace std;

#include "ID.h"

#include "Matrix.h"

class hdline : public ACE_Hash_Map_Manager<ID,double,ACE_Null_Mutex> {
public:
    hdline() {
    }
    hdline(const hdline& alt) {
    }
};

//typedef ACE_Hash_Map_Manager<ID,ID,ACE_Null_Mutex> hline;

typedef ACE_Hash_Map_Manager<ID,double,ACE_Null_Mutex> hd;
typedef ACE_Hash_Map_Iterator<ID,double,ACE_Null_Mutex> hd_iterator;

typedef ACE_Hash_Map_Manager<ID,hdline,ACE_Null_Mutex> hdmatrix;
typedef ACE_Hash_Map_Entry<ID,hdline> hdmatrix_entry;
typedef ACE_Hash_Map_Iterator<ID,hdline,ACE_Null_Mutex> hmdatrix_iterator;

class DMatrix {
private:
    hdmatrix matrix;
public:
    double get(ID x, ID y) {
        hdmatrix_entry *entry;
        if (0==matrix.find(y,entry)) {
            double result;
            if (0==entry->int_id_.find(x,result)) {
                return result;
            }
        }
        return 0;
    }

    void set(ID x, ID y, double val) {
        hdmatrix_entry *entry;
        if (matrix.find(y,entry)!=0) {
            matrix.bind(y,hdline());
            int find_again = matrix.find(y,entry);
            assert(find_again==0);
        }
        entry->int_id_.rebind(x,val);
    }
};



#endif
