/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#include <yarp/sig/PointCloud.h>

using namespace yarp::sig;


const std::map<std::pair<int, int>, int> offsetMap =  {
        // PCL_NORMAL
        {std::make_pair(PCL_NORMAL, PC_CURVATURE_DATA) , sizeof(yarp::sig::NORMAL_NO_CURV)},

        // PCL_POINT_XYZ_RGBA
        {std::make_pair(PCL_POINT_XYZ_RGBA, PC_RGBA_DATA) , sizeof(yarp::sig::XYZ_DATA)},

        // PCL_POINT_XYZ_I
        {std::make_pair(PCL_POINT_XYZ_I, PC_INTENSITY_DATA) , sizeof(yarp::sig::XYZ_DATA)},

        // PCL_INTEREST_POINT_XYZ
        {std::make_pair(PCL_INTEREST_POINT_XYZ, PC_INTEREST_DATA) , sizeof(yarp::sig::XYZ_DATA)},

        // PCL_POINT_XYZ_NORMAL
        {std::make_pair(PCL_POINT_XYZ_NORMAL, PC_NORMAL_DATA) ,    sizeof(yarp::sig::XYZ_DATA)},
        {std::make_pair(PCL_POINT_XYZ_NORMAL, PC_CURVATURE_DATA) , sizeof(yarp::sig::XYZ_DATA) + sizeof(yarp::sig::NORMAL_NO_CURV)},

       // PCL_XYZ_NORMAL_RGBA
        {std::make_pair(PCL_POINT_XYZ_NORMAL_RGBA, PC_NORMAL_DATA) ,    sizeof(yarp::sig::XYZ_DATA)},
        {std::make_pair(PCL_POINT_XYZ_NORMAL_RGBA, PC_RGBA_DATA) ,      sizeof(yarp::sig::XYZ_DATA) + sizeof(yarp::sig::NORMAL_NO_CURV)},
        {std::make_pair(PCL_POINT_XYZ_NORMAL_RGBA, PC_CURVATURE_DATA) , sizeof(yarp::sig::XYZ_DATA) + sizeof(yarp::sig::NORMAL_NO_CURV) + sizeof(yarp::sig::RGBA_DATA)},

        // PCL_XYZ_I_NORMAL TBD
                                     };
const std::map<int, std::vector<int> > compositionMap = {
        // recipe for basic data
        {PC_XY_DATA,        std::vector<int> {PC_XY_DATA}},
        {PC_XYZ_DATA,       std::vector<int> {PC_XYZ_DATA}},
        {PC_RGBA_DATA,      std::vector<int> {PC_RGBA_DATA}},
        {PC_INTENSITY_DATA, std::vector<int> {PC_INTENSITY_DATA}},
        {PC_INTEREST_DATA,  std::vector<int> {PC_INTEREST_DATA}},
        {PCL_NORMAL,        std::vector<int> {PC_NORMAL_DATA, PC_CURVATURE_DATA, PC_PADDING3}},
        {PC_NORMAL_DATA,    std::vector<int> {PC_NORMAL_DATA}},
        {PC_CURVATURE_DATA, std::vector<int> {PC_CURVATURE_DATA}},
        {PC_RANGE_DATA,     std::vector<int> {PC_RANGE_DATA}},
        {PC_VIEWPOINT_DATA, std::vector<int> {PC_VIEWPOINT_DATA}},
        // PCL_POINT_XYZ_RGBA
        {PCL_POINT_XYZ_RGBA, std::vector<int> {PC_XYZ_DATA, PC_RGBA_DATA, PC_PADDING3}},
        // PCL_POINT_XYZ_I
        {PCL_POINT_XYZ_I, std::vector<int> {PC_XYZ_DATA, PC_INTENSITY_DATA}},
        // PCL_INTEREST_POINT_XYZ
        {PCL_INTEREST_POINT_XYZ, std::vector<int> {PC_XYZ_DATA, PC_INTEREST_DATA}},
        // PCL_POINT_XYZ_NORMAL
        {PCL_POINT_XYZ_NORMAL, std::vector<int> {PC_XYZ_DATA, PC_NORMAL_DATA, PC_CURVATURE_DATA, PC_PADDING3}},
        // PCL_POINT_XYZ_NORMAL_RGBA
        {PCL_POINT_XYZ_NORMAL_RGBA, std::vector<int> {PC_XYZ_DATA, PC_NORMAL_DATA, PC_RGBA_DATA, PC_CURVATURE_DATA, PC_PADDING2}}

    };

    // TODO unify the info if possible
