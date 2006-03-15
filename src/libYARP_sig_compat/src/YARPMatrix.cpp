/////////////////////////////////////////////////////////////////////////
///                                                                   ///
///       YARP - Yet Another Robotic Platform (c) 2001-2004           ///
///                                                                   ///
///                    #Add our name(s) here#                         ///
///                                                                   ///
///     "Licensed under the Academic Free License Version 1.0"        ///
///                                                                   ///
/// The complete license description is contained in the              ///
/// licence.template file included in this distribution in            ///
/// $YARP_ROOT/conf. Please refer to this file for complete           ///
/// information about the licensing of YARP                           ///
///                                                                   ///
/// DISCLAIMERS: LICENSOR WARRANTS THAT THE COPYRIGHT IN AND TO THE   ///
/// SOFTWARE IS OWNED BY THE LICENSOR OR THAT THE SOFTWARE IS         ///
/// DISTRIBUTED BY LICENSOR UNDER A VALID CURRENT LICENSE. EXCEPT AS  ///
/// EXPRESSLY STATED IN THE IMMEDIATELY PRECEDING SENTENCE, THE       ///
/// SOFTWARE IS PROVIDED BY THE LICENSOR, CONTRIBUTORS AND COPYRIGHT  ///
/// OWNERS "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, ///
/// INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,   ///
/// FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT. IN NO      ///
/// EVENT SHALL THE LICENSOR, CONTRIBUTORS OR COPYRIGHT OWNERS BE     ///
/// LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN   ///
/// ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN ///
/// CONNECTION WITH THE SOFTWARE.                                     ///
///                                                                   ///
/////////////////////////////////////////////////////////////////////////

///
///
///       YARP - Yet Another Robotic Platform (c) 2001-2003 
///
///                    #pasa#
///
///     "Licensed under the Academic Free License Version 1.0"
///

///
/// $Id: YARPMatrix.cpp,v 1.1 2006-03-15 09:33:51 eshuy Exp $
///
///

///////////////////////////////////////////////////////////////////////////
//
// NAME
//  VisMatrix.cpp -- implementation of matrix/vector ops
//
// DESCRIPTION
//  The YMatrix class provides some basic matrix operations, using calls
//  to external software (IMSL for the moment) to perform the more
//  complicated operations.
//
// SEE ALSO
//  YMatrix.h        more complete description
//
// BUGS
//  I sure hope that the IMSL routines don't modify their input
//  parameters!
//
// DESIGN
//  Right now, we call out to IMSL to perform the linear systems
//  solving.  We could change this later.
//
//  To use IMSL, include the following in your 
//  Build Settings Link Object/Library modules:
//      mathd.lib libf.lib
//
// Copyright © 1996-1998 Microsoft Corporation, All Rights Reserved
//
///////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////
//
// Modified by PASA. 
//	see VisDMatrix.h
//
// 
///////////////////////////////////////////////////////////////////////////

#include <yarp/YARPMatrix.h>
#include <string.h>


//  IMSL routines used (double precision)

#ifdef VIS_USE_IMSL
#include "..\VisXIMSL\VisXIMSL.h"
#else // VIS_USE_IMSL
static void VisGaussJordanSolveDMatrix(YMatrix& A);
#endif // VIS_USE_IMSL

// LATER:  The vector and matrix arithmetic operators assume that both
// arguments are of the same size.  We could easily change many of these
// operators to do the right thing when the arguments are not the same
// size.  (For example, adding a 3-vector to a 4-vector.)

#ifdef __QNX6__
#define UNUSED(x) do {} while (&x == 0)
#else
#define UNUSED(x)
#endif

//
// Constructors (default destructor and copy constructor)
//

////////////////////////////////////////////////////////////////////////////
//  
//  FUNCTION:        YVector::YVector
//  
//  DECLARATION:
//          YVector::YVector(int length, double *storage);
//  
//  INPUT:
//      length (int) - length of vector
//  
//      *storage (double) - optional storage location (will not dealloca
//                  te on destruction)
//  
//  DISCRIPTION:
//      Create a dynamically sized double precision vector
//  
////////////////////////////////////////////////////////////////////////////
void YVector::Resize(int length, const double *storage)
{
  if(m_length!=length || m_data==NULL)
    {
      m_length = length;

      if (m_data != NULL)
	delete[] m_data;

      m_data = new double[m_length];
    }

  if (storage != 0)
    {
      memcpy (m_data, storage, sizeof(double) * m_length);
    }
  else
    {
      memset (m_data, 0, sizeof(double) * m_length);
    }
}

YVector::YVector(int length, const double *storage) :  m_length(0), m_data(0)
{
    Resize(length, storage);
}

YVector::YVector(void)
  : m_length(0), m_data(0)
{
}

YVector::YVector(const YVector &refvector) :  m_length(0), m_data(0)
{
    //Resize(refvector.Length());
	*this = refvector;
}

YVector::~YVector ()
{
	if (m_data != NULL)
		delete[] m_data;
}

////////////////////////////////////////////////////////////////////////////
//  
//  FUNCTION:        YMatrix::YMatrix
//  
//  DECLARATION:
//          YMatrix::YMatrix(int rows, int cols, double *storage);
//  
//  INPUT:
//      rows (int) - number of rows in matrix
//  
//      cols (int) - number of columns in matrix
//  
//      *storage (double) - optional storage location (will not dealloca
//                  te on destruction)
//  
//  DISCRIPTION:
//      Create a dynamically sized double precision matrix
//  
////////////////////////////////////////////////////////////////////////////
void YMatrix::Resize(int rows, int cols, const double *storage)
{
	if (m_nRows != rows || m_nCols != cols)
	{
		// deallocate the array of ptrs to rows.
		// if dims are the same do not deallocate.
		if (m_data != NULL)
		{
			delete[] m_data[0];
			delete[] m_data;

			m_data = new double *[rows];
			m_data[0] = new double[rows * cols];
			for (int i = 1; i < rows; i++)
			{
				m_data[i] = m_data[i-1] + cols;
			}

			m_nRows = rows;
			m_nCols = cols;
		}
		else
		{
			// allocate.
			m_data = new double *[rows];
			m_data[0] = new double[rows * cols];
			for (int i = 1; i < rows; i++)
			{
				m_data[i] = m_data[i-1] + cols;
			}

			m_nRows = rows;
			m_nCols = cols;
		}
	}

	if (storage != 0)
		memcpy (m_data[0], storage, sizeof(double) * rows * cols);
	else
		memset (m_data[0], 0, sizeof(double) * rows * cols);
}

YMatrix::YMatrix(int rows, int cols, const double *storage) : m_nRows(0), m_nCols(0), m_data(0)
{
    Resize(rows, cols, storage);
}

YMatrix::YMatrix(void) : m_nRows(0), m_nCols(0), m_data(0)
{
}

YMatrix::YMatrix(const YMatrix &refmatrix) : m_nRows(0), m_nCols(0), m_data(0)
{
    //Resize(refmatrix.NRows(), refmatrix.NCols());
	*this = refmatrix;
}

YMatrix::~YMatrix ()
{
	if (m_data != NULL)
	{
		delete[] m_data[0];
		delete[] m_data;
	}
}

//
// Assignment
//

////////////////////////////////////////////////////////////////////////////
//  
//  FUNCTION:        operator=
//  
//  DECLARATION:
//          YVector& YVector::operator=(const YVector &vec);
//  
//  RETURN VALUE:
//      vector being copied
//  INPUT:
//      &vec (const YVector) - vector being copied
//  
//  DISCRIPTION:
//      assignment operator
//  
////////////////////////////////////////////////////////////////////////////
YVector& YVector::operator=(const YVector &vec)
{
	if (Length() != vec.Length())
		Resize(vec.Length());
    memcpy(m_data, vec.m_data, m_length*sizeof(double));
    return *this;
}

