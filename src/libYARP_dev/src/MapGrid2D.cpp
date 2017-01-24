/*
 * Copyright (C) 2016 The RobotCub Consortium
 * Author: Marco Randazzo.
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */

#include <yarp/dev/MapGrid2D.h>
#include <yarp/os/Bottle.h>
#include <yarp/sig/ImageFile.h>
#include <yarp/os/LogStream.h>
#include <algorithm>
#include <fstream>
#include <cmath>

using namespace yarp::dev;
using namespace yarp::sig;
using namespace yarp::os;
using namespace yarp::math;
using namespace std;

bool MapGrid2D::isIdenticalTo(const MapGrid2D& other) const
{
    if (m_map_name != other.m_map_name) return false;
    if (m_origin.x != other.m_origin.x) return false;
    if (m_origin.y != other.m_origin.y) return false;
    if (m_origin.theta != other.m_origin.theta) return false;
    if (m_resolution != other.m_resolution) return false;
    if (m_width != other.width()) return false;
    if (m_height != other.height()) return false;
    for (size_t y = 0; y < m_height; y++) for (size_t x = 0; x < m_width;  x++)
        if (m_map_occupancy.safePixel(x, y) != other.m_map_occupancy.safePixel(x, y)) return false;
    for (size_t y = 0; y < m_height; y++) for (size_t x = 0; x < m_width; x++)
        if (m_map_flags.safePixel(x, y) != other.m_map_flags.safePixel(x, y)) return false;
    return true;
}

MapGrid2D::MapGrid2D()
{
    m_origin.x = 0;
    m_origin.y = 0;
    m_origin.theta = 0;
    m_resolution = 1.0; //each pixel corresponds to 1 m
    m_width = 2;
    m_height = 2;
    m_map_occupancy.setQuantum(1); //we do not want extra padding in map images
    m_map_flags.setQuantum(1);
    m_map_occupancy.resize(m_width, m_height);
    m_map_flags.resize(m_width, m_height);
    m_occupied_thresh = 0.80;
    m_free_thresh = 0.20;
    for (size_t y = 0; y < m_height; y++)
    {  
        for (size_t x = 0; x < m_width; x++)
        {
            m_map_occupancy.safePixel(x, y) = 0;
            m_map_flags.safePixel(x, y) = MapGrid2D::map_flags::MAP_CELL_FREE;
        }
    }
}

MapGrid2D::~MapGrid2D()
{

}

bool MapGrid2D::isNotFree(MapGrid2D::XYCell cell) const
{
    if (isInsideMap(cell))
    {
        if (m_map_occupancy.safePixel(cell.x, cell.y) != 0) return true;
        if (m_map_flags.safePixel(cell.x, cell.y) == MapGrid2D::map_flags::MAP_CELL_KEEP_OUT) return true;
        if (m_map_flags.safePixel(cell.x, cell.y) == MapGrid2D::map_flags::MAP_CELL_TEMPORARY_OBSTACLE) return true;
        if (m_map_flags.safePixel(cell.x, cell.y) == MapGrid2D::map_flags::MAP_CELL_ENLARGED_OBSTACLE) return true;
    }
    return false;
}

bool MapGrid2D::isFree(MapGrid2D::XYCell cell) const
{
    if (isInsideMap(cell))
    {
        if (m_map_occupancy.safePixel(cell.x, cell.y) == 0 &&
            m_map_flags.safePixel(cell.x, cell.y) == MapGrid2D::map_flags::MAP_CELL_FREE)
            return true;
    }
    return false;
}

bool MapGrid2D::isKeepOut(XYCell cell) const
{
    if (isInsideMap(cell))
    {
        if (m_map_flags.safePixel(cell.x, cell.y) == MapGrid2D::map_flags::MAP_CELL_KEEP_OUT)
            return true;
    }
    return false;
}

bool MapGrid2D::isWall(XYCell cell) const
{
    if (isInsideMap(cell))
    {
       // if (m_map_occupancy.safePixel(cell.x, cell.y) == 0)
       //     return true;
        if (m_map_flags.safePixel(cell.x, cell.y) == MapGrid2D::map_flags::MAP_CELL_WALL)
            return true;
    }
    return false;
}

size_t MapGrid2D::height() const
{
    return m_height;
}

size_t MapGrid2D::width() const
{
    return m_width;
}

