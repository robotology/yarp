/*
 * Copyright (C) 2016 The RobotCub Consortium
 * Author: Marco Randazzo.
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */

#include <yarp/sig/MapGrid2D.h>
#include <yarp/os/Bottle.h>
#include <yarp/sig/ImageFile.h>
#include <yarp/os/LogStream.h>
#include <algorithm>
#include <fstream>

using namespace yarp::sig;
using namespace yarp::os;
using namespace std;

bool MapGrid2D::isIdenticalTo(const MapGrid2D& other)
{
    if (m_map_name != other.m_map_name) return false;
    if (m_origin.x != other.m_origin.x) return false;
    if (m_origin.y != other.m_origin.y) return false;
    if (m_origin.theta != other.m_origin.theta) return false;
    if (m_resolution != other.m_resolution) return false;
    if (width() != other.width()) return false;
    if (height() != other.height()) return false;
    for (size_t y = 0; y < height(); y++) for (size_t x = 0; x < width(); x++)
        if (m_map_occupancy.safePixel(x, y) != other.m_map_occupancy.safePixel(x, y)) return false;
    for (size_t y = 0; y < height(); y++) for (size_t x = 0; x < width(); x++)
        if (m_map_flags.safePixel(x, y) != other.m_map_flags.safePixel(x, y)) return false;
    return true;
}

MapGrid2D::MapGrid2D()
{
    m_origin.x = 0;
    m_origin.y = 0;
    m_origin.theta = 0;
    m_resolution = 1.0; //each pixel corresponds to 1 m
    m_map_occupancy.resize(2, 2);
    m_map_flags.resize(2, 2);
}

MapGrid2D::~MapGrid2D()
{

}

bool MapGrid2D::isFree(int x, int y)
{
    if (x < 0 || x > m_map_occupancy.width() ||
        y < 0 || y < m_map_occupancy.height())
    {
        if (m_map_occupancy.safePixel(x, y) == 0 &&
            m_map_flags.safePixel(x, y) == 0)
            return true;
    }
    return false;
}

bool MapGrid2D::isObstacle(int x, int y)
{
    if (x < 0 || x > m_map_occupancy.width() ||
        y < 0 || y < m_map_occupancy.height())
    {
        if (m_map_flags.safePixel(x, y) == 0)
            return true;
    }
    return false;
}

bool MapGrid2D::isKeepOut(int x, int y)
{
    if (x < 0 || x > m_map_occupancy.width() ||
        y < 0 || y < m_map_occupancy.height())
    {
        if (m_map_flags.safePixel(x, y) == 0)
            return true;
    }
    return false;
}

bool MapGrid2D::isWall(int x, int y)
{
    if (x < 0 || x > m_map_occupancy.width() ||
        y < 0 || y < m_map_occupancy.height())
    {
        if (m_map_occupancy.safePixel(x,y)==0)
        return true;
    }
    return false;
}

size_t MapGrid2D::height() const
{
    return m_map_occupancy.height();
}

size_t MapGrid2D::width() const
{
    return m_map_occupancy.width();
}

class vec2d
{
public:
    size_t x;
    size_t y;
    vec2d()  { x = 0; y = 0; }
    vec2d(size_t xx, size_t yy) { x = xx; y = yy; }
};

bool MapGrid2D::getMapImage(yarp::sig::ImageOf<PixelRgb>& image)
{
    image.resize(m_map_occupancy.width(),m_map_occupancy.height());
    m_map_occupancy.zero();
    for (size_t y = 0; y < m_map_occupancy.height(); y++)
        for (size_t x = 0; x < m_map_occupancy.width(); x++)
        {
            PixelRgb pix;
            if (m_map_occupancy.safePixel(x, y) == 0)
            {
                pix.r = 0; pix.g = 0; pix.b = 0;
            }
            image.safePixel(x, y) = pix;
        }
    return true;
}

