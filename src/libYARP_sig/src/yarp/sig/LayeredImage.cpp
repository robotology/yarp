/*
 * SPDX-FileCopyrightText: 2024-2024 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/sig/Image.h>
#include <yarp/sig/ImageNetworkHeader.h>

#include <yarp/os/Bottle.h>
#include <yarp/os/ConnectionReader.h>
#include <yarp/os/ConnectionWriter.h>
#include <yarp/os/Log.h>
#include <yarp/os/Time.h>
#include <yarp/os/Vocab.h>
#include <yarp/os/LogStream.h>

#include <yarp/sig/LayeredImage.h>
#include <yarp/sig/ImageUtils.h>

#include <cstdio>
#include <cstring>
#include <string>
#include <utility>
#include <math.h>

using namespace yarp::sig;
using namespace yarp::os;

inline void writeToConnection(const Image& img, ConnectionWriter& connection)
{
    ImageNetworkHeader imghdr;

    imghdr.setFromImage(img);
    size_t hdrsize = sizeof(imghdr);
    connection.appendInt32(BOTTLE_TAG_BLOB);
    connection.appendInt32(hdrsize);
    connection.appendBlock((char*)(&imghdr), hdrsize);

    size_t imgsize = img.getRawImageSize();
    connection.appendInt32(BOTTLE_TAG_BLOB);
    connection.appendInt32(imgsize);
    connection.appendBlock((char*)(img.getRawImage()), imgsize);

    return;
}

inline bool readFromConnection(FlexImage& dest, ConnectionReader& connection)
{
    bool ok = true;
    ImageNetworkHeader imghdr;

    connection.expectInt32();
    size_t sizeData = connection.expectInt32();
    ok &= connection.expectBlock((char*)(&imghdr), sizeData);
    if (!ok) { return false; }
    imghdr.setToImage(dest);

    connection.expectInt32();
    size_t sizeImg = connection.expectInt32();
    size_t psizeImg = dest.getRawImageSize();
    if (sizeImg != psizeImg)
    {
        return false;
    }
    unsigned char* pImg = dest.getRawImage();
    ok &= connection.expectBlock((char*)pImg, sizeImg);

    return ok;
}

LayeredImage::LayeredImage()
{
}


LayeredImage::~LayeredImage()
{
}


void LayeredImage::clear()
{
    background.zero();
    layers.clear();
}

bool LayeredImage::read(yarp::os::ConnectionReader& connection)
{
    bool ok = true;

    connection.convertTextMode();

    // LIST OF ELEMENTS
    connection.expectInt32();
    size_t elems = connection.expectInt32();

    //ELEMENT 1
    connection.expectInt32();
    size_t layersNum = connection.expectInt32();
    if (elems != 1 + (1 * 2) + (layersNum*10))
    {
        return false;
    }

    // ELEMENT 2-3
    ok &= readFromConnection(background, connection);

    // ELEMENT 4-...
    // each layer contains 8+2 elems
    layers.clear();
    for (size_t i = 0; i < layersNum; i++)
    {
        yarp::sig::ImageLayer::colorkey_s colorkey = yarp::sig::ImageLayer::colorkey_s {};
        yarp::sig::ImageLayer::alpha_s alpha = yarp::sig::ImageLayer::alpha_s {};

        connection.expectInt32();
        int32_t enable_val = connection.expectInt8(); //1
        connection.expectInt32();
        colorkey.enable = connection.expectInt8(); //2
        connection.expectInt32();
        colorkey.value = connection.expectInt32(); //3
        connection.expectInt32();
        alpha.enable = connection.expectInt8(); //4
        connection.expectInt32();
        alpha.value = connection.expectFloat32(); // 5
        connection.expectInt32();
        bool can_be_compressed = connection.expectInt8(); //6
        connection.expectInt32();
        int32_t offset_x = connection.expectInt32(); //7
        connection.expectInt32();
        int32_t offset_y = connection.expectInt32(); //8

        FlexImage fleximg;
        ok &= readFromConnection(fleximg, connection); //9-10
        yarp::sig::ImageLayer oneLayer(fleximg, enable_val, colorkey, alpha, can_be_compressed, offset_x, offset_y);
        layers.emplace_back(oneLayer);
    }

    return true;
}


bool LayeredImage::write(yarp::os::ConnectionWriter& connection) const
{
    size_t layers_num = layers.size();

    //LIST OF ELEMENTS
    connection.appendInt32(BOTTLE_TAG_LIST);
    connection.appendInt32(1+(1*2)+(layers_num*10));

    //ELEMENT 1
    connection.appendInt32(BOTTLE_TAG_INT32);
    connection.appendInt32(layers_num);

    // ELEMENT 2-3
    writeToConnection(background, connection);

    // ELEMENT 4-...
    // each layer contains 8+2 elems
    for (size_t i = 0; i < layers_num; i++)
    {
        connection.appendInt32(BOTTLE_TAG_INT8); //1
        connection.appendInt8 (layers[i].enable);
        connection.appendInt32(BOTTLE_TAG_INT8); //2
        connection.appendInt8(layers[i].colorkey.enable);
        connection.appendInt32(BOTTLE_TAG_INT32); //3
        connection.appendInt32(layers[i].colorkey.value);
        connection.appendInt32(BOTTLE_TAG_INT8); //4
        connection.appendInt8(layers[i].alpha.enable);
        connection.appendInt32(BOTTLE_TAG_FLOAT32); //5
        connection.appendFloat32(layers[i].alpha.value);
        connection.appendInt32(BOTTLE_TAG_INT8); //6
        connection.appendInt8(layers[i].can_be_compressed);
        connection.appendInt32(BOTTLE_TAG_INT32); //7
        connection.appendInt32(layers[i].offset_x);
        connection.appendInt32(BOTTLE_TAG_INT32); //8
        connection.appendInt32(layers[i].offset_y);
        writeToConnection(layers[i].layer, connection); // 9-10
    }

    connection.convertTextMode();
    return !connection.isError();
}


LayeredImage::LayeredImage(const LayeredImage& alt) :
        Portable()
{
    background = alt.background;
    this->layers = alt.layers;
}

LayeredImage::LayeredImage(LayeredImage&& other) noexcept
{
}

LayeredImage& LayeredImage::operator=(const LayeredImage& alt)
{
    background = alt.background;
    this->layers = alt.layers;
    return *this;
}

bool LayeredImage::operator==(const LayeredImage& alt) const
{
    size_t l1 = this->layers.size();
    size_t l2 = alt.layers.size();

    if (l1 != l2)
    {
        return false;
    }

    if (background != alt.background)
    {
        return false;
    }

    for (size_t i = 0; i < l1; i++)
    {
        if ((this->layers[i].enable != alt.layers[i].enable) ||
            (this->layers[i].colorkey.enable != alt.layers[i].colorkey.enable) ||
            (this->layers[i].colorkey.value != alt.layers[i].colorkey.value) ||
            (this->layers[i].alpha.enable != alt.layers[i].alpha.enable) ||
            (fabs(this->layers[i].alpha.value - alt.layers[i].alpha.value) > 0.001) ||
            (this->layers[i].layer != alt.layers[i].layer) ||
            (this->layers[i].can_be_compressed != alt.layers[i].can_be_compressed) ||
            (this->layers[i].offset_x != alt.layers[i].offset_x) ||
            (this->layers[i].offset_y != alt.layers[i].offset_y))
        {
            return false;
        }
    }

    return true;
}

yarp::sig::FlexImage LayeredImage::convert_to_flexImage()
{
    yarp::sig::FlexImage outimg = background;

    bool ret = true;
    for (size_t i = 0; i < this->layers.size(); i++)
    {
        if (layers[i].enable == false)
        {
            continue;
        }

        ret &= yarp::sig::utils::sum(outimg, layers[i].layer, layers[i].colorkey.enable, layers[i].colorkey.value, layers[i].alpha.enable, layers[i].alpha.value, layers[i].offset_x, layers[i].offset_y);
    }

    return outimg;
}

LayeredImage::operator yarp::sig::FlexImage()
{
    return convert_to_flexImage();
}
