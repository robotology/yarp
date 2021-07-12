/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
 */

/*
  This file is in a pretty hacky state.  Sorry!

*/

#include <yarp/sig/Image.h>

#include <yarp/os/Bottle.h>
#include <yarp/os/ConnectionReader.h>
#include <yarp/os/ConnectionWriter.h>
#include <yarp/os/Log.h>
#include <yarp/os/Time.h>
#include <yarp/os/Vocab.h>

#include <yarp/sig/ImageNetworkHeader.h>
#include <yarp/sig/impl/IplImage.h>
#include <yarp/sig/impl/DeBayer.h>

#include <cstdio>
#include <cstring>
#include <string>


using namespace yarp::sig;
using namespace yarp::os;

#define DBGPF1 if (0)

//inline int PAD_BYTES (int len, int pad)
//{
//    const int rem = len % pad;
//    return (rem != 0) ? (pad - rem) : rem;
//}

/**
* This helper function groups code to avoid duplication. It is not a member function of Image because
* there are problems with ImageNetworkHeader, anyhow the function is state-less and uses only parameters.
*/
inline bool readFromConnection(Image &dest, ImageNetworkHeader &header, ConnectionReader& connection)
{
    dest.resize(header.width, header.height);
    unsigned char *mem = dest.getRawImage();
    size_t allocatedBytes = dest.getRawImageSize();
    yAssert(mem != nullptr);
    //this check is redundant with assertion, I would remove it
    if (dest.getRawImageSize() != static_cast<size_t>(header.imgSize)) {
        printf("There is a problem reading an image\n");
        printf("incoming: width %zu, height %zu, code %zu, quantum %zu, topIsLow %zu, size %zu\n",
               static_cast<size_t>(header.width),
               static_cast<size_t>(header.height),
               static_cast<size_t>(header.id),
               static_cast<size_t>(header.quantum),
               static_cast<size_t>(header.topIsLow),
               static_cast<size_t>(header.imgSize));
        printf("my space: width %zu, height %zu, code %d, quantum %zu, size %zu\n",
            dest.width(), dest.height(), dest.getPixelCode(), dest.getQuantum(), allocatedBytes);
    }
    yAssert(allocatedBytes == (size_t) header.imgSize);
    bool ok = connection.expectBlock(reinterpret_cast<char*>(mem), allocatedBytes);
    return (!connection.isError() && ok);
}



class ImageStorage {
public:
    IplImage* pImage;
    char **Data;  // this is not IPL. it's char to maintain IPL compatibility
    int extern_type_id;
    size_t extern_type_quantum;
    size_t quantum;
    bool topIsLow;
    int type_id;

protected:
    Image& owner;

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
    bool _set_ipl_header(size_t x, size_t y, int pixel_type, size_t quantum,
                         bool topIsLow);
    void _free_ipl_header();
    void _alloc_complete(size_t x, size_t y, int pixel_type, size_t quantum,
                         bool topIsLow);
    void _free_complete();


    // computes the # of padding bytes. These are always at the end of the row.
    int _pad_bytes (size_t linesize, size_t align) const;

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

    void resize(size_t x, size_t y, int pixel_type,
                size_t quantum, bool topIsLow);

    void _alloc_complete_extern(const void *buf, size_t x, size_t y, int pixel_type,
                                size_t quantum, bool topIsLow);
};


void ImageStorage::resize(size_t x, size_t y, int pixel_type,
                          size_t quantum, bool topIsLow) {
    int need_recreation = 1;

    if (quantum==0) {
        quantum = YARP_IMAGE_ALIGN;
    }

    if (need_recreation) {
        _free_complete();
        DBGPF1 printf("HIT recreation for %p %p: %zu %zu %d\n", static_cast<void*>(this), static_cast<void*>(pImage), x, y, pixel_type);
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
        (type_id == VOCAB_PIXEL_HSV_FLOAT)) {
        iplAllocateImageFP(pImage, 0, 0);
    } else {
        iplAllocateImage (pImage, 0, 0);
    }

    iplSetBorderMode (pImage, IPL_BORDER_CONSTANT, IPL_SIDE_ALL, 0);
}

