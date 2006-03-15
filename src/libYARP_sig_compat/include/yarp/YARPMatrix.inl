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
/// $Id: YARPMatrix.inl,v 1.1 2006-03-15 09:31:28 eshuy Exp $
///
///

// VisDMatrix.inl
//
// Copyright © 1996-1998 Microsoft Corporation, All Rights Reserved


inline bool YVector::operator!=(const YVector& refvector) const
{
    return !(*this == refvector);
}

inline bool YMatrix::operator!=(const YMatrix& refmatrix) const
{
    return !(*this == refmatrix);
}

inline bool YVector::operator!=(double dbl) const
{
    return !(*this == dbl);
}

inline bool YMatrix::operator!=(double dbl) const
{
    return !(*this == dbl);
}

inline YMatrix& YMatrix::Invert(void)
{
	*this = Inverted();

	return *this;
}

inline YMatrix& YMatrix::Transpose(void)
{
	*this = Transposed();

	return *this;
}


//
// Self-describing stream output.
//
// LATER:  We should consider having both op<< and op>> with these streams.
//
#ifdef VIS_INCLUDE_SDSTREAM
inline CVisSDStream& operator<<(CVisSDStream& s, YVector& o)
	{ return VisSDReadWriteClass(s, o); }

inline CVisSDObject<YVector>& CVisSDObject<YVector>::Exemplar(void)
{
	// Global variable used with self-describing streams.
	// LATER:  Find a way to avoid using this global variable.
	extern VisMatrixExport CVisSDObject<YVector> g_visdvectorExemplar;

	return g_visdvectorExemplar;
}


inline CVisSDStream& operator<<(CVisSDStream& s, YMatrix& o)
	{ return VisSDReadWriteClass(s, o); }

inline CVisSDObject<YMatrix>& CVisSDObject<YMatrix>::Exemplar(void)
{
	// Global variable used with self-describing streams.
	// LATER:  Find a way to avoid using this global variable.
	extern VisMatrixExport CVisSDObject<YMatrix> g_visdmatrixExemplar;

	return g_visdmatrixExemplar;
}
#endif