////////////////////////////////////////////////////////////////////////////
//  
//  FUNCTION:        operator=
//  
//  DECLARATION:
//          YVector& YVector::operator=(double value);
//  
//  RETURN VALUE:
//      reference to l.h.s.
//  INPUT:
//      value (double) - fill value
//  
//  DISCRIPTION:
//      Fill vector with constant value
//  
////////////////////////////////////////////////////////////////////////////
YVector& YVector::operator=(double value)
{
	assert (Length() != 0);
    if (value == 0.0)   // IEEE float
        memset(m_data, 0, m_length*sizeof(double));
    else
        for (int i = 0; i < m_length; i++)
            m_data[i] = value;
    return *this;
}

////////////////////////////////////////////////////////////////////////////
//  
//  FUNCTION:        operator=
//  
//  DECLARATION:
//          YMatrix& YMatrix::operator=(const YMatrix &mat);
//  
//  RETURN VALUE:
//      reference to l.h.s.
//  INPUT:
//      &mat (const YMatrix) - matrix being copied
//  
//  DISCRIPTION:
//      Assignment operator
//  
////////////////////////////////////////////////////////////////////////////
YMatrix& YMatrix::operator=(const YMatrix &mat)
{
	if (NRows() != mat.NRows() || NCols() != mat.NCols())
		Resize(mat.m_nRows,mat.m_nCols);
    memcpy(m_data[0], mat.m_data[0], m_nRows*m_nCols*sizeof(double));
    return *this;
}

////////////////////////////////////////////////////////////////////////////
//  
//  FUNCTION:        operator=
//  
//  DECLARATION:
//          YMatrix& YMatrix::operator=(double value);
//  
//  RETURN VALUE:
//      reference to l.h.s.
//  INPUT:
//      value (double) - fill value
//  
//  DISCRIPTION:
//      Assignment operator
//  
////////////////////////////////////////////////////////////////////////////
YMatrix& YMatrix::operator=(double value)
{
	assert (m_nRows != 0 && m_nCols != 0);
    if (value == 0.0)   // IEEE float
        memset(m_data[0], 0, m_nRows*m_nCols*sizeof(double));
    else {
        int n = m_nRows * m_nCols;
        double *p = m_data[0];
        for (int i = 0; i < n; i++)
            p[i] = value;
    }
    return *this;
}

//
//  Comparison operators
//

bool YVector::operator==(const YVector& refvector) const
{
    if (Length() == refvector.Length())
	{
		if (Length() == 0)
			return true;

		if (memcmp(m_data, refvector.m_data, Length() * sizeof(double)) == 0)
			return true;
	}

    return false; 
}

bool YMatrix::operator==(const YMatrix& refmatrix) const
{
    if ((NRows() == refmatrix.NRows())
			&& (NCols() == refmatrix.NCols()))
	{
		if ((NRows() == 0) || (NCols() == 0))
			return true;

		int cbRow = NCols()  * sizeof(double);
		for (int iRow = 0; iRow < NRows(); ++iRow)
		{
			if (memcmp(m_data[iRow], refmatrix.m_data[iRow], cbRow) != 0)
				return false;
		}
		
		return true;
	}


    return false; 
}

////////////////////////////////////////////////////////////////////////////
//  
//  FUNCTION:        operator==
//  
//  DECLARATION:
//          bool YVector::operator==(double dbl) const;
//			bool YMatrix::operator==(double dbl) const;
//
//  RETURN VALUE:
//      bool
//
//  INPUT:
//      dbl (double) - check value
//  
//  DISCRIPTION:
//      Compare operator(s)
//  
////////////////////////////////////////////////////////////////////////////
bool YVector::operator==(double dbl) const
{
	int n=Length();
	if (n == 0)
		return true;

	bool retflag=true;
	for (int i=0;i<n;i++)
	{
		retflag &= ((*this)[i] == dbl);
	}

    return retflag; 
}

bool YMatrix::operator==(double dbl) const
{
	int n=NRows();
	int m=NCols();
    if ((n*m) == 0)
		return true;

	bool retflag=true;
	for (int i=0;i<n;i++)
		for (int j=0;j<m;j++)
			retflag &= (m_data[i][j] == dbl);

	return retflag;
}

bool YVector::operator<(const YVector& refvector) const
{
    if (Length() == refvector.Length())
	{
		if (Length() == 0)
			return false;

		return (memcmp(m_data, refvector.m_data,
				Length() * sizeof(double)) < 0);
	}

    return (Length() < refvector.Length()); 
}

bool YMatrix::operator<(const YMatrix& refmatrix) const
{
    if (NRows() == refmatrix.NRows())
	{
		if (NCols() == refmatrix.NCols())
		{
			if ((NRows() == 0) || (NCols() == 0))
				return false;

			int cbRow = NCols()  * sizeof(double);
			for (int iRow = 0; iRow < NRows(); ++iRow)
			{
				int wCmp = memcmp(m_data[iRow], refmatrix.m_data[iRow], cbRow);
				if (wCmp != 0)
					return (wCmp < 0);
			}
			
			return false;
		}
    
		return (NCols() < refmatrix.NCols()); 
	}

    return (NRows() < refmatrix.NRows()); 
}


//
//  Matrix / vector products, dot product
//

YVector& YVector::operator+=(const YVector& refvector)
{
    int n = refvector.Length(); 
    assert(n == Length()); 

    for (int i=0; i<n; i++)
        (*this)[i] += refvector[i];

    return *this; 
}

////////////////////////////////////////////////////////////////////////////
//  
//  FUNCTION:        operator+=
//  
//  DECLARATION:
//          YVector& YVector::operator+=(double dbl);
//  
//  RETURN VALUE:
//      reference to l.h.s.
//
//  INPUT:
//      dbl (double) - add value
//  
//  DISCRIPTION:
//      Sum and assign operator
//  
////////////////////////////////////////////////////////////////////////////
YVector& YVector::operator+=(double dbl)
{
	int n = Length();
    assert(n != 0); 

    for (int i=0; i<n; i++)
        (*this)[i] += dbl;

    return *this; 
}

YVector& YVector::operator-=(const YVector& refvector)
{
    int n = refvector.Length(); 
    assert(n == Length()); 

    for (int i=0; i<n; i++)
        (*this)[i] -= refvector[i];

    return *this; 
}

////////////////////////////////////////////////////////////////////////////
//  
//  FUNCTION:        operator-=
//  
//  DECLARATION:
//          YVector& YVector::operator-=(double dbl);
//  
//  RETURN VALUE:
//      reference to l.h.s.
//
//  INPUT:
//      dbl (double) - subtract value
//  
//  DISCRIPTION:
//      Subtract and assign operator
//  
////////////////////////////////////////////////////////////////////////////
YVector& YVector::operator-=(double dbl)
{
	int n = Length();
    assert(n != 0); 

    for (int i=0; i<n; i++)
        (*this)[i] -= dbl;

    return *this; 
}

YVector& YVector::operator*=(double dbl)
{
    for (int i=0; i<Length(); i++)
        (*this)[i] *= dbl;

    return *this; 
}

YVector& YVector::operator/=(double dbl)
{
	assert(dbl != 0);

    for (int i=0; i<Length(); i++)
        (*this)[i] /= dbl;

    return *this; 
}

YVector YVector::operator+(const YVector& refvector) const
{
    assert(Length() == refvector.Length()); 
    YVector vectorRet(*this);

    return (vectorRet += refvector); 
}

