/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/sig/PointCloudBase.h>
#include <yarp/os/Type.h>

using namespace yarp::sig;

namespace {
YARP_LOG_COMPONENT(POINTCLOUDBASE, "yarp.sig.PointCloudBase")
}

// Map that contains the offset of the basic types respect the origin of the struct
// representing the composite types.
const std::map<std::pair<int, int>, size_t> offsetMap = {
    // PCL_NORMAL
    { std::make_pair(PCL_NORMAL, PC_CURVATURE_DATA), sizeof(yarp::sig::DataNormalNoCurvature) },

    // PCL_POINT_XYZ_RGBA
    { std::make_pair(PCL_POINT_XYZ_RGBA, PC_RGBA_DATA), sizeof(yarp::sig::DataXYZ) },

    // PCL_POINT_XYZ_I
    { std::make_pair(PCL_POINT_XYZ_I, PC_INTENSITY_DATA), sizeof(yarp::sig::DataXYZ) },

    // PCL_INTEREST_POINT_XYZ
    { std::make_pair(PCL_INTEREST_POINT_XYZ, PC_INTEREST_DATA), sizeof(yarp::sig::DataXYZ) },

    // PCL_POINT_XYZ_NORMAL
    { std::make_pair(PCL_POINT_XYZ_NORMAL, PC_NORMAL_DATA), sizeof(yarp::sig::DataXYZ) },
    { std::make_pair(PCL_POINT_XYZ_NORMAL, PC_CURVATURE_DATA), sizeof(yarp::sig::DataXYZ) + sizeof(yarp::sig::DataNormalNoCurvature) },

    // PCL_XYZ_NORMAL_RGBA
    { std::make_pair(PCL_POINT_XYZ_NORMAL_RGBA, PC_NORMAL_DATA), sizeof(yarp::sig::DataXYZ) },
    { std::make_pair(PCL_POINT_XYZ_NORMAL_RGBA, PC_RGBA_DATA), sizeof(yarp::sig::DataXYZ) + sizeof(yarp::sig::DataNormalNoCurvature) },
    { std::make_pair(PCL_POINT_XYZ_NORMAL_RGBA, PC_CURVATURE_DATA), sizeof(yarp::sig::DataXYZ) + sizeof(yarp::sig::DataNormalNoCurvature) + sizeof(yarp::sig::DataRGBA) },

    // PCL_XYZ_I_NORMAL TBD
};
// Map that contains the information about the basic types that form
// the composite ones and in which order
const std::map<int, std::vector<int> > compositionMap = {
    // recipe for basic data
    { PC_XY_DATA,                std::vector<int> {PC_XY_DATA} },
    { PC_XYZ_DATA,               std::vector<int> {PC_XYZ_DATA} },
    { PC_RGBA_DATA,              std::vector<int> {PC_RGBA_DATA} },
    { PC_INTENSITY_DATA,         std::vector<int> {PC_INTENSITY_DATA} },
    { PC_INTEREST_DATA,          std::vector<int> {PC_INTEREST_DATA} },
    { PCL_NORMAL,                std::vector<int> {PC_NORMAL_DATA, PC_CURVATURE_DATA, PC_PADDING3} },
    { PC_NORMAL_DATA,            std::vector<int> {PC_NORMAL_DATA} },
    { PC_CURVATURE_DATA,         std::vector<int> {PC_CURVATURE_DATA} },
    { PC_RANGE_DATA,             std::vector<int> {PC_RANGE_DATA} },
    { PC_VIEWPOINT_DATA,         std::vector<int> {PC_VIEWPOINT_DATA} },
    // PCL_POINT_XYZ_RGBA
    { PCL_POINT_XYZ_RGBA,        std::vector<int> {PC_XYZ_DATA, PC_RGBA_DATA, PC_PADDING3} },
    // PCL_POINT_XYZ_I
    { PCL_POINT_XYZ_I,           std::vector<int> {PC_XYZ_DATA, PC_INTENSITY_DATA} },
    // PCL_INTEREST_POINT_XYZ
    { PCL_INTEREST_POINT_XYZ,    std::vector<int> {PC_XYZ_DATA, PC_INTEREST_DATA} },
    // PCL_POINT_XYZ_NORMAL
    { PCL_POINT_XYZ_NORMAL,      std::vector<int> {PC_XYZ_DATA, PC_NORMAL_DATA, PC_CURVATURE_DATA, PC_PADDING3} },
    // PCL_POINT_XYZ_NORMAL_RGBA
    { PCL_POINT_XYZ_NORMAL_RGBA, std::vector<int> {PC_XYZ_DATA, PC_NORMAL_DATA, PC_RGBA_DATA, PC_CURVATURE_DATA, PC_PADDING2} }
};

