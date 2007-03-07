// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2007 Lorenzo Natale
 * CopyPolicy: Released under the terms of the GNU GPL v2.0.
 *
 */

// $Id: Matrix.h,v 1.1 2007-03-07 15:28:24 natta Exp $

#ifndef _YARP2_MATRIX_
#define _YARP2_MATRIX_

#include <stdlib.h> //defines size_t
#include <yarp/os/Portable.h>
#include <yarp/sig/Vector.h>

//LATER REMOVE IT
#include <stdio.h>

/**
 * \file Vector.h contains the definition of a Vector type 
 */
namespace yarp {
	class MatrixBase;
    template<class T> class VectorImpl;
    namespace sig {
		  //class Vector
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
	virtual void resize(int r, int c)=0;
	virtual int rows() const =0;
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

class yarp::sig::Matrix: public yarp:: MatrixBase
{
private:
	VectorOf<double> storage;
	double *first;
	double **matrix; //double pointer access to elements

	int nrows;
	int ncols;

private:
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
	  nrows(0),
	  ncols(0),
	  first(0),
	  matrix(0)
	{}

	Matrix(int r, int c):
	nrows(r),
	ncols(c),
	first(0),
	matrix(0)
	{
		storage.resize(r*c);
		updatePointers();
	}

	Matrix(const Matrix &m):
		storage(m.storage),
		first(0),
		matrix(0)
	{
		nrows=m.nrows;
		ncols=m.ncols;
		updatePointers();
	}

	const Matrix &operator=(const Matrix &r)
	{
		storage=r.storage;
		nrows=r.nrows;
		ncols=r.ncols;
		updatePointers();
		return *this;
	}

	const Matrix &operator=(double v)
	{
		double *tmp=storage.getFirst();

		for(int k=0; k<nrows*ncols; k++)
			tmp[k]=v;
	}

	~Matrix()
	{
		if (matrix!=0)
			delete [] matrix;
	}

	virtual int rows() const
	{ return nrows; }
		
	virtual int cols() const
	{ return ncols; }

	virtual void resize(int r, int c)
	{
		nrows=r;
		ncols=c;

		storage.resize(r*c);
		updatePointers();
	}

	virtual const char *getMemoryBlock() const
	{
		return (char *) storage.getFirst();
	}

	/**
	* Single element access, no range check.
	* @param i the index of the element to access.
	* @return a reference to the requested element.
	*/
	inline double *operator[](int r)
	{
		return matrix[r];
	}

	/**
	* Single element access, no range check, const version.
	* @param i the index of the element to access.
	* @return a reference to the requested element.
	*/
	inline const double *operator[](int r) const
	{
		return matrix[r];
	}

	void zero()
	{
		for (int k=0; k<ncols*nrows; k++)
		{
			storage[k]=0;
		}
	}

	Matrix transposed()
	{
		Matrix ret;
		ret.resize(ncols, nrows);

		for(int r=0; r<nrows; r++)
			for(int c=0;c<ncols; c++)
				ret[c][r]=(*this)[r][c];
		
		return ret;
	}
	
	Matrix &eye(int r, int c)
	{
		resize(r,c);
		return eye();
	}

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

	Matrix submatrix(int r1, int r2, int c1, int c2) const
	{
		Matrix ret;
		ret.resize(r2-r1, c2-c1);

		for(int r=r1; r<r2; r++)
		{
			for(int c=c1;c<c2;c++)
				ret[r][c]=(*this)[r][c];
		}
		return ret;
	}

	Vector getRow(int r) const
	{
		Vector ret;
		ret.resize(ncols);

		for(int c=0;c<ncols;c++)
			ret[c]=(*this)[r][c];
		
		return ret;
	}

	Vector getCol(int c) const
	{
		Vector ret;
		ret.resize(nrows);

		for(int r=0;r<nrows;r++)
			ret[r]=(*this)[r][c];
		
		return ret;
	}

	void print()
	{
		for(int r=0; r<nrows; r++)
		{
			for(int c=0;c<ncols;c++)
			{
				printf("%lf\t", (*this)[r][c]);
			}
			printf("\n");
		}
		printf("\n");
	}

	inline double *data()
	{
		return storage.getFirst();
	}

	inline const double *data() const
	{
		return storage.getFirst();
	}
};


#endif