////////////////////////////////////////////////////////////////////////////
//  
//  FUNCTION:        operator+
//  
//  DECLARATION:
//          YVector YVector::operator+(double dbl) const;
//  
//  RETURN VALUE:
//		ret = v + dbl;     
//
//  INPUT:
//      dbl (double) - sum value
//  
//  DISCRIPTION:
//      Sum operator
//  
////////////////////////////////////////////////////////////////////////////
YVector YVector::operator+(double dbl) const
{
    assert(Length() != 0); 
    YVector vectorRet(*this);

    return (vectorRet += dbl); 
}

YVector YVector::operator-(void) const
{
    YVector vectorRet(Length());

    for (int i=0; i < Length(); i++)
        vectorRet[i] = - (*this)[i];

    return vectorRet; 
}

YVector YVector::operator-(const YVector& refvector) const
{
    assert(Length() == refvector.Length()); 
    YVector vectorRet(*this);

    return (vectorRet -= refvector); 
}

////////////////////////////////////////////////////////////////////////////
//  
//  FUNCTION:        operator-
//  
//  DECLARATION:
//          YVector YVector::operator-(double dbl) const;
//  
//  RETURN VALUE:
//		ret = v - dbl;     
//
//  INPUT:
//      dbl (double) - subtract value
//  
//  DISCRIPTION:
//      Subtract operator
//  
////////////////////////////////////////////////////////////////////////////
YVector YVector::operator-(double dbl) const
{
    assert(Length() != 0); 
    YVector vectorRet(*this);

    return (vectorRet -= dbl); 
}

YVector YVector::operator*(double dbl) const
{
    YVector vectorRet(*this);

    return (vectorRet *= dbl); 
}

YVector YVector::operator/(double dbl) const
{
	assert(dbl != 0);

    YVector vectorRet(*this);

    return (vectorRet /= dbl); 
}

////////////////////////////////////////////////////////////////////////////
//  
//  FUNCTION:        operator*
//  
//  DECLARATION:
//          double YVector::operator*(const YVector& refvector) const;
//  
//  RETURN VALUE:
//		scalar product
//
//  INPUT:
//      refvector 
//  
//  DISCRIPTION:
//      Scalar product
//  
////////////////////////////////////////////////////////////////////////////
double YVector::operator*(const YVector& refvector) const
{
    double sum = 0.0;
    assert(Length() == refvector.Length());

    for (int i = 0; i < Length(); i++)
        sum += (*this)[i] * refvector[i];

    return sum;
}

////////////////////////////////////////////////////////////////////////////
//  
//  FUNCTION:        norm2, norm2square
//  
//  DECLARATION:
//          double YVector::norm2(void) const;
//          double YVector::norm2square(void) const;
//  
//  RETURN VALUE:
//		norm 2.
//
//  INPUT:
//      none 
//  
//  DISCRIPTION:
//      Compute the 2 norm of the vector.
//  
////////////////////////////////////////////////////////////////////////////
double YVector::norm2(void) const
{
	int n = Length();
	assert(n != 0);
   
	double sum = 0.0;
    for (int i = 0; i < n; i++)
        sum += (m_data[i] * m_data[i]);

    return sqrt(sum);
}

double YVector::norm2square(void) const
{
	int n = Length();
	assert(n != 0);
   
	double sum = 0.0;
    for (int i = 0; i < n; i++)
        sum += (m_data[i] * m_data[i]);

    return sum;
}

////////////////////////////////////////////////////////////////////////////
//  
//  FUNCTION:        sin,cos
//  
//  DECLARATION:
//          YVector& YVector::cos(void);
//          YVector& YVector::sin(void);
//  
//  RETURN VALUE:
//		reference to l.h.s.
//
//  INPUT:
//      none 
//  
//  DISCRIPTION:
//      Compute the sin (cos) of the vector.
//  
////////////////////////////////////////////////////////////////////////////
YVector& YVector::cos(void)
{
	int n=Length();
	assert(n != 0);

	for (int i = 0; i < n; i++)
		m_data[i]=::cos(m_data[i]);

	return *this;
}

YVector& YVector::sin(void)
{
	int n=Length();
	assert(n != 0);

	for (int i = 0; i < n; i++)
		m_data[i]=::sin(m_data[i]);

	return *this;
}

//
// Specific Matrix code starts here (??).
//
YMatrix& YMatrix::operator+=(const YMatrix& refmatrix)
{
	assert((NRows() == refmatrix.NRows())
			&& (NCols() == refmatrix.NCols()));

    for (int i = 0; i < NRows(); i++)
        for (int j = 0; j < NCols(); j++)
            (*this)[i][j] += refmatrix[i][j];

    return *this;
}

YMatrix& YMatrix::operator-=(const YMatrix& refmatrix)
{
	assert((NRows() == refmatrix.NRows())
			&& (NCols() == refmatrix.NCols()));

    for (int i = 0; i < NRows(); i++)
        for (int j = 0; j < NCols(); j++)
            (*this)[i][j] -= refmatrix[i][j];

    return *this;
}

////////////////////////////////////////////////////////////////////////////
//  
//  FUNCTION:        operator+=, operator-=
//  
//  DECLARATION:
//          YMatrix& YMatrix::operator+=(double dbl);
//          YMatrix& YMatrix::operator-=(double dbl);
//  
//  RETURN VALUE:
//		reference to l.h.s.
//
//  INPUT:
//      dbl (double)
//  
//  DISCRIPTION:
//      Sum or subtract dbl.
//  
////////////////////////////////////////////////////////////////////////////
YMatrix& YMatrix::operator+=(double dbl)
{
	assert((NRows() != 0) && (NCols() != 0));

    for (int i = 0; i < NRows(); i++)
        for (int j = 0; j < NCols(); j++)
            (*this)[i][j] += dbl;

    return *this;
}

YMatrix& YMatrix::operator-=(double dbl)
{
	assert((NRows() != 0) && (NCols() != 0));

    for (int i = 0; i < NRows(); i++)
        for (int j = 0; j < NCols(); j++)
            (*this)[i][j] -= dbl;

    return *this;
}

YMatrix& YMatrix::operator*=(const YMatrix& refmatrix)
{
    YMatrix matrixT(*this);

    // Note that operator= requires that the LHS and RHS have the same
	// dimensions.
	return (*this = (matrixT * refmatrix));
}

YMatrix& YMatrix::operator*=(double dbl)
{
    for (int i = 0; i < NRows(); i++)
        for (int j = 0; j < NCols(); j++)
            (*this)[i][j] *= dbl;

    return *this;
}

YMatrix& YMatrix::operator/=(double dbl)
{
	assert(dbl != 0);

    for (int i = 0; i < NRows(); i++)
        for (int j = 0; j < NCols(); j++)
            (*this)[i][j] /= dbl;

    return *this;
}

YMatrix YMatrix::operator-(void) const
{
    YMatrix matrixRet(NRows(), NCols());

    for (int i = 0; i < NRows(); i++)
        for (int j = 0; j < NCols(); j++)
            matrixRet[i][j] = - (*this)[i][j];

    return matrixRet;
}

YMatrix YMatrix::operator+(const YMatrix& refmatrix) const
{
    YMatrix matrixRet(*this);

    return (matrixRet += refmatrix);
}

YMatrix YMatrix::operator-(const YMatrix& refmatrix) const
{
    YMatrix matrixRet(*this);

    return (matrixRet -= refmatrix);
}

////////////////////////////////////////////////////////////////////////////
//  
//  FUNCTION:        operator+, operator-
//  
//  DECLARATION:
//          YMatrix YMatrix::operator+(double dbl) const;
//          YMatrix YMatrix::operator-(double dbl) const;
//  
//  RETURN VALUE:
//		A new matrix (Sum or difference).
//
//  INPUT:
//      dbl (double)
//  
//  DISCRIPTION:
//      Sum or subtract dbl.
//  
////////////////////////////////////////////////////////////////////////////
YMatrix YMatrix::operator+(double dbl) const
{
    YMatrix matrixRet(*this);

    return (matrixRet += dbl);
}

