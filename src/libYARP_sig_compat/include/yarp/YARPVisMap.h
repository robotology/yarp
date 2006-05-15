// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-
#ifndef YARPVISMAP_INC
#define YARPVISMAP_INC

#include <yarp/YARPImage.h>

// sorry, will have to fix this for windows
#include <assert.h>


// should fix padding - easy!

// will assume regions of same width selected within the images
template <class Operator, class TPixel1>
void YARPVisMap(Operator& op, 
                YARPImageOf<TPixel1>& img1)
{
    assert(img1.GetPadding()==0);
    TPixel1 *it1 = img1.GetTypedBuffer();
    int h = img1.GetHeight();
    int w = img1.GetWidth();
    for (int i=0; i<h*w; i++)
        {
            op(*it1);
            it1++;
        }
}


template <class Operator, class TPixel1, class TPixel2>
void YARPVisMap(Operator& op, 
                YARPImageOf<TPixel1>& img1,
                YARPImageOf<TPixel2>& img2)
{
    assert(img1.GetPadding()==0);
    assert(img2.GetPadding()==0);
    TPixel1 *it1 = img1.GetTypedBuffer();
    TPixel2 *it2 = img2.GetTypedBuffer();
    int h = img1.GetHeight();
    int w = img1.GetWidth();
    assert(h==img2.GetHeight() && w==img2.GetWidth());
    for (int i=0; i<h*w; i++)
        {
            op(*it1,*it2);
            it1++;
            it2++;
        }
}


template <class Operator, class TPixel1, class TPixel2, class TPixel3>
void YARPVisMap(Operator& op, 
                YARPImageOf<TPixel1>& img1,
                YARPImageOf<TPixel2>& img2,
                YARPImageOf<TPixel3>& img3)
{
    assert(img1.GetPadding()==0);
    assert(img2.GetPadding()==0);
    assert(img3.GetPadding()==0);
    TPixel1 *it1 = img1.GetTypedBuffer();
    TPixel2 *it2 = img2.GetTypedBuffer();
    TPixel3 *it3 = img3.GetTypedBuffer();
    int h = img1.GetHeight();
    int w = img1.GetWidth();
    assert(h==img2.GetHeight() && w==img2.GetWidth());
    assert(h==img3.GetHeight() && w==img3.GetWidth());
    for (int i=0; i<h*w; i++)
        {
            op(*it1,*it2,*it3);
            it1++;
            it2++;
            it3++;
        }
}

template <class Operator, class TPixel1, class TPixel2, class TPixel3, class TPixel4>
void YARPVisMap(Operator& op, 
                YARPImageOf<TPixel1>& img1,
                YARPImageOf<TPixel2>& img2,
                YARPImageOf<TPixel3>& img3,
                YARPImageOf<TPixel4>& img4)
{
    assert(img1.GetPadding()==0);
    assert(img2.GetPadding()==0);
    assert(img3.GetPadding()==0);
    assert(img4.GetPadding()==0);
    TPixel1 *it1 = img1.GetTypedBuffer();
    TPixel2 *it2 = img2.GetTypedBuffer();
    TPixel3 *it3 = img3.GetTypedBuffer();
    TPixel4 *it4 = img4.GetTypedBuffer();
    int h = img1.GetHeight();
    int w = img1.GetWidth();
    assert(h==img2.GetHeight() && w==img2.GetWidth());
    assert(h==img3.GetHeight() && w==img3.GetWidth());
    assert(h==img4.GetHeight() && w==img4.GetWidth());
    for (int i=0; i<h*w; i++)
        {
            op(*it1,*it2,*it3,*it4);
            it1++;
            it2++;
            it3++;
            it4++;
        }
}


template <class Operator, class TPixel1, class TPixel2, class TPixel3, class TPixel4, class TPixel5>
void YARPVisMap(Operator& op, 
                YARPImageOf<TPixel1>& img1,
                YARPImageOf<TPixel2>& img2,
                YARPImageOf<TPixel3>& img3,
                YARPImageOf<TPixel4>& img4,
                YARPImageOf<TPixel5>& img5)
{
    assert(img1.GetPadding()==0);
    assert(img2.GetPadding()==0);
    assert(img3.GetPadding()==0);
    assert(img4.GetPadding()==0);
    assert(img5.GetPadding()==0);
    TPixel1 *it1 = img1.GetTypedBuffer();
    TPixel2 *it2 = img2.GetTypedBuffer();
    TPixel3 *it3 = img3.GetTypedBuffer();
    TPixel4 *it4 = img4.GetTypedBuffer();
    TPixel4 *it5 = img5.GetTypedBuffer();
    int h = img1.GetHeight();
    int w = img1.GetWidth();
    assert(h==img2.GetHeight() && w==img2.GetWidth());
    assert(h==img3.GetHeight() && w==img3.GetWidth());
    assert(h==img4.GetHeight() && w==img4.GetWidth());
    assert(h==img5.GetHeight() && w==img5.GetWidth());
    for (int i=0; i<h*w; i++)
        {
            op(*it1,*it2,*it3,*it4,*it5);
            it1++;
            it2++;
            it3++;
            it4++;
            it5++;
        }
}



