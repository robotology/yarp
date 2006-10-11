// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
  This file is in a pretty hacky state.  Sorry! -paulfitz

*/

#include <ace/OS_NS_stdlib.h>
#include <ace/OS_NS_stdio.h>

#include <yarp/Logger.h>
#include <yarp/IOException.h>
#include <yarp/sig/Image.h>
#include <yarp/sig/IplImage.h>

#include <yarp/os/Bottle.h>
#include <yarp/os/Vocab.h>
#include <yarp/os/ConstString.h>
#include <yarp/os/Time.h>

#include <assert.h>


using namespace yarp;
using namespace yarp::sig;
using namespace yarp::os;

#define DBGPF1 if (0)


#include <iostream>
using namespace std;

using namespace yarp::sig;

inline int PAD_BYTES (int len, int pad)
{
	const int rem = len % pad;
	return (rem != 0) ? (pad - rem) : rem;
}



class ImageStorage {
public:
    IplImage* pImage;
    char **Data;       // this is not IPL. it's char to maintain IPL compatibility  
    int quantum;

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
    void _set_ipl_header(int x, int y, int pixel_type, int quantum = 0);
    void _free_ipl_header();
    void _alloc_complete(int x, int y, int pixel_type, int quantum = 0);
    void _free_complete();
  
  
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
        is_owner = 1;
        quantum = 0;
    }

    ~ImageStorage() {
        _free_complete();
    }

    void resize(int x, int y, int pixel_type, 
                int pixel_size, int quantum);

    void _alloc_complete_extern(void *buf, int x, int y, int pixel_type,
                                int quantum);

};


void ImageStorage::resize(int x, int y, int pixel_type, 
                          int pixel_size, int quantum) {
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

    //YARP_ASSERT(quantum==0 || quantum==YARP_IMAGE_ALIGN);
    //this->quantum = quantum = YARP_IMAGE_ALIGN;
    if (quantum==0) {
        quantum = YARP_IMAGE_ALIGN;
    }

    if (need_recreation) {
        _free_complete();
        DBGPF1 ACE_OS::printf("HIT recreation for %ld %ld: %d %d %d\n", (long int) this, (long int) pImage, x, y, pixel_type);
        _alloc_complete (x, y, pixel_type, quantum);
    }
}




