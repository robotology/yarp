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
/// $Id: YARPMatrix.h,v 1.1 2006-03-15 09:31:28 eshuy Exp $
///
///

///////////////////////////////////////////////////////////////////////////
//
// NAME
//  YARPMatrix.h -- double precision matrix/vector operations
//
// DESCRIPTION
//  The YMatrix class provides some basic matrix operations, using calls
//  to external software (IMSL for the moment) to perform the more
//  complicated operations.
//
//  To take advantage of the IMSL numerical analysis routines,
//  define VIS_USE_IMSL in the Build Settings C/C++ property page.
//
// Copyright © 1996-1998 Microsoft Corporation, All Rights Reserved
//
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//
// Modifiche di PASA. 
//
// OK --> 1) Aggiunta operatori del tipo double op Matrix/Vector.
// OK --> 1.1) Aggiunta operatori del tipo op= double e somma con double.
// 2) Supporto least square (copiate da NR). ----->>Da completare<<-----
// 2.bis) Supporto QR decomposition.
// OK --> 3) Operatore () con indici che partono da 1.
// OK --> 4) Operatori cos e sin (solo per vettori).
// 5) moltiplicazione vettore * matrice.
// 6) allocazione automatica su op= vettore/matrice non allocata (da testare). 
//
// 
///////////////////////////////////////////////////////////////////////////

// Readapted by pasa on Dec 2000
// Avoid MFC.
// Dll-> Static library.
// Modified class names,
// 
 
#ifndef __VIS_MATRIX_DMATRIX_H__
#define __VIS_MATRIX_DMATRIX_H__

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include <yarp/YARPConfig.h>

#include <iostream>

#include <math.h>
#include <assert.h>

#define VISAPI 
#define VisMatrixExport

//#ifndef _CRTDBG_MAP_ALLOC
//#define _CRTDBG_MAP_ALLOC
//#endif

// UNDONE: Move inline functions to an INL file.


// LATER:  Could we use std::vector internally or derive from it?  (Would
// the data be adjacent in memory and the operators be as efficient?)


// LATER:  Should there be options to initialize elements when creating or
// resizing?

////////////////////////////////////////////////////////////////////////////
//  
//  CLASS:
//      YVector
//  
////////////////////////////////////////////////////////////////////////////
class YVector
{
public:
    // Constructors (default destructor and copy constructor)
    YVector(void);
    YVector(int length, const double *storage = 0);
    YVector(const YVector &vec);
	virtual ~YVector ();

    // Vector length
    int Length(void) const { return m_length; }
    void Resize(int length, const double *storage = 0);

    // Element access
    double& operator[](int i) { return m_data[i]; }
    const double& operator[](int i) const { return m_data[i]; }

    double& operator()(int i) { return m_data[--i]; }
    const double& operator()(int i) const { return m_data[--i]; }

    // Assignment
    YVector& operator=(const YVector &vec);
    YVector& operator=(double value);

    // We define operator < so that this class can be used in STL containers.
    bool operator==(const YVector& refvector) const;
    bool operator!=(const YVector& refvector) const;
    bool operator==(double dbl) const;
    bool operator!=(double dbl) const;
    bool operator<(const YVector& refvector) const;
	
    YVector operator+(const YVector &b) const;

    YVector operator-(void) const;
    YVector operator-(const YVector &b) const;

    YVector operator*(double dbl) const;
    double operator*(const YVector &b) const;   // dot product

    // norm 2.
    double norm2(void) const;
    double norm2square(void) const;

    // assume vector is row and multiply.
    //YVector operator*(const YMatrix &A) const;

    YVector operator/(double dbl) const;

    YVector& operator+=(const YVector &b);
    YVector& operator-=(const YVector &b);

    YVector& operator+=(double dbl);
    YVector& operator-=(double dbl);
    YVector operator+(double dbl) const;
    YVector operator-(double dbl) const;

    YVector& operator*=(double dbl);
    YVector& operator/=(double dbl);

    YVector& cos(void);
    YVector& sin(void);

    // Self-describing input/output format
    //enum FieldType { eftName, eftLength, eftData, eftEnd};
    //const char *ReadWriteField(CVisSDStream& s, int field_id);

    // Flag used with the std::ostream file I/O methods.
    // This may not be supported in future releases.
    static bool s_fVerboseOutput; // print out dimensions on output

    inline double * data(void) { return m_data; }
    inline const double * data(void) const {return m_data; }

 private:
    int m_length;       // number of elements
    double *m_data;     // pointer to data
    //CVisMemBlockOf<double> m_memblockStorage;  // reference counted storage
};


