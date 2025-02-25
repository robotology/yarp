/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "FakeBot.h"

#include <yarp/sig/all.h>
#include <yarp/sig/ImageFile.h>
#include <random>

using namespace yarp::os;
using namespace yarp::sig;
using namespace yarp::sig::draw;
using namespace yarp::sig::file;
using namespace yarp::dev;

YARP_LOG_COMPONENT(FAKEBOT, "yarp.devices.FakeBot")


#define MAXRND 50000
namespace {
int rnds[MAXRND];
}

void FakeBot::init() {
    int m_w = 640;
    int m_h = 480;
    int rr = 30;

    back.resize(m_w,m_h);
    PixelRgb w{255,255,255};
    PixelRgb r{128,128,255};
    PixelRgb b{0,0,255};
    IMGFOR(back,x,y) {
        back(x,y) = r;
        if (y>=m_h*0.75) {
            back(x,y) = b;
        }
    }
    for (int i=0; i<=m_w; i+=m_w/10) {
        addCircle(back,b,i,int(m_h*0.77),rr);
    }

    std::default_random_engine randengine;
    std::uniform_real_distribution<double> udist1(-m_w/4,m_w/4);
    std::uniform_real_distribution<double> udist2(-int(m_h*0.2),rr);
    for (int j=0; j<40; j++) {
        int xx = m_w/2 + udist1(randengine);
        int yy = int(m_h*0.2) - udist2(randengine);
        addCircle(back,w,xx,yy,rr);
    }

    m_x = (back.width()-this->m_w)/2;
    m_y = (back.height()-this->m_h)/2;
    m_dx = m_dy = 0;

    std::normal_distribution<double> ndist(0,255);
    for (int & rnd : rnds) {
        rnd = int(ndist(randengine));
    }

    fore.resize(64,64);
    fore.zero();
    m_tx = back.width()/2;
    m_ty = back.height()*0.4;
    m_tdx = 1;
    m_tdy = 0;
}


void scramble(unsigned char& ch, float f) {
    int x = ch;
    static int idx = 0;
    //x += (int)(Random::normal(0,x*f));
    x += (int)(rnds[idx]*f);
    idx = (idx+17)%MAXRND;
    if (x < 0) {
        x = 0;
    }
    if (x > 255) {
        x = 255;
    }
    ch = (unsigned char) x;
}


bool FakeBot::open(yarp::os::Searchable& config)
{
    if (!this->parseParams(config)) {return false;}

    if (m_background !="") {
        yarp::sig::file::read(back, m_background);
    }

    if (m_target !="") {
        yarp::sig::file::read(fore, m_target);
    }

    if (m_lifetime>=0) {
        start();
    }
    return true;
}


// IFrameGrabberImage
ReturnValue FakeBot::getImage(yarp::sig::ImageOf<yarp::sig::PixelRgb>& image) {
    if (fabs(dpos[0])>0.001||fabs(dpos[0])>0.001) {
        pos[0] = m_dx+dpos[0];
        dpos[0] = 0;
        pos[1] = m_dy+dpos[1];
        dpos[1] = 0;
    }
    pos[0] += vel[0];
    pos[1] += vel[1];
    double xx = pos[0];
    double yy = pos[1];
    double dx = (xx-m_dx)*5;
    double dy = (yy-m_dy)*5;
    m_tx += m_tdx;
    m_ty += m_tdy;
    if (m_tdx>0 && m_tx>back.width()*0.75) {
        m_tdx *= -1;
    }
    if (m_tdx<0 && m_tx<back.width()*0.25) {
        m_tdx *= -1;
    }
    dx /= 40;
    dy /= 40;
    if (amp[0]>0.5) {
        m_dx += dx;
    }
    if (amp[1]>0.5) {
        m_dy += dy;
    }
    image.resize(m_w,m_h);
    back.safePixel(-1,-1) = PixelRgb{255,0,0};
    loc[0] = m_dx;
    loc[1] = m_dy;

    double m_dx_scaled = m_dx* m_sx;
    double m_dy_scaled = m_dy* m_sy;
    IMGFOR(image,x,y) {
        int x0 = int(x+m_x+m_dx_scaled*0.5+0.5);
        int y0 = int(y+m_y+m_dy_scaled*0.5+0.5);
        image(x,y) = back.safePixel(x0,y0);

        if (fore.isPixel(int(x0-m_tx),int(y0-m_ty))) {
         PixelRgb& pix = fore(int(x0-m_tx),int(y0-m_ty));
         if (pix.r<200||pix.g>100||pix.b>100) {
             image(x,y) = pix;
         }
     }
    }
    IMGFOR(image,x2,y2) {
        PixelRgb& pix = image(x2,y2);
        auto f = (float)(m_noise);
        scramble(pix.r,f);
        scramble(pix.g,f);
        scramble(pix.b,f);
    }
    Time::delay(0.1); // simulated hardware delay, using mutable clock
    return ReturnValue_ok;
}

void FakeBot::run() {
    if (m_lifetime>=0) {
        Time::delay(m_lifetime);
        std::exit(0);
    }
}
