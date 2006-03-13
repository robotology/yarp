
#include <ace/OS_NS_stdlib.h>
#include <ace/OS_NS_stdio.h>

#include <yarp/sig/Image.h>

using namespace yarp::sig;


/*

  This structure just so happens to match with IplImage, the
  image structure used by IPL and OpenCV.
  It is safe to cast between this and an IplImage.

 */

typedef struct _YARPInternalImage {
    int  nSize;         /* sizeof(IplImage) */
    int  ID;            /* version (=0)*/
    int  nChannels;     /* Most of OpenCV functions support 1,2,3 or 4 channels */
    int  alphaChannel;  /* ignored by OpenCV */
    int  depth;         /* pixel depth in bits: IPL_DEPTH_8U, IPL_DEPTH_8S, IPL_DEPTH_16S,
                           IPL_DEPTH_32S, IPL_DEPTH_32F and IPL_DEPTH_64F are supported */
    char colorModel[4]; /* ignored by OpenCV */
    char channelSeq[4]; /* ditto */
    int  dataOrder;     /* 0 - interleaved color channels, 1 - separate color channels.
                           cvCreateImage can only create interleaved images */
    int  origin;        /* 0 - top-left origin,
                           1 - bottom-left origin (Windows bitmaps style) */
    int  align;         /* Alignment of image rows (4 or 8).
                           OpenCV ignores it and uses widthStep instead */
    int  width;         /* image width in pixels */
    int  height;        /* image height in pixels */
    void /*struct _IplROI*/ *roi;/* image ROI. if NULL, the whole image is selected */
    void /*struct _IplImage*/ *maskROI; /* must be NULL */
    void  *imageId;     /* ditto */
    void /*struct _IplTileInfo*/ *tileInfo; /* ditto */
    int  imageSize;     /* image data size in bytes
                           (==image->height*image->widthStep
                           in case of interleaved data)*/
    char *imageData;  /* pointer to aligned image data */
    int  widthStep;   /* size of aligned image row in bytes */
    int  BorderMode[4]; /* ignored by OpenCV */
    int  BorderConst[4]; /* ditto */
    char *imageDataOrigin; /* pointer to very origin of image data
                              (not necessarily aligned) -
                              needed for correct deallocation */
}
YARPInternalImage;



class ImageStorage {
public:
};


Image::Image() {
  implementation = NULL;
  data = NULL;
  imgWidth = imgHeight = 0;
  imgPixelSize = imgRowSize = 0;
  fprintf(stderr,"YARP2 version of Image class is not yet implemented\n");
  exit(1);
}


Image::~Image() {
}