////////////////////////////////////////////////////////////////////////////
//  
//  CLASS:
//      YMatrix
//  
////////////////////////////////////////////////////////////////////////////
class YMatrix
{
 public:
  // Constructors (default destructor and copy constructor)
  YMatrix(void);
  YMatrix(int rows, int cols, const double *storage = 0);
  YMatrix(const YMatrix &mat);
  virtual ~YMatrix ();

  // Matrix shape
  int NRows(void) const { return m_nRows; }
  int NCols(void) const { return m_nCols; }
  void Resize(int rows, int cols, const double *storage = 0);

  // Element access
  double *operator[](int i) { return m_data[i]; }
  const double *operator[](int i) const { return m_data[i]; }

  double& operator()(int i,int j) { return m_data[--i][--j]; }
  const double& operator()(int i,int j) const { return m_data[--i][--j]; }

  // Assignment
  YMatrix& operator=(const YMatrix &mat);
  YMatrix& operator=(double value);

  // We define operator < so that this class can be used in STL containers.
  bool operator==(const YMatrix& refmatrix) const;
  bool operator!=(const YMatrix& refmatrix) const;
  bool operator==(double dbl) const;
  bool operator!=(double dbl) const;
  bool operator<(const YMatrix& refmatrix) const;

  YMatrix operator+(double dbl) const;
  YMatrix operator-(double dbl) const;
  YMatrix operator+(const YMatrix& A) const;

  YMatrix operator-(void) const;
  YMatrix operator-(const YMatrix& A) const;

  YMatrix operator*(double dbl) const;
  YMatrix operator*(const YMatrix& refmatrix) const;
  YVector operator*(const YVector& refvector) const;

  YMatrix operator/(double dbl) const;
    
  YMatrix& operator+=(const YMatrix& A);
  YMatrix& operator-=(const YMatrix& A);
  YMatrix& operator+=(double dbl);
  YMatrix& operator-=(double dbl);
  YMatrix& operator*=(double dbl);
  YMatrix& operator*=(const YMatrix& A);
  YMatrix& operator/=(double dbl);
	
  bool IsSymmetric(void) const;

  YMatrix Inverted(void) const;
  YMatrix& Invert(void);

  YMatrix Transposed(void) const;
  YMatrix& Transpose(void);

  // Self-describing input/output format
  //enum FieldType { eftName, eftDims, eftData, eftEnd};
  //const char *ReadWriteField(CVisSDStream& s, int field_id);
    
  // Flag used with the std::ostream file I/O methods.
  // This may not be supported in future releases.
  static bool s_fVerboseOutput; // print out dimensions on output

  inline double ** data(void) { return m_data; }
  inline const double ** data(void) const { return (const double **) m_data; };

 private:
  int m_nRows;        // number of rows in matrix
  int m_nCols;        // number of columns in matrix
  double **m_data;    // Iliffe vector (array of pointers to data)
  //CVisMemBlockOf<double> m_memblockStorage;  // reference counted storage
  //CVisMemBlockOf<double *> m_memblockIliffe;   // Iliffe vector r. c. storage
};


//
//  Supported functions
//

//
// New pasa operator(s).
//
VisMatrixExport YVector VISAPI operator*(double d,const YVector& A);
VisMatrixExport YMatrix VISAPI operator*(double d,const YMatrix& A);

VisMatrixExport YVector VISAPI operator+(double d,const YVector& A);
VisMatrixExport YVector VISAPI operator-(double d,const YVector& A);
VisMatrixExport YMatrix VISAPI operator+(double d,const YMatrix& A);
VisMatrixExport YMatrix VISAPI operator-(double d,const YMatrix& A);

VisMatrixExport YVector VISAPI cos(const YVector& A);
VisMatrixExport YVector VISAPI sin(const YVector& A);

// LATER:  Add these and fns to work with other exponents.
// VisMatrixExport YMatrix VISAPI VisMatrixSqrt(const YMatrix& A);
// VisMatrixExport void VISAPI VisMatrixSqrt(const YMatrix& A, YMatrix &Asqrt);

VisMatrixExport YMatrix VISAPI VisDMatrixSqrtInverse(const YMatrix& A);
VisMatrixExport void VISAPI VisDMatrixSqrtInverse(const YMatrix& A,
		YMatrix &AsqrtInv);

VisMatrixExport YVector VISAPI VisDMatrixSolve(const YMatrix& A,
		const YVector& b);
VisMatrixExport void VISAPI VisDMatrixSolve(const YMatrix& A,
		const YVector& b, YVector& x);



// A is symmetric positive definite
VisMatrixExport YVector VISAPI VisDMatrixSolveSPD(const YMatrix& A,
		const YVector& b);
VisMatrixExport void VISAPI VisDMatrixSolveSPD(const YMatrix& A,
		const YVector& b, YVector& x, int n = -1);

