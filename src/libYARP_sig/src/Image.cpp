/*
 * Copyright (C) 2007 RobotCub Consortium
 * Authors: Paul Fitzpatrick, Giorgio Metta
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */


/*
  This file is in a pretty hacky state.  Sorry!

*/

#include <yarp/sig/IplImage.h>

#include <yarp/os/Log.h>
#include <yarp/sig/Image.h>
#include <yarp/sig/ImageNetworkHeader.h>

#include <yarp/os/Bottle.h>
#include <yarp/os/Vocab.h>
#include <yarp/os/ConstString.h>
#include <yarp/os/Time.h>

#include <yarp/sig/impl/DeBayer.h>

#include <cstdio>
#include <cstring>

using namespace yarp::sig;
using namespace yarp::os;

#define DBGPF1 if (0)

//inline int PAD_BYTES (int len, int pad)
//{
//	const int rem = len % pad;
//	return (rem != 0) ? (pad - rem) : rem;
//}

/**
* This helper function groups code to avoid duplication. It is not a member function of Image because 
* there are problems with ImageNetworkHeader, anyhow the function is state-less and uses only parameters.
*/
inline bool readFromConnection(Image &dest, ImageNetworkHeader &header, ConnectionReader& connection)
{
    dest.resize(header.width, header.height);
    unsigned char *mem = dest.getRawImage();
    int allocatedBytes = dest.getRawImageSize();
    yAssert(mem != NULL);
    //this check is redundant with assertion, I would remove it
    if (dest.getRawImageSize() != header.imgSize) {
        printf("There is a problem reading an image\n");
        printf("incoming: width %d, height %d, code %d, quantum %d, size %d\n",
            (int)header.width, (int)header.height,
            (int)header.id,
            (int)header.quantum, (int)header.imgSize);
        printf("my space: width %d, height %d, code %d, quantum %d, size %d\n",
            dest.width(), dest.height(), dest.getPixelCode(), dest.getQuantum(), allocatedBytes);
    }
    yAssert(allocatedBytes == header.imgSize);
    bool ok = connection.expectBlock((char *)mem, allocatedBytes);
    return (!connection.isError() && ok);
}



class ImageStorage {
public:
    IplImage* pImage;
    char **Data;  // this is not IPL. it's char to maintain IPL compatibility
    int extern_type_id;
    int extern_type_quantum;
    int quantum;
    bool topIsLow;

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
    void _set_ipl_header(int x, int y, int pixel_type, int quantum,
                         bool topIsLow);
    void _free_ipl_header();
    void _alloc_complete(int x, int y, int pixel_type, int quantum,
                         bool topIsLow);
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
        topIsLow = true;
        extern_type_id = 0;
        extern_type_quantum = -1;
    }

    ~ImageStorage() {
        _free_complete();
    }

    void resize(int x, int y, int pixel_type,
                int pixel_size, int quantum, bool topIsLow);

    void _alloc_complete_extern(void *buf, int x, int y, int pixel_type,
                                int quantum, bool topIsLow);

};


void ImageStorage::resize(int x, int y, int pixel_type,
                          int pixel_size, int quantum, bool topIsLow) {
    int need_recreation = 1;

    if (quantum==0) {
        quantum = YARP_IMAGE_ALIGN;
    }

    if (need_recreation) {
        _free_complete();
        DBGPF1 printf("HIT recreation for %ld %ld: %d %d %d\n", (long int) this, (long int) pImage, x, y, pixel_type);
        _alloc_complete (x, y, pixel_type, quantum, topIsLow);
    }
    extern_type_id = pixel_type;
    extern_type_quantum = quantum;
}




// allocates an empty image.
void ImageStorage::_alloc (void) {
    yAssert(pImage != NULL);

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
    yAssert(pImage != NULL);
    yAssert(Data==NULL);

    if (pImage != NULL)
        if (pImage->imageData != NULL)
            iplDeallocateImage (pImage);

    //iplAllocateImage (pImage, 0, 0);
    pImage->imageData = (char*)buf;
    // probably need to do more for real IPL

    //iplSetBorderMode (pImage, IPL_BORDER_CONSTANT, IPL_SIDE_ALL, 0);
}

