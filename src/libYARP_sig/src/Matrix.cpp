/*
* Author: Lorenzo Natale.
* Copyright (C) 2007 The RobotCub Consortium
* CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
*/

#include <yarp/sig/Matrix.h>

#include <yarp/conf/system.h>

#include <yarp/sig/Vector.h>
#include <yarp/os/Bottle.h>
#include <yarp/os/ManagedBytes.h>
#include <yarp/os/NetFloat64.h>
#include <yarp/os/NetInt32.h>

#include <yarp/os/impl/PlatformVector.h>
#include <yarp/os/impl/PlatformStdio.h>
#include <yarp/os/impl/Logger.h>

using namespace yarp::sig;
using namespace yarp::os;

#define RES(v) ((PlatformVector<T> *)v)
#define RES_ITERATOR(v) ((PLATFORM_VECTOR_ITERATOR(double) *)v)

YARP_BEGIN_PACK
class MatrixPortContentHeader
{
public:
    yarp::os::NetInt32 outerListTag;
    yarp::os::NetInt32 outerListLen;
    yarp::os::NetInt32 rowsTag;
    yarp::os::NetInt32 rows;
    yarp::os::NetInt32 colsTag;
    yarp::os::NetInt32 cols;
    yarp::os::NetInt32 listTag;
    yarp::os::NetInt32 listLen;
};
YARP_END_PACK

/// network stuff
#include <yarp/os/NetInt32.h>

bool yarp::sig::removeCols(const Matrix &in, Matrix &out, int first_col, int how_many)
{
    int nrows = in.rows();
    int ncols = in.cols();
    Matrix ret(nrows, ncols-how_many);
    for(int r=0; r<nrows; r++)
        for(int c_in=0,c_out=0;c_in<ncols; c_in++)
        {
            if (c_in==first_col)
            {
                c_in=c_in+(how_many-1);
                continue;
            }
            ret[r][c_out]=(in)[r][c_in];
            c_out++;
        }
    out=ret;
    return true;
}

bool yarp::sig::removeRows(const Matrix &in, Matrix &out, int first_row, int how_many)
{
    int nrows = in.rows();
    int ncols = in.cols();
    Matrix ret(nrows-how_many, ncols);
    for(int c=0; c<ncols; c++)
        for(int r_in=0, r_out=0; r_in<nrows; r_in++)
        {
            if (r_in==first_row)
            {
                r_in=r_in+(how_many-1);
                continue;
            }
            ret[r_out][c]=(in)[r_in][c];
            r_out++;
        }
     out=ret;
     return true;
}

bool yarp::sig::submatrix(const Matrix &in, Matrix &out, int r1, int r2, int c1, int c2)
{
    double *t=out.data();
    const double *i=in.data()+in.cols()*r1+c1;
    const int offset=in.cols()-(c2-c1+1);

    for(int r=0;r<=(r2-r1);r++)
    {
        for(int c=0;c<=(c2-c1);c++)
        {
            *t++=*i++;
        }
        i+=offset;
    }

    return true;
}


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
        if ( r != (int)(header.rows) || c!=(int)(header.cols))
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
    header.outerListTag = BOTTLE_TAG_LIST;
    header.outerListLen = 3;
    header.rowsTag = BOTTLE_TAG_INT;
    header.colsTag = BOTTLE_TAG_INT;
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