// installs an external buffer as the image data
void ImageStorage::_alloc_extern (const void *buf)
{
    yAssert(pImage != nullptr);
    yAssert(Data==nullptr);

    if (pImage != nullptr) {
        if (pImage->imageData != nullptr) {
            iplDeallocateImage (pImage);
        }
    }

    //iplAllocateImage (pImage, 0, 0);
    pImage->imageData = const_cast<char*>(reinterpret_cast<const char*>(buf));
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
    if (pImage != nullptr) {
        if (pImage->imageData != nullptr) {
            if (is_owner) {
                iplDeallocateImage (pImage);
                delete[] Data;
            } else {
                delete[] Data;
            }

            is_owner = 1;
            Data = nullptr;
            pImage->imageData = nullptr;
        }
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


void ImageStorage::_alloc_complete(size_t x, size_t y, int pixel_type, size_t quantum,
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
    {VOCAB_PIXEL_YUV_420,               iplPixelTypeMono}, // {1, 12,  "GRAY", "GRAY"}},
    {VOCAB_PIXEL_YUV_444,               iplPixelTypeMono}, // {1, 24,  "GRAY", "GRAY"}},
    {VOCAB_PIXEL_YUV_422,               iplPixelTypeMono16},
    {VOCAB_PIXEL_YUV_411,               iplPixelTypeMono}, // {1, 12,  "GRAY", "GRAY"}},
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

bool ImageStorage::_set_ipl_header(size_t x, size_t y, int pixel_type, size_t quantum,
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

void ImageStorage::_alloc_complete_extern(const void *buf, size_t x, size_t y, int pixel_type, size_t quantum, bool topIsLow)
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



int ImageStorage::_pad_bytes (size_t linesize, size_t align) const
{
    return yarp::sig::PAD_BYTES (linesize, align);
}

const std::map<YarpVocabPixelTypesEnum, size_t> Image::pixelCode2Size = {
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
        delete static_cast<ImageStorage*>(implementation);
        implementation = nullptr;
    }
}


size_t Image::getPixelSize() const {
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


void Image::resize(size_t imgWidth, size_t imgHeight) {

    int code = getPixelCode();
    bool change = false;
    if (code!=imgPixelCode) {
        setPixelCode(code);
        change = true;
    }
    if (imgPixelCode!=(static_cast<ImageStorage*>(implementation))->extern_type_id) {
        change = true;
    }
    if (imgQuantum!=(static_cast<ImageStorage*>(implementation))->extern_type_quantum) {
        change = true;
    }

    if (imgWidth!=width()||imgHeight!=height()) {
        change = true;
    }

    if (change) {
        (static_cast<ImageStorage*>(implementation))->resize(imgWidth,
                                                             imgHeight,
                                                             imgPixelCode,
                                                             imgQuantum,
                                                             topIsLow);
        synchronize();
        //printf("CHANGE! %ld\n", (long int)(this));
    }
}

void Image::setPixelSize(size_t imgPixelSize) {
    if(imgPixelSize == pixelCode2Size.at(static_cast<YarpVocabPixelTypesEnum>(imgPixelCode))) {
        return;
    }

    setPixelCode(-imgPixelSize);
}

void Image::setPixelCode(int imgPixelCode) {
    this->imgPixelCode = imgPixelCode;
    this->imgPixelSize = (imgPixelCode < 0) ? -imgPixelCode : pixelCode2Size.at(static_cast<YarpVocabPixelTypesEnum>(imgPixelCode));

    if (implementation) {
        auto* impl = static_cast<ImageStorage*>(implementation);
        impl->type_id = imgPixelCode;
    }
}


void Image::setQuantum(size_t imgQuantum) {
    this->imgQuantum = imgQuantum;

    if (implementation) {
        auto* impl = static_cast<ImageStorage*>(implementation);
        impl->quantum = imgQuantum;
    }
}


void Image::setTopIsLowIndex(bool flag) {
    topIsLow = flag;

    if (implementation) {
        auto* impl = static_cast<ImageStorage*>(implementation);
        if (impl->pImage) {
            impl->pImage->origin = topIsLow ? IPL_ORIGIN_TL : IPL_ORIGIN_BL;
        }
    }
}

void Image::synchronize() {
    auto* impl = static_cast<ImageStorage*>(implementation);
    yAssert(impl!=nullptr);
    if (impl->pImage!=nullptr) {
        imgWidth = impl->pImage->width;
        imgHeight = impl->pImage->height;
        data = impl->Data;
        imgQuantum = impl->quantum;
        imgRowSize = impl->pImage->widthStep;
        setPixelCode(impl->type_id);
        topIsLow = impl->pImage->origin == IPL_ORIGIN_TL;
    } else {
        data = nullptr;
        imgWidth = 0;
        imgHeight = 0;
    }
}


unsigned char *Image::getRawImage() const {
    auto* impl = static_cast<ImageStorage*>(implementation);
    yAssert(impl!=nullptr);
    if (impl->pImage!=nullptr) {
        return reinterpret_cast<unsigned char*>(impl->pImage->imageData);
    }
    return nullptr;
}

size_t Image::getRawImageSize() const {
    auto* impl = static_cast<ImageStorage*>(implementation);
    yAssert(impl!=nullptr);
    if (impl->pImage!=nullptr) {
        return impl->pImage->imageSize;
    }
    return 0;
}

#ifndef YARP_NO_DEPRECATED // Since YARP 3.2.0
void *Image::getIplImage() {
    return (static_cast<ImageStorage*>(implementation))->pImage;
}

const void *Image::getIplImage() const {
    return (static_cast<const ImageStorage*>(implementation))->pImage;
}

void Image::wrapIplImage(void *iplImage) {
    yAssert(iplImage!=nullptr);
    auto* p = static_cast<IplImage *>(iplImage);
    std::string str = p->colorModel;
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
               Vocab32::decode(code).c_str(),
               Vocab32::decode(getPixelCode()).c_str());
        printf("Making a copy instead of just wrapping...\n");
        FlexImage img;
        img.setQuantum(p->align);
        img.setTopIsLowIndex(p->origin == IPL_ORIGIN_TL);
        img.setPixelCode(code);
        img.setExternal(p->imageData, p->width, p->height);
        copy(img);
    } else {
        setQuantum(p->align);
        setTopIsLowIndex(p->origin == IPL_ORIGIN_TL);
        setPixelCode(code);
        setExternal(p->imageData, p->width, p->height);
    }
}

#endif // YARP_NO_DEPRECATED


bool Image::read(yarp::os::ConnectionReader& connection) {

    // auto-convert text mode interaction
    connection.convertTextMode();

    ImageNetworkHeader header;

    bool ok = connection.expectBlock(reinterpret_cast<char*>(&header),sizeof(header));
    if (!ok) {
        return false;
    }

    //first check that the received image size is reasonable
    if (header.width == 0 || header.height == 0)
    {
        // I maintain the previous logic, although we should probably return false
        return !connection.isError();
    }

    setPixelCode(header.id);

    size_t q = getQuantum();
    if (q==0) {
        //q = YARP_IMAGE_ALIGN;
        setQuantum(header.quantum);
        q = getQuantum();
    }
    if (q != static_cast<size_t>(header.quantum)) {
        if ((header.depth*header.width)%header.quantum==0 &&
            (header.depth*header.width)%q==0) {
            header.quantum = q;
        }
    }

    setTopIsLowIndex(header.topIsLow == 0);

    // handle easy case, received and current image are compatible, no conversion needed
    if (getPixelCode() == header.id && q == static_cast<size_t>(header.quantum) && imgPixelSize == static_cast<size_t>(header.depth))
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
        flex.setTopIsLowIndex(header.topIsLow == 0);

        bool ok = readFromConnection(flex, header, connection);
        if (!ok) {
            return false;
        }

        if (getPixelCode() == VOCAB_PIXEL_BGR && header.id==VOCAB_PIXEL_ENCODING_BAYER_GRBG8) {
            return deBayer_GRBG8_TO_BGR(flex, *this, 3);
        }
        if (getPixelCode() == VOCAB_PIXEL_BGRA && header.id == VOCAB_PIXEL_ENCODING_BAYER_GRBG8) {
            return deBayer_GRBG8_TO_BGR(flex, *this, 4);
        }
        if (getPixelCode() == VOCAB_PIXEL_RGB && header.id==VOCAB_PIXEL_ENCODING_BAYER_GRBG8) {
            return deBayer_GRBG8_TO_RGB(flex, *this, 3);
        }
        if (getPixelCode() == VOCAB_PIXEL_RGBA && header.id == VOCAB_PIXEL_ENCODING_BAYER_GRBG8) {
            return deBayer_GRBG8_TO_RGB(flex, *this, 4);
        }

        YARP_FIXME_NOTIMPLEMENTED("Conversion from bayer encoding not yet implemented\n");
        return false;
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
    flex.setTopIsLowIndex(header.topIsLow == 0);
    ok = readFromConnection(flex, header, connection);
    if (ok) {
        copy(flex);
    }

    return ok;
}


bool Image::write(yarp::os::ConnectionWriter& connection) const {
    ImageNetworkHeader header;
    header.setFromImage(*this);
    connection.appendBlock(reinterpret_cast<char*>(&header),sizeof(header));
    unsigned char *mem = getRawImage();
    if (header.width!=0&&header.height!=0) {
        yAssert(mem!=nullptr);

        // Note use of external block.
        // Implies care needed about ownership.
        connection.appendExternalBlock(reinterpret_cast<char *>(mem),header.imgSize);
    }

    // if someone is foolish enough to connect in text mode,
    // let them see something readable.
    connection.convertTextMode();

    return !connection.isError();
}


Image::Image(const Image& alt) : Portable()
{
    initialize();
    copy(alt);
}

Image::Image(Image&& other) noexcept
    : implementation(std::exchange(other.implementation, nullptr))
{
    synchronize();
}

Image& Image::operator=(Image&& other) noexcept
{
    delete static_cast<ImageStorage*>(implementation);
    implementation = std::exchange(other.implementation, nullptr);
    synchronize();
    return *this;
}


Image& Image::operator=(const Image& alt)
{
    if (&alt != this) {
        copy(alt);
    }
    return *this;
}


bool Image::copy(const Image& alt)
{
    if (&alt != this)
    {
        int myCode = getPixelCode();
        if (myCode==0) {
            setPixelCode(alt.getPixelCode());
            setQuantum(alt.getQuantum());
        }
        resize(alt.width(),alt.height());
        setTopIsLowIndex(alt.topIsLowIndex());

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
    }
    return true;
}


bool Image::move(Image&& alt) noexcept
{
    // Cannot move an image of the wrong type inside an ImageOf that does not
    // support it.
    yAssert(dynamic_cast<FlexImage*>(this) || getPixelCode() == alt.getPixelCode() || alt.getPixelCode() == 0);
    if (&alt != this) {
        delete static_cast<ImageStorage*>(implementation);
        implementation = std::exchange(alt.implementation, nullptr);
        synchronize();
    }
    return true;
}


bool Image::swap(Image& alt)
{
    // Cannot swap two ImageOf of different type, or an image of the wrong type
    // inside an ImageOf that does not support it.
    yAssert(dynamic_cast<FlexImage*>(this) || getPixelCode() == alt.getPixelCode() || alt.getPixelCode() == 0);
    yAssert(dynamic_cast<FlexImage*>(&alt) || getPixelCode() == alt.getPixelCode() || getPixelCode() == 0);
    if (&alt != this) {
        std::swap(alt.implementation, implementation);
        synchronize();
        alt.synchronize();
    }
    return true;
}


void Image::setExternal(const void *data, size_t imgWidth, size_t imgHeight) {
    if (imgQuantum==0) {
        imgQuantum = 1;
    }
    (static_cast<ImageStorage*>(implementation))->_alloc_complete_extern(data,
                                                                         imgWidth,
                                                                         imgHeight,
                                                                         getPixelCode(),
                                                                         imgQuantum,
                                                                         topIsLow);
    synchronize();
}


bool Image::copy(const Image& alt, size_t w, size_t h) {
    if (getPixelCode()==0) {
        setPixelCode(alt.getPixelCode());
        setQuantum(alt.getQuantum());
        setTopIsLowIndex(alt.topIsLowIndex());
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
        img.setTopIsLowIndex(topIsLowIndex());
        img.copy(alt);
        return copy(img,w,h);
    }

    resize(w,h);
    size_t d = getPixelSize();

    size_t nw = w;
    size_t nh = h;
    w = alt.width();
    h = alt.height();

    float di = (static_cast<float>(h))/nh;
    float dj = (static_cast<float>(w))/nw;

    for (size_t i=0; i<nh; i++)
        {
            auto i0 = static_cast<size_t>(di*i);
            for (size_t j=0; j<nw; j++)
                {
                    auto j0 = static_cast<size_t>(dj*j);
                    memcpy(getPixelAddress(j,i),
                           alt.getPixelAddress(j0,i0),
                           d);
                }
        }
    return true;
}
