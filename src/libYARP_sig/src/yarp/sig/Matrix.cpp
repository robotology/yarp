/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/sig/Matrix.h>

#include <yarp/conf/system.h>

#include <yarp/sig/Vector.h>
#include <yarp/os/Bottle.h>
#include <yarp/os/ConnectionReader.h>
#include <yarp/os/ConnectionWriter.h>
#include <yarp/os/ManagedBytes.h>
#include <yarp/os/NetFloat64.h>
#include <yarp/os/NetInt32.h>

#include <vector>
#include <cstdio>

using namespace yarp::sig;
using namespace yarp::os;

#define RES(v) ((std::vector<T> *)v)

YARP_BEGIN_PACK
class MatrixPortContentHeader
{
public:
    yarp::os::NetInt32 outerListTag{0};
    yarp::os::NetInt32 outerListLen{0};
    yarp::os::NetInt32 rowsTag{0};
    yarp::os::NetInt32 rows{0};
    yarp::os::NetInt32 colsTag{0};
    yarp::os::NetInt32 cols{0};
    yarp::os::NetInt32 listTag{0};
    yarp::os::NetInt32 listLen{0};

    MatrixPortContentHeader() = default;
};
YARP_END_PACK

// network stuff
#include <yarp/os/NetInt32.h>

bool yarp::sig::removeCols(const Matrix &in, Matrix &out, size_t first_col, size_t how_many)
{
    size_t nrows = in.rows();
    size_t ncols = in.cols();
    Matrix ret(nrows, ncols-how_many);
    for (size_t r = 0; r < nrows; r++) {
        for(size_t c_in=0,c_out=0;c_in<ncols; c_in++)
        {
            if (c_in==first_col)
            {
                c_in=c_in+(how_many-1);
                continue;
            }
            ret[r][c_out]=(in)[r][c_in];
            c_out++;
        }
    }
    out=ret;
    return true;
}

bool yarp::sig::removeRows(const Matrix &in, Matrix &out, size_t first_row, size_t how_many)
{
    size_t nrows = in.rows();
    size_t ncols = in.cols();
    Matrix ret(nrows-how_many, ncols);
    for (size_t c = 0; c < ncols; c++) {
        for(size_t r_in=0, r_out=0; r_in<nrows; r_in++)
        {
            if (r_in==first_row)
            {
                r_in=r_in+(how_many-1);
                continue;
            }
            ret[r_out][c]=(in)[r_in][c];
            r_out++;
        }
    }
     out=ret;
     return true;
}