// Map that contains the size of the struct given the enum representing the type
const std::map<int, size_t> sizeMap = {
    { PC_PADDING3,             3*sizeof(float) },
    { PC_PADDING2,             2*sizeof(float) },
    { PC_XY_DATA,                sizeof(yarp::sig::DataXY) },
    { PC_XYZ_DATA,               sizeof(yarp::sig::DataXYZ) },
    { PC_RGBA_DATA,              sizeof(yarp::sig::DataRGBA) },
    { PC_INTENSITY_DATA,         sizeof(float) },
    { PC_INTEREST_DATA,          sizeof(float) },
    { PC_NORMAL_DATA,            sizeof(yarp::sig::DataNormalNoCurvature) },
    { PCL_NORMAL,                sizeof(yarp::sig::DataNormal) },
    { PC_CURVATURE_DATA,         sizeof(yarp::sig::DataCurvature) },
    { PC_RANGE_DATA,             sizeof(yarp::sig::Range) },
    { PC_VIEWPOINT_DATA,         sizeof(yarp::sig::DataViewpoint) },
    { PCL_POINT_XYZ_RGBA,        sizeof(yarp::sig::DataXYZRGBA) },
    { PCL_POINT_XYZ_I,           sizeof(yarp::sig::DataXYZI) },
    { PCL_INTEREST_POINT_XYZ,    sizeof(yarp::sig::DataInterestPointXYZ) },
    { PCL_POINT_XYZ_NORMAL,      sizeof(yarp::sig::DataXYZNormal) },
    { PCL_POINT_XYZ_NORMAL_RGBA, sizeof(yarp::sig::DataXYZNormalRGBA) }
};


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

yarp::os::Type PointCloudBase::getType() const
{
    return yarp::os::Type::byName("yarp/pointCloud");
}

bool PointCloudBase::isOrganized() const
{
    return height() > 1;
}

void PointCloudBase::copyFromRawData(const char* dst, const char* source, std::vector<int>& recipe)
{
    char* tmpSrc = const_cast<char*>(source);
    char* tmpDst = const_cast<char*>(dst);
    if (recipe.empty()) {
        return;
    }
    yCAssert(POINTCLOUDBASE, tmpSrc && tmpDst);

    size_t sizeDst = pointType2Size(getPointType());
    const size_t numPts = height() * width();
    for (size_t i = 0; i < numPts; i++) {
        for (int j : recipe) {
            size_t sizeToRead = pointType2Size(j);
            if ((header.pointType & j)) {
                size_t offset = getOffset(header.pointType, j);
                std::memcpy(tmpDst + i * sizeDst + offset, tmpSrc, sizeToRead);
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
    if (it != compositionMap.end()) {
        ret = it->second;
    }
    return ret;
}


size_t PointCloudBase::pointType2Size(int type) const
{
    size_t size = 0;

    auto it = sizeMap.find(type);
    if (it != sizeMap.end()) {
        size = it->second;
    }

    return size;
}

size_t PointCloudBase::getOffset(int type_composite, int type_basic) const
{
    size_t offset = 0;
    auto it = offsetMap.find(std::make_pair(type_composite, type_basic));
    if (it != offsetMap.end()) {
        offset = it->second;
    }
    return offset;
}
