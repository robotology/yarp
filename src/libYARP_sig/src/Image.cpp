
#include <ace/OS_NS_stdlib.h>
#include <ace/OS_NS_stdio.h>

#include <yarp/sig/Image.h>
#include <yarp/sig/IplImage.h>

#include <assert.h>

using namespace yarp::sig;

#define DBGPF1 if (0)

#define ACE_ASSERT assert

#include <iostream>
using namespace std;

using namespace yarp::sig;

inline int PAD_BYTES (int len, int pad)
{
	const int rem = len % pad;
	return (rem != 0) ? (pad - rem) : rem;
}

static int _GetPixelSize(int pixel_type)
{
  int result = 0;
  switch (pixel_type)
    {
    case YARP_PIXEL_MONO:
      result = sizeof(PixelMono);
      break;
    case YARP_PIXEL_RGB:
      result = sizeof(PixelRgb);
      break;
    case YARP_PIXEL_HSV:
      result = sizeof(PixelHsv);
      break;
    case YARP_PIXEL_BGR:
      result = sizeof(PixelBgr);
      break;
    case YARP_PIXEL_MONO_SIGNED:
      result = sizeof(PixelMonoSigned);
      break;
    case YARP_PIXEL_RGB_SIGNED:
      result = sizeof(PixelRgbSigned);
      break;
    case YARP_PIXEL_MONO_FLOAT:
      result = sizeof(PixelFloat);
      break;
    case YARP_PIXEL_RGB_FLOAT:
      result = sizeof(PixelRgbFloat);
      break;
    case YARP_PIXEL_HSV_FLOAT:
      result = sizeof(PixelHsvFloat);
      break;
    default:
      // only other acceptable possibility is that the size is being supplied
      // for an unknown type
      //ACE_ASSERT (pixel_type<0);
      result = -pixel_type;
      break;
    }
  //printf("Getting pixel size for %d (%d)\n", pixel_type, result);
  return result;
}


class ImageStorage {
public:
  IplImage* pImage;
  char **Data;       // this is not IPL. it's char to maintain IPL compatibility  
protected:
  Image& owner;

  int type_id;
  
  int is_owner;
  
  // ipl allocation is done in two steps.
  // _alloc allocates the actual ipl pointer.
  // _alloc_data allocates the image array and data.
  // memory is allocated in a single chunk. Row ptrs are then
  // made to point appropriately. This is compatible with IPL and
  // SOMEONE says it's more efficient on NT.
  void _alloc (void);
  void _alloc_extern (void *buf);
  void _alloc_data (void);
  void _free (void);
  void _free_data (void);
  
  void _make_independent(); 
  void _set_ipl_header(int x, int y, int pixel_type);
  void _free_ipl_header();
  void _alloc_complete(int x, int y, int pixel_type);
  void _free_complete();
  
  void _alloc_complete_extern(void *buf, int x, int y, int pixel_type);
  
  // computes the # of padding bytes. These are always at the end of the row.
  int _pad_bytes (int linesize, int align) const;
  
  inline int GetPadding() const { 
    return _pad_bytes (pImage->width * pImage->nChannels, 
		       YARP_IMAGE_ALIGN); 
  }

public:
  ImageStorage(Image& owner) : owner(owner) {
    type_id = 0;
    pImage = NULL;
    Data = NULL;
    is_owner = 0;
  }

  ~ImageStorage() {
    _free_complete();
  }

  void resize(int x, int y, int pixel_type, 
	      int pixel_size, int row_size);

};


void ImageStorage::resize(int x, int y, int pixel_type, 
			  int pixel_size, int row_size) {
  int need_recreation = 1;
  /*
  if (pImage != NULL) {
    if (x == pImage->width && y == pImage->height) {
      if (pImage->imageSize == row_size * y * pixel_size &&
	  pImage->imageData != NULL && Data != NULL)
	{
	  need_recreation = 0;
	}
    }
  }
  */

  if (need_recreation) {
    _free_complete();
    DBGPF1 ACE_OS::printf("HIT recreation for %ld %ld: %d %d %d\n", (long int) this, (long int) pImage, x, y, pixel_type);
    _alloc_complete (x, y, pixel_type);
  }
}