YMatrix YMatrix::operator-(double dbl) const
{
    YMatrix matrixRet(*this);

    return (matrixRet -= dbl);
}

YMatrix YMatrix::operator*(double dbl) const
{
    YMatrix matrixRet(*this);

    return (matrixRet *= dbl);
}

YMatrix YMatrix::operator/(double dbl) const
{
    YMatrix matrixRet(*this);

    return (matrixRet /= dbl);
}

YVector YMatrix::operator*(const YVector& refvector) const
{
    YVector vectorRet(NRows());

    assert(NRows() == vectorRet.Length()
			&& NCols() == refvector.Length());

    for (int i = 0; i < NRows(); i++) {
        double sum = 0.0;
        for (int j = 0; j < NCols(); j++)
            sum += (*this)[i][j] * refvector[j];
        vectorRet[i] = sum;
    }

	return vectorRet;
}

YMatrix YMatrix::operator*(const YMatrix& refmatrix) const
{
    YMatrix matrixRet(NRows(), refmatrix.NCols());

    assert(NCols() == refmatrix.NRows() &&
           NRows() == matrixRet.NRows() &&
           refmatrix.NCols() == matrixRet.NCols());

    for (int i = 0; i < NRows(); i++) {
        for (int j = 0; j < refmatrix.NCols(); j++) {
            double sum = 0.0;
            for (int k = 0; k < NCols(); k++)
                sum += (*this)[i][k] * refmatrix[k][j];
            matrixRet[i][j] = sum;
        }
    }

    return matrixRet;
}


//
//  Matrix inverse
//

YMatrix YMatrix::Inverted(void) const
{
    YMatrix matrixInverse(NRows(), NCols());

#ifdef VIS_USE_IMSL

    VisIMSL_dlinrg(NRows(), (*this)[0], NRows(),
			matrixInverse[0], NRows());

#else // VIS_USE_IMSL

    // Use Gauss-Jordan elimination
    int i, j, n = NRows();
    YMatrix matrixT(n, 2*n);
    for (i = 0; i < n; i++)       // copy into a single system
        for (j = 0; j < n; j++)
            matrixT[i][j] = (*this)[i][j], matrixT[i][j+n] = (i == j);
    VisGaussJordanSolveDMatrix(matrixT);
    for (i = 0; i < n; i++)
        for (j = 0; j < n; j++)
            matrixInverse[i][j] = matrixT[i][j+n];

#endif // VIS_USE_IMSL

	return matrixInverse;
}

YMatrix YMatrix::Transposed(void) const
{
    int i, j, n = NRows(), m = NCols();
    YMatrix matrixTranspose(m, n);

    assert(m > 0 && n > 0);

    for (i = 0; i < m; i++)
        for (j = 0; j < n; j++)
            matrixTranspose[i][j] = (*this)[j][i];

	return matrixTranspose;
}

bool YMatrix::IsSymmetric(void) const
{
    if (NRows() != NCols())
        return false;

    for (int i = 0; i < NRows(); i++)
        for (int j = i+1; j < NCols(); j++)
            if ((*this)[i][j] != (*this)[j][i])
                return false;

    return true;
}


//
// Global functions
//


////////////////////////////////////////////////////////////////////////////
//  
//  FUNCTION:        VisDMatrixSqrtInverse
//  
//  DECLARATION:
//          YMatrix VisDMatrixSqrtInverse(const YMatrix& A);
//  
//  RETURN VALUE:
//      result matrix
//  INPUT:
//      A (const YMatrix&) - input matrix
//  
//  DISCRIPTION:
//      Matrix square root inverse
//  
////////////////////////////////////////////////////////////////////////////
VisMatrixExport YMatrix VISAPI VisDMatrixSqrtInverse(const YMatrix& A)
{
    YMatrix result(A.NRows(), A.NCols());
    VisDMatrixSqrtInverse(A, result);
    return result;
}

////////////////////////////////////////////////////////////////////////////
//  
//  FUNCTION:        VisDMatrixSqrtInverse
//  
//  DECLARATION:
//     void VisDMatrixSqrtInverse(const YMatrix& A, YMatrix &AsqrtInv);
//  
//  INPUT:
//      A (const YMatrix&) - input matrix
//  
//      &AsqrtInv (YMatrix) - result matrix
//  
//  DISCRIPTION:
//      Matrix t
//  
////////////////////////////////////////////////////////////////////////////
VisMatrixExport void VISAPI VisDMatrixSqrtInverse(const YMatrix& A,
		YMatrix &AsqrtInv)
{
#ifdef VIS_USE_IMSL

    assert(A.NRows() == A.NCols() && A.NRows() > 0);
    assert(A.NRows() == AsqrtInv.NRows());
    assert(A.NCols() == AsqrtInv.NCols());

    int i, j, n = A.NRows();
    YMatrix B(n, n);

	B = A.Transposed() * A;

    YVector Eval(n);
    YMatrix Evec(n,n);

    VisIMSL_devcsf(n, B[0], n, &Eval[0], Evec[0], n);

    for (i=0; i<n; i++){
        assert(Eval[i] > 0.0);
        Eval[i] = 1.0/sqrt(Eval[i]); // can be easily modified for sqrt
    }

    for (i = 0; i < n; i++){
        for (j = 0; j < n; j++){
            AsqrtInv[i][j] = 0.0; 
            for (int k=0; k<n; k++)
                AsqrtInv[i][j] += Eval[k] * Evec[k][i] * Evec[k][j];
        }
    }

#else // VIS_USE_IMSL
	UNUSED(A);
	UNUSED(AsqrtInv);

	// Not implemented!
	assert(0);

    //throw CVisError("Inverse sqrt only works with IMSL for now",
    //                eviserrorOpFailed, "VisDMatrixSqrtInverse",
    //                "VisDMatrix.cpp", __LINE__);

#endif // VIS_USE_IMSL
}


//
//  Linear systems solution
//

////////////////////////////////////////////////////////////////////////////
//  
//  FUNCTION:        VisDMatrixSolve
//  
//  DECLARATION:
//          YVector VisDMatrixSolve(const YMatrix& A,
//                        const YVector& b);
//  
//  RETURN VALUE:
//      result vector
//  INPUT:
//      A (const YMatrix&) - input matrix (l.h.s. of linear system)
//                  
//  
//      b (const YVector&) - input vector (r.h.s. of linear system)
//                  
//  
//  DISCRIPTION:
//      Linear system solution
//  
////////////////////////////////////////////////////////////////////////////
VisMatrixExport YVector VISAPI VisDMatrixSolve(const YMatrix& A,
		const YVector& b)
{
    YVector result(b.Length());
    VisDMatrixSolve(A, b, result);
    return result;
}

////////////////////////////////////////////////////////////////////////////
//  
//  FUNCTION:        VisDMatrixSolve
//  
//  DECLARATION:
//          void VisDMatrixSolve(const YMatrix& A, const YVector& b,
//                        YVector& x);
//  
//  INPUT:
//      A (const YMatrix&) - input matrix (l.h.s. of linear system)
//                  
//  
//      b (const YVector&) - input vector (r.h.s. of linear system)
//                  
//  
//      x (YVector&) - output vector (solution to linear system)
//                  
//  
//  DISCRIPTION:
//      Linear system solution
//  
////////////////////////////////////////////////////////////////////////////
VisMatrixExport void VISAPI VisDMatrixSolve(const YMatrix& A,
		const YVector& b, YVector& x)
{
    assert(A.NRows() == A.NCols() && A.NRows() > 0);
    assert(A.NRows() == b.Length());

#ifdef VIS_USE_IMSL

    VisIMSL_dlslrg(A.NRows(), A[0], A.NRows(), &b[0], 2, &x[0]);

#else // VIS_USE_IMSL

    // Use Gauss-Jordan elimination
    int i, j, n = A.NRows();
    YMatrix B(n, n+1);
    for (i = 0; i < n; i++) {       // copy into a single system
        for (j = 0; j < n; j++)
            B[i][j] = A[i][j];
        B[i][n] = b[i];
    }
    VisGaussJordanSolveDMatrix(B);
    for (i = 0; i < n; i++)
        x[i] = B[i][n];

#endif // VIS_USE_IMSL
}