bool yarp::sig::submatrix(const Matrix &in, Matrix &out, size_t r1, size_t r2, size_t c1, size_t c2)
{
    double *t=out.data();
    const double *i=in.data()+in.cols()*r1+c1;
    const int offset=in.cols()-(c2-c1+1);

    if (i == nullptr || t == nullptr) {
        return false;
    }

    for(size_t r=0;r<=(r2-r1);r++)
    {
        for(size_t c=0;c<=(c2-c1);c++)
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
    if (!ok) {
        return false;
    }
    size_t r=rows();
    size_t c=cols();
    if (header.listLen > 0)
    {
        if ( r != (size_t)(header.rows) || c!=(size_t)(header.cols))
        {
            resize(header.rows, header.cols);
        }

        int l=0;
        double *tmp=data();
        for (l = 0; l < header.listLen; l++) {
            tmp[l] = connection.expectFloat64();
        }
    } else {
        return false;
    }

    return true;
}


bool Matrix::write(yarp::os::ConnectionWriter& connection) const {
    MatrixPortContentHeader header;

    //header.totalLen = sizeof(header)+sizeof(double)*this->size();
    header.outerListTag = BOTTLE_TAG_LIST;
    header.outerListLen = 3;
    header.rowsTag = BOTTLE_TAG_INT32;
    header.colsTag = BOTTLE_TAG_INT32;
    header.listTag = BOTTLE_TAG_LIST + BOTTLE_TAG_FLOAT64;
    header.rows=rows();
    header.cols=cols();
    header.listLen = header.rows*header.cols;

    connection.appendBlock((char*)&header, sizeof(header));

    int l=0;
    const double *tmp=data();
    for (l = 0; l < header.listLen; l++) {
        connection.appendFloat64(tmp[l]);
    }

    // if someone is foolish enough to connect in text mode,
    // let them see something readable.
    connection.convertTextMode();

    return true;
}


std::string Matrix::toString(int precision, int width, const char* endRowStr) const {

    // If the width is less than 1, use tabs, else use width number of spaces.
    std::string spacer((width<0) ? "\t" : " ");

    // Buffering.
    std::string ret = "";
    char buffer[350];
    const double* src = (*this).data();

    // Iterate through copying the contents from the matrix, into a string.
    // Avoid unnecessary string resizes by only adding spacers at the beginning.
    size_t r, c;
    for (r = 0; r < nrows; r++) {
        if (r) { ret += endRowStr; }
        for (c = 0; c < ncols; c++) {
            if (c) { ret += spacer; }
            sprintf(buffer, "% *.*lf", width, precision, *src); src++;
            ret += buffer;
        }
    }

    return ret;
}


void Matrix::updatePointers()
{
    if (matrix != nullptr) {
        delete[] matrix;
    }

    size_t r=0;
    matrix=new double* [nrows];
    if (nrows > 0) {
        matrix[0] = storage;
    }
    for(r=1;r<nrows; r++)
    {
        matrix[r]=matrix[r-1]+ncols;
    }
}

const Matrix &Matrix::operator=(const Matrix &r)
{
    if (this == &r) {
        return *this;
    }

    if(nrows!=r.nrows || ncols!=r.ncols)
    {
        if (storage) {
            delete[] storage;
        }

        nrows=r.nrows;
        ncols=r.ncols;

        storage=new double[ncols*nrows];
        memcpy(storage, r.storage, ncols*nrows*sizeof(double));
        updatePointers();
    }
    else
    {
        if (!storage) {
            storage = new double[ncols * nrows];
        }
        memcpy(storage, r.storage, ncols*nrows*sizeof(double));
    }

    return *this;
}

const Matrix &Matrix::operator=(double v)
{
    size_t nelem = nrows*ncols;
    for (size_t k = 0; k < nelem; k++) {
        storage[k] = v;
    }

    return *this;
}

Matrix::~Matrix()
{
    if (matrix != nullptr) {
        delete[] matrix;
    }

    if (storage != nullptr) {
        delete[] storage;
    }
}

void Matrix::resize(size_t new_r, size_t new_c)
{
    if (new_r == nrows && new_c == ncols) {
        return;
    }

    auto* new_storage=new double[new_r*new_c];

    const size_t copy_r=(new_r<nrows) ? new_r:nrows;
    const size_t copy_c=(new_c<ncols) ? new_c:ncols;
    //copy_r = (new_r<nrows) ? new_r:nrows;

    if (storage!=nullptr)
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
        for(size_t r=0; r<copy_r;r++)
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

Matrix Matrix::removeCols(size_t first_col, size_t how_many)
{
    Matrix ret;
    ret.resize(nrows, ncols-how_many);

    for (size_t r = 0; r < nrows; r++) {
        for(size_t c_in=0,c_out=0;c_in<ncols; c_in++)
        {
            if (c_in==first_col)
            {
                c_in=c_in+(how_many-1);
                continue;
            }
            ret[r][c_out]=(*this)[r][c_in];
            c_out++;
        }
    }

    if (storage) {
        delete[] storage;
    }

    nrows=ret.nrows;
    ncols=ret.ncols;
    storage=new double[ncols*nrows];
    memcpy(storage, ret.storage, ncols*nrows*sizeof(double));
    updatePointers();
    return ret;
}

Matrix Matrix::removeRows(size_t first_row, size_t how_many)
{
    Matrix ret;
    ret.resize(nrows-how_many, ncols);

    for (size_t c = 0; c < ncols; c++) {
        for(size_t r_in=0, r_out=0; r_in<nrows; r_in++)
        {
            if (r_in==first_row)
            {
                r_in=r_in+(how_many-1);
                continue;
            }
            ret[r_out][c]=(*this)[r_in][c];
            r_out++;
        }
    }

    if (storage) {
        delete[] storage;
    }

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

    for (size_t r = 0; r < nrows; r++) {
        for (size_t c = 0; c < ncols; c++) {
            ret[c][r] = (*this)[r][c];
        }
    }

    return ret;
}

Vector Matrix::getRow(size_t r) const
{
    Vector ret;
    ret.resize(ncols);

    for (size_t c = 0; c < ncols; c++) {
        ret[c] = (*this)[r][c];
    }

    return ret;
}

Vector Matrix::getCol(size_t c) const
{
    Vector ret;
    ret.resize(nrows);

    for (size_t r = 0; r < nrows; r++) {
        ret[r] = (*this)[r][c];
    }

    return ret;
}

Vector Matrix::subrow(size_t r, size_t c, size_t size) const
{
    if (r >= rows() || c + size - 1 >= cols()) {
        return Vector(0);
    }

    Vector ret(size);

    for (size_t i = 0; i < size; i++) {
        ret[i] = (*this)[r][c + i];
    }

    return ret;
}

Vector Matrix::subcol(size_t r, size_t c, size_t size) const
{
    if (r + size - 1 >= rows() || c >= cols()) {
        return Vector(0);
    }

    Vector ret(size);

    for (size_t i = 0; i < size; i++) {
        ret[i] = (*this)[r + i][c];
    }

    return ret;
}

const Matrix &Matrix::eye()
{
    zero();
    size_t tmpR=nrows;
    if (ncols < nrows) {
        tmpR = ncols;
    }

    size_t c=0;
    for (size_t r = 0; r < tmpR; r++, c++) {
        (*this)[r][c] = 1.0;
    }

    return *this;
}

const Matrix &Matrix::diagonal(const Vector &d)
{
    zero();
    size_t tmpR=nrows;
    if (ncols < nrows) {
        tmpR = ncols;
    }

    size_t c=0;
    for (size_t r = 0; r < tmpR; r++, c++) {
        (*this)[r][c] = d[r];
    }

    return *this;
}

bool Matrix::operator==(const yarp::sig::Matrix &r) const
{
    //check dimensions first
    if ((rows() != r.rows()) || (cols() != r.cols())) {
        return false;
    }

    const double *tmp1=data();
    const double *tmp2=r.data();

    if (tmp1 == nullptr || tmp2 == nullptr) {
        return false;
    }

    int c=rows()*cols();
    while(c--)
    {
        if (*tmp1++ != *tmp2++) {
            return false;
        }
    }

    return true;
}

bool Matrix::setRow(size_t row, const Vector &r)
{
    if ((row >= nrows) || (r.length() != ncols)) {
        return false;
    }

    for (size_t c = 0; c < ncols; c++) {
        (*this)[row][c] = r[c];
    }

    return true;
}

bool Matrix::setCol(size_t col, const Vector &c)
{
    if ((col >= ncols) || (c.length() != nrows)) {
        return false;
    }

    for (size_t r = 0; r < nrows; r++) {
        (*this)[r][col] = c[r];
    }

    return true;
}

bool Matrix::setSubmatrix(const yarp::sig::Matrix &m, size_t r, size_t c)
{
    if ((c + m.cols() > ncols) || (r + m.rows() > nrows)) {
        return false;
    }

    for (size_t i = 0; i < m.rows(); i++) {
        for (size_t j = 0; j < m.cols(); j++) {
            (*this)[r + i][c + j] = m(i, j);
        }
    }
    return true;
}

bool Matrix::setSubrow(const Vector &v, size_t r, size_t c)
{
    size_t s = v.size();
    if (r >= nrows || c + s - 1 >= (size_t)ncols) {
        return false;
    }

    for (size_t i = 0; i < s; i++) {
        (*this)[r][i + c] = v[i];
    }
    return true;
}

bool Matrix::setSubcol(const Vector &v, size_t r, size_t c)
{
    size_t s = v.size();
    if (r + s - 1 >= (size_t)nrows || c >= ncols) {
        return false;
    }

    for (size_t i = 0; i < s; i++) {
        (*this)[r + i][c] = v[i];
    }
    return true;
}

Matrix::Matrix(size_t r, size_t c):
    storage(nullptr),
    matrix(nullptr),
    nrows(r),
    ncols(c)
{
    storage=new double [r*c];
    memset(storage, 0, r*c*sizeof(double));
    updatePointers();
}

Matrix::Matrix(const Matrix &m): yarp::os::Portable(),
    storage(nullptr),
    matrix(nullptr)
{
    nrows=m.nrows;
    ncols=m.ncols;

    if (m.storage!=nullptr)
    {
        storage=new double [nrows*ncols];
        memcpy(storage, m.storage, nrows*ncols*sizeof(double));

        updatePointers();
    }
}