// allocates an empty image.
void ImageStorage::_alloc (void) {
  ACE_ASSERT (pImage != NULL);

  if (pImage != NULL)
    if (pImage->imageData != NULL)
      _free(); // was iplDeallocateImage(pImage); but that won't work with refs
	
  if ( (type_id == YARP_PIXEL_MONO_FLOAT) || (type_id == YARP_PIXEL_RGB_FLOAT) ||
       (type_id == YARP_PIXEL_HSV_FLOAT) )
    iplAllocateImageFP(pImage, 0, 0);
  else
    iplAllocateImage (pImage, 0, 0);

  iplSetBorderMode (pImage, IPL_BORDER_CONSTANT, IPL_SIDE_ALL, 0);
}

// installs an external buffer as the image data
void ImageStorage::_alloc_extern (void *buf)
{
  ACE_ASSERT (pImage != NULL);
  ACE_ASSERT(Data==NULL);

  if (pImage != NULL)
    if (pImage->imageData != NULL)
      iplDeallocateImage (pImage);

  //iplAllocateImage (pImage, 0, 0);
  pImage->imageData = (char*)buf;
  // HIT probably need to do more for real IPL
  
  //iplSetBorderMode (pImage, IPL_BORDER_CONSTANT, IPL_SIDE_ALL, 0);
}

// allocates the Data pointer.
void ImageStorage::_alloc_data (void)
{
  DBGPF1 ACE_OS::printf("alloc_data1\n"), fflush(stdout);
  ACE_ASSERT (pImage != NULL);

  ACE_ASSERT(Data==NULL);
  /*
    if (Data != NULL)
    {
    DBGPF1 printf("HIT Deleting Data\n"), fflush(stdout);
    //delete[] Data; //HIT2
    free(Data);
    Data = NULL;
    }
  */

  //int hh = pImage->height * sizeof(char *);

  char **ptr = new char *[pImage->height];

  Data = ptr;

  ACE_ASSERT (Data != NULL);

  ACE_ASSERT (pImage->imageData != NULL);

  //int nPlanes = pImage->nChannels;
  //int width = pImage->width;
  int height = pImage->height;

  char * DataArea = pImage->imageData;
    
  for (int r = 0; r < height; r++)
    {
      Data[r] = DataArea;
      DataArea += pImage->widthStep;
    }
  DBGPF1 ACE_OS::printf("alloc_data4\n");
}

void ImageStorage::_free (void)
{
  //	ACE_ASSERT (pImage != NULL);
  if (pImage != NULL)
    if (pImage->imageData != NULL)
      {
	//cout << "HIT maybe deleting ipl image" << endl;
	if (is_owner)
	  {
	    DBGPF1 cout << "HIT really truly deleting ipl image" << endl;
	    iplDeallocateImage (pImage);
	    if (Data!=NULL)
	      {
		delete[] Data;
	      }
	  }
	else
	  {
	    if (Data!=NULL)
	      {
		delete[] Data;
	      }
	  }

	is_owner = 1;
	Data = NULL;
	pImage->imageData = NULL;
      }
}

void ImageStorage::_free_data (void)
{
  ACE_ASSERT(Data==NULL); // Now always free Data at same time
  // as image buffer, for correct refcounting
  /*
    if (Data != NULL)
    {
    delete[] Data;
    }
    Data = NULL;
  */
}


void ImageStorage::_free_complete()
{
	_free();
	_free_data();
	_free_ipl_header();
}


void ImageStorage::_free_ipl_header()
{
	if (pImage!=NULL)
	{
		iplDeallocate (pImage, IPL_IMAGE_HEADER);
	}
	pImage = NULL;
}