// Symmetric Positive Definite matrix A:

////////////////////////////////////////////////////////////////////////////
//  
//  FUNCTION:        VisDMatrixSolveSPD
//  
//  DECLARATION:
//          YVector VisDMatrixSolveSPD(const YMatrix& A,
//                        const YVector& b);
//  
//  RETURN VALUE:
//      result of linear system solution
//  INPUT:
//      A (const YMatrix&) - input matrix
//  
//      b (const YVector&) - input vector (r.h.s. of linear system)
//                  
//  
//  DISCRIPTION:
//      Linear system solution of a symmetric positive definite matrix
//      
//  
////////////////////////////////////////////////////////////////////////////
VisMatrixExport YVector VISAPI VisDMatrixSolveSPD(const YMatrix& A,
		const YVector& b)
{
    YVector result(b.Length());
    VisDMatrixSolveSPD(A, b, result);
    return result;
}

////////////////////////////////////////////////////////////////////////////
//  
//  FUNCTION:        VisDMatrixSolveSPD
//  
//  DECLARATION:
//          void VisDMatrixSolveSPD(const YMatrix& A, const YVector& b,
//                      YVector& x, int n);
//  
//  INPUT:
//      A (const YMatrix&) - input matrix (l.h.s. of linear system)
//                  
//  
//      b (const YVector&) - input vector (r.h.s. of linear system)
//                  
//  
//      x (YVector&) - output vector (solution to linear system)
//                  
//  
//      n (int) - size of linear system (may be smaller than size of A)
//                  
//  
//  DISCRIPTION:
//      Linear system solution of a symmetric positive definite matrix
//      
//  
////////////////////////////////////////////////////////////////////////////
VisMatrixExport void VISAPI VisDMatrixSolveSPD(const YMatrix& A,
		const YVector& b, YVector& x, int n)
{
    if (n < 1)      // optionally solve a sub-system (faster)
        n = A.NRows();
    assert(A.NRows() == A.NCols() && A.NRows() > 0);
    assert(A.NRows() == b.Length());

#if defined(_DEBUG) && defined(VIS_USE_IMSL)

    // Before solving, check for all positive eigenvalues
    assert(A.IsSymmetric());
    YVector eigenvalues(b.Length());
    VisIMSL_devlsf(n, A[0], A.NCols(), &eigenvalues[0]);
    for (int i = 0; i < n; i++)
        assert(eigenvalues[i] > 0.0);

#endif // defined(_DEBUG) && defined(VIS_USE_IMSL)

    // Now solve it
#ifdef VIS_USE_IMSL

    x = 0.0;
    VisIMSL_dlslds(n, A[0], A.NCols(), &b[0], &x[0]);

#else // VIS_USE_IMSL

    // Use Gauss-Jordan elimination
    int i, j;
    YMatrix B(n, n+1);
    for (i = 0; i < n; i++) {       // copy into a single system
        for (j = 0; j < n; j++)
            B[i][j] = A[i][j];
        B[i][n] = b[i];
    }
    VisGaussJordanSolveDMatrix(B);
    for (i = 0; i < n; i++)
        x[i] = B[i][n];

#endif // VIS_USE_IMSL
}



////////////////////////////////////////////////////////////////////////////
//  
//  FUNCTION:        VisDMatrixLeastSquares
//  
//  DECLARATION:
//      YMatrix VisDMatrixLeastSquares(const YMatrix& A,
//                        const YMatrix& B);
//      void VisDMatrixLeastSquares(const YMatrix& A,
//                        const YMatrix& B, YMatrix& X);
//      YVector VisDMatrixLeastSquares(const YMatrix& A,
//                        const CVisDVectir& b);
//      void VisDMatrixLeastSquares(const YMatrix& A,
//                        const YVector& b, YVector& x);
//  
//  INPUT:
//      A (const YMatrix&) - input matrix (l.h.s. of linear system)
//                  
//      B (const YMatrix&) - input matrix (r.h.s. of linear system)
//  
//      X (YMatrix&) - output matrix (solution to linear system)
//
//      b (const YVector&) - input vector (r.h.s. of linear system)
//                  
//      x (YVector&) - output vector (solution to linear system)
//  
//  DISCRIPTION:
//      Least squares system solution using Householder transforms
//      (with IMSL), or using normal matrix solution (without)
//      
//  
////////////////////////////////////////////////////////////////////////////
VisMatrixExport YMatrix VISAPI VisDMatrixLeastSquares(const YMatrix& A,
		const YMatrix& B)
{
    YMatrix result(B.NRows(), B.NCols());
    VisDMatrixLeastSquares(A, B, result);
    return result;
}

VisMatrixExport void VISAPI VisDMatrixLeastSquares(const YMatrix& A,
		const YMatrix& B, YMatrix& X)
{
    assert(A.NRows() == B.NRows() && A.NRows() > 0);

#ifdef VIS_USE_IMSL

    // Transpose the matrices and form into one
    YMatrix M(A.NCols() + B.NCols(), A.NRows());
    for (int i = 0; i < A.NRows(); i++) {
        for (int j = 0; j < A.NCols(); j++)
            M[j][i] = A[i][j];
        for (j = 0; j < B.NCols(); j++)
            M[j+A.NCols()][i] = B[i][j];
    }

    // Use the IMSL least squares solver (Householder QR decomposition)
    YMatrix N(X.NCols(), X.NRows());
    VisIMSL_dlqrrv(A.NRows(), A.NCols(), B.NCols(), &M[0][0], A.NRows(),
                   &N[0][0], X.NRows());

    // Transpose the result
    for (i = 0; i < X.NRows(); i++) {
        for (int j = 0; j < X.NCols(); j++)
            X[i][j] = N[j][i];
    }

#else // VIS_USE_IMSL

    // Form the normal equations
    YMatrix An(A.NCols(), A.NCols()), Bn(B.NCols(), A.NCols());
    An = 0.0, Bn = 0.0;
    int k = 0;
    int i = 0;
    for (i = 0; i < A.NRows(); i++) {
        for (int j = 0; j < A.NCols(); j++) {
            for (k = 0; k < A.NCols(); k++)
                An[j][k] += A[i][j] * A[i][k];
            for (k = 0; k < B.NCols(); k++)
                Bn[k][j] += A[i][j] * B[i][k];
        }
    }

    // Solve for each column of X independently
    YVector b(X.NRows());
    for (int j = 0; j < X.NCols(); j++) {
        // Copy the input (rhs)
        for (i = 0; i < X.NRows(); i++)
            b[i] = Bn[i][j];

        // Solve the system
        YVector x = VisDMatrixSolveSPD(An, b);
        
        // Copy to the output (solution)
        for (i = 0; i < X.NRows(); i++)
            X[i][j] = x[i];
    }

#endif // VIS_USE_IMSL
}

VisMatrixExport YVector VISAPI VisDMatrixLeastSquares(const YMatrix& A,
		const YVector& b)
{
    YVector result(A.NCols());
    VisDMatrixLeastSquares(A, b, result);
    return result;
}