bool MapGrid2D::getMapImage(yarp::sig::ImageOf<PixelRgb>& image) const
{
    image.setQuantum(1);
    image.resize(m_width, m_height);
    image.zero();
    for (size_t y = 0; y < m_height; y++)
    {
        for (size_t x = 0; x < m_width; x++)
        {
            image.safePixel(x, y) = CellDataToPixel(m_map_flags.safePixel(x, y));
        }
    }
    return true;
}

bool MapGrid2D::setMapImage(yarp::sig::ImageOf<PixelRgb>& image) 
{
    if (image.width() != (int)(m_width) ||
        image.height() != (int)(m_height))
    {
        yError() << "The size of given iamge does not correspond to the current map. Use method setSize() first.";
        return false;
    }
    for (size_t y = 0; y < m_height; y++)
    {
        for (size_t x = 0; x < m_width; x++)
        {
            m_map_flags.safePixel(x, y) = PixelToCellData(image.safePixel(x, y));
        }
    }
    return true;
}

bool MapGrid2D::enlargeObstacles(unsigned int size)
{
    if (size == 0)
    {
        for (size_t y = 0; y < m_height; y++)
        {
            for (size_t x = 0; x < m_width; x++)
            {
                if (this->m_map_flags.safePixel(x, y) == MapGrid2D::map_flags::MAP_CELL_ENLARGED_OBSTACLE)
                {
                    this->m_map_flags.safePixel(x, y) = MapGrid2D::map_flags::MAP_CELL_FREE;
                }
            }
        }
        return true;
    }

    for (size_t repeat = 0; repeat < size; repeat++)
    {
        //contains the cells to be enlarged;
        std::vector<XYCell> list_of_cells;
        for (size_t y = 0; y < m_height; y++)
        {
            for (size_t x = 0; x < m_width; x++)
            {
                //this check could be optimized...
                if (this->m_map_flags.safePixel(x, y) == MAP_CELL_KEEP_OUT ||
                    this->m_map_flags.safePixel(x, y) == MAP_CELL_ENLARGED_OBSTACLE ||
                    this->m_map_flags.safePixel(x, y) == MAP_CELL_WALL ||
                    this->m_map_flags.safePixel(x, y) == MAP_CELL_UNKNOWN ||
                    this->m_map_flags.safePixel(x, y) == MAP_CELL_TEMPORARY_OBSTACLE)
                {
                    list_of_cells.push_back(XYCell(x, y));
                }
            }
        }

        //process each cell of the list and enlarges it
        for (auto it = list_of_cells.begin(); it != list_of_cells.end(); it++)
        {
            enlargeCell(*it);
        }
    }
    return true;
}