bool MapGrid2D::enlargeObstacles(unsigned int size)
{
    if (size == 0)
    {
        for (int y = 0; y < this->m_map_occupancy.height(); y++)
        {
            for (int x = 0; x < this->m_map_occupancy.width(); x++)
            {
                if (this->m_map_flags.safePixel(x, y) == 100) this->m_map_flags.safePixel(x, y) = 0;
            }
        }
        return true;
    }

    for (size_t repeat = 0; repeat < size; repeat++)
    {
        //contains the cells to be enlarged;
        std::vector<vec2d> list_of_cells;
        for (int y = 0; y < this->m_map_occupancy.height(); y++)
        {
            for (int x = 0; x < this->m_map_occupancy.width(); x++)
            {
                if (this->m_map_occupancy.safePixel(x, y) == 100 ||
                    this->m_map_flags.safePixel(x, y) == 100)
                {
                    list_of_cells.push_back(vec2d(x, y));
                }
            }
        }

        //process each cell of the list and enlarges it
        for (auto it = list_of_cells.begin(); it != list_of_cells.end(); it++)
        {
            enlargeCell(it->x, it->y);
        }
    }
    return true;
}

void MapGrid2D::enlargeCell(size_t i, size_t j)
{
    int il = i - 1>0 ? i - 1 : 0;
    int ir = i + 1<m_map_flags.width() - 1 ? i + 1 : m_map_flags.width() - 1;
    int ju = j - 1>0 ? j - 1 : 0;
    int jd = j + 1<m_map_flags.height() - 1 ? j + 1 : m_map_flags.height() - 1;

    if (m_map_flags.pixel(il, j) == 0) m_map_flags.pixel(il, j) = 0;
    if (m_map_flags.pixel(ir, j) == 0) m_map_flags.pixel(ir, j) = 0;
    if (m_map_flags.pixel(i, ju) == 0) m_map_flags.pixel(i, ju) = 0;
    if (m_map_flags.pixel(i, jd) == 0) m_map_flags.pixel(i, jd) = 0;
    if (m_map_flags.pixel(il, ju) == 0) m_map_flags.pixel(il, ju) = 0;
    if (m_map_flags.pixel(il, jd) == 0) m_map_flags.pixel(il, jd) = 0;
    if (m_map_flags.pixel(ir, ju) == 0) m_map_flags.pixel(ir, ju) = 0;
    if (m_map_flags.pixel(ir, jd) == 0) m_map_flags.pixel(ir, jd) = 0;
}

bool  MapGrid2D::loadFromFile(std::string yaml_filename)
{
    std::string file_string;
    std::ifstream file;
    file.open(yaml_filename.c_str());
    if (!file.is_open())
    {
        yError() << "failed to open file" << yaml_filename;
        return false;
    }

    string line;
    while (getline(file, line))
    {
        if (line.find("origin") != std::string::npos)
        {
            std::replace(line.begin(), line.end(), ',', ' ');
            std::replace(line.begin(), line.end(), '[', '(');
            std::replace(line.begin(), line.end(), ']', ')');
            /*
            auto it = line.find('[');
            if (it != string::npos) line.replace(it, 1, "(");
            it = line.find(']');
            if(it != string::npos) line.replace(it, 1, ")");*/
        }
        file_string += (line + '\n');
    }
    file.close();

    bool ret = true;
    Bottle bbb;
    bbb.fromString(file_string);
    string debug_s = bbb.toString();
    string pgm_filename;
    
    if (bbb.check("image:") == false) { yError() << "missing image"; ret = false; }
    pgm_filename = bbb.find("image:").asString();

    if (bbb.check("resolution:") == false) { yError() << "missing resolution"; ret = false; }
    m_resolution = bbb.find("resolution:").asDouble();

    if (bbb.check("name:") == false) { yError() << "missing name"; ret = false; }
    m_map_name = bbb.find("name:").asString();

    if (bbb.check("origin:") == false) { yError() << "missing origin"; ret = false; }
    Bottle* b = bbb.find("origin:").asList();
    if (b)
    {
        m_origin.x = b->get(0).asDouble();
        m_origin.y = b->get(1).asDouble();
        m_origin.theta = b->get(2).asDouble();
    }

    yarp::sig::ImageOf<yarp::sig::PixelRgb> img;
    ret = yarp::sig::file::read(img, pgm_filename.c_str());
    return ret;

}

bool  MapGrid2D::saveToFile(std::string yaml_filename)
{
    std::string pgm_filename;
    std::ofstream file;
    file.open(yaml_filename.c_str());
    if (!file.is_open())
    {
        return false;
    }
    file << "image: " << pgm_filename << endl;
    file << "resolution: " << m_resolution << endl;
    file << "origin: [ " << m_origin.x << " " << m_origin.y << "" << m_origin.theta << " ]"<< endl;
    file << "name:" << m_map_name << endl;
    file.close();

    yarp::sig::ImageOf<yarp::sig::PixelRgb> img;
    bool ret = yarp::sig::file::write(img, pgm_filename.c_str());
    return ret;
}

