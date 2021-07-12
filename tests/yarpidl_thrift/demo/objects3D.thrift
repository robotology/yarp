/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

struct RotationMatrix
{
  1: list<double> content;
}
(
  yarp.name = "yarp::sig::Matrix"
  yarp.includefile="yarp/sig/Matrix.h"
)

struct PointXYZ
{
1: double x;
2: double y;
3: double z;
}

struct Polygonx
{
1: list<i32> vertices;
}

struct RGBA{
1: i32 rgba
}

struct SurfaceMesh
{
1: string meshName
2: list<PointXYZ> points;
3: optional list<RGBA> rgbColour;
4: optional list<Polygonx> polygons;
}

struct Box3D
{
1: list<PointXYZ> corners;
2: RotationMatrix orientation;
}

struct SurfaceMeshWithBoundingBox
{
1:SurfaceMesh mesh;
2:Box3D boundingBox;
}