void MapGrid2D::enlargeCell(XYCell cell)
{
    int i = cell.x;
    int j = cell.y;
    int il = cell.x - 1>0 ? cell.x - 1 : 0;
    int ir = cell.x + 1<(int)(m_width) - 1 ? cell.x + 1 : (int)(m_width)-1;
    int ju = cell.y - 1>0 ? cell.y - 1 : 0;
    int jd = cell.y + 1<(int)(m_height) - 1 ? cell.y + 1 : (int)(m_height)-1;

    if (m_map_flags.pixel(il, j) == MAP_CELL_FREE) m_map_flags.pixel(il, j) = MAP_CELL_ENLARGED_OBSTACLE;
    if (m_map_flags.pixel(ir, j) == MAP_CELL_FREE) m_map_flags.pixel(ir, j) = MAP_CELL_ENLARGED_OBSTACLE;
    if (m_map_flags.pixel(i, ju) == MAP_CELL_FREE) m_map_flags.pixel(i, ju) = MAP_CELL_ENLARGED_OBSTACLE;
    if (m_map_flags.pixel(i, jd) == MAP_CELL_FREE) m_map_flags.pixel(i, jd) = MAP_CELL_ENLARGED_OBSTACLE;
    if (m_map_flags.pixel(il, ju) == MAP_CELL_FREE) m_map_flags.pixel(il, ju) = MAP_CELL_ENLARGED_OBSTACLE;
    if (m_map_flags.pixel(il, jd) == MAP_CELL_FREE) m_map_flags.pixel(il, jd) = MAP_CELL_ENLARGED_OBSTACLE;
    if (m_map_flags.pixel(ir, ju) == MAP_CELL_FREE) m_map_flags.pixel(ir, ju) = MAP_CELL_ENLARGED_OBSTACLE;
    if (m_map_flags.pixel(ir, jd) == MAP_CELL_FREE) m_map_flags.pixel(ir, jd) = MAP_CELL_ENLARGED_OBSTACLE;
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
    string pgm_occ_filename;
    string ppm_flg_filename;

    if (bbb.check("image:") == false) { yError() << "missing image"; ret = false; }
    pgm_occ_filename = bbb.find("image:").asString();
    ppm_flg_filename = (pgm_occ_filename.substr(0, pgm_occ_filename.size()-4))+"_yarpflags"+".ppm";

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

    yarp::sig::ImageOf<yarp::sig::PixelRgb> img_occ;
    yarp::sig::ImageOf<yarp::sig::PixelRgb> img_flg;
    ret = yarp::sig::file::read(img_occ, pgm_occ_filename.c_str());
    if (ret == false)
    {
        yError() << "Unable to load file" << pgm_occ_filename;
        return false;
    }
    ret = yarp::sig::file::read(img_flg, ppm_flg_filename.c_str());
    if (ret == false)
    {
        yError() << "Unable to load file" << ppm_flg_filename;
        return false;
    }
    if (img_occ.width() != (int)(img_flg.width()) ||
        img_occ.height() != (int)(img_flg.height()))
    {
        yError() << "File:" << pgm_occ_filename << "does not have the same width/height of" << ppm_flg_filename;
        return false;
    }
    else
    {
        m_width = (int)(img_occ.width());
        m_height = (int)(img_occ.height());
    }
    m_map_occupancy.resize(m_width, m_height);
    m_map_flags.resize(m_width, m_height);

    for (size_t y = 0; y < m_height; y++)
    {
        for (size_t x = 0; x < m_width; x++)
        {
            yarp::sig::PixelRgb pix_occ = img_occ.safePixel(x, y);
            yarp::sig::PixelRgb pix_flg = img_flg.safePixel(x, y);
            m_map_flags.safePixel(x, y) = PixelToCellData(img_flg.safePixel(x, y));

            double color_avg = (pix_flg.r + pix_flg.g + pix_flg.b) / 3;
            unsigned char occ = (unsigned char)((255 - color_avg) / 255.0);
            m_map_occupancy.safePixel(x, y) = occ*100;
        }
    }

    return true;
}

MapGrid2D::CellData MapGrid2D::PixelToCellData(const yarp::sig::PixelRgb& pixin) const
{
    if (pixin.r == 0 && pixin.g == 0 && pixin.b == 0)   return MAP_CELL_WALL;
    else if (pixin.r == 205 && pixin.g == 205 && pixin.b == 205) return  MAP_CELL_UNKNOWN;
    else if (pixin.r == 254 && pixin.g == 254 && pixin.b == 254) return  MAP_CELL_FREE;
    else if (pixin.r == 255 && pixin.g == 0 && pixin.b == 0) return  MAP_CELL_KEEP_OUT;
    return  MAP_CELL_UNKNOWN;
}

yarp::sig::PixelRgb MapGrid2D::CellDataToPixel(const MapGrid2D::CellData& pixin) const
{
    yarp::sig::PixelRgb pixout_flg;
    if (pixin == MAP_CELL_WALL) { pixout_flg.r = 0; pixout_flg.g = 0; pixout_flg.b = 0;}
    else if (pixin == MAP_CELL_UNKNOWN) { pixout_flg.r = 205; pixout_flg.g = 205; pixout_flg.b = 205; }
    else if (pixin == MAP_CELL_FREE) { pixout_flg.r = 254; pixout_flg.g = 254; pixout_flg.b = 254; }
    else if (pixin == MAP_CELL_KEEP_OUT) { pixout_flg.r = 255; pixout_flg.g = 0; pixout_flg.b = 0; }
    else if (pixin == MAP_CELL_ENLARGED_OBSTACLE) { pixout_flg.r = 255; pixout_flg.g = 200; pixout_flg.b = 0; }
    else if (pixin == MAP_CELL_TEMPORARY_OBSTACLE) { pixout_flg.r = 100; pixout_flg.g = 100; pixout_flg.b = 200; }
    else
    {
        //invalid
        pixout_flg.r = 200; pixout_flg.g = 0; pixout_flg.b = 200;
    }
    return pixout_flg;
}