// allocates the Data pointer.
void ImageStorage::_alloc_data (void)
{
    DBGPF1 printf("alloc_data1\n"), fflush(stdout);
    yAssert(pImage != NULL);

    yAssert(Data==NULL);

    char **ptr = new char *[pImage->height];

    Data = ptr;

    yAssert(Data != NULL);

    yAssert(pImage->imageData != NULL);

    int height = pImage->height;

    char * DataArea = pImage->imageData;

    for (int r = 0; r < height; r++)
        {
            if (topIsLow) {
                Data[r] = DataArea;
            } else {
                Data[height-r-1] = DataArea;
            }
            DataArea += pImage->widthStep;
        }
    DBGPF1 printf("alloc_data4\n");
}

void ImageStorage::_free (void)
{
    if (pImage != NULL)
        if (pImage->imageData != NULL)
            {
                if (is_owner)
                    {
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
    yAssert(Data==NULL); // Now always free Data at same time
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


void ImageStorage::_alloc_complete(int x, int y, int pixel_type, int quantum,
                                   bool topIsLow)
{
    _make_independent();
    _free_complete();
    _set_ipl_header(x, y, pixel_type, quantum, topIsLow);
    _alloc ();
    _alloc_data ();
}



void ImageStorage::_make_independent()
{
    // actually I think this isn't really needed -paulfitz
}


void ImageStorage::_set_ipl_header(int x, int y, int pixel_type, int quantum,
                                   bool topIsLow)
{
    if (quantum==0) {
        quantum = IPL_ALIGN_QWORD;
    }
    int origin = topIsLow?IPL_ORIGIN_TL:IPL_ORIGIN_BL;
    int implemented_yet = 1;
    // used to allocate the ipl header.
    switch (pixel_type)
        {
        case VOCAB_PIXEL_MONO:
            pImage = iplCreateImageHeader(
                                          1,
                                          0,
                                          IPL_DEPTH_8U,			
                                          (char *)"GRAY",
                                          (char *)"GRAY",
                                          IPL_DATA_ORDER_PIXEL,
                                          origin,
                                          quantum,		
                                          x,
                                          y,
                                          NULL,
                                          NULL,
                                          NULL,
                                          NULL);
            DBGPF1 printf("Set pImage to %ld\n", (long int) pImage);
            DBGPF1 printf("Set init h to %ld\n", (long int) pImage->height);
            break;

        case VOCAB_PIXEL_MONO16:
            pImage = iplCreateImageHeader(
                                          1,
                                          0,
                                          IPL_DEPTH_16U,
                                          (char *)"GRAY",
                                          (char *)"GRAY",
                                          IPL_DATA_ORDER_PIXEL,
                                          origin,
                                          quantum,
                                          x,
                                          y,
                                          NULL,
                                          NULL,
                                          NULL,
                                          NULL);
            break;

        case VOCAB_PIXEL_RGB:
            pImage = iplCreateImageHeader(
                                          3,
                                          0,
                                          IPL_DEPTH_8U,			
                                          (char *)"RGB",
                                          (char *)"RGB",
                                          IPL_DATA_ORDER_PIXEL,
                                          origin,
                                          quantum,		
                                          x,
                                          y,
                                          NULL,
                                          NULL,
                                          NULL,
                                          NULL);
            break;

        case VOCAB_PIXEL_RGBA:
            pImage = iplCreateImageHeader(
                                          4,
                                          0,
                                          IPL_DEPTH_8U,			
                                          (char *)"RGBA",
                                          (char *)"RGBA",
                                          IPL_DATA_ORDER_PIXEL,
                                          origin,
                                          quantum,		
                                          x,
                                          y,
                                          NULL,
                                          NULL,
                                          NULL,
                                          NULL);
            break;

        case VOCAB_PIXEL_BGRA:
            pImage = iplCreateImageHeader(
                                          4,
                                          0,
                                          IPL_DEPTH_8U,			
                                          (char *)"BGRA",
                                          (char *)"BGRA",
                                          IPL_DATA_ORDER_PIXEL,
                                          origin,
                                          quantum,		
                                          x,
                                          y,
                                          NULL,
                                          NULL,
                                          NULL,
                                          NULL);
            break;

        case VOCAB_PIXEL_RGB_INT:
            pImage = iplCreateImageHeader(
                                          3,
                                          0,
                                          IPL_DEPTH_32S,
                                          (char *)"RGB",
                                          (char *)"RGB",
                                          IPL_DATA_ORDER_PIXEL,
                                          origin,
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
                                          (char *)"HSV",
                                          (char *)"HSV",
                                          IPL_DATA_ORDER_PIXEL,
                                          origin,
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
                                          (char *)"RGB",
                                          (char *)"BGR",
                                          IPL_DATA_ORDER_PIXEL,
                                          origin,
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
                                          (char *)"GRAY",
                                          (char *)"GRAY",
                                          IPL_DATA_ORDER_PIXEL,
                                          origin,
                                          quantum,		
                                          x,
                                          y,
                                          NULL,
                                          NULL,
                                          NULL,
                                          NULL);
            break;

        case VOCAB_PIXEL_RGB_SIGNED:
            yAssert(implemented_yet == 0);
            break;

        case VOCAB_PIXEL_MONO_FLOAT:
            pImage = iplCreateImageHeader(
                                          1,
                                          0,
                                          IPL_DEPTH_32F,
                                          (char *)"GRAY",
                                          (char *)"GRAY",
                                          IPL_DATA_ORDER_PIXEL,
                                          origin,
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
                                          (char *)"RGB",
                                          (char *)"RGB",
                                          IPL_DATA_ORDER_PIXEL,
                                          origin,
                                          quantum,		
                                          x,
                                          y,
                                          NULL,
                                          NULL,
                                          NULL,
                                          NULL);
            //yAssert(implemented_yet == 0);
            break;

        case VOCAB_PIXEL_HSV_FLOAT:
            yAssert(implemented_yet == 0);
            break;

        case VOCAB_PIXEL_INT:
            pImage = iplCreateImageHeader(
                                          1,
                                          0,
                                          IPL_DEPTH_32S,
                                          (char *)"GRAY",
                                          (char *)"GRAY",
                                          IPL_DATA_ORDER_PIXEL,
                                          origin,
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
            printf ("*** Trying to allocate an invalid pixel type image\n");
            exit(1);
            break;

        case -2:
            pImage = iplCreateImageHeader(
                                          1,
                                          0,
                                          IPL_DEPTH_16U,
                                          (char *)"GRAY",
                                          (char *)"GRAY",
                                          IPL_DATA_ORDER_PIXEL,
                                          origin,
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
                                          (char *)"GRAY",
                                          (char *)"GRAY",
                                          IPL_DATA_ORDER_PIXEL,
                                          origin,
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
            yAssert(implemented_yet == 0);
            break;
        }

    type_id = pixel_type;
    this->quantum = quantum;
    this->topIsLow = topIsLow;
}

void ImageStorage::_alloc_complete_extern(void *buf, int x, int y, int pixel_type, int quantum, bool topIsLow)
{
    if (quantum==0) {
        quantum = 1;
    }
    this->quantum = quantum;
    this->topIsLow = topIsLow;

    _make_independent();
    _free_complete();
    _set_ipl_header(x, y, pixel_type, quantum, topIsLow);
    Data = NULL;
    _alloc_extern (buf);
    _alloc_data ();
    is_owner = 0;
}



int ImageStorage::_pad_bytes (int linesize, int align) const
{
    return yarp::sig::PAD_BYTES (linesize, align);
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
    topIsLow = true;
    implementation = new ImageStorage(*this);
    yAssert(implementation!=NULL);
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
        memset(getRawImage(),0,getRawImageSize());
    }
}


void Image::resize(int imgWidth, int imgHeight) {
    yAssert(imgWidth>=0 && imgHeight>=0);

    int code = getPixelCode();
    int size = getPixelSize();
    bool change = false;
    if (code!=imgPixelCode) {
        imgPixelCode = code;
        change = true;
    }
    if (imgPixelCode!=((ImageStorage*)implementation)->extern_type_id) {
        change = true;
    }
    if (imgQuantum!=((ImageStorage*)implementation)->extern_type_quantum) {
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
                                                imgQuantum,
                                                topIsLow);
        synchronize();
        //printf("CHANGE! %ld\n", (long int)(this));
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
    yAssert(impl!=NULL);
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
    yAssert(impl!=NULL);
    if (impl->pImage!=NULL) {
        return (unsigned char *)impl->pImage->imageData;
    }
    return NULL;
}

int Image::getRawImageSize() const {
    ImageStorage *impl = (ImageStorage*)implementation;
    yAssert(impl!=NULL);
    if (impl->pImage!=NULL) {
        return impl->pImage->imageSize;
    }
    return 0;
}

void *Image::getIplImage() {
    return ((ImageStorage*)implementation)->pImage;
}

const void *Image::getIplImage() const {
    return ((const ImageStorage*)implementation)->pImage;
}

void Image::wrapIplImage(void *iplImage) {
    yAssert(iplImage!=NULL);
    IplImage *p = (IplImage *)iplImage;
    ConstString str = p->colorModel;
    int code = -1;
    int color_code = -1;
    if (str=="rgb"||str=="RGB"||
        str=="bgr"||str=="BGR"||
        str=="gray"||str=="GRAY"||
        str=="graygray"||str=="GRAYGRAY") {
        str = p->channelSeq;
        if (str=="rgb"||str=="RGB") {
            color_code = VOCAB_PIXEL_RGB;
        } else if (str=="bgr"||str=="BGR") {
            color_code = VOCAB_PIXEL_BGR;
        } else if (str=="gray"||str=="GRAY"||
                   str=="graygray"||str=="GRAYGRAY") {
            color_code = VOCAB_PIXEL_MONO;
        } else {
            printf("specific IPL RGB order (%s) is not yet supported\n",
                   str.c_str());
            printf("Try RGB, BGR, or \n");
            printf("Or fix code at %s line %d\n",__FILE__,__LINE__);
            exit(1);
        }
    }

    // Type translation is approximate.  Patches welcome to flesh out
    // the types available.
    if (p->depth == IPL_DEPTH_8U) {
        code = color_code;
    } else if (p->depth == IPL_DEPTH_8S) {
        switch (color_code) {
        case VOCAB_PIXEL_MONO:
            code = VOCAB_PIXEL_MONO_SIGNED;
            break;
        case VOCAB_PIXEL_RGB:
            code = VOCAB_PIXEL_RGB_SIGNED;
            break;
        case VOCAB_PIXEL_BGR:
            code = color_code; // nothing better available
            break;
        }
    } else if (p->depth == IPL_DEPTH_16U || p->depth == IPL_DEPTH_16S) {
        switch (color_code) {
        case VOCAB_PIXEL_MONO:
            code = VOCAB_PIXEL_MONO16;
            break;
        case VOCAB_PIXEL_RGB:
        case VOCAB_PIXEL_BGR:
            fprintf(stderr,"No translation currently available for this pixel type\n");
            exit(1);
            break;
        }
    } else if (p->depth == IPL_DEPTH_32S) {
        switch (color_code) {
        case VOCAB_PIXEL_MONO:
            code = VOCAB_PIXEL_INT;
            break;
        case VOCAB_PIXEL_RGB:
        case VOCAB_PIXEL_BGR:
            code = VOCAB_PIXEL_RGB_INT;
            break;
        }
    } else if (p->depth == IPL_DEPTH_32F) {
        switch (color_code) {
        case VOCAB_PIXEL_MONO:
            code = VOCAB_PIXEL_MONO_FLOAT;
            break;
        case VOCAB_PIXEL_RGB:
        case VOCAB_PIXEL_BGR:
            code = VOCAB_PIXEL_RGB_FLOAT;
            break;
        }
    }

    if (code==-1) {
        fprintf(stderr,"IPL pixel type / depth combination is not yet supported\n");
        fprintf(stderr,"Please email a YARP developer to complain, quoting this:\n");
        fprintf(stderr,"   %s:%d\n", __FILE__, __LINE__);
    }

    if (getPixelCode()!=code && getPixelCode()!=-1) {
        printf("your specific IPL format (%s depth %d -> %s) does not match your YARP format (%s)\n",
               str.c_str(),
               p->depth,
               Vocab::decode(code).c_str(),
               Vocab::decode(getPixelCode()).c_str());
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



bool Image::read(yarp::os::ConnectionReader& connection) {

    // auto-convert text mode interaction
    connection.convertTextMode();

    ImageNetworkHeader header;

    bool ok = connection.expectBlock((char*)&header,sizeof(header));
    if (!ok) return false;

    //first check that the received image size is reasonable
    if (header.width == 0 || header.height == 0)
    {
        // I maintain the prevous logic, although we should probably return false
        return !connection.isError(); 
    }

    imgPixelCode = header.id;

    int q = getQuantum();
    if (q==0) {
        //q = YARP_IMAGE_ALIGN;
        setQuantum(header.quantum);
        q = getQuantum();
    }
    if (q!=header.quantum) {
        if ((header.depth*header.width)%header.quantum==0 &&
            (header.depth*header.width)%q==0) {
            header.quantum = q;
        }
    }

    // handle easy case, received and current image are compatible, no conversion needed
    if (getPixelCode() == header.id && q == header.quantum)
    {
        return readFromConnection(*this, header, connection);
    }

    // image is bayer 8 bits, current image is MONO, copy as is (keep raw format)
    if (getPixelCode() == VOCAB_PIXEL_MONO && isBayer8(header.id))
    {
        return readFromConnection(*this, header, connection);
    }
    // image is bayer 16 bits, current image is MONO16, copy as is (keep raw format)
    if (getPixelCode() == VOCAB_PIXEL_MONO16 && isBayer16(header.id))
    {
        return readFromConnection(*this, header, connection);
    }

    ////////////////////
    // Received and current images are binary incompatible do our best to convert
    //

    // handle here all bayer encoding 8 bits
    if (isBayer8(header.id))
    {
        FlexImage flex;
        flex.setPixelCode(VOCAB_PIXEL_MONO);
        flex.setQuantum(header.quantum);

        bool ok = readFromConnection(flex, header, connection);
        if (!ok)
            return false;

        if (getPixelCode() == VOCAB_PIXEL_BGR && header.id==VOCAB_PIXEL_ENCODING_BAYER_GRBG8)
            return deBayer_GRBG8_TO_BGR(flex, *this, 3);
        else if (getPixelCode() == VOCAB_PIXEL_BGRA && header.id == VOCAB_PIXEL_ENCODING_BAYER_GRBG8)
            return deBayer_GRBG8_TO_BGR(flex, *this, 4);
        if (getPixelCode() == VOCAB_PIXEL_RGB && header.id==VOCAB_PIXEL_ENCODING_BAYER_GRBG8)
            return deBayer_GRBG8_TO_RGB(flex, *this, 3);
        if (getPixelCode() == VOCAB_PIXEL_RGBA && header.id == VOCAB_PIXEL_ENCODING_BAYER_GRBG8)
            return deBayer_GRBG8_TO_RGB(flex, *this, 4);
        else
        {
            YARP_FIXME_NOTIMPLEMENTED("Conversion from bayer encoding not yet implemented\n");
            return false;
        }
    }

    // handle here all bayer encodings 16 bits
    if (isBayer16(header.id))
    {
        // as bayer16 seems unlikely we defer implementation for later
        YARP_FIXME_NOTIMPLEMENTED("Conversion from bayer encoding 16 bits not yet implemented\n");
        return false;
    }

    // Received image has valid YARP pixels and can be converted using Image primitives
    // prepare a FlexImage, set it to be compatible with the received image
    // read new image into FlexImage then copy from it.
    FlexImage flex;
    flex.setPixelCode(header.id);
    flex.setQuantum(header.quantum);
    ok = readFromConnection(flex, header, connection);
    if (ok)
        copy(flex);

    return ok;    
}


bool Image::write(yarp::os::ConnectionWriter& connection) {
    ImageNetworkHeader header;
    header.setFromImage(*this);
    /*
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
    */

    connection.appendBlock((char*)&header,sizeof(header));
    unsigned char *mem = getRawImage();
    if (header.width!=0&&header.height!=0) {
        yAssert(mem!=NULL);

        // Note use of external block.
        // Implies care needed about ownership.
        connection.appendExternalBlock((char *)mem,header.imgSize);
    }

    // if someone is foolish enough to connect in text mode,
    // let them see something readable.
    connection.convertTextMode();

    return !connection.isError();
}


Image::Image(const Image& alt) : Portable() {
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
        setPixelSize(alt.getPixelSize());
        setQuantum(alt.getQuantum());
    }
    resize(alt.width(),alt.height());
    int q1 = alt.getQuantum();
    int q2 = getQuantum();
    if (q1==0) { q1 = YARP_IMAGE_ALIGN; }
    if (q2==0) { q2 = YARP_IMAGE_ALIGN; }

    bool o1 = alt.topIsLowIndex();
    bool o2 = topIsLowIndex();

    yAssert(width()==alt.width());
    yAssert(height()==alt.height());
    if (getPixelCode()==alt.getPixelCode()) {
        if (getQuantum()==alt.getQuantum()) {
            yAssert(getRawImageSize()==alt.getRawImageSize());
            yAssert(q1==q2);
        }
    }
    copyPixels(alt.getRawImage(),alt.getPixelCode(),
               getRawImage(),getPixelCode(),
               width(),height(),
               getRawImageSize(),q1,q2,o1,o2);
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
                                                            imgQuantum,
                                                            topIsLow);
    synchronize();
}


bool Image::copy(const Image& alt, int w, int h) {
    if (getPixelCode()==0) {
        setPixelCode(alt.getPixelCode());
        setPixelSize(alt.getPixelSize());
        setQuantum(alt.getQuantum());
    }
    if (&alt==this) {
        FlexImage img;
        img.copy(alt);
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
                    memcpy(getPixelAddress(j,i),
                           alt.getPixelAddress(j0,i0),
                           d);
                }
        }
    return true;
}

