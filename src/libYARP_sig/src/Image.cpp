/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 * Copyright (C) 2006-2010 RobotCub Consortium
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

/*
  This file is in a pretty hacky state.  Sorry!

*/

#include <yarp/sig/impl/IplImage.h>

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
    yAssert(mem != nullptr);
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
    void _alloc ();
    void _alloc_extern (const void *buf);
    void _alloc_data ();
    void _free ();
    void _free_data ();

    void _make_independent();
    bool _set_ipl_header(int x, int y, int pixel_type, int quantum,
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
        pImage = nullptr;
        Data = nullptr;
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
                int quantum, bool topIsLow);

    void _alloc_complete_extern(const void *buf, int x, int y, int pixel_type,
                                int quantum, bool topIsLow);

};


void ImageStorage::resize(int x, int y, int pixel_type,
                          int quantum, bool topIsLow) {
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
void ImageStorage::_alloc () {


    _free(); // was iplDeallocateImage(pImage); but that won't work with refs

    if ((type_id == VOCAB_PIXEL_MONO_FLOAT) ||
        (type_id == VOCAB_PIXEL_RGB_FLOAT)  ||
        (type_id == VOCAB_PIXEL_HSV_FLOAT))
        iplAllocateImageFP(pImage, 0, 0);
    else
        iplAllocateImage (pImage, 0, 0);

    iplSetBorderMode (pImage, IPL_BORDER_CONSTANT, IPL_SIDE_ALL, 0);
}

// installs an external buffer as the image data
void ImageStorage::_alloc_extern (const void *buf)
{
    yAssert(pImage != nullptr);
    yAssert(Data==nullptr);

    if (pImage != nullptr)
        if (pImage->imageData != nullptr)
            iplDeallocateImage (pImage);

    //iplAllocateImage (pImage, 0, 0);
    pImage->imageData = (char*)buf;
    // probably need to do more for real IPL

    //iplSetBorderMode (pImage, IPL_BORDER_CONSTANT, IPL_SIDE_ALL, 0);
}

// allocates the Data pointer.
void ImageStorage::_alloc_data ()
{
    DBGPF1 printf("alloc_data1\n"), fflush(stdout);
    yAssert(pImage != nullptr);

    yAssert(Data==nullptr);

    char **ptr = new char *[pImage->height];

    Data = ptr;

    yAssert(Data != nullptr);

    yAssert(pImage->imageData != nullptr);

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

void ImageStorage::_free ()
{
    if (pImage != nullptr)
        if (pImage->imageData != nullptr)
            {
                if (is_owner)
                    {
                        iplDeallocateImage (pImage);
                        if (Data!=nullptr)
                            {
                                delete[] Data;
                            }
                    }
                else
                    {
                        if (Data!=nullptr)
                            {
                                delete[] Data;
                            }
                    }

                is_owner = 1;
                Data = nullptr;
                pImage->imageData = nullptr;
            }
}

void ImageStorage::_free_data ()
{
    yAssert(Data==nullptr); // Now always free Data at same time
}


void ImageStorage::_free_complete()
{
    _free();
    _free_data();
    _free_ipl_header();
}


void ImageStorage::_free_ipl_header()
{
    if (pImage!=nullptr)
        {
            iplDeallocate (pImage, IPL_IMAGE_HEADER);
        }
    pImage = nullptr;
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

struct pixelTypeIplParams
{
    int   nChannels;
    int   depth;
    const char* colorModel;
    const char* channelSeq;
};

const pixelTypeIplParams iplPixelTypeMono{1, IPL_DEPTH_8U,  "GRAY", "GRAY"};
const pixelTypeIplParams iplPixelTypeMono16{1, IPL_DEPTH_16U,  "GRAY", "GRAY"};

const std::map<int, pixelTypeIplParams> pixelCode2iplParams = {
    {VOCAB_PIXEL_MONO,                  iplPixelTypeMono},
    {VOCAB_PIXEL_ENCODING_BAYER_GRBG8,  iplPixelTypeMono},
    {VOCAB_PIXEL_ENCODING_BAYER_BGGR8,  iplPixelTypeMono},
    {VOCAB_PIXEL_ENCODING_BAYER_GBRG8,  iplPixelTypeMono},
    {VOCAB_PIXEL_ENCODING_BAYER_RGGB8,  iplPixelTypeMono},
    {VOCAB_PIXEL_YUV_420,               iplPixelTypeMono},
    {VOCAB_PIXEL_YUV_444,               iplPixelTypeMono},
    {VOCAB_PIXEL_YUV_422,               iplPixelTypeMono},
    {VOCAB_PIXEL_YUV_411,               iplPixelTypeMono},
    {VOCAB_PIXEL_MONO16,                iplPixelTypeMono16},
    {VOCAB_PIXEL_ENCODING_BAYER_GRBG16, iplPixelTypeMono16},
    {VOCAB_PIXEL_ENCODING_BAYER_BGGR16, iplPixelTypeMono16},
    {VOCAB_PIXEL_ENCODING_BAYER_GBRG16, iplPixelTypeMono16},
    {VOCAB_PIXEL_ENCODING_BAYER_RGGB16, iplPixelTypeMono16},
    {VOCAB_PIXEL_RGB,                   {3, IPL_DEPTH_8U,  "RGB",  "RGB" }},
    {VOCAB_PIXEL_RGBA,                  {4, IPL_DEPTH_8U,  "RGBA", "RGBA"}},
    {VOCAB_PIXEL_BGRA,                  {4, IPL_DEPTH_8U,  "BGRA", "BGRA"}},
    {VOCAB_PIXEL_INT,                   {1, IPL_DEPTH_32S, "GRAY", "GRAY"}},
    {VOCAB_PIXEL_HSV,                   {3, IPL_DEPTH_8U,  "HSV",  "HSV" }},
    {VOCAB_PIXEL_BGR,                   {3, IPL_DEPTH_8U,  "RGB",  "BGR" }},
    {VOCAB_PIXEL_MONO_SIGNED,           {1, IPL_DEPTH_8S,  "GRAY", "GRAY"}},
    {VOCAB_PIXEL_RGB_INT,               {3, IPL_DEPTH_32S, "RGB",  "RGB" }},
    {VOCAB_PIXEL_MONO_FLOAT,            {1, IPL_DEPTH_32F, "GRAY", "GRAY"}},
    {VOCAB_PIXEL_RGB_FLOAT,             {3, IPL_DEPTH_32F, "RGB",  "RGB" }},
    {-2,                                iplPixelTypeMono16},
    {-4,                                {1, IPL_DEPTH_32S, "GRAY", "GRAY"}}
};

bool ImageStorage::_set_ipl_header(int x, int y, int pixel_type, int quantum,
                                   bool topIsLow)
{
    if (pImage != nullptr) {
        iplDeallocateImage(pImage);
        pImage = nullptr;
    }

    if (pixel_type == VOCAB_PIXEL_INVALID) {
        // not a type!
        printf ("*** Trying to allocate an invalid pixel type image\n");
        std::exit(1);
     }
    if (pixelCode2iplParams.find(pixel_type) == pixelCode2iplParams.end()) {
        // unknown pixel type. Should revert to a non-IPL mode... how?
        return false;
    }

    const pixelTypeIplParams& param = pixelCode2iplParams.at(pixel_type);

    if (quantum==0) {
        quantum = IPL_ALIGN_QWORD;
    }
    int origin = topIsLow ? IPL_ORIGIN_TL : IPL_ORIGIN_BL;

    pImage = iplCreateImageHeader(param.nChannels, 0, param.depth, const_cast<char*>(param.colorModel), const_cast<char*>(param.channelSeq), IPL_DATA_ORDER_PIXEL, origin, quantum, x, y, nullptr, nullptr, nullptr, nullptr);

    type_id = pixel_type;
    this->quantum = quantum;
    this->topIsLow = topIsLow;
    return true;
}

void ImageStorage::_alloc_complete_extern(const void *buf, int x, int y, int pixel_type, int quantum, bool topIsLow)
{
    if (quantum==0) {
        quantum = 1;
    }
    this->quantum = quantum;
    this->topIsLow = topIsLow;

    _make_independent();
    _free_complete();
    _set_ipl_header(x, y, pixel_type, quantum, topIsLow);
    Data = nullptr;
    _alloc_extern (buf);
    _alloc_data ();
    is_owner = 0;
}



int ImageStorage::_pad_bytes (int linesize, int align) const
{
    return yarp::sig::PAD_BYTES (linesize, align);
}

const std::map<YarpVocabPixelTypesEnum, unsigned int> Image::pixelCode2Size = {
    {VOCAB_PIXEL_INVALID,               0 },
    {VOCAB_PIXEL_MONO,                  sizeof(yarp::sig::PixelMono)},
    {VOCAB_PIXEL_MONO16,                sizeof(yarp::sig::PixelMono16)},
    {VOCAB_PIXEL_RGB,                   sizeof(yarp::sig::PixelRgb)},
    {VOCAB_PIXEL_RGBA,                  sizeof(yarp::sig::PixelRgba)},
    {VOCAB_PIXEL_BGRA,                  sizeof(yarp::sig::PixelBgra)},
    {VOCAB_PIXEL_INT,                   sizeof(yarp::sig::PixelInt)},
    {VOCAB_PIXEL_HSV,                   sizeof(yarp::sig::PixelHsv)},
    {VOCAB_PIXEL_BGR,                   sizeof(yarp::sig::PixelBgr)},
    {VOCAB_PIXEL_MONO_SIGNED,           sizeof(yarp::sig::PixelMonoSigned)},
    {VOCAB_PIXEL_RGB_SIGNED,            sizeof(yarp::sig::PixelRgbSigned)},
    {VOCAB_PIXEL_RGB_INT,               sizeof(yarp::sig::PixelRgbInt)},
    {VOCAB_PIXEL_MONO_FLOAT,            sizeof(yarp::sig::PixelFloat)},
    {VOCAB_PIXEL_RGB_FLOAT,             sizeof(yarp::sig::PixelRgbFloat)},
    {VOCAB_PIXEL_HSV_FLOAT,             sizeof(yarp::sig::PixelHsvFloat)},
    {VOCAB_PIXEL_ENCODING_BAYER_GRBG8,  1 },
    {VOCAB_PIXEL_ENCODING_BAYER_GRBG16, 2 },
    {VOCAB_PIXEL_ENCODING_BAYER_BGGR8,  1 },
    {VOCAB_PIXEL_ENCODING_BAYER_BGGR16, 2 },
    {VOCAB_PIXEL_ENCODING_BAYER_GBRG8,  1 },
    {VOCAB_PIXEL_ENCODING_BAYER_GBRG16, 2 },
    {VOCAB_PIXEL_ENCODING_BAYER_RGGB8,  1 },
    {VOCAB_PIXEL_ENCODING_BAYER_RGGB16, 2 },
    {VOCAB_PIXEL_YUV_420, 1},
    {VOCAB_PIXEL_YUV_444, 1},
    {VOCAB_PIXEL_YUV_422, 1},
    {VOCAB_PIXEL_YUV_411, 1}
};

Image::Image() {
    initialize();
}

void Image::initialize() {
    implementation = nullptr;
    data = nullptr;
    imgWidth = imgHeight = 0;
    imgPixelSize = imgRowSize = 0;
    imgPixelCode = 0;
    imgQuantum = 0;
    topIsLow = true;
    implementation = new ImageStorage(*this);
    yAssert(implementation!=nullptr);
}


Image::~Image() {
    if (implementation!=nullptr) {
        delete (ImageStorage*)implementation;
        implementation = nullptr;
    }
}


int Image::getPixelSize() const {
    return imgPixelSize;
}


int Image::getPixelCode() const {
    return imgPixelCode;
}


void Image::zero() {
    if (getRawImage()!=nullptr) {
        memset(getRawImage(),0,getRawImageSize());
    }
}


void Image::resize(int imgWidth, int imgHeight) {
    yAssert(imgWidth>=0 && imgHeight>=0);

    int code = getPixelCode();
    bool change = false;
    if (code!=imgPixelCode) {
        setPixelCode(code);
        change = true;
    }
    if (imgPixelCode!=((ImageStorage*)implementation)->extern_type_id) {
        change = true;
    }
    if (imgQuantum!=((ImageStorage*)implementation)->extern_type_quantum) {
        change = true;
    }

    if (imgWidth!=width()||imgHeight!=height()) {
        change = true;
    }

    if (change) {
        ((ImageStorage*)implementation)->resize(imgWidth,imgHeight,
                                                imgPixelCode,
                                                imgQuantum,
                                                topIsLow);
        synchronize();
        //printf("CHANGE! %ld\n", (long int)(this));
    }
}

void Image::setPixelSize(int imgPixelSize) {
    if(imgPixelSize == (int)pixelCode2Size.at((YarpVocabPixelTypesEnum)imgPixelCode))
        return;

    setPixelCode(-imgPixelSize);
    return;
}

void Image::setPixelCode(int imgPixelCode) {
    this->imgPixelCode = imgPixelCode;

    if(imgPixelCode < 0)
    {
        imgPixelSize = -imgPixelCode;
    }

    imgPixelSize = pixelCode2Size.at((YarpVocabPixelTypesEnum)imgPixelCode);
}


void Image::setQuantum(int imgQuantum) {
    this->imgQuantum = imgQuantum;
}


void Image::synchronize() {
    ImageStorage *impl = (ImageStorage*)implementation;
    yAssert(impl!=nullptr);
    if (impl->pImage!=nullptr) {
        imgWidth = impl->pImage->width;
        imgHeight = impl->pImage->height;
        data = impl->Data;
        imgQuantum = impl->quantum;
        imgRowSize = impl->pImage->widthStep;
    } else {
        data = nullptr;
        imgWidth = imgHeight = 0;
    }
}


unsigned char *Image::getRawImage() const {
    ImageStorage *impl = (ImageStorage*)implementation;
    yAssert(impl!=nullptr);
    if (impl->pImage!=nullptr) {
        return (unsigned char *)impl->pImage->imageData;
    }
    return nullptr;
}

int Image::getRawImageSize() const {
    ImageStorage *impl = (ImageStorage*)implementation;
    yAssert(impl!=nullptr);
    if (impl->pImage!=nullptr) {
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
    yAssert(iplImage!=nullptr);
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
            std::exit(1);
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
            std::exit(1);
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

    setPixelCode(header.id);

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
    if (getPixelCode() == header.id && q == header.quantum && imgPixelSize == header.depth)
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
        yAssert(mem!=nullptr);

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

    int myCode = getPixelCode();
    if (myCode==0) {
        setPixelCode(alt.getPixelCode());
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

    return true;
}


void Image::setExternal(const void *data, int imgWidth, int imgHeight) {
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
