// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
* Copyright (C) 2007 Lorenzo Natale
* CopyPolicy: Released under the terms of the GNU GPL v2.0.
*
*/

// $Id: Matrix.cpp,v 1.18 2008-10-14 15:56:49 eshuy Exp $ 
#include <yarp/sig/Vector.h>
#include <yarp/sig/Matrix.h>
#include <yarp/os/impl/IOException.h>
#include <yarp/os/Bottle.h>
#include <yarp/ManagedBytes.h>
#include <yarp/os/NetFloat64.h>

#include <yarp/gsl_compatibility.h>

#include <ace/config.h>
#include <ace/Vector_T.h>

using namespace yarp::sig;
using namespace yarp::os::impl;

#define RES(v) ((ACE_Vector<T> *)v)
#define RES_ITERATOR(v) ((ACE_Vector_Iterator<double> *)v)

#include <yarp/os/begin_pack_for_net.h>

class MatrixPortContentHeader
{
public:
    yarp::os::NetInt32 listTag;
    yarp::os::NetInt32 listLen;
    yarp::os::NetInt32 rows;
    yarp::os::NetInt32 cols;
} PACKED_FOR_NET;

#include <yarp/os/end_pack_for_net.h>

/// network stuff
#include <yarp/os/NetInt32.h>
#include <yarp/os/begin_pack_for_net.h>

bool Matrix::read(yarp::os::ConnectionReader& connection) {
    // auto-convert text mode interaction
    connection.convertTextMode();
    MatrixPortContentHeader header;
    bool ok = connection.expectBlock((char*)&header, sizeof(header));
    if (!ok) return false;
    int r=rows();
    int c=cols();
    if (header.listLen > 0)
        {
            if ( (r*c) != (int)(header.listLen))
                {
                    resize(header.rows, header.cols);
                }
            
            int l=0;
            double *tmp=data();
            for(l=0;l<header.listLen;l++)
                tmp[l]=connection.expectDouble();
        }
    else
        return false;

    return true;
}


bool Matrix::write(yarp::os::ConnectionWriter& connection) {
    MatrixPortContentHeader header;

    //header.totalLen = sizeof(header)+sizeof(double)*this->size();
    header.listTag = BOTTLE_TAG_LIST + BOTTLE_TAG_DOUBLE;
    header.rows=rows();
    header.cols=cols();
    header.listLen = header.rows*header.cols;

    connection.appendBlock((char*)&header, sizeof(header));

    int l=0;
    const double *tmp=data();
    for(l=0;l<header.listLen;l++)
        connection.appendDouble(tmp[l]);

    // if someone is foolish enough to connect in text mode,
    // let them see something readable.
    connection.convertTextMode();

    return true;
}

/// vector implementations
#include <yarp/os/impl/String.h>
using namespace yarp::os::impl;
using namespace yarp::os;

/**
* Quick implementation, space for improvement.
*/
ConstString Matrix::toString() const
{
    String ret;
    char tmp[80];
    int c=0;
    int r=0;
    for(r=0;r<rows()-1;r++)
    {
        for(c=0;c<cols()-1;c++)
        {
            sprintf(tmp, "%lf\t", (*this)[r][c]);
            //ret.append(tmp, strlen(tmp));
            ret+=tmp;
        }

        sprintf(tmp, "%lf;", (*this)[r][c]);
        //ret.append(tmp, strlen(tmp));
        ret+=tmp;
    }

    // last row
    for(c=0;c<cols()-1;c++)
    {
        sprintf(tmp, "%lf\t", (*this)[r][c]);
        //ret.append(tmp, strlen(tmp));
        ret+=tmp;
    }
    // last element
    sprintf(tmp, "%lf", (*this)[r][c]);
    //ret.append(tmp, strlen(tmp));
    ret+=tmp;

    return ConstString(ret.c_str());
}

