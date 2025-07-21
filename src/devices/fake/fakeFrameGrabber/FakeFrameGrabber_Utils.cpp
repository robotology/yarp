/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "FakeFrameGrabber.h"

#include <yarp/os/LogComponent.h>
#include <yarp/os/LogStream.h>
#include <yarp/sig/ImageDraw.h>

#include <cstdio>
#include <thread>
#include <random>

using namespace yarp::os;
using namespace yarp::dev;
using namespace yarp::sig;
using namespace yarp::sig::draw;

namespace {
YARP_LOG_COMPONENT(FAKEFRAMEGRABBER, "yarp.device.fakeFrameGrabber")

//the following data are used by [time] test
constexpr char num[12][16]
{
    // '0'
    "***"
    "* *"
    "* *"
    "* *"
    "***",

    // '1'
    " * "
    " * "
    " * "
    " * "
    " * ",

    // '2'
    "***"
    "  *"
    "***"
    "*  "
    "***",

    // '3'
    "***"
    "  *"
    "***"
    "  *"
    "***",

    // '4'
    "* *"
    "* *"
    "***"
    "  *"
    "  *",

    // '5'
    "***"
    "*  "
    "***"
    "  *"
    "***",

    // '6'
    "***"
    "*  "
    "***"
    "* *"
    "***",

    // '7'
    "***"
    "  *"
    "  *"
    "  *"
    "  *",

    // '8'
    "***"
    "* *"
    "***"
    "* *"
    "***",

    // '9'
    "***"
    "* *"
    "***"
    "  *"
    "***",

    // ' '
    "   "
    "   "
    "   "
    "   "
    "   ",

    // '.'
    "   "
    "   "
    "   "
    " **"
    " **",
};
constexpr size_t num_width = 3;
constexpr size_t num_height = 5;

} // namespace

void FakeFrameGrabber::printTime(unsigned char* pixbuf, size_t pixbuf_w, size_t pixbuf_h, size_t x, size_t y, char* s, size_t size)
{
    for (size_t i = 0; i < size; i++)
    {
        const char* num_p = nullptr;
        switch (s[i]) {
            case '0': num_p = num[0]; break;
            case '1': num_p = num[1]; break;
            case '2': num_p = num[2]; break;
            case '3': num_p = num[3]; break;
            case '4': num_p = num[4]; break;
            case '5': num_p = num[5]; break;
            case '6': num_p = num[6]; break;
            case '7': num_p = num[7]; break;
            case '8': num_p = num[8]; break;
            case '9': num_p = num[9]; break;
            case ' ': num_p = num[10]; break;
            case '.': num_p = num[11]; break;
            default: num_p = num[10]; break;
        }

        for (size_t yi = 0; yi < num_height; yi++) {
            for (size_t xi = 0; xi < num_width; xi++) {
                size_t ii = yi * num_width + xi;
                if (num_p[ii] == '*') {
                    for (size_t r = yi * num_height; r < yi*num_height + num_height; r++) {
                        size_t off = i * (num_height + 20);
                        for (size_t c = xi * num_height + off; c < xi*num_height + num_height + off; c++) {
                            if (c >= pixbuf_h || r >= pixbuf_w) {
                                //avoid drawing out of the image memory
                                return;
                            }
                            unsigned char *pixel = pixbuf;
                            size_t offset = c * sizeof(yarp::sig::PixelRgb) + r * (pixbuf_w * sizeof(yarp::sig::PixelRgb));
                            pixel = pixel + offset;
                            pixel[0] = 0;
                            pixel[1] = 0;
                            pixel[2] = 255;
                        }
                    }
                }
            }
        }
    }
}