VisMatrixExport void VISAPI VisDMatrixLeastSquares(const YMatrix& A,
		const YVector& b, YVector& x)
{
    YMatrix B(b.Length(), 1, (double *) &b[0]);
    YMatrix X(x.Length(), 1, &x[0]);
    VisDMatrixLeastSquares(A, B, X);
}

////////////////////////////////////////////////////////////////////////////
//  
//  FUNCTION:     VisGaussJordanSolveDMatrix   
//  
//  DECLARATION:
//          static void VisGaussJordanSolveDMatrix(YMatrix& A)
//  
//  INPUT:
//    YMatrix& A - a matrix of coefficients.
//  
//  DISCRIPTION:
//
// Gauss-Jordan elimination, no pivoting:  not very stable or accurate
//
//  
////////////////////////////////////////////////////////////////////////////


static void VisGaussJordanSolveDMatrix(YMatrix& A)
{
    int n = A.NRows(), m = A.NCols(), i, j, k;
    
    // Reduce to triangular form
    for (i = 0; i < n; i++) {
        
        // Make sure diagonal entry is non-zero
        if (A[i][i] == 0.0) {
            for (j = i+1; j < n; j++)
                if (A[j][i] != 0.0)   // find non-zero entry
                    break;
            if (j >= n)       // if failed, matrix is singular
				assert (0);
                //throw CVisError("matrix is singular", eviserrorOpFailed,
                //         "VisGaussJordanSolveDMatrix()", __FILE__, __LINE__);
            for (k = i; k < m; k++)
                A[i][k] += A[j][k];
        }

        // VisDMatrixScale this row to unity
        double aii_inv = 1.0 / A[i][i];
        A[i][i] = 1.0;
        for (k = i+1; k < m; k++)
            A[i][k] *= aii_inv;

        // Subtract from other rows
        for (j = i+1; j < n; j++) {
            double aij = A[j][i];
            A[j][i] = 0.0;
            for (k = i+1; k < m; k++)
                A[j][k] -= aij * A[i][k];
        }
    }

    // Back substitute
    for (i = n-1; i > 0; i--) {
        for (j = 0; j < i; j++)  {
            double aji = A[j][i];
            A[j][i] = 0.0;
            for (k = n; k < m; k++)
                A[j][k] -= aji * A[i][k];
        }
    }
}



// QR factorization related functions
VisMatrixExport YVector VISAPI VisDMatrixSolveQR(const YMatrix& A,
		const YVector& b)
{
    int nr = A.NRows(); 
    int nc = A.NCols(); 
    assert(nr >= nc && nc > 0);
    assert(nr == b.Length());

    YVector result(nc);
    VisDMatrixSolveQR(A, b, result);
    return result;
}

// IMSL (Fortune) takes column input for a matrix (unlike C which uses row
// indexing
// This is why transpose of A is used 
// unlike SPD solver, QR solver doesn't require A to be square
VisMatrixExport void VISAPI VisDMatrixSolveQR(const YMatrix& A,
		const YVector& b, YVector& x)
{
    int nr = A.NRows(); 
    int nc = A.NCols(); 
    assert(nr >= nc && nc > 0);
    assert(nr == b.Length());

    // Transpose the matrice
    YMatrix M = A.Transposed(); 

    x = 0.0; 
    //VisIMSL_SolveQR(nr, nc, A[0], &b[0], &x[0]);
#ifdef VIS_USE_IMSL
    VisIMSL_SolveQR(nr, nc, M[0], &b[0], &x[0]);
#else // VIS_USE_IMSL
	assert (0);
    //throw CVisError("QR solver only works with IMSL for now",
    //                eviserrorOpFailed, "VisDMatrixSolveQR",
    //                "VisDMatrix.cpp", __LINE__);
#endif // VIS_USE_IMSL
}

////////////////////////////////////////////////////////////////////////////
//  
//  FUNCTION:        VisDMatrixSVD
//  
//  DECLARATION:
//      void VisDMatrixSVD(const YMatrix& A, YVector& s,
//                        YMatrix& U, YMatrix& V,
//                        int compute_left = 1, compute_right = 1);
//  
//  INPUT:
//      A (const YMatrix&) - input matrix
//                  
//      s (YVector&) - singular values (output vector)
//
//      U (YMatrix&) - left singular vectors
//  
//      V (YMatrix&) - right singular vectors
//
//      compute_left (int) - compute the left singular vectors
//  
//      compute_right (int) - compute the right singular vectors
//  
//  DISCRIPTION:
//      Singular value decomposition (SVD), using IMSL routine.
//      The matrices returned are of the form A = U diag(s) V^T
//      with the columns of U and V being orthonormal.
//
//      If A in M x N, then s must be of length N,
//      U must be M x N (if computed), and V must be N x N (if computed).
//  
////////////////////////////////////////////////////////////////////////////

VisMatrixExport void VISAPI VisDMatrixSVD(const YMatrix& A, YVector& s, 
                  YMatrix& U, YMatrix& V,
                  int compute_left, int compute_right)
{
#ifdef VIS_USE_IMSL
    long nr = A.NRows(); 
    long nc = A.NCols();
    YVector s2(__min(nr+1, nc));    // required by LSVRR
    YMatrix AT(nc, nr);             // A^T
    YMatrix UT(compute_left ? nc : 1, compute_left ? nr : 1);
    YMatrix VT(compute_right ? nc : 1, compute_right ? nc : 1);
    double eps = 0.0;       // ignored
    long rank_a;             // ignored
    long iPath = compute_left * 20 + compute_right;

    // Perform the transpose
    for (int r = 0; r < nr; r++)
        for (int c = 0; c < nc; c++)
            AT[c][r] = A[r][c];

    VisIMSL_dlsvrr(nr, nc, &AT[0][0], nr, iPath, eps, rank_a,
                   &s2[0], &UT[0][0], nr, &VT[0][0], nc);

    // Copy the singular values
    memcpy(&s[0], &s2[0], __min(s2.Length(), s.Length()) * sizeof(double));
    // cout << "U^T = " << UT << "s = " << s << "V^T = " << VT;

    // Perform the transpose
    if (compute_left) {
        U = 0.0;    // in case nr < nc
        int nc2 = __min(nr, nc);
        for (int r = 0; r < nr; r++)
            for (int c = 0; c < nc2; c++)
                U[r][c] = UT[c][r];
    }
    if (compute_right) {
        for (int r = 0; r < nc; r++)
            for (int c = 0; c < nc; c++)
                V[r][c] = VT[c][r];
    }
#else // VIS_USE_IMSL
	assert (0);
	UNUSED (A);
	UNUSED (s);
	UNUSED (U);
	UNUSED (V);
	UNUSED (compute_left);
	UNUSED (compute_right);
    //throw CVisError(
	//		"Singular value decomposition only works with IMSL for now",
      //              eviserrorOpFailed, "VisDMatrixSVD",
      //              "VisDMatrix.cpp", __LINE__);
#endif // VIS_USE_IMSL
}


////////////////////////////////////////////////////////////////////////////
//  
//  FUNCTION:        VisDMatrixSVD
//  
//  DECLARATION:
//      void VisDMatrixSVD(YMatrix& a, YVector& w, YMatrix& v);
//      void VisDMatrixSVD(const YMatrix& a,
//						   YMatrix& u, 
//						   YVector& w, 
//						   YMatrix& v);
//      void VisDMatrixSVD(const YMatrix& a, 
//                         const YVector& b, 
//                         YVector& x);
//  
//  INPUT:
//      a (const YMatrix&) - input matrix and U on exit!
//                  
//      w (YVector&) - singular values (output vector)
//
//      v (YMatrix&) - right singular vectors (output matrix)
//  
//		x (YVector&) - solution of the LS problem using SVD
//
//		b (YVector&) - r.h.s. of the linear equation to be solved
//
//  DISCRIPTION:
//      Singular value decomposition (SVD), using NR routine.
//      The matrices returned are of the form A = U diag(w) V^T
//      with the columns of U and V being orthonormal.
//
//      If A in M x N, then w must be of length N,
//      U is of course M x N, and V must be N x N.
//  
////////////////////////////////////////////////////////////////////////////
VisMatrixExport void VISAPI VisDMatrixSVD(YMatrix& a, YVector& w, YMatrix& v)
{
	SVD(a,w,v);	
}