/**
* Quick implementation, space for improvement.
*/
ConstString Matrix::toString(int precision, int width, const char* endRowStr) const
{
    ConstString ret;
    char tmp[350];
    int c, r;
    if(width>0) // if width is specified use a space as separator
    {
        for(r=0;r<nrows;r++)
        {        
            for(c=0;c<ncols;c++)
            {
                sprintf(tmp, "% *.*lf ", width, precision, (*this)[r][c]);
                ret+=tmp;
            }
            ret = ret.substr(0,ret.length()-1);     // remove the last character (space)
            if(r<nrows-1)                          // if it is not the last row
                ret+= endRowStr;
        }
    }
    else    // if width is not specified use tab as separator
    {
        for(r=0;r<nrows;r++)
        {
            for(c=0;c<ncols;c++)
            {
                sprintf(tmp, "% .*lf\t", precision, (*this)[r][c]);
                ret+=tmp;
            }
            ret = ret.substr(0,ret.length()-1);     // remove the last character (tab)
            if(r<nrows-1)                          // if it is not the last row
                ret+= endRowStr;
        }
    }

    return ret;
}

void Matrix::updatePointers()
{
    if (matrix!=0)
        delete [] matrix;

    int r=0;
    matrix=new double* [nrows];
    if (nrows>0) matrix[0]=storage;
    for(r=1;r<nrows; r++)
    {
        matrix[r]=matrix[r-1]+ncols;
    }
}

const Matrix &Matrix::operator=(const Matrix &r)
{
    if (this == &r) return *this;
    
    if(nrows!=r.nrows || ncols!=r.ncols)
    {
        if (storage)
            delete [] storage;

        nrows=r.nrows;
        ncols=r.ncols;

        storage=new double[ncols*nrows];
        memcpy(storage, r.storage, ncols*nrows*sizeof(double));
        updatePointers();
    }
    else
    {
        if(!storage)
            storage=new double[ncols*nrows];
        memcpy(storage, r.storage, ncols*nrows*sizeof(double));
    }

    return *this;
}

const Matrix &Matrix::operator=(double v)
{
    int nelem = nrows*ncols;
    for(int k=0; k<nelem; k++)
        storage[k]=v;

    return *this;
}

Matrix::~Matrix()
{
    if (matrix!=0)
        delete [] matrix;

    if (storage!=0)
        delete [] storage;
}

void Matrix::resize(int new_r, int new_c)
{
    if(new_r==nrows && new_c==ncols)
        return;

    double *new_storage=new double[new_r*new_c];

    const int copy_r=(new_r<nrows) ? new_r:nrows;
    const int copy_c=(new_c<ncols) ? new_c:ncols;
    //copy_r = (new_r<nrows) ? new_r:nrows;

    if (storage!=0)
    {
        double *tmp_new=new_storage;
        double *tmp_current=storage;
        // copy content

        // favor performance for small matrices
#if 0
        const int stepN=(new_c-copy_c);
        const int stepC=(ncols-copy_c);

        for(int r=0; r<copy_r;r++)
        {
            for(int c=0;c<copy_c;c++)
                *tmp_new++=*tmp_current++;
            tmp_new+=stepN;
            tmp_current=matrix[r];
        }
#endif

        // favor performance with large matrices
        for(int r=0; r<copy_r;r++)
        {
            tmp_current=matrix[r];
            memcpy(tmp_new, tmp_current, sizeof(double)*copy_c);
            tmp_new+=new_c;
        }


        delete [] storage;
    }
    else
    {
        //zero memory
        memset(new_storage, 0, sizeof(double)*new_r*new_c);
    }

    storage=new_storage;

    nrows=new_r;
    ncols=new_c;

    // storage.resize(r*c);
    updatePointers();
}

void Matrix::zero()
{
    memset(storage, 0, sizeof(double)*ncols*nrows);
}

Matrix Matrix::removeCols(int first_col, int how_many) 
{
    Matrix ret;
    ret.resize(nrows, ncols-how_many);

    for(int r=0; r<nrows; r++)
        for(int c_in=0,c_out=0;c_in<ncols; c_in++)
        {
            if (c_in==first_col)
            {
                c_in=c_in+(how_many-1);
                continue;
            }
            ret[r][c_out]=(*this)[r][c_in];
            c_out++;
        }

    if (storage) 
      delete [] storage; 
    
    nrows=ret.nrows;
    ncols=ret.ncols;
    storage=new double[ncols*nrows];
    memcpy(storage, ret.storage, ncols*nrows*sizeof(double));
    updatePointers();
    return ret;
}