template <class Operator, class TPixel1, class TPixel2, class TPixel3, class TPixel4, class TPixel5, class TPixel6>
void YARPVisMap(Operator& op, 
                YARPImageOf<TPixel1>& img1,
                YARPImageOf<TPixel2>& img2,
                YARPImageOf<TPixel3>& img3,
                YARPImageOf<TPixel4>& img4,
                YARPImageOf<TPixel5>& img5,
                YARPImageOf<TPixel6>& img6)
{
    assert(img1.GetPadding()==0);
    assert(img2.GetPadding()==0);
    assert(img3.GetPadding()==0);
    assert(img4.GetPadding()==0);
    assert(img5.GetPadding()==0);
    assert(img6.GetPadding()==0);
    TPixel1 *it1 = img1.GetTypedBuffer();
    TPixel2 *it2 = img2.GetTypedBuffer();
    TPixel3 *it3 = img3.GetTypedBuffer();
    TPixel4 *it4 = img4.GetTypedBuffer();
    TPixel4 *it5 = img5.GetTypedBuffer();
    TPixel4 *it6 = img6.GetTypedBuffer();
    int h = img1.GetHeight();
    int w = img1.GetWidth();
    assert(h==img2.GetHeight() && w==img2.GetWidth());
    assert(h==img3.GetHeight() && w==img3.GetWidth());
    assert(h==img4.GetHeight() && w==img4.GetWidth());
    assert(h==img5.GetHeight() && w==img5.GetWidth());
    assert(h==img6.GetHeight() && w==img6.GetWidth());
    for (int i=0; i<h*w; i++)
        {
            op(*it1,*it2,*it3,*it4,*it5,*it6);
            it1++;
            it2++;
            it3++;
            it4++;
            it5++;
            it6++;
        }
}


template <class Operator, class TPixel1, class TPixel2, class TPixel3, class TPixel4, class TPixel5, class TPixel6, class TPixel7>
void YARPVisMap(Operator& op, 
                YARPImageOf<TPixel1>& img1,
                YARPImageOf<TPixel2>& img2,
                YARPImageOf<TPixel3>& img3,
                YARPImageOf<TPixel4>& img4,
                YARPImageOf<TPixel5>& img5,
                YARPImageOf<TPixel6>& img6,
                YARPImageOf<TPixel7>& img7)
{
    assert(img1.GetPadding()==0);
    assert(img2.GetPadding()==0);
    assert(img3.GetPadding()==0);
    assert(img4.GetPadding()==0);
    assert(img5.GetPadding()==0);
    assert(img6.GetPadding()==0);
    assert(img7.GetPadding()==0);
    TPixel1 *it1 = img1.GetTypedBuffer();
    TPixel2 *it2 = img2.GetTypedBuffer();
    TPixel3 *it3 = img3.GetTypedBuffer();
    TPixel4 *it4 = img4.GetTypedBuffer();
    TPixel4 *it5 = img5.GetTypedBuffer();
    TPixel4 *it6 = img6.GetTypedBuffer();
    TPixel4 *it7 = img7.GetTypedBuffer();
    int h = img1.GetHeight();
    int w = img1.GetWidth();
    assert(h==img2.GetHeight() && w==img2.GetWidth());
    assert(h==img3.GetHeight() && w==img3.GetWidth());
    assert(h==img4.GetHeight() && w==img4.GetWidth());
    assert(h==img5.GetHeight() && w==img5.GetWidth());
    assert(h==img6.GetHeight() && w==img6.GetWidth());
    assert(h==img7.GetHeight() && w==img7.GetWidth());
    for (int i=0; i<h*w; i++)
        {
            op(*it1,*it2,*it3,*it4,*it5,*it6,*it7);
            it1++;
            it2++;
            it3++;
            it4++;
            it5++;
            it6++;
            it7++;
        }
}



template <class Operator, class TPixel1, class TPixel2, class TPixel3, class TPixel4, class TPixel5, class TPixel6, class TPixel7, class TPixel8>
void YARPVisMap(Operator& op, 
                YARPImageOf<TPixel1>& img1,
                YARPImageOf<TPixel2>& img2,
                YARPImageOf<TPixel3>& img3,
                YARPImageOf<TPixel4>& img4,
                YARPImageOf<TPixel5>& img5,
                YARPImageOf<TPixel6>& img6,
                YARPImageOf<TPixel7>& img7,
                YARPImageOf<TPixel8>& img8)
{
    assert(img1.GetPadding()==0);
    assert(img2.GetPadding()==0);
    assert(img3.GetPadding()==0);
    assert(img4.GetPadding()==0);
    assert(img5.GetPadding()==0);
    assert(img6.GetPadding()==0);
    assert(img7.GetPadding()==0);
    assert(img8.GetPadding()==0);
    TPixel1 *it1 = img1.GetTypedBuffer();
    TPixel2 *it2 = img2.GetTypedBuffer();
    TPixel3 *it3 = img3.GetTypedBuffer();
    TPixel4 *it4 = img4.GetTypedBuffer();
    TPixel4 *it5 = img5.GetTypedBuffer();
    TPixel4 *it6 = img6.GetTypedBuffer();
    TPixel4 *it7 = img7.GetTypedBuffer();
    TPixel4 *it8 = img8.GetTypedBuffer();
    int h = img1.GetHeight();
    int w = img1.GetWidth();
    assert(h==img2.GetHeight() && w==img2.GetWidth());
    assert(h==img3.GetHeight() && w==img3.GetWidth());
    assert(h==img4.GetHeight() && w==img4.GetWidth());
    assert(h==img5.GetHeight() && w==img5.GetWidth());
    assert(h==img6.GetHeight() && w==img6.GetWidth());
    assert(h==img7.GetHeight() && w==img7.GetWidth());
    assert(h==img8.GetHeight() && w==img8.GetWidth());
    for (int i=0; i<h*w; i++)
        {
            op(*it1,*it2,*it3,*it4,*it5,*it6,*it7,*it8);
            it1++;
            it2++;
            it3++;
            it4++;
            it5++;
            it6++;
            it7++;
            it8++;
        }
}

#endif