bool MapGrid2D::read(yarp::os::ConnectionReader& connection)
{
    // auto-convert text mode interaction
    connection.convertTextMode();

    connection.expectInt();
    connection.expectInt();

    connection.expectInt();
    size_t w = connection.expectInt();
    connection.expectInt();
    size_t h = connection.expectInt();
    connection.expectInt();
    m_origin.x = connection.expectDouble();
    connection.expectInt();
    m_origin.y = connection.expectDouble(); 
    connection.expectInt();
    m_origin.theta = connection.expectDouble(); 
    connection.expectInt();
    int siz = connection.expectInt();
    char buff[255]; memset(buff, 0, 255);
    connection.expectBlock((char*)buff, siz);
    m_map_name = buff;
    m_map_occupancy.resize(w,h);
    m_map_flags.resize(w,h);
    bool ok = true;
    unsigned char *mem = 0;
    int            memsize = 0;
    connection.expectInt();
    memsize = connection.expectInt();
    if (memsize != m_map_occupancy.getRawImageSize()) { return false; }
    mem = m_map_occupancy.getRawImage();
    ok &= connection.expectBlock((char*)mem, memsize);
    connection.expectInt();
    memsize = connection.expectInt();
    if (memsize != m_map_flags.getRawImageSize()) { return false; }
    mem = m_map_flags.getRawImage();
    ok &= connection.expectBlock((char*)mem, memsize);
    if (!ok) return false;

    return !connection.isError(); 
        return true;
}

bool MapGrid2D::write(yarp::os::ConnectionWriter& connection)
{
    connection.appendInt(BOTTLE_TAG_LIST);
    connection.appendInt(8);
    connection.appendInt(BOTTLE_TAG_INT);
    connection.appendInt(m_map_occupancy.width());
    connection.appendInt(BOTTLE_TAG_INT);
    connection.appendInt(m_map_occupancy.height());
    connection.appendInt(BOTTLE_TAG_DOUBLE);
    connection.appendDouble(m_origin.x);
    connection.appendInt(BOTTLE_TAG_DOUBLE);
    connection.appendDouble(m_origin.y);
    connection.appendInt(BOTTLE_TAG_DOUBLE);
    connection.appendDouble(m_origin.theta);
    connection.appendInt(BOTTLE_TAG_STRING);
    connection.appendRawString(m_map_name.c_str());

    unsigned char *mem = 0;
    int            memsize = 0;
    mem     = m_map_occupancy.getRawImage();
    memsize = m_map_occupancy.getRawImageSize();
    connection.appendInt(BOTTLE_TAG_BLOB);
    connection.appendInt(memsize);
    connection.appendExternalBlock((char*)mem, memsize);
    mem     = m_map_flags.getRawImage();
    memsize = m_map_flags.getRawImageSize();
    connection.appendInt(BOTTLE_TAG_BLOB);
    connection.appendInt(memsize);
    connection.appendExternalBlock((char*)mem, memsize);
   
    connection.convertTextMode();
    return !connection.isError();
}

yarp::sig::Vector MapGrid2D::cell2World(yarp::sig::VectorOf<int> cell)
{
    int crop_x = 0;
    int crop_y = 0;
    cell[0] += crop_x;
    cell[1] += crop_y;
    yarp::sig::Vector v(2);
    v[0] = double(cell[0])*this->m_resolution;
    v[1] = double(cell[1])*this->m_resolution;
    v[0] = v[0] + this->m_origin.x;
    v[1] = -(v[1] + this->m_origin.y);
    return v;
}

yarp::sig::VectorOf<int> MapGrid2D::world2Cell(yarp::sig::Vector world)
{
    int crop_x = 0;
    int crop_y = 0;
    yarp::sig::VectorOf<int> c(2);
    c[0] = int((world[0] - this->m_origin.y) / this->m_resolution);
    c[1] = int((-world[1] - this->m_origin.y) / this->m_resolution);
    c[0] -= crop_x;
    c[1] -= crop_y;
    return c;
}