// allocates an empty image.
void ImageStorage::_alloc (void) {
    ACE_ASSERT (pImage != NULL);

    if (pImage != NULL)
        if (pImage->imageData != NULL)
            _free(); // was iplDeallocateImage(pImage); but that won't work with refs
	
    if ( (type_id == VOCAB_PIXEL_MONO_FLOAT) || (type_id == VOCAB_PIXEL_RGB_FLOAT) ||
         (type_id == VOCAB_PIXEL_HSV_FLOAT) )
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


void ImageStorage::_alloc_complete(int x, int y, int pixel_type, int quantum)
{
	_make_independent();
	_free_complete();
	_set_ipl_header(x, y, pixel_type, quantum);
	_alloc ();
	_alloc_data ();
}



void ImageStorage::_make_independent()
{
	// needs to be filled out once references are permitted -paulfitz
	// actually I think this isn't really needed -paulfitz
}


void ImageStorage::_set_ipl_header(int x, int y, int pixel_type, int quantum)
{
    if (quantum==0) {
        quantum = IPL_ALIGN_QWORD;
    }
    int implemented_yet = 1;
	// used to allocate the ipl header.
	switch (pixel_type)
        {
        case VOCAB_PIXEL_MONO:
            pImage = iplCreateImageHeader(
                                          1,
                                          0,
                                          IPL_DEPTH_8U,			
                                          "GRAY",
                                          "GRAY",
                                          IPL_DATA_ORDER_PIXEL,	 
                                          IPL_ORIGIN_TL,			
                                          quantum,		
                                          x,
                                          y,
                                          NULL,
                                          NULL,
                                          NULL,
                                          NULL);
            DBGPF1 ACE_OS::printf("Set pImage to %ld\n", (long int) pImage);
            DBGPF1 ACE_OS::printf("Set init h to %d\n", (long int) pImage->height);
            break;

        case VOCAB_PIXEL_RGB:
            pImage = iplCreateImageHeader(
                                          3,
                                          0,
                                          IPL_DEPTH_8U,			
                                          "RGB",
                                          "RGB",
                                          IPL_DATA_ORDER_PIXEL,	 
                                          IPL_ORIGIN_TL,			
                                          quantum,		
                                          x,
                                          y,
                                          NULL,
                                          NULL,
                                          NULL,
                                          NULL);
            break;

        case VOCAB_PIXEL_HSV:
            pImage = iplCreateImageHeader(
                                          3,
                                          0,
                                          IPL_DEPTH_8U,			
                                          "HSV",
                                          "HSV",
                                          IPL_DATA_ORDER_PIXEL,	 
                                          IPL_ORIGIN_TL,			
                                          quantum,		
                                          x,
                                          y,
                                          NULL,
                                          NULL,
                                          NULL,
                                          NULL);
            break;

        case VOCAB_PIXEL_BGR:
            pImage = iplCreateImageHeader(
                                          3,
                                          0,
                                          IPL_DEPTH_8U,			
                                          "RGB",
                                          "BGR",
                                          IPL_DATA_ORDER_PIXEL,	 
                                          IPL_ORIGIN_TL,			
                                          quantum,		
                                          x,
                                          y,
                                          NULL,
                                          NULL,
                                          NULL,
                                          NULL);
            break;

        case VOCAB_PIXEL_MONO_SIGNED:
            pImage = iplCreateImageHeader(
                                          1,
                                          0,
                                          IPL_DEPTH_8S,			
                                          "GRAY",
                                          "GRAY",
                                          IPL_DATA_ORDER_PIXEL,	 
                                          IPL_ORIGIN_TL,			
                                          quantum,		
                                          x,
                                          y,
                                          NULL,
                                          NULL,
                                          NULL,
                                          NULL);
            break;

        case VOCAB_PIXEL_RGB_SIGNED:
            ACE_ASSERT (implemented_yet == 0);
            break;

        case VOCAB_PIXEL_MONO_FLOAT:
            pImage = iplCreateImageHeader(
                                          1,
                                          0,
                                          IPL_DEPTH_32F,
                                          "GRAY",
                                          "GRAY",
                                          IPL_DATA_ORDER_PIXEL,
                                          IPL_ORIGIN_TL,
                                          quantum,
                                          x,
                                          y,
                                          NULL,
                                          NULL,
                                          NULL,
                                          NULL);
            break;

        case VOCAB_PIXEL_RGB_FLOAT:
            pImage = iplCreateImageHeader(
                                          3,
                                          0,
                                          IPL_DEPTH_32F,			
                                          "RGB",
                                          "RGB",
                                          IPL_DATA_ORDER_PIXEL,	 
                                          IPL_ORIGIN_TL,			
                                          quantum,		
                                          x,
                                          y,
                                          NULL,
                                          NULL,
                                          NULL,
                                          NULL);
            //ACE_ASSERT (implemented_yet == 0);
            break;

        case VOCAB_PIXEL_HSV_FLOAT:
            ACE_ASSERT (implemented_yet == 0);
            break;

        case VOCAB_PIXEL_INT:
            pImage = iplCreateImageHeader(
                                          1,
                                          0,
                                          IPL_DEPTH_32S,
                                          "GRAY",
                                          "GRAY",
                                          IPL_DATA_ORDER_PIXEL,
                                          IPL_ORIGIN_TL,
                                          quantum,
                                          x,
                                          y,
                                          NULL,
                                          NULL,
                                          NULL,
                                          NULL);
            break;

        case VOCAB_PIXEL_INVALID:
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
                                          IPL_ORIGIN_TL,
                                          quantum,
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
                                          IPL_ORIGIN_TL,
                                          quantum,
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
	this->quantum = quantum;
}

void ImageStorage::_alloc_complete_extern(void *buf, int x, int y, int pixel_type, int quantum)
{
    if (quantum==0) {
        quantum = 1;
    }
    this->quantum = quantum;

	_make_independent();
	_free_complete();
	_set_ipl_header(x, y, pixel_type, quantum);
	Data = NULL;
	_alloc_extern (buf);
	_alloc_data ();
	is_owner = 0;
}



// LATER: implement for LINUX.
int ImageStorage::_pad_bytes (int linesize, int align) const
{
    return PAD_BYTES (linesize, align);
}








Image::Image() {
    initialize();
}

void Image::initialize() {
    implementation = NULL;
    data = NULL;
    imgWidth = imgHeight = 0;
    imgPixelSize = imgRowSize = 0;
    imgPixelCode = 0;
    imgQuantum = 0;
    implementation = new ImageStorage(*this);
    ACE_ASSERT(implementation!=NULL);
}


Image::~Image() {
    if (implementation!=NULL) {
        delete (ImageStorage*)implementation;
        implementation = NULL;
    }
}


int Image::getPixelSize() const {
    return imgPixelSize;
}


int Image::getPixelCode() const {
    return imgPixelCode;
}


void Image::zero() {
    if (getRawImage()!=NULL) {
        ACE_OS::memset(getRawImage(),0,getRawImageSize());
    }
}


void Image::resize(int imgWidth, int imgHeight) {
    int code = getPixelCode();
    int size = getPixelSize();
    bool change = false;
    if (code!=imgPixelCode) { 
        imgPixelCode = code; 
        change = true;
    }
    if (size!=imgPixelSize) { 
        imgPixelSize = size; 
        change=true;
    }
    if (imgWidth!=width()||imgHeight!=height()) {
        change = true;
    }

    if (change) {
        ((ImageStorage*)implementation)->resize(imgWidth,imgHeight,
                                                imgPixelCode,
                                                imgPixelSize,
                                                imgQuantum);
        synchronize();
    }
}


void Image::setPixelCode(int imgPixelCode) {
    this->imgPixelCode = imgPixelCode;
}


void Image::setPixelSize(int imgPixelSize) {
    this->imgPixelSize = imgPixelSize;
}


void Image::setQuantum(int imgQuantum) {
    this->imgQuantum = imgQuantum;
}


void Image::synchronize() {
    ImageStorage *impl = (ImageStorage*)implementation;
    ACE_ASSERT(impl!=NULL);
    if (impl->pImage!=NULL) {
        imgWidth = impl->pImage->width;
        imgHeight = impl->pImage->height;
        data = impl->Data;
        imgQuantum = impl->quantum;
        imgRowSize = impl->pImage->widthStep;
    } else {
        data = NULL;
        imgWidth = imgHeight = 0;
    }
    imgPixelSize = getPixelSize();
    imgPixelCode = getPixelCode();
}


unsigned char *Image::getRawImage() const {
    ImageStorage *impl = (ImageStorage*)implementation;
    ACE_ASSERT(impl!=NULL);
    if (impl->pImage!=NULL) {
        return (unsigned char *)impl->pImage->imageData;
    }
    return NULL;
}

int Image::getRawImageSize() const {
    ImageStorage *impl = (ImageStorage*)implementation;
    ACE_ASSERT(impl!=NULL);
    if (impl->pImage!=NULL) {
        return impl->pImage->imageSize;
    }
    return 0;
}

void *Image::getIplImage() {
    // this parameter doesn't seem to get set by YARP.
    // this set should be moved back to the point of creation,
    // but I am not sure where that is.
    ((ImageStorage*)implementation)->pImage->origin = 0;

    return ((ImageStorage*)implementation)->pImage;
}


void Image::wrapIplImage(void *iplImage) {
    YARP_ASSERT(iplImage!=NULL);
    IplImage *p = (IplImage *)iplImage;
    YARP_ASSERT(p->depth==IPL_DEPTH_8U);
    ConstString str = p->colorModel;
    int code = -1;
    if (str=="rgb"||str=="RGB") {
        str = p->channelSeq;
        if (str=="rgb"||str=="RGB") {
            code = VOCAB_PIXEL_RGB;
        } else if (str=="bgr"||str=="BGR") {
            code = VOCAB_PIXEL_BGR;
        } else {
            printf("specific IPL RGB order (%s) is not yet supported\n", 
                   str.c_str());
            printf("Try RGB or BGR\n");
            printf("Or fix code at %s line %d\n",__FILE__,__LINE__);
            ACE_OS::exit(1);
        }
    } else {
        printf("specific IPL format (%s) is not yet supported\n", 
               str.c_str());
        printf("Try RGB or BGR\n");
        printf("Or fix code at %s line %d\n",__FILE__,__LINE__);
        ACE_OS::exit(1);
    }
    if (getPixelCode()!=code && getPixelCode()!=-1) {
        printf("your specific IPL format (%s) does not match your YARP format\n",
               str.c_str());
        printf("Making a copy instead of just wrapping...\n");
        FlexImage img;
        img.setQuantum(p->align);
        img.setPixelCode(code);
        img.setExternal(p->imageData,p->width,p->height);
        copy(img);
    } else {
        setQuantum(p->align);
        setPixelCode(code);
        setExternal(p->imageData,p->width,p->height);
    }
}



/*
  void Image::wrapRawImage(void *buf, int imgWidth, int imgHeight) {
  fprintf(stderr,"YARP2 version of Image class is not yet implemented\n");
  }
*/




#include <yarp/os/NetInt32.h>
#include <yarp/os/begin_pack_for_net.h>

class YARPImagePortContentHeader
{
public:

    /*
    // YARP1 codes
    yarp::os::NetInt32 len;
    yarp::os::NetInt32 w;
    yarp::os::NetInt32 id;
    yarp::os::NetInt32 h;
    yarp::os::NetInt32 depth;
    yarp::os::NetInt32 ext1;
    yarp::os::NetInt32 ext2;
    //double timestamp;
    */

    // YARP2 codes
    //yarp::os::NetInt32 totalLen; // not included any more - redundant
    yarp::os::NetInt32 listTag;
    yarp::os::NetInt32 listLen;
    yarp::os::NetInt32 paramNameTag;
    yarp::os::NetInt32 paramName;
    yarp::os::NetInt32 paramIdTag;
    yarp::os::NetInt32 id;
    yarp::os::NetInt32 paramListTag;
    yarp::os::NetInt32 paramListLen;
    yarp::os::NetInt32 depth;
    yarp::os::NetInt32 imgSize;
    yarp::os::NetInt32 quantum;
    yarp::os::NetInt32 width;
    yarp::os::NetInt32 height;
    yarp::os::NetInt32 paramBlobTag;
    yarp::os::NetInt32 paramBlobLen;

} PACKED_FOR_NET;

#include <yarp/os/end_pack_for_net.h>


bool Image::read(yarp::os::ConnectionReader& connection) {
    
    try {
        // auto-convert text mode interaction
        connection.convertTextMode();

        YARPImagePortContentHeader header;
        
        connection.expectBlock((char*)&header,sizeof(header));
        
        imgPixelCode = header.id;

        int q = getQuantum();
        if (q==0) {
            q = YARP_IMAGE_ALIGN;
        }
        if (q!=header.quantum) {
            
            if ((header.depth*header.width)%q==0) {
                header.quantum = q;
            }
        }
        
        if (getPixelCode()!=header.id||q!=header.quantum) {
            // we're trying to read an incompatible image type
            // rather than just fail, we'll read it (inefficiently)
            FlexImage flex;
            flex.setPixelCode(header.id);
            flex.setQuantum(header.quantum);
            flex.resize(header.width,header.height);
            if (header.width!=0&&header.height!=0) {
                unsigned char *mem = flex.getRawImage();
                ACE_ASSERT(mem!=NULL);
                if (flex.getRawImageSize()!=header.imgSize) {
                    printf("There is a problem reading an image\n");
                    printf("incoming: width %d, height %d, quantum %d, size %d\n",
                           (int)header.width, (int)header.height, 
                           (int)header.quantum, (int)header.imgSize);
                    printf("my space: width %d, height %d, quantum %d, size %d\n",
                           flex.width(), flex.height(), flex.getQuantum(), 
                           flex.getRawImageSize());
                }
                ACE_ASSERT(flex.getRawImageSize()==header.imgSize);
                try {
                    connection.expectBlock((char *)flex.getRawImage(),
                                           flex.getRawImageSize());
                } catch (IOException e) {
                    return false;
                }
            }
            copy(flex);
        } else {
            ACE_ASSERT(getPixelCode()==header.id);
            resize(header.width,header.height);
            unsigned char *mem = getRawImage();
            if (header.width!=0&&header.height!=0) {
                ACE_ASSERT(mem!=NULL);
                if (getRawImageSize()!=header.imgSize) {
                    printf("There is a problem reading an image\n");
                    printf("incoming: width %d, height %d, quantum %d, size %d\n",
                           (int)header.width, (int)header.height, 
                           (int)header.quantum, (int)header.imgSize);
                    printf("my space: width %d, height %d, quantum %d, size %d\n",
                           width(), height(), getQuantum(), getRawImageSize());
                }
                ACE_ASSERT(getRawImageSize()==header.imgSize);
                connection.expectBlock((char *)getRawImage(),getRawImageSize());
            }
        }
        
    } catch (IOException e) {
        return false;
    }

    return true;
}


bool Image::write(yarp::os::ConnectionWriter& connection) {
    YARPImagePortContentHeader header;

    try {
        header.listTag = BOTTLE_TAG_LIST;
        header.listLen = 4;
        header.paramNameTag = BOTTLE_TAG_VOCAB;
        header.paramName = VOCAB3('m','a','t');
        header.paramIdTag = BOTTLE_TAG_VOCAB;
        header.id = getPixelCode();
        header.paramListTag = BOTTLE_TAG_LIST + BOTTLE_TAG_INT;
        header.paramListLen = 5;
        header.depth = getPixelSize();
        header.imgSize = getRawImageSize();
        header.quantum = getQuantum();
        header.width = width();
        header.height = height();
        header.paramBlobTag = BOTTLE_TAG_BLOB;
        header.paramBlobLen = getRawImageSize();
        
        connection.appendBlock((char*)&header,sizeof(header));
        unsigned char *mem = getRawImage();
        if (header.width!=0&&header.height!=0) {
            ACE_ASSERT(mem!=NULL);
            
            // Note use of external block.  
            // Implies care needed about ownership.
            connection.appendExternalBlock((char *)mem,header.imgSize);
        }

        // if someone is foolish enough to connect in text mode,
        // let them see something readable.
        connection.convertTextMode();

        return true;
    } catch (IOException e) {
        // miserable failure
    }
    return false;
}


Image::Image(const Image& alt) {
    initialize();
    copy(alt);
}


const Image& Image::operator=(const Image& alt) {
    copy(alt);
    return *this;
}


bool Image::copy(const Image& alt) {
    bool ok = false;
    int myCode = getPixelCode();
    if (myCode==0) {
        setPixelCode(alt.getPixelCode());
    }
    resize(alt.width(),alt.height());
    int q1 = alt.getQuantum();
    int q2 = getQuantum();
    if (q1==0) { q1 = YARP_IMAGE_ALIGN; }
    if (q2==0) { q2 = YARP_IMAGE_ALIGN; }

    YARP_ASSERT(width()==alt.width());
    YARP_ASSERT(height()==alt.height());
    if (getPixelCode()==alt.getPixelCode()) {
        if (getQuantum()==alt.getQuantum()) {
            YARP_ASSERT(getRawImageSize()==alt.getRawImageSize());
            YARP_ASSERT(q1==q2);
        }
    }
    copyPixels(alt.getRawImage(),alt.getPixelCode(),
               getRawImage(),getPixelCode(),
               width(),height(),
               getRawImageSize(),q1,q2);
    ok = true;
    return ok;
}


void Image::setExternal(void *data, int imgWidth, int imgHeight) {
    if (imgQuantum==0) {
        imgQuantum = 1;
    }
    ((ImageStorage*)implementation)->_alloc_complete_extern(data,
                                                            imgWidth,
                                                            imgHeight,
                                                            getPixelCode(),
                                                            imgQuantum);
    synchronize();
}


bool Image::copy(const Image& alt, int w, int h) {
    if (&alt==this) {
        FlexImage img;
        img.copy(*this);
        return copy(img,w,h);
    }

    if (getPixelCode()!=alt.getPixelCode()) {
        FlexImage img;
        img.setPixelCode(getPixelCode());
        img.setPixelSize(getPixelSize());
        img.setQuantum(getQuantum());
        img.copy(alt);
        return copy(img,w,h);
    }

    resize(w,h);
    int d = getPixelSize();

    int nw = w;
    int nh = h;
    w = alt.width();
    h = alt.height();

	float di = ((float)h)/nh;
	float dj = ((float)w)/nw;

	for (int i=0; i<nh; i++)
        {
            int i0 = (int)(di*i);
            for (int j=0; j<nw; j++)
                {
                    int j0 = (int)(dj*j);
                    ACE_OS::memcpy(getPixelAddress(j,i),
                                   alt.getPixelAddress(j0,i0),
                                   d);
                }
        }
    return true;
}


void Image::setStamp() {
    int ct = 0;
    if (stamp.isValid()) {
        ct = stamp.getCount()+1;
    }
    double now = Time::now();
    stamp = Stamp(ct,now);
}

