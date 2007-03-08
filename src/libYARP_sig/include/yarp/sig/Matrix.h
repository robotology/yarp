// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2007 Lorenzo Natale
 * CopyPolicy: Released under the terms of the GNU GPL v2.0.
 *
 */

// $Id: Matrix.h,v 1.6 2007-03-08 13:56:05 natta Exp $ 

#ifndef _YARP2_MATRIX_
#define _YARP2_MATRIX_

#include <stdlib.h> //defines size_t
#include <yarp/os/Portable.h>
#include <yarp/sig/Vector.h>

/**
 * \file Matrix.h contains the definition of a Matrix type 
 */
namespace yarp {
	class MatrixBase;
    namespace sig {
		  class Matrix;
   }
}

/**
 * \ingroup sig_class
 *
 * A Base calss for a Matrix, provide default implementation for 
 * read/write methods.
 */
class yarp::MatrixBase:public yarp::os::Portable
{
public:
	virtual const char *getMemoryBlock() const =0;
	
    /**
    * Resize the matrix, specify new dimensions (r,c).
    */
    virtual void resize(int r, int c)=0;

    /**
    * Get number of rows.
    */
    virtual int rows() const =0;

    /**
    * Get number of columns.
    */
    virtual int cols() const =0;

	 /**
     * Read matrix from a connection.
     * return true iff a vector was read correctly
     */
    virtual bool read(yarp::os::ConnectionReader& connection);

    /**
     * Write matrix to a connection.
     * return true iff a vector was written correctly
     */
    virtual bool write(yarp::os::ConnectionWriter& connection);
};

/**
 * \ingroup sig_class
 *
 * A class for a Matrix. A Matrix can be sent/read to/from 
 * a port. Use the [] operator to access the r row of the matrix.
 * The function returns a pointer so [][] access the r,c element
 * in the matrix.
 */
class yarp::sig::Matrix: public yarp:: MatrixBase
{
private:
	VectorOf<double> storage;
	double *first;
	double **matrix; //double pointer access to elements

	int nrows;
	int ncols;

private:
     /**
     * Update pointer to data, call this every time you
     * change the size of the object.
     */
	void updatePointers()
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
	}

public:
	Matrix():
	  first(0),
	  matrix(0),
	  nrows(0),
	  ncols(0)
	{}

	Matrix(int r, int c):
	first(0),
	matrix(0),
	nrows(r),
	ncols(c)
	{
		storage.resize(r*c);
		updatePointers();
	}

    /**
    * Copy constructor.
    */
    Matrix(const Matrix &m):
		storage(m.storage),
		first(0),
		matrix(0)
	{
		nrows=m.nrows;
		ncols=m.ncols;
		updatePointers();
	}

    /**
    * Copy operator.
    */
	const Matrix &operator=(const Matrix &r)
	{
		storage=r.storage;
		nrows=r.nrows;
		ncols=r.ncols;
		updatePointers();
		return *this;
	}

    /**
    * Set all elements of the matrix to a given value.
    * @param v a scalar
    * @return a reference to the object
    */
	const Matrix &operator=(double v)
	{
		double *tmp=storage.getFirst();

		for(int k=0; k<nrows*ncols; k++)
			tmp[k]=v;

        return *this;
	}

	~Matrix()
	{
		if (matrix!=0)
			delete [] matrix;
	}

    /**
    * Return number of rows.
    */
	virtual int rows() const
	{ return nrows; }
	
    /**
    * Return number of columns.
    */
	virtual int cols() const
	{ return ncols; }

    /**
    * Resize the matrix.
    * @param r number of rows
    * @param c number of columns
    */
	virtual void resize(int r, int c)
	{
		nrows=r;
		ncols=c;

		storage.resize(r*c);
		updatePointers();
	}

    /**
    * Return pointer to internal memory.
    */
	virtual const char *getMemoryBlock() const
	{
		return (char *) storage.getFirst();
	}

	/**
	* Single element access, no range check. Returns
    * a pointer to the r-th row.
	* @param r row number.
	* @return a pointer to the first element of the row.
	*/
	inline double *operator[](int r)
	{
		return matrix[r];
	}

	/**
	* Single element access, no range check (const version). 
    * Returns a pointer to the r-th row.
	* @param r row number.
	* @return a (const) pointer to the first element of the row.
	*/
	inline const double *operator[](int r) const
	{
		return matrix[r];
	}

    /**
	* Single element access, no range check. 
    * @param r row number.
    * @param c col number.
	* @return a reference to the element.
	*/
    inline const double &operator()(int r, int c) const
    {
        return matrix[r][c];
    }

    /**
	* Single element access, no range check. 
    * @param r row number.
    * @param c col number.
	* @return a (const) reference to the element.
	*/
    inline double &operator()(int r, int c)
    {
        return matrix[r][c];
    }

    /**
	* Zero the matrix. Set all elements of the matrix to zero.
	*/
	void zero()
	{
		for (int k=0; k<ncols*nrows; k++)
		{
			storage[k]=0;
		}
	}

    /**
	* Return the transposed of the matrix. 
    * @return the transposed copy of the matrix.
	*/
	Matrix transposed()
	{
		Matrix ret;
		ret.resize(ncols, nrows);

		for(int r=0; r<nrows; r++)
			for(int c=0;c<ncols; c++)
				ret[c][r]=(*this)[r][c];
		
		return ret;
	}
	
    /**
    * Build an identity matrix, resize the matrix.
    * @param r number of rows
    * @param c number of columns
    */
	static Matrix eye(int r, int c)
	{
        Matrix ret;
		ret.resize(r,c);
        ret.eye();
		return ret;
	}

    /**
    * Build a matrix of zeros.
    * @param r number of rows
    * @param c number of columns
    */
    static Matrix zeros(int r, int c)
    {
        Matrix ret;
        ret.resize(r,c);
        ret.zero();
        return ret;
    }

    /**
    * Build an identity matrix, don't resize.
    * @return a reference to the object.
    */
	Matrix &eye()
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

    /**
    * Extract a submatrix from (r1,c1 to r2,c2)
    * @param r1 start point row
    * @param c1 start point col
    * @param r2 end point row
    * @param c2 end point col
    * @return the sumbatrix
    */
	Matrix submatrix(int r1, int c1, int r2, int c2) const
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

    /**
    * Get a row of the matrix as a vector.
    * @param r the row number
    * @return a vector which contains the requested row
    */
	Vector getRow(int r) const
	{
		Vector ret;
		ret.resize(ncols);

		for(int c=0;c<ncols;c++)
			ret[c]=(*this)[r][c];
		
		return ret;
	}

    /**
    * Get a columns of the matrix as a vector.
    * @param c the column number
    * @return a vector which contains the requested row
    */
	Vector getCol(int c) const
	{
		Vector ret;
		ret.resize(nrows);

		for(int r=0;r<nrows;r++)
			ret[r]=(*this)[r][c];
		
		return ret;
	}

    /**
    * Print matrix to a string. Useful for display, writing to text.
    * Warning: the string format might change in the future. This method
    * is here to ease debugging.
    * @return a const string which contain the matrix in text form.
    */
    yarp::os::ConstString toString() const;

    /**
    * Return a pointer to the first element
    * @return the pointer to the first element
    */
	inline double *data()
	{
		return storage.getFirst();
	}

    /**
    * Return a pointer to the first element (const version).
    * @return the (const) pointer to the first element
    */
	inline const double *data() const
	{
		return storage.getFirst();
	}
};


#endif