Matrix Matrix::removeRows(int first_row, int how_many) 
{
    Matrix ret;
    ret.resize(nrows-how_many, ncols);

    for(int c=0; c<ncols; c++)
        for(int r_in=0, r_out=0; r_in<nrows; r_in++)
        {
            if (r_in==first_row)
            {
                r_in=r_in+(how_many-1);
                continue;
            }
            ret[r_out][c]=(*this)[r_in][c];
            r_out++;
        }

    if (storage) 
        delete [] storage; 
    
    nrows=ret.nrows;
    ncols=ret.ncols;
    storage=new double[ncols*nrows];
    memcpy(storage, ret.storage, ncols*nrows*sizeof(double));
    updatePointers();
    return ret;
}

Matrix Matrix::transposed() const
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

Vector Matrix::subrow(int r, int c, int size) const
{
    if(r>=rows() || c+size-1>=cols())
        return Vector(0);

    Vector ret(size);

    for(int i=0;i<size;i++)
        ret[i]=(*this)[r][c+i];

    return ret;
}

Vector Matrix::subcol(int r, int c, int size) const
{
    if(r+size-1>=rows() || c>=cols())
        return Vector(0);

    Vector ret(size);

    for(int i=0;i<size;i++)
        ret[i]=(*this)[r+i][c];

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
    //check dimensions first
    if ( (rows()!=r.rows()) || (cols()!=r.cols()))
        return false;

    const double *tmp1=data();
    const double *tmp2=r.data();

    int c=rows()*cols();
    while(c--)
    {
        if (*tmp1++!=*tmp2++)
            return false;
    }

    return true;
}

bool Matrix::setRow(int row, const Vector &r)
{
    if((row<0) || (row>=nrows) || (r.length() != (size_t)ncols))
        return false;

    for(int c=0;c<ncols;c++)
        (*this)[row][c]=r[c];

    return true;
}

bool Matrix::setCol(int col, const Vector &c)
{
    if((col<0) || (col>=ncols) || (c.length() != (size_t)nrows))
        return false; 

    for(int r=0;r<nrows;r++)
        (*this)[r][col]=c[r];

    return true;
}

bool Matrix::setSubmatrix(const yarp::sig::Matrix &m, int r, int c)
{
    if((c<0) || (c+m.cols()>ncols) || (r<0) || (r+m.rows()>nrows))
        return false;

    for(int i=0;i<m.rows();i++)
        for(int j=0;j<m.cols();j++)
            (*this)[r+i][c+j] = m(i,j);
    return true;
}

bool Matrix::setSubrow(const Vector &v, int r, int c)
{
    size_t s = v.size();
    if(r<0 || r>=nrows || c<0 || c+s-1>=(size_t)ncols)
        return false;

    for(size_t i=0;i<s;i++)
        (*this)[r][i+c] = v[i];
    return true;
}

bool Matrix::setSubcol(const Vector &v, int r, int c)
{
    size_t s = v.size();
    if(r<0 || r+s-1>=(size_t)nrows || c<0 || c>=ncols)
        return false;

    for(size_t i=0;i<s;i++)
        (*this)[r+i][c] = v[i];
    return true;
}

Matrix::Matrix(int r, int c):
    storage(0),
    matrix(0),
    nrows(r),
    ncols(c)
{
    storage=new double [r*c];
    memset(storage, 0, r*c*sizeof(double));
    updatePointers();
}

Matrix::Matrix(const Matrix &m): yarp::os::Portable(),
    storage(0),
    matrix(0)
{
    nrows=m.nrows;
    ncols=m.ncols;

    if (m.storage!=0) 
    {
        storage=new double [nrows*ncols];
        memcpy(storage, m.storage, nrows*ncols*sizeof(double));

        updatePointers();
    }
}