const std::map<int, size_t> sizeMap = {
        {PC_PADDING3,             3*sizeof(float)},
        {PC_PADDING2,             2*sizeof(float)},
        {PC_XY_DATA,                sizeof(yarp::sig::XY_DATA)},
        {PC_XYZ_DATA,               sizeof(yarp::sig::XYZ_DATA)},
        {PC_RGBA_DATA,              sizeof(yarp::sig::RGBA_DATA)},
        {PC_INTENSITY_DATA,         sizeof(yarp::sig::intensity)},
        {PC_INTEREST_DATA,          sizeof(yarp::sig::strength)},
        {PC_NORMAL_DATA,            sizeof(yarp::sig::NORMAL_NO_CURV)},
        {PCL_NORMAL,                sizeof(yarp::sig::NORMAL_DATA)},
        {PC_CURVATURE_DATA,         sizeof(yarp::sig::CURVATURE_DATA)},
        {PC_RANGE_DATA,             sizeof(yarp::sig::range)},
        {PC_VIEWPOINT_DATA,         sizeof(yarp::sig::VIEWPOINT_DATA)},
        {PCL_POINT_XYZ_RGBA,        sizeof(yarp::sig::XYZ_RGBA_DATA)},
        {PCL_POINT_XYZ_I,           sizeof(yarp::sig::XYZ_I_DATA)},
        {PCL_INTEREST_POINT_XYZ,    sizeof(yarp::sig::INTEREST_POINT_XYZ_DATA)},
        {PCL_POINT_XYZ_NORMAL,      sizeof(yarp::sig::XYZ_NORMAL_DATA)},
        {PCL_POINT_XYZ_NORMAL_RGBA, sizeof(yarp::sig::XYZ_NORMAL_RGBA_DATA)}
    };


void PointCloudBase::resize(size_t width, size_t height)
{
    header.width = width;
    header.height = height;
}

size_t PointCloudBase::height() const
{
    return header.height;
}

size_t PointCloudBase::width() const
{
    return header.width;
}

int PointCloudBase::getPointType() const
{
    return header.pointType;

}

yarp::os::Type PointCloudBase::getType()
{
    return yarp::os::Type::byName("yarp/pointCloud");
}

void PointCloudBase::fromExternalPC(const char* source, int type, size_t width, size_t height, bool isDense)
{
    yAssert(source);
    header.isDense = isDense;
    resize(width, height);
    if (this->getPointType() == type)
    {
        memcpy(const_cast<char*> (getRawData()), source, dataSizeBytes());
    }
    else
    {
        std::vector<int> recipe = getComposition(type);
        copyFromRawData(getRawData(), source, recipe);
    }
}

bool PointCloudBase::isOrganized()
{
    return height() > 1;
}

PointCloudBase::PointCloudBase()
{}

void PointCloudBase::copyFromRawData(const char* dst, const char* source, std::vector<int> &recipe)
{
    char* tmpSrc = const_cast<char*> (source);
    char* tmpDst = const_cast<char*> (dst);
    if (recipe.empty())
    {
        return;
    }
    yAssert(tmpSrc && tmpDst);

    size_t sizeDst = pointType2Size(getPointType());
    const size_t numPts  = height()*width();
    for (uint i=0; i < numPts; i++)
    {
        for (size_t j = 0; j < recipe.size(); j++)
        {
            size_t sizeToRead = pointType2Size(recipe[j]);
            if ((header.pointType & recipe[j]))
            {
                size_t offset = getOffset(header.pointType, recipe[j]);
                std::memcpy(tmpDst + i*sizeDst + offset, tmpSrc, sizeToRead);
            }

            // increment anyways, if the field is missing in the destination, simply skip it
            tmpSrc += sizeToRead;
        }
    }
}



std::vector<int> PointCloudBase::getComposition(int type_composite) const
{
    //todo probably
    std::vector<int> ret;
    auto it = compositionMap.find(type_composite);
    if (it != compositionMap.end())
    {
        ret = it->second;
    }
    return ret;
}


size_t PointCloudBase::pointType2Size(int type) const
{
    size_t size = 0;

    auto it = sizeMap.find(type);
    if (it != sizeMap.end())
    {
        size = it->second;
    }

    return size;
}

size_t PointCloudBase::getOffset(int type_composite, int type_basic) const
{
    size_t offset = 0;
    auto it = offsetMap.find(std::make_pair(type_composite, type_basic));
    if (it != offsetMap.end())
    {
        offset = it->second;
    }
    return offset;

}