void FakeFrameGrabber::createTestImage(yarp::sig::ImageOf<yarp::sig::PixelRgb>& image,
                                       double& timestamp)
{
    //block to avoid race while an rpc can reset the image properties(e.g. w/h)
    std::lock_guard<std::mutex> lock(rpc_methods_mutex);

    //to test IPreciselyTimed, make timestamps be mysteriously NNN.NNN42
    double t = Time::now();
    t -= (((t*1000) - static_cast<int64_t>(t*1000)) / 1000);
    t += 0.00042;
    timestamp = t;

    //set the image size
    image.resize(m_width,m_height);

    //check the counter inside the image size
    if (m_ct>=image.height()) {
        m_ct = 0;
    }
    if (m_by>=image.height()) {
        m_by = image.height()-1;
    }
    if (m_bx>=image.width()) {
        m_bx = image.width()-1;
    }

    if (m_mode == "[Time]")
    {
        {
            if (m_have_bg) {
                image.copy(background);
            } else {
                image.zero();
            }
            char txtbuf[50];
            static const double start_time = t;
            double time = t - start_time;
            std::snprintf(txtbuf, 50, "%.3f", time);
            int len = strlen(txtbuf);
            if (len < 20)
            {
                printTime((unsigned char*)image.getRawImage(), image.width(), image.height(), 0, 0, txtbuf, len);
            }
        }
    }
    else if (m_mode == "[ball]")
    {
        {
            if (m_have_bg) {
                image.copy(background);
            } else {
                image.zero();
            }
            addCircle(image,PixelRgb{0,255,0},m_bx,m_by,15);
            addCircle(image,PixelRgb{0,255,255},m_bx,m_by,8);
            if (m_ct%5!=0) {
                m_rnd *= 65537;
                m_rnd += 17;
                int delta_x = (m_rnd % 5) - 2;
                m_bx += delta_x;
                m_rnd *= 65537;
                m_rnd += 17;
                int delta_y = (m_rnd % 5) - 2;
                m_by += delta_y;
            } else {
                int dx = m_width/2 - m_bx;
                int dy = m_height/2 - m_by;
                if (dx>0) { m_bx++; }
                if (dx<0) { m_bx--; }
                if (dy>0) { m_by++; }
                if (dy<0) { m_by--; }
            }
        }
    }
    else if (m_mode == "[grid]")
    {
        {
            size_t ww = image.width();
            size_t hh = image.height();
            if (ww>1&&hh>1) {
                for (size_t x=0; x<ww; x++) {
                    for (size_t y=0; y<hh; y++) {
                        double xx = ((double)x)/(ww-1);
                        double yy = ((double)y)/(hh-1);
                        bool act = (y==m_ct);
                        auto r = static_cast<unsigned char>(0.5 + 255 * xx);
                        auto g = static_cast<unsigned char>(0.5 + 255 * yy);
                        auto b = static_cast<unsigned char>(act * 255);
                        image.pixel(x, y) = PixelRgb{r, g, b};
                    }
                }
            }
        }
    }
    else if (m_mode == "[size]")
    {
        static int count = 0;
        count++;
        if (count== 100)
        {
            yCDebug(FAKEFRAMEGRABBER) << "size 100, 100";
            image.resize(100,100);
        }
        else if (count == 200)
        {
            yCDebug(FAKEFRAMEGRABBER) << "size 200, 100";
            image.resize(200, 100);
        }
        else if (count == 300)
        {
            yCDebug(FAKEFRAMEGRABBER) << "size 300, 50";
            image.resize(300, 50);
            count = 0;
        }

        size_t ww = m_width = image.width();
        size_t hh = m_height = image.height();
        if (ww>1 && hh>1) {
            for (size_t x = 0; x<ww; x++) {
                for (size_t y = 0; y<hh; y++) {
                    double xx = ((double)x) / (ww - 1);
                    double yy = ((double)y) / (hh - 1);
                    bool act = (y == m_ct);
                    auto r = static_cast<unsigned char>(0.5 + 255 * xx);
                    auto g = static_cast<unsigned char>(0.5 + 255 * yy);
                    auto b = static_cast<unsigned char>(act * 255);
                    image.pixel(x, y) = PixelRgb{r, g, b};
                }
            }
        }
    }
    else if (m_mode == "[line]")
    {
        {
            if (m_have_bg) {
                image.copy(background);
            } else {
                image.zero();
            }
            for (size_t i=0; i<image.width(); i++) {
                image.pixel(i,m_ct).r = 255;
            }
        }
    }
    else if (m_mode == "[rand]")
    {
        {
            static unsigned char r = 128;
            static unsigned char g = 128;
            static unsigned char b = 128;

            size_t ww = image.width();
            size_t hh = image.height();

            if (ww>1&&hh>1) {
                for (size_t x=0; x<ww; x++) {
                    for (size_t y=0; y<hh; y++) {
                        r += udist(randengine);
                        g += udist(randengine);
                        b += udist(randengine);
                        image.pixel(x,y) = PixelRgb{r,g,b};
                    }
                }
            }
        }
    }
    else if (m_mode == "[none]")
    {
        {
            if (m_have_bg) {
                image.copy(background);
            } else {
                image.zero();
            }
        }
    }
    else
    {
        yCError(FAKEFRAMEGRABBER, "Invalid mode %s", m_mode.c_str());
    }

    if (m_add_noise)
    {
        static const double nsr = 1.0 - m_snr;
        for (size_t x = 0; x < image.width(); ++x) {
            for (size_t y = 0; y < image.height(); ++y) {
                auto rand = ucdist(randengine);
                image.pixel(x,y) = PixelRgb {
                    static_cast<unsigned char>(image.pixel(x,y).r * m_snr + (rand * nsr * 255)),
                    static_cast<unsigned char>(image.pixel(x,y).g * m_snr + (rand * nsr * 255)),
                    static_cast<unsigned char>(image.pixel(x,y).b * m_snr + (rand * nsr * 255))
                };
            }
        }
    }

    if (m_add_timestamp)
    {
        char ttxt[50];
        std::snprintf(ttxt, 50, "%021.10f", timestamp);
        image.pixel(0, 0).r = ttxt[0] - '0';
        image.pixel(0, 0).g = ttxt[1] - '0';
        image.pixel(0, 0).b = ttxt[2] - '0';

        image.pixel(1, 0).r = ttxt[3] - '0';
        image.pixel(1, 0).g = ttxt[4] - '0';
        image.pixel(1, 0).b = ttxt[5] - '0';

        image.pixel(2, 0).r = ttxt[6] - '0';
        image.pixel(2, 0).g = ttxt[7] - '0';
        image.pixel(2, 0).b = ttxt[8] - '0';

        image.pixel(3, 0).r = ttxt[9] - '0';
        image.pixel(3, 0).g = ttxt[10] - '0';
        image.pixel(3, 0).b = ttxt[11] - '0';

        image.pixel(4, 0).r = ttxt[12] - '0';
        image.pixel(4, 0).g = ttxt[13] - '0';
        image.pixel(4, 0).b = ttxt[14] - '0';

        image.pixel(5, 0).r = ttxt[15] - '0';
        image.pixel(5, 0).g = ttxt[16] - '0';
        image.pixel(5, 0).b = ttxt[17] - '0';

        image.pixel(6, 0).r = ttxt[18] - '0';
        image.pixel(6, 0).g = ttxt[19] - '0';
        image.pixel(6, 0).b = ttxt[20] - '0';
    }

    m_ct++;
}



// From iCub staticgrabber device.
// DF2 bayer sequence.
// -- in staticgrabber: first row GBGBGB, second row RGRGRG.
// -- changed here to:  first row GRGRGR, second row BGBGBG.
bool FakeFrameGrabber::makeSimpleBayer(
        ImageOf<PixelRgb>& img,
        ImageOf<PixelMono>& bayer) {

    bayer.resize(img.width(), img.height());

    const size_t w = img.width();
    const size_t h = img.height();

    size_t i, j;
    for (i = 0; i < h; i++) {
        auto* row = (PixelRgb *)img.getRow(i);
        auto* rd = (PixelMono *)bayer.getRow(i);

        for (j = 0; j < w; j++) {

            if ((i%2) == 0) {
                switch (j%4) {
                    case 0:
                    case 2:
                        *rd++ = row->g;
                        row++;
                        break;

                    case 1:
                    case 3:
                        *rd++ = row->r;
                        row++;
                        break;
                }
            }

            if ((i%2) == 1) {
                switch (j%4) {
                    case 1:
                    case 3:
                        *rd++ = row->g;
                        row++;
                        break;

                    case 0:
                    case 2:
                        *rd++ = row->b;
                        row++;
                        break;
                }
            }
        }
    }

    return true;
}