VisMatrixExport void VISAPI VisDMatrixSVD(const YMatrix& a,
										  YMatrix& u,
										  YVector& w,
										  YMatrix& v)
{
	u = a;
	SVD(u,w,v);
}

VisMatrixExport void VISAPI VisDMatrixSVD(const YMatrix& a,
										  const YVector& b,
										  YVector& x)
{
	const double Tolerance=1e-6;
	YMatrix u = a;
	YMatrix v(a.NCols(),a.NCols());
	YVector w(a.NCols());
	int k;

	SVD(u,w,v);

	double wmax,wmin;
	wmax=wmin=0.0;
	for (k=1;k<=a.NCols();k++) 
		if (w(k)>wmax) wmax=w(k);

	wmin=wmax*Tolerance;
	
	for (k=1;k<=a.NCols();k++) 
		if (w(k)<wmin) w(k)=0.0;

	SvdSolve(u,w,v,b,x);
}

////////////////////////////////////////////////////////////////////////////
//  
//  FUNCTION:        VisDMatrixLU
//  
//  DECLARATION:
//      void VisDMatrixLU(YMatrix& a, YVector& indx, double& d);
//      void VisDMatrixLU(const YMatrix& a,
//						  const YVector& b, 
//						  YVector& x);
//  
//  INPUT:
//      a (const YMatrix&) - input matrix and U on exit!
//                  
//		x (YVector&) - solution of the LS problem using LU
//
//		b (YVector&) - r.h.s. of the linear equation to be solved
//
//  DISCRIPTION:
//      LU decomposition, using NR routine.
//
//      A must be square. 
//  
////////////////////////////////////////////////////////////////////////////
VisMatrixExport void VISAPI VisDMatrixLU(YMatrix& a, YVector& indx, double& d)
{
	LU (a,indx,d);
}

VisMatrixExport void VISAPI VisDMatrixLU(const YMatrix& a,
										 const YVector& b,
										 YVector& x)
{
	YMatrix _acopy(a);
	YVector _bcopy(b);
	YVector indx(b.Length());
	double d;

	LU (_acopy,indx,d);
	LuSolve(_acopy,indx,_bcopy);
	x=_bcopy;
}

VisMatrixExport void VISAPI VisMinEigenValue(YMatrix& A, YVector& x)
{
#ifdef OLDANDSLOW
	int N = A.NRows();

	YVector Eval(N);
	YMatrix Evec(N, N);

	VisIMSL_devcsf(N, A[0], N, &Eval[0], Evec[0], N);
	for (int i = 0; i < N; i++)
		x[i] = Evec[N-1][i]; 
#else // OLDANDSLOW
    VisMinMaxEigenValue(A, x, true); 
#endif // OLDANDSLOW
}

VisMatrixExport void VISAPI VisMaxEigenValue(YMatrix& A, YVector& x)
{
#ifdef OLDANDSLOW
	int N = A.NRows();

	YVector Eval(N);
	YMatrix Evec(N, N);

	VisIMSL_devcsf(N, A[0], N, &Eval[0], Evec[0], N);
	for (int i = 0; i < N; i++)
		x[i] = Evec[0][i]; 
#else // OLDANDSLOW
    VisMinMaxEigenValue(A, x, false); 
#endif // OLDANDSLOW
}

VisMatrixExport double VISAPI VisMinMaxEigenValue(YMatrix& A, YVector& x,
		bool fMin)
{
	int n = A.NRows();
    double e = 0.0; 

    assert(x.Length() == n); 

#ifdef VIS_USE_IMSL
	VisIMSL_devesf(n, 1, A[0], n, (fMin & 1 : 0), &e, &x[0], n);
#else // VIS_USE_IMSL
	assert (0);
	UNUSED (A);
	UNUSED (x);
	UNUSED (fMin);

    //throw CVisError("Eigenvalues only work with IMSL for now",
      ///              eviserrorOpFailed, "VisMinMaxEigenValue",
         //           "VisDMatrix.cpp", __LINE__);
#endif // VIS_USE_IMSL

    return e; 
}

// implementation of Equality constrained Least-Squares on p.585 Golub and
// Van Loan also see MSR-TR-97-23
// current implementation assumes rank(A) = n and rank(B) = p -> user's
// responsibility
VisMatrixExport void VISAPI VisDMatrixEQConstrainedLS(YMatrix& A, YVector& b,
		YMatrix& C, YVector& d, YVector& x)
{
#ifdef VIS_USE_IMSL
    int m = A.NRows(); 
    int n = A.NCols(); 
    int p = C.NRows(); 
    int i, j; 

    assert(b.Length() == m);
    assert(x.Length() == n); 
    assert(d.Length() == p); 

    YVector x1(p); 
    YVector x2(n-p); 

    x1 = 0.0; x2 = 0.0; 

    // QR factorization of C^T, i.e., C^T = Q [R | 0]^T
    // YMatrix QR(n, p); 
    YMatrix Q(n, n); 
    YMatrix RT(p, p); 
    long* ipvt = new long[p]; 
    long pivot = 1; 

    YMatrix QRTrans(p, n); 
    YMatrix QTrans(n, n); 

    VisIMSL_GetQR(n, p, C[0], QTrans[0], QRTrans[0], ipvt, pivot); 

    // copy RT from QRTrans
    // Not necessary to create RT, but for clarity
    for (i = 0; i < p; i++) 
    {
        for (j = 0; j < p; j++)
        {
            RT[i][j] = QRTrans[i][j];
        }
    }

	Q = QTrans.Transposed();

    // permute d by P^{-1} d = dPermu
    // this step is necessary: see RoomBuilder paper
    // double* dPermu = new double[p]; 
    YVector dPermu(p); 
    VisIMSL_DPERMU(p, &d[0], ipvt, 1, &dPermu[0]);

    // AQ = [A1, A2]
    YMatrix AQ = A * Q; 
    YMatrix A1(m, p); 
    YMatrix A2(m, n-p); 

    for (i = 0; i < m; i++)
    {
        for (j = 0; j < p; j++)
        {
            A1[i][j] = AQ[i][j]; 
        }
        for (j = p; j < n; j++)
        {
            A2[i][j-p] = AQ[i][j]; 
        }
    }

    // solve x1; RT is a lower triangular matrix
    // or we can call IMSL to solve it
    for (i = 0; i < p; i++)
    {
        double sum = 0; 
        for (j = 0; j < i; j++)
        {
            sum += RT[i][j] * x1[j]; 
        }
        x1[i] = (dPermu[i] - sum)/RT[i][i]; 
    }

    // solve x2; unconstrained LS
    YVector b1 = A1 * x1; 
    YVector b2 = b - b1; 
    VisDMatrixLeastSquares(A2, b2, x2); 

    // get x
    YVector z(n); 
    for (j = 0; j < p; j++)
        z[j] = x1[j];
    for (j = p; j < n; j++)
        z[j] = x2[j-p]; 

    x = Q * z; 
    
    delete ipvt; 
    //delete dPermu; 
#else // VIS_USE_IMSL
	assert (0);
	UNUSED (A);
	UNUSED (b);
	UNUSED (C);
	UNUSED (d);
	UNUSED (x);
	
    //throw CVisError("Constrained least squares only works with IMSL for now",
      //              eviserrorOpFailed, "VisDMatrixEQConstrainedLS",
        //            "VisDMatrix.cpp", __LINE__);
#endif // VIS_USE_IMSL
}


