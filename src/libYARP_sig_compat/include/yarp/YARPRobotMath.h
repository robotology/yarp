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
/// $Id: YARPRobotMath.h,v 1.1 2006-03-15 09:31:28 eshuy Exp $
///
///

///////////////////////////////////////////////////////////////////////////
//
// NAME
//  YARPRobotMath.h -- double precision matrix/vector operations for robotics
//
// DESCRIPTION
//
// Per ora sono stati implementati alcuni tipi e funzioni di utilita' 
// generale.. sarebbe bello poter estendere e migliorare tutto quanto.
// 
///////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////
//
// tipi definiti in questo file:
//	YHmgTrsf  4x4
//	YRotMatrix 3x3
//	YDiff   6x1
//	YForce  6x1
//	Y3DVector 3x1 
//
//
//////////////////////////////////////////////////////////////////////////

//
// Pasa's note: da completare:
//	1. assertions.
//	2. migliorare alcune routines.. troppe copie!
//	3. transYDiff & YForce non sono stati riscritti (da fare!).
// 

#ifndef __YARPRobotMathh__
#define __YARPRobotMathh__

#include <yarp/YARPConfig.h>

#ifdef WIN32

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#endif

#include <yarp/YARPMatrix.h>

//
// general purpose constants.
const double pi			=3.14159265359;       // PI definition.
const double radToDeg	=57.29577951308;	  // rad to deg conversion.
const double degToRad	=0.01745329251994;	  // vv.

//
// forward definition.
//
class Y3DVector;
class YRotMatrix;

//
// homogeneous transform
//
class VisMatrixExport YHmgTrsf : public YMatrix {
public:
	YHmgTrsf(void);
	virtual ~YHmgTrsf() {}

	YHmgTrsf(const YHmgTrsf&);
	YHmgTrsf(const YMatrix&);
	YHmgTrsf(const double);

    void operator=(const YMatrix& m) { YMatrix::operator=(m); }
    void operator=(double f) { YMatrix::operator=(f); }
    //void operator=(const YHmgTrsf& m) { operator=((const YMatrix&)m); }

	Y3DVector Position(void) const;
	YRotMatrix Rotation(void) const;
	Y3DVector Z(void) const;
	Y3DVector X(void) const;
	Y3DVector Y(void) const;

	inline void Zero(void);
	inline void Identity(void);
	inline Y3DVector Mpy(const Y3DVector& v) const;

	inline Y3DVector operator*(const Y3DVector&) const;
	inline YHmgTrsf operator*(const YHmgTrsf&);

	// rccl compatibility!
	inline double& nx(void) { return (*this)(1,1); }
	inline double& ny(void) { return (*this)(2,1); }
	inline double& nz(void) { return (*this)(3,1); }
	inline double& ox(void) { return (*this)(1,2); }
	inline double& oy(void) { return (*this)(2,2); }
	inline double& oz(void) { return (*this)(3,2); }
	inline double& ax(void) { return (*this)(1,3); }
	inline double& ay(void) { return (*this)(2,3); }
	inline double& az(void) { return (*this)(3,3); }
	inline double& px(void) { return (*this)(1,4); }
	inline double& py(void) { return (*this)(2,4); }
	inline double& pz(void) { return (*this)(3,4); }
};
								  
//
// rotation matrix
//
class VisMatrixExport YRotMatrix : public YMatrix {
public:
	YRotMatrix(void);
	YRotMatrix(const YRotMatrix& m) : YMatrix(m) {}
	YRotMatrix(const YMatrix& m) : YMatrix(m) {}

	virtual ~YRotMatrix() {}

	void operator=(const YMatrix& m) { YMatrix::operator=(m); }
	void operator=(double f) { YMatrix::operator=(f); }
    void operator=(const YRotMatrix& m) { operator=((const YMatrix&)m); }
	
	Y3DVector Z(void) const;
	Y3DVector X(void) const;
	Y3DVector Y(void) const;

	inline void Zero(void);
	inline void Identity(void);
};

//
// 3d vector (position, etc)
//
class VisMatrixExport Y3DVector : public YVector {
public:
	Y3DVector(void);
	virtual ~Y3DVector() {}

	Y3DVector(const Y3DVector& v) : YVector(v) {}
	Y3DVector(const YVector& v) : YVector(v) {}

	void operator=(const YVector& m) { YVector::operator=(m); }
    void operator=(double f) { YVector::operator=(f); }
    void operator=(const Y3DVector& m) { operator=((const YVector&)m); }

	inline YRotMatrix CrossMatrix(void) const;
  	Y3DVector Cross(const Y3DVector& v) const;
	inline double Scalar(const Y3DVector& v) const; // si puo' fare con op *
};

//
// 6d velocity vector
//
class VisMatrixExport YDiff : public YVector {
public:
	YDiff(void); 
	YDiff(const YDiff& d) : YVector(d) {}
	YDiff(const YVector& d) : YVector(d) {}
	virtual ~YDiff() {}

	void operator=(const YVector& m) { YVector::operator=(m); }
    void operator=(double f) { YVector::operator=(f); }
    void operator=(const YDiff& m) { operator=((const YVector&)m); }

// se t va da B->C allora l'uscita e' YDiff in C
	inline YDiff& TransYDiff(const YHmgTrsf& t);
};

//
// 6d force/torque vector
//
class VisMatrixExport YForce : public YVector {
public: 
	YForce(void);
	YForce(const YForce& f) : YVector(f) {}
	YForce(const YVector& f) : YVector(f) {}
	virtual ~YForce() {}

	void operator=(const YVector& m) { YVector::operator=(m); }
    void operator=(double f) { YVector::operator=(f); }
    void operator=(const YForce& m) { operator=((const YVector&)m); }

	inline YForce& TransForce(const YHmgTrsf& t);
};

//
// coordinate system transform for 6d vectors.
// 
YForce TransForce(const YForce&,const YHmgTrsf&);
YDiff  TransYDiff(const YDiff&,const YHmgTrsf&);

#include "YARPRobotMath.inl"

#endif