void ImageStorage::_alloc_complete(int x, int y, int pixel_type)
{
	_make_independent();
	_free_complete();
	_set_ipl_header(x, y, pixel_type);
	_alloc ();
	_alloc_data ();
}



void ImageStorage::_make_independent()
{
	// needs to be filled out once references are permitted -paulfitz
	// actually I think this isn't really needed -paulfitz
}


void ImageStorage::_set_ipl_header(int x, int y, int pixel_type)
{
  int implemented_yet = 1;
	// used to allocate the ipl header.
	switch (pixel_type)
	{
	case YARP_PIXEL_MONO:
		pImage = iplCreateImageHeader(
					1,
					0,
					IPL_DEPTH_8U,			
					"GRAY",
					"GRAY",
					IPL_DATA_ORDER_PIXEL,	 
					IPL_ORIGIN_BL,			
					IPL_ALIGN_QWORD,		
					x,
					y,
					NULL,
					NULL,
					NULL,
					NULL);
		DBGPF1 ACE_OS::printf("Set pImage to %ld\n", (long int) pImage);
		DBGPF1 ACE_OS::printf("Set init h to %d\n", (long int) pImage->height);
		break;

	case YARP_PIXEL_RGB:
		pImage = iplCreateImageHeader(
					3,
					0,
					IPL_DEPTH_8U,			
					"RGB",
					"RGB",
					IPL_DATA_ORDER_PIXEL,	 
					IPL_ORIGIN_BL,			
					IPL_ALIGN_QWORD,		
					x,
					y,
					NULL,
					NULL,
					NULL,
					NULL);
		break;

	case YARP_PIXEL_HSV:
		pImage = iplCreateImageHeader(
					3,
					0,
					IPL_DEPTH_8U,			
					"HSV",
					"HSV",
					IPL_DATA_ORDER_PIXEL,	 
					IPL_ORIGIN_BL,			
					IPL_ALIGN_QWORD,		
					x,
					y,
					NULL,
					NULL,
					NULL,
					NULL);
		break;

	case YARP_PIXEL_BGR:
		pImage = iplCreateImageHeader(
					3,
					0,
					IPL_DEPTH_8U,			
					"RGB",
					"BGR",
					IPL_DATA_ORDER_PIXEL,	 
					IPL_ORIGIN_BL,			
					IPL_ALIGN_QWORD,		
					x,
					y,
					NULL,
					NULL,
					NULL,
					NULL);
		break;

	case YARP_PIXEL_MONO_SIGNED:
		pImage = iplCreateImageHeader(
					1,
					0,
					IPL_DEPTH_8S,			
					"GRAY",
					"GRAY",
					IPL_DATA_ORDER_PIXEL,	 
					IPL_ORIGIN_BL,			
					IPL_ALIGN_QWORD,		
					x,
					y,
					NULL,
					NULL,
					NULL,
					NULL);
		break;

	case YARP_PIXEL_RGB_SIGNED:
		ACE_ASSERT (implemented_yet == 0);
		break;

	case YARP_PIXEL_MONO_FLOAT:
             	  pImage = iplCreateImageHeader(
					  1,
					  0,
					  IPL_DEPTH_32F,
					  "GRAY",
					  "GRAY",
					  IPL_DATA_ORDER_PIXEL,
					  IPL_ORIGIN_BL,
					  IPL_ALIGN_QWORD,
					  x,
					  y,
					  NULL,
					  NULL,
					  NULL,
					  NULL);
		break;

	case YARP_PIXEL_RGB_FLOAT:
		pImage = iplCreateImageHeader(
					3,
					0,
					IPL_DEPTH_32F,			
					"RGB",
					"RGB",
					IPL_DATA_ORDER_PIXEL,	 
					IPL_ORIGIN_BL,			
					IPL_ALIGN_QWORD,		
					x,
					y,
					NULL,
					NULL,
					NULL,
					NULL);
		//ACE_ASSERT (implemented_yet == 0);
		break;

	case YARP_PIXEL_HSV_FLOAT:
		ACE_ASSERT (implemented_yet == 0);
		break;

	case YARP_PIXEL_INT:
             	  pImage = iplCreateImageHeader(
					  1,
					  0,
					  IPL_DEPTH_32S,
					  "GRAY",
					  "GRAY",
					  IPL_DATA_ORDER_PIXEL,
					  IPL_ORIGIN_BL,
					  IPL_ALIGN_QWORD,
					  x,
					  y,
					  NULL,
					  NULL,
					  NULL,
					  NULL);
		break;

	case YARP_PIXEL_INVALID:
		// not a type!
		ACE_OS::printf ("*** Trying to allocate an invalid pixel type image\n");
		ACE_OS::exit(1);
		break;
	  
	case -2:
	  pImage = iplCreateImageHeader(
					1,
					0,
					IPL_DEPTH_16S,
					"GRAY",
					"GRAY",
					IPL_DATA_ORDER_PIXEL,
					IPL_ORIGIN_BL,
					IPL_ALIGN_QWORD,
					x,
					y,
					NULL,
					NULL,
					NULL,
					NULL);
	  break;

	case -4:
	  pImage = iplCreateImageHeader(
					1,
					0,
					IPL_DEPTH_32S,
					"GRAY",
					"GRAY",
					IPL_DATA_ORDER_PIXEL,
					IPL_ORIGIN_BL,
					IPL_ALIGN_QWORD,
					x,
					y,
					NULL,
					NULL,
					NULL,
					NULL);
	  break;

	default:
		// unknown pixel type. Should revert to a non-IPL mode... how?
		// LATER: implement this.
		ACE_ASSERT (implemented_yet == 0);
		break;
	}

	type_id = pixel_type;
}