//
//  Standard library Input/output (for debugging, mostly)
//

bool YVector::s_fVerboseOutput = true;
bool YMatrix::s_fVerboseOutput = true;


////////////////////////////////////////////////////////////////////////////
//  
//  FUNCTION:        operator<<
//  
//  DECLARATION:
//          ostream& operator<<(ostream& os, const YVector& v);
//  
//  RETURN VALUE:
//      reference to output stream
//  INPUT:
//      os (ostream&) - output stream
//  
//      v (YVector&) - vector being printed
//  
//  DISCRIPTION:
//      Print vector on output stream
//  
////////////////////////////////////////////////////////////////////////////
#ifndef __QNX__
VisMatrixExport std::ostream& VISAPI operator<<(std::ostream& os, const YVector& v)
#else
VisMatrixExport ostream& VISAPI operator<<(ostream& os, const YVector& v)
#endif
{
    if (YVector::s_fVerboseOutput)
        os << "# YVector<" << v.Length() << "> = {";
    else
        os << "{";
    for (int i = 0; i < v.Length(); i++)
        os << v[i] << ((i == v.Length()-1) ? "}" : ", ");
#ifndef __QNX__
    if (YVector::s_fVerboseOutput)
        os << std::endl;
#else
    if (YVector::s_fVerboseOutput)
        os << endl;
#endif

    return os;
}

////////////////////////////////////////////////////////////////////////////
//  
//  FUNCTION:        operator<<
//  
//  DECLARATION:
//          ostream& operator<<(ostream& os, const YMatrix& mat );
//  
//  RETURN VALUE:
//      reference to output stream
//  INPUT:
//      os (ostream&) - output stream
//  
//      v (YVector&) - matrix being printed
//  
//  DISCRIPTION:
//      Print matrix on output stream
//  
////////////////////////////////////////////////////////////////////////////

#ifndef __QNX__
VisMatrixExport std::ostream& VISAPI operator<<(std::ostream& os, const YMatrix& mat)
#else
VisMatrixExport ostream& VISAPI operator<<(ostream& os, const YMatrix& mat)
#endif
{
#ifndef __QNX__
    if (YMatrix::s_fVerboseOutput)
        os << "# YMatrix<" << mat.NRows() << "," << 
                              mat.NCols() << "> =" << std::endl;
#else
    if (YMatrix::s_fVerboseOutput)
        os << "# YMatrix<" << mat.NRows() << "," << 
                              mat.NCols() << "> =" << endl;
#endif
    for (int i = 0; i < mat.NRows(); i++) {
        os << ((i == 0) ? "{{" :  " {");
        for (int j = 0; j < mat.NCols(); j++)
            os << mat[i][j] << ((j == mat.NCols()-1) ? "}" : ", ");
        os << ((i == mat.NRows()-1) ? "}" : ",\n");
    }
#ifndef __QNX__
    if (YMatrix::s_fVerboseOutput)
        os << std::endl;
#else
    if (YMatrix::s_fVerboseOutput)
        os << endl;
#endif

    return os;
}



//
//  Self-describing stream Input/output
//
// LATER:  We might want to add newlines and indents to format this better.
//


// Global variables used with self-describing streams.
// LATER:  Find a way to avoid using these global variables.
#ifdef VIS_INCLUDE_SDSTREAM
VisMatrixExport CVisSDObject<YVector> g_visdvectorExemplar;
VisMatrixExport CVisSDObject<YMatrix> g_visdmatrixExemplar;
#endif

#if 0 // REMOVED
const char *YVector::ReadWriteField(CVisSDStream& s, int field_id)
{
#ifdef VIS_INCLUDE_SDSTREAM
    switch (FieldType(field_id))
	{
    case eftName:
        return "class YVector {";

    case eftLength:
		if (s.Status() == CVisSDStream::Read)
		{
			int lengthNew = m_length;
			s << lengthNew;
			assert(lengthNew >= 0);
			if (lengthNew != m_length)
				Resize(lengthNew);
		}
		else
		{
			s << m_length;
		}
		return "int m_length;             // number of elements";

    case eftData:
		s.OpenParen();
		for (int i = 0; i < m_length; i++)
		{
			s << m_data[i];
			if (i < m_length - 1)
				s.Comma();
		}
		s.CloseParen();
        return "double m_data[m_length];  // elements";
    }

    return 0;
#else
	assert(false);  // SDStreams are not included
	return 0;
#endif
}


const char *YMatrix::ReadWriteField(CVisSDStream& s, int field_id)
{
#ifdef VIS_INCLUDE_SDSTREAM
    switch (FieldType(field_id))
	{
    case eftName:
        return "class YMatrix {";

    case eftDims:
		s.OpenParen();
		if (s.Status() == CVisSDStream::Read)
		{
			int nRowsNew = m_nRows;
			int nColsNew = m_nCols;
			s << nRowsNew;
			s.Comma();
			s << nColsNew;
			assert((nRowsNew >= 0) && (nColsNew >= 0));
			if ((nRowsNew != m_nRows) || (nColsNew != m_nCols))
				Resize(nRowsNew, nColsNew);
		}
		else
		{
			s << m_nRows;
			s.Comma();
			s << m_nCols;
		}
		s.CloseParen();
		return "int m_nRows, m_nCols;     // dimensions";

    case eftData:
		s.NewLine();
		s.OpenParen();
		for (int r = 0; r < m_nRows; r++) {
			s.OpenParen();
			for (int c = 0; c < m_nCols; c++) {
				s << (*this)[r][c];
				if (c < m_nCols - 1)
					s.Comma();
			}
			s.CloseParen();
			if (r < m_nRows - 1)
				s.Comma(), s.NewLine();
		}
		s.CloseParen();
		s.NewLine();
        return "double m_data[m_nRows][m_nCols];  // elements";
    }

    return 0;
#else
	assert(false);  // SDStreams are not included
	return 0;
#endif
}
#endif

//
//
//

//
// New operator(s) by pasa.
//

VisMatrixExport YVector VISAPI operator*(double d,const YVector& A)
{
    YVector vectorRet(A);

    return (vectorRet *= d); 
}

VisMatrixExport YMatrix VISAPI operator*(double d,const YMatrix& A)
{
    YMatrix matrixRet(A);

    return (matrixRet *= d);
}

VisMatrixExport YVector VISAPI operator+(double d,const YVector& A)
{
	YVector vectorRet(A);

	return (vectorRet += d);
}

VisMatrixExport YVector VISAPI operator-(double d,const YVector& A)
{
	YVector vectorRet(A);

	return (vectorRet -= d);
}

VisMatrixExport YMatrix VISAPI operator+(double d,const YMatrix& A)
{
	YMatrix matrixRet(A);

	return (matrixRet += d);
}

VisMatrixExport YMatrix VISAPI operator-(double d,const YMatrix& A)
{
	YMatrix matrixRet(A);

	return (matrixRet -= d);
}

////////////////////////////////////////////////////////////////////////////
//  
//  FUNCTION:        sin,cos
//  
//  DECLARATION:
//          YVector YVector::cos(const YVector &A);
//          YVector YVector::sin(const YVector &A);
//  
//  RETURN VALUE:
//		reference to l.h.s.
//
//  INPUT:
//      none 
//  
//  DISCRIPTION:
//      Compute the sin (cos) of the vector.
//  
////////////////////////////////////////////////////////////////////////////
VisMatrixExport YVector VISAPI cos(const YVector &A)
{
	YVector vectorRet(A);

	return vectorRet.cos();
}

VisMatrixExport YVector VISAPI sin(const YVector &A)
{
	YVector vectorRet(A);

	return vectorRet.sin();
}