VisMatrixExport YMatrix VISAPI VisDMatrixLeastSquares(const YMatrix& A,
		const YMatrix& B);
VisMatrixExport void VISAPI VisDMatrixLeastSquares(const YMatrix& A,
		const YMatrix& B,
                           YMatrix& X);
VisMatrixExport YVector VISAPI VisDMatrixLeastSquares(const YMatrix& A,
		const YVector& b);
VisMatrixExport void VISAPI VisDMatrixLeastSquares(const YMatrix& A,
		const YVector& b,
                           YVector& x);



// QR factorization related functions
VisMatrixExport YVector VISAPI VisDMatrixSolveQR(const YMatrix& A,
		const YVector& b);
VisMatrixExport void VISAPI VisDMatrixSolveQR(const YMatrix& A,
		const YVector& b, YVector& x);
VisMatrixExport void VISAPI VisDMatrixEQConstrainedLS(YMatrix& A,
		YVector& b, YMatrix& C, YVector& d, YVector& x); 


// Singular Value Decomposition (SVD)
VisMatrixExport void VISAPI VisDMatrixSVD(const YMatrix& A, YVector& s, 
		YMatrix& U, YMatrix& V, int compute_left = 1,
		int compute_right = 1);

/**
 * SVD decomposition.
 * SVD(A, W, V)
 * Returns the singular value decomposition of A (in Matlab this is what is 
 * called the "economy size" SVD). 
 * SVD(A)=A*S*V'
 * where A=[mxn]; S=[nxn]; V=[nxn]
 * and S=diag(w)
 * The algorithm assumes m>=n and requires a,w,v be correctly allocated.
 * @param a YMatrix which contains the matrix to be decomposed (mxn). On exit it will
 * be filled with the values of A (mxn)
 * @param w a YVector which will contain the singular values (1xn)
 * @param v YMatrix which will contains V (nxn)
 */
void SVD(YMatrix& a, YVector& w, YMatrix& v);
void SvdSolve(const YMatrix& u,
			  const YVector& w, 
              const YMatrix& v,
			  const YVector& b,
			  YVector& x);
 
VisMatrixExport void VISAPI VisDMatrixSVD(YMatrix& a, YVector& w, YMatrix& v);
VisMatrixExport void VISAPI VisDMatrixSVD(const YMatrix& a, 
					  YMatrix& u, 
					  YVector& w, 
					  YMatrix& v);
VisMatrixExport void VISAPI VisDMatrixSVD(const YMatrix& a,
					  const YVector& b,
					  YVector& x);
//
// LU decomposition.
//
void LU(YMatrix& a, YVector& indx, double& d);
void LuSolve(YMatrix& a, YVector& indx, YVector& b);

VisMatrixExport void VISAPI VisDMatrixLU(YMatrix& a, YVector& indx, double& d);
VisMatrixExport void VISAPI VisDMatrixLU(const YMatrix& a,
					 const YVector& b,
					 YVector& x);

// LATER:  Variations that don't find the eigenvector.
VisMatrixExport void VISAPI VisMinEigenValue(YMatrix& A, YVector& x);
VisMatrixExport void VISAPI VisMaxEigenValue(YMatrix& A, YVector& x);

// min = 1 -> minimum eigenvalue problem; min = 0 -> maximum eigenvalue
VisMatrixExport double VISAPI VisMinMaxEigenValue(YMatrix& A, YVector& x,
		bool fMin); 



#ifndef __QNX__
//  Rudimentary output for debugging
// This may not be supported in future releases.
std::ostream& VISAPI operator<<(std::ostream& os, const YVector& v);
// VisMatrixExport istream& operator>>(istream& os, YVector& v);
// This may not be supported in future releases.
std::ostream& VISAPI operator<<(std::ostream& os, const YMatrix& mat);
// VisMatrixExport istream& operator>>(istream& os, YVector& v);

#else

//  Rudimentary output for debugging
// This may not be supported in future releases.
ostream& VISAPI operator<<(ostream& os, const YVector& v);
// VisMatrixExport istream& operator>>(istream& os, YVector& v);
// This may not be supported in future releases.
ostream& VISAPI operator<<(ostream& os, const YMatrix& mat);
// VisMatrixExport istream& operator>>(istream& os, YVector& v);
#endif


// Self-describing stream I/O
//inline CVisSDStream& operator<<(CVisSDStream& s, YVector& o);
//inline CVisSDStream& operator<<(CVisSDStream& s, YMatrix& o);

#include "YARPMatrix.inl"
#include "YARPVectorPortContent.h"

#endif // __VIS_MATRIX_DMATRIX_H__
