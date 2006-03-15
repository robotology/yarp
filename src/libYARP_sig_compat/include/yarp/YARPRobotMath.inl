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
/// $Id: YARPRobotMath.inl,v 1.1 2006-03-15 09:31:28 eshuy Exp $
///
///

//
// classe YHmgTrsf.
//

//
// Questo e' altamente inefficiente.
//
inline Y3DVector YHmgTrsf::Mpy(const Y3DVector& v) const 
{
	Y3DVector ret;
	ret=Rotation()*v+Position();
	return ret;
}

inline void YHmgTrsf::Zero(void) { (*this)=0.0; }

inline void YHmgTrsf::Identity(void)
{ 
	(*this)=0.0;
	(*this)(1,1)=1.0;
	(*this)(2,2)=1.0;
	(*this)(3,3)=1.0;
	(*this)(4,4)=1.0;
}

//
// Anche questa operazione e' una porcheria.
//	
inline Y3DVector YHmgTrsf::operator*(const Y3DVector& v) const 
{ 
	return Mpy(v); 
}

inline YHmgTrsf YHmgTrsf::operator*(const YHmgTrsf& h) 
{ 
	return YMatrix::operator*(h); 
}
	
//
// classe YRotMatrix.
//
inline void YRotMatrix::Zero(void) 
{ 
	(*this)=0.0; 
}

inline void YRotMatrix::Identity(void) 
{
	(*this)=0.0;
	(*this)(1,1)=1.0;
	(*this)(2,2)=1.0;
	(*this)(3,3)=1.0;
}

//
// classe Y3DVector.
//
inline double Y3DVector::Scalar(const Y3DVector& v) const
{
	double ret=0;
	ret+=(*this)(1)*v(1);
	ret+=(*this)(2)*v(2);
	ret+=(*this)(3)*v(3);
	return ret;
}

//
// classe YDiff.
//
inline YDiff& YDiff::TransYDiff(const YHmgTrsf& t)  // trasformazione di coordinate
{
	do {} while (&t == 0);
	return *this;
	// to be implemented!
#if 0
	Matrix jt(6,6);
	jt=0.0;
	
	jt.SubMatrix(1,3,1,3)=t.Rotation();
	jt.SubMatrix(4,6,4,6)=t.Rotation();
	jt.SubMatrix(1,3,4,6)=-t.Rotation()*(t.Position().CrossMatrix());
	(*this)=jt*(*this);
	return *this;
#endif
}

//
// classe YForce.
//
inline YForce& YForce::TransForce(const YHmgTrsf& t)
{
	do {} while (&t == 0);
	return *this;
	// to be implemented!
#if 0
	Matrix jt(6,6);
	jt=0.0;
	
	jt.SubMatrix(1,3,1,3)=t.Rotation();
	jt.SubMatrix(4,6,4,6)=t.Rotation();
	jt.SubMatrix(1,3,4,6)=-t.Rotation()*(t.Position().CrossMatrix());
	(*this)=jt.t()*(*this);
	return *this;
#endif
}