// LATER: implement for LINUX.
int ImageStorage::_pad_bytes (int linesize, int align) const
{
  return PAD_BYTES (linesize, align);
}








Image::Image() {
  implementation = NULL;
  data = NULL;
  imgWidth = imgHeight = 0;
  imgPixelSize = imgRowSize = 0;
  imgPixelCode = 0;
  fprintf(stderr,"YARP2 version of Image class is not yet implemented\n");
  implementation = new ImageStorage(*this);
  ACE_ASSERT(implementation!=NULL);
}


Image::~Image() {
  if (implementation!=NULL) {
    delete (ImageStorage*)implementation;
    implementation = NULL;
  }
}


int Image::pixelSize() const {
  return imgPixelSize;
}


int Image::pixelCode() const {
  return imgPixelCode;
}


void Image::zero() {
}


void Image::resize(int imgWidth, int imgHeight) {
  ((ImageStorage*)implementation)->resize(imgWidth,imgHeight,
					  imgPixelCode,
					  imgPixelSize,
					  imgRowSize);
  synchronize();
}


void Image::setPixelCode(int imgPixelCode) {
  this->imgPixelCode = imgPixelCode;
}


void Image::setPixelSize(int imgPixelSize) {
  this->imgPixelSize = imgPixelSize;
}


void Image::setRowSize(int imgRowSize) {
  this->imgRowSize = imgRowSize;
}


void Image::synchronize() {
  ImageStorage *impl = (ImageStorage*)implementation;
  ACE_ASSERT(impl!=NULL);
  if (impl->pImage!=NULL) {
    imgWidth = impl->pImage->width;
    imgHeight = impl->pImage->height;
    data = impl->Data;
  } else {
    data = NULL;
    imgWidth = imgHeight = 0;
  }
}


void *Image::getRawImage() {
  fprintf(stderr,"YARP2 version of Image class is not yet implemented\n");
  return NULL;
}

void *Image::getIplImage() {
  fprintf(stderr,"YARP2 version of Image class is not yet implemented\n");
  return NULL;
}

void Image::wrapRawImage(void *buf, int imgWidth, int imgHeight) {
  fprintf(stderr,"YARP2 version of Image class is not yet implemented\n");
}


