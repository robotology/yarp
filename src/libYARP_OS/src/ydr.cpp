/*
 * Copyright (C) 2006 RobotCub Consortium
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

// This file is disabled right now until types can be tested
// extensively on all platforms.
// One dummy function is included to avoid warnings on OSX
//   -- paulfitz


#if 0

#include <yarp/os/impl/ydr.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <yarp/os/all.h>
using namespace yarp::os;

// don't use any of these types until configuration has
// been tested on all OSes.
#ifdef u_int
#undef u_int
#endif
#ifdef u_short
#undef u_short
#endif
#ifdef bool_t
#undef bool_t
#endif
#ifdef u_char
#undef u_char
#endif
#ifdef size_t
#undef size_t
#endif
#define u_int unsigned int
#define u_short unsigned short
#define u_char unsigned char
#define bool_t bool
#define size_t unsigned int

#define control(x) (*((YDRHelper *)(x->controller)))

class YDRHelper {
public:
    bool save;
    Bottle b;
    Value last;
    int index;
    void *obuf;
    size_t obuflen;

    YDRHelper() {
        save = true;
        index = 0;
    }

    bool create(void* buf, size_t buflen, int op) {
        if (op==YDR_FREE) return true;
        save = (op==YDR_ENCODE);
        if (!save) {
            printf("reading binary\n");
            b.fromBinary((const char *)buf,buflen);
            printf("read binary\n");
        } else {
            b.clear();
        }
        obuf = buf;
        obuflen = buflen;
        return true;
    }

    Value& get() {
        Value& result = b.get(index);
        //printf("getting value %s\n", result.toString().c_str());
        index++;
        last = result;
        return result;
    }

    bool add(const Value& v) {
        //printf("adding value %s\n", v.toString().c_str());
        b.add(v);
        return true;
    }

    bool process_int(int *p) {
        if (save) {
            return add(Value((int)(*p)));
        } else {
            *p = (int)(get().asInt());
            return last.isInt();
        }
    }

    bool process_u_int(u_int *p) {
        if (save) {
            return add(Value((int)(*p)));
        } else {
            *p = (u_int)(get().asInt());
            return last.isInt();
        }
    }

    bool process_char(char *p) {
        if (save) {
            return add(Value((int)(*p)));
        } else {
            *p = (char)(get().asInt());
            return last.isInt();
        }
    }

    bool process_short(short *p) {
        if (save) {
            return add(Value((int)(*p)));
        } else {
            *p = (short)(get().asInt());
            return last.isInt();
        }
    }

    bool process_u_short(u_short *p) {
        if (save) {
            return add(Value((int)(*p)));
        } else {
            *p = (u_short)(get().asInt());
            return last.isInt();
        }
    }

    bool process_u_char(u_char *p) {
        if (save) {
            return add(Value((int)(*p)));
        } else {
            *p = (u_char)(get().asInt());
            return last.isInt();
        }
    }

    bool process_bool(bool_t *p) {
        if (save) {
            return add(Value((int)(*p)));
        } else {
            *p = (u_char)(get().asInt());
            return last.isInt();
        }
    }

    bool process_double(double *p) {
        if (save) {
            return add(Value((double)(*p)));
        } else {
            *p = (double)(get().asDouble());
            return last.isDouble();
        }
    }

    bool process_float(float *p) {
        if (save) {
            return add(Value((double)(*p)));
        } else {
            *p = (float)(get().asDouble());
            return last.isDouble();
        }
    }

    bool process_bytes(char **data, size_t *count, size_t max_count) {
        if (save) {
            return add(Value(*data,*count));
        } else {
            if (get().isBlob()) {
                unsigned int len = (unsigned int)last.asBlobLength();
                if (len<=max_count) {
                    const char *src = last.asBlob();
                    memcpy(*data,src,len);
                    *count = len;
                    return true;
                } else {
                    *count = 0;
                }
            }
            return false;
        }
    }

    size_t getpos() {
        int size = 0;
        b.toBinary(&size);
        return size;
    }

    bool destroy() {
        if (save) {
            int size = 0;
            const char *data = b.toBinary(&size);
            if ((unsigned int)size<obuflen) {
                memcpy(obuf,data,obuflen);
            }
        }
        return true;
    }
};

int ydr_u_int(YDR *xdrs, u_int *p) {
    return control(xdrs).process_u_int(p)?1:0;
}

int ydr_u_short(YDR *xdrs, u_short *p) {
    return control(xdrs).process_u_short(p)?1:0;
}

int ydr_u_char(YDR *xdrs, u_char *p) {
    return control(xdrs).process_u_char(p)?1:0;
}

int ydr_double(YDR *xdrs, double *p) {
    return control(xdrs).process_double(p)?1:0;
}

int ydr_float(YDR *xdrs, float *p) {
    return control(xdrs).process_float(p)?1:0;
}

int ydr_bool(YDR *xdrs, bool_t *p) {
    return control(xdrs).process_bool(p)?1:0;
}

int ydr_char(YDR *xdrs, char *p) {
    return control(xdrs).process_char(p)?1:0;
}

int ydr_short(YDR *xdrs, short *p) {
    return control(xdrs).process_short(p)?1:0;
}

int ydr_int(YDR *xdrs, int *p) {
    return control(xdrs).process_int(p)?1:0;
}

int ydr_bytes(YDR *xdrs, char **data, size_t *count, size_t max_count) {
    return control(xdrs).process_bytes(data,count,max_count)?1:0;
}

int ydr_array(YDR *ydrs, void *addr, unsigned int *size,
              unsigned int maxsize, unsigned int elsize,
              void *substore) {
    printf("ydr_array not implemented yet\n");
    return false;
}

int ydr_vector(YDR *ydrs, void *addr, unsigned int count,
              unsigned int elsize,
              void *substore) {
    printf("ydr_vector not implemented yet\n");
    return false;
}

int ydr_longlong_t(YDR *ydrs, long long int *p) {
    printf("ydr_longlong_t not implemented yet\n");
    return false;
}


u_int ydr_getpos(YDR *xdrs) {
    return control(xdrs).getpos();
}

void ydr_destroy(YDR *xdrs) {
    control(xdrs).destroy();
    delete &control(xdrs);
}

int ydrmem_create(YDR *xdrs, void* buf, size_t buflen, int op) {
    xdrs->controller = new YDRHelper;
    if (xdrs->controller == NULL) return 0;
    control(xdrs).create(buf,buflen,op);
    return 1;
}

#else

int i_am_a_dummy_function_for_osx() {
    return 1;
}

#endif