bool  MapGrid2D::saveToFile(std::string yaml_filename) const
{
    std::string pgm_occ_filename;
    std::string ppm_flg_filename;
    std::ofstream file;
    file.open(yaml_filename.c_str());
    if (!file.is_open())
    {
        return false;
    }
    file << "image: " << pgm_occ_filename << endl;
    file << "resolution: " << m_resolution << endl;
    file << "origin: [ " << m_origin.x << " " << m_origin.y << "" << m_origin.theta << " ]"<< endl;
    file << "name:" << m_map_name << endl;
    file.close();

    yarp::sig::ImageOf<yarp::sig::PixelRgb> img_flg;
    yarp::sig::ImageOf<yarp::sig::PixelRgb> img_occ;

    img_flg.resize(m_width, m_height);
    for (size_t y = 0; y < m_height; y++)
    {
        for (size_t x = 0; x < m_width; x++)
        {
            yarp::sig::PixelMono pix = m_map_flags.safePixel(x, y);
            yarp::sig::PixelRgb pix_occ;
            img_flg.safePixel(x, y) =  CellDataToPixel(pix);
            img_occ.safePixel(x, y) = pix_occ;
        }
    }

    ppm_flg_filename = (pgm_occ_filename.substr(0, pgm_occ_filename.size() - 4)) + "_yarpflags" + ".ppm";
    bool ret = true;
    ret &= yarp::sig::file::write(img_occ, pgm_occ_filename.c_str());
    ret &= yarp::sig::file::write(img_flg, ppm_flg_filename.c_str());
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
    connection.appendInt(m_width);
    connection.appendInt(BOTTLE_TAG_INT);
    connection.appendInt(m_height);
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

MapGrid2D::XYWorld MapGrid2D::cell2World(MapGrid2D::XYCell cell) const
{
    //convert a cell (from the upper-left corner) to the map reference frame (located in m_origin, measured in meters)
    //beware: the location of m_origin is referred to the lower-left corner (ROS convention)
    /*
    int crop_x = 0;
    int crop_y = 0;
    cell.x += crop_x;
    cell.y += crop_y;
    MapGrid2D::XYWorld v;
    v.x = double(cell.x)*this->m_resolution;
    v.y = double(cell.y)*this->m_resolution;
    v.x = v.x + this->m_origin.x;
    v.y = -(v.y + this->m_origin.y);
    return v;
    */
    int crop_x = 0;
    int crop_y = 0;
    cell.x += crop_x;
    cell.y += crop_y;
    MapGrid2D::XYWorld v;
    v.x = double(cell.x)*this->m_resolution;
    v.y = double(cell.y)*this->m_resolution;
    v.x = + v.x + m_origin.x + 0 * this->m_resolution;
    v.y = + v.y - m_origin.y - (m_height+1)*this->m_resolution;
    return v;
}


MapGrid2D::XYCell MapGrid2D::world2Cell(MapGrid2D::XYWorld world) const
{
    //convert a world location (wrt the map reference frame located in m_origin, measured in meters), to a cell from the upper-left corner.
    //beware: the location of m_origin is referred to the lower-left corner (ROS convention)
    /*
    int crop_x = 0;
    int crop_y = 0;
    MapGrid2D::XYCell c;
    c.x = int((world.x - this->m_origin.x) / this->m_resolution);
    c.y = int((-world.y - this->m_origin.y) / this->m_resolution);
    c.x -= crop_x;
    c.y -= crop_y;
    return c;
    */
    int crop_x = 0;
    int crop_y = 0;
    MapGrid2D::XYCell c;
    c.x = int((+world.x - this->m_origin.x) / this->m_resolution) + 0;
    c.y = int((-world.y + this->m_origin.y) / this->m_resolution) + m_height - 1;
    c.x -= crop_x;
    c.y -= crop_y;
    return c;
}

bool MapGrid2D::isInsideMap(MapGrid2D::XYWorld world) const
{
    XYCell cell = world2Cell(world);
    return isInsideMap(cell);
}

bool MapGrid2D::isInsideMap(MapGrid2D::XYCell cell) const
{
    if (cell.x < 0) return false;
    if (cell.y < 0) return false;
    if (cell.x >= (int)(m_width)) return false;
    if (cell.y >= (int)(m_height)) return false;
    return true;
}

bool MapGrid2D::setOrigin(double x, double y, double theta)
{
    //the given x and y are referred to the bottom left corner, pointing outwards.
    //To check if it is inside the map, I have to convert it to a cell with x and y referred to the upper left corner, pointing inwards
    if (m_resolution<=0)
    {
        yWarning() << "MapGrid2D::setOrigin() requested is not inside map!";
        return false;
    }

    int xc = (int)(x/m_resolution);
    int yc = (int)(y / m_resolution);

    XYCell orig(-xc, (m_height-1) + yc);
    if (isInsideMap(orig))
    {
        m_origin.x = x;
        m_origin.y = y;
        m_origin.theta = fmod(theta, 360.0);
        return true;
    }
    else
    {
        yWarning() << "MapGrid2D::setOrigin() requested is not inside map!";
        m_origin.x = x;
        m_origin.y = y;
        m_origin.theta = fmod(theta, 360.0);
        return true;
    }
}

void MapGrid2D::getOrigin(double& x, double& y, double& theta) const
{
    x = m_origin.x;
    y = m_origin.y;
    theta = m_origin.theta;
}

bool MapGrid2D::setResolution(double resolution)
{
    if (resolution <= 0)
    {
        yError() << "MapGrid2D::setResolution() invalid value:" << resolution;
        return false;
    }
    m_resolution = resolution;
    return true;
}

void MapGrid2D::getResolution(double& resolution) const
{
    resolution = m_resolution;
}

bool MapGrid2D::setMapName(std::string map_name)
{
    if (map_name != "")
    {
        m_map_name = map_name;
        return true;
    }
    yError() << "MapGrid2D::setMapName() invalid map name";
    return false;
}

std::string MapGrid2D::getMapName() const
{
    return m_map_name;
}

bool MapGrid2D::setSize_in_meters(double x, double y)
{
    if (x <= 0 && y <= 0)
    {
        yError() << "MapGrid2D::setSize() invalid size";
        return false;
    }
    if (m_resolution <= 0)
    {
        yError() << "MapGrid2D::setSize() invalid map resolution.";
        return false;
    }
    size_t w = (size_t)(x*m_resolution);
    size_t h = (size_t)(y*m_resolution);
    setSize_in_cells(w,h);
    return true;
}

bool MapGrid2D::setSize_in_cells(size_t x, size_t y)
{
    if (x == 0 && y == 0)
    {
        yError() << "MapGrid2D::setSize() invalid size";
        return false;
    }
    m_map_occupancy.resize(x, y);
    m_map_flags.resize(x, y);
    m_map_occupancy.zero();
    m_map_flags.zero();
    m_width = x;
    m_height = y;
    return true;
}

void MapGrid2D::getSize_in_meters(double& x, double& y) const
{
    x = m_width*m_resolution;
    y = m_height*m_resolution;
}

void MapGrid2D::getSize_in_cells(size_t&x, size_t& y) const
{
    x = m_width;
    y = m_height;
}

bool MapGrid2D::setMapFlag(XYCell cell, map_flags flag)
{
    if (isInsideMap(cell) == false)
    {
        yError() << "Invalid cell requested " << cell.x << " " << cell.y;
        return false;
    }
    m_map_flags.safePixel(cell.x, cell.y) = flag;
    return true;
}

bool MapGrid2D::setOccupancyData(XYCell cell, double occupancy)
{
    if (isInsideMap(cell) == false)
    {
        yError() << "Invalid cell requested " << cell.x << " " << cell.y;
        return false;
    }
    m_map_occupancy.safePixel(cell.x, cell.y) = (yarp::sig::PixelMono)(occupancy*255/100);
    return true;
}

bool MapGrid2D::getOccupancyData(XYCell cell, double& occupancy) const
{
    if (isInsideMap(cell) == false)
    {
        yError() << "Invalid cell requested " << cell.x << " " << cell.y;
        return false;
    }
    occupancy = m_map_occupancy.safePixel(cell.x, cell.y) * 100.0 / 255.0;
    return true;
}

bool MapGrid2D::setOccupancyGrid(yarp::sig::ImageOf<yarp::sig::PixelMono>& image)
{
    if (image.width() != m_width ||
        image.height() != m_height)
    {
        yError() << "The size of given occupancy grid does not correspond to the current map. Use method setSize() first.";
        return false;
    }
    m_map_occupancy = image;
    return true;
}

bool MapGrid2D::getOccupancyGrid(yarp::sig::ImageOf<yarp::sig::PixelMono>& image) const
{
    image = m_map_occupancy;
    return true;
}