Matrix Matrix::submatrix(int r1, int c1, int r2, int c2) const
{
    Matrix ret;
    ret.resize(r2-r1, c2-c1);

    int rr=0;
    int cc=0;
    for(int r=r1; r<r2; r++)
    {
        for(int c=c1;c<c2;c++)
        {
            ret[rr][cc]=(*this)[r][c];
            cc++;
        }
        rr++;
    }
    return ret;
}

void Matrix::updatePointers()
{
    first=storage.getFirst();

    if (matrix!=0)
        delete [] matrix;

    int r=0;
    matrix=new double* [nrows];
    matrix[0]=first;
    for(r=1;r<nrows; r++)
    {
        matrix[r]=matrix[r-1]+ncols;
    }
    updateGslData();
}

const Matrix &Matrix::operator=(const Matrix &r)
{
    storage=r.storage;
    nrows=r.nrows;
    ncols=r.ncols;
    updatePointers();
    return *this;
}

const Matrix &Matrix::operator=(double v)
{
    double *tmp=storage.getFirst();

    for(int k=0; k<nrows*ncols; k++)
        tmp[k]=v;

    return *this;
}

Matrix::~Matrix()
{
    if (matrix!=0)
        delete [] matrix;
    freeGslData();
}

void Matrix::resize(int r, int c)
{
    nrows=r;
    ncols=c;

    storage.resize(r*c,0.0);
    updatePointers();
}

void Matrix::zero()
{
    for (int k=0; k<ncols*nrows; k++)
    {
        storage[k]=0;
    }
}

Matrix Matrix::transposed()
{
    Matrix ret;
    ret.resize(ncols, nrows);

    for(int r=0; r<nrows; r++)
        for(int c=0;c<ncols; c++)
            ret[c][r]=(*this)[r][c];

    return ret;
}

Vector Matrix::getRow(int r) const
{
    Vector ret;
    ret.resize(ncols);

    for(int c=0;c<ncols;c++)
        ret[c]=(*this)[r][c];

    return ret;
}

Vector Matrix::getCol(int c) const
{
    Vector ret;
    ret.resize(nrows);

    for(int r=0;r<nrows;r++)
        ret[r]=(*this)[r][c];

    return ret;
}

const Matrix &Matrix::eye()
{
    zero();
    int tmpR=nrows;
    if (ncols<nrows)
        tmpR=ncols;

    int c=0;
    for(int r=0; r<tmpR; r++,c++)
        (*this)[r][c]=1.0;

    return *this;
}

const Matrix &Matrix::diagonal(const Vector &d)
{
    zero();
    int tmpR=nrows;
    if (ncols<nrows)
        tmpR=ncols;

    int c=0;
    for(int r=0; r<tmpR; r++,c++)
        (*this)[r][c]=d[r];

    return *this;
}

bool Matrix::operator==(const yarp::sig::Matrix &r) const
{
    const double *tmp1=data();
    const double *tmp2=r.data();

    //check dimensions first
    if ( (rows()!=r.rows()) || (cols()!=r.cols()))
        return false;

    int c=rows()*cols();
    while(c--)
    {
        if (*tmp1++!=*tmp2++)
            return false;
    }

    return true;
}


void Matrix::allocGslData()
{
    gsl_matrix *mat=new gsl_matrix;
    gsl_block *bl=new gsl_block;
    
    mat->block=bl;

    //this is constant (at least for now)
    mat->owner=1;

    gslData=mat;
}

void Matrix::freeGslData()
{
    gsl_matrix *tmp=(gsl_matrix *) (gslData);

    if (tmp!=0)
    {
        delete tmp->block;
        delete tmp;
    }

    gslData=0;
}

void Matrix::updateGslData()
{
    gsl_matrix *tmp=static_cast<gsl_matrix *>(gslData);
    tmp->block->data=Matrix::data();
    tmp->data=tmp->block->data;
    tmp->block->size=rows()*cols();
    tmp->owner=1;
    tmp->tda=cols();
    tmp->size1=rows();
    tmp->size2=cols();
}
