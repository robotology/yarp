/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/sig/Matrix.h>
#include <yarp/os/impl/BufferedConnectionWriter.h>
#include <yarp/os/Bottle.h>
#include <yarp/os/Thread.h>
#include <yarp/os/Port.h>
#include <yarp/os/Time.h>
#include <yarp/os/DummyConnector.h>
#include <yarp/os/PortablePair.h>

#include <yarp/gsl/Gsl.h>
#include <yarp/gsl/impl/gsl_structs.h>

#include <cmath>
#include <vector>

#include <catch.hpp>
#include <harness.h>

using namespace yarp::os::impl;
using namespace yarp::os;
using namespace yarp::sig;

//
class MThread1:public Thread
{
public:
    MThread1(Port *p)
    {
        portOut=p;
    }

    bool threadInit() override
    {
        success=false;
        return true;
    }

    void run() override
    {
        Matrix m;

        int times=10;

        while(times--)
        {
            m.resize(4,4);
            int r=0;
            int c=0;
            for (r = 0; r < 4; r++) {
                for (c = 0; c < 4; c++) {
                    m[r][c] = 99;
                }
            }

            portOut->write(m);
            Time::delay(0.1);

            m.resize(2,4);
            for (r = 0; r < 2; r++) {
                for (c = 0; c < 4; c++) {
                    m[r][c] = 66;
                }
            }

            portOut->write(m);
        }

        success=true;
    }

    Port *portOut;
    bool success;
};

class MThread2:public Thread
{
public:
    MThread2(Port *p)
    {
        portIn=p;
    }

    bool threadInit() override
    {
        success=false;
        return true;
    }

    void run() override
    {
        Matrix m;

        int times=10;
        bool ok=true;
        while(times--)
        {
            portIn->read(m);
            if ((m.rows() != 4) || (m.cols() != 4)) {
                ok = false;
            }


            portIn->read(m);

            if ((m.rows() != 2) || (m.cols() != 4)) {
                ok = false;
            }
        }

        success=ok;
    }

    Port *portIn;
    bool success;
};

bool checkConsistency(Matrix &a)
{
    gsl_matrix *tmp;

    yarp::gsl::GslMatrix tmpGSL(a);
    tmp=(gsl_matrix *)(tmpGSL.getGslMatrix());

    bool ret=true;
    if (tmp->size1 != a.rows()) {
        ret = false;
    }

    if (tmp->size2 != a.cols()) {
        ret = false;
    }

    if (tmp->block->size != a.cols() * a.rows()) {
        ret = false;
    }

    if (tmp->data != a.data()) {
        ret = false;
    }

    if (tmp->block->data != a.data()) {
        ret = false;
    }

    return ret;
}

void makeTestMatrix(Matrix& m, unsigned int rr, unsigned int cc)
{
    m.resize((int)rr,(int)cc);
    for(unsigned int r=0; r<rr; r++) {
        for(unsigned int c=0; c<cc; c++) {
            m[r][c] = r*cc+c;
        }
    }
}


TEST_CASE("sig::MatrixTest", "[yarp::sig]")
{
    NetworkBase::setLocalMode(true);

    SECTION("checking operator ==")
    {
        Matrix M1(3,3);
        Matrix M2(3,3);

        M1=1;
        M2=1; //now we have to identical vectors

        bool ok=false;
        if (M1 == M2) {
            ok = true;
        }

        M1=2;
        M2=1; //now vectors are different
        if (M1 == M2) {
            ok = false;
        }

        CHECK(ok); // operator== for matrix work
    }

    SECTION("check matrix send/receive")
    {
        Port portIn;
        Port portOut;

        MThread2 *receiverThread=new MThread2(&portIn);
        MThread1 *senderThread=new MThread1(&portOut);

        portOut.open("/harness_sig/mtest/o");
        portIn.open("/harness_sig/mtest/i");

        Network::connect("/harness_sig/mtest/o", "/harness_sig/mtest/i");

        receiverThread->start();
        senderThread->start();

        receiverThread->stop();
        senderThread->stop();

        portOut.close();
        portIn.close();

        CHECK(senderThread->success); // Send matrix test
        CHECK(receiverThread->success); // Receive matrix test

        delete receiverThread;
        delete senderThread;
    }

    SECTION("check matrix copy constructor works.")
    {
        Matrix m(10,40);
        int r=0;
        int c=0;
        for(r=0; r<10; r++)
        {
            for (c = 0; c < 40; c++) {
                m[r][c] = 1333;
            }
        }

        Matrix m2(m);
        CHECK(m.rows() == m2.rows()); // rows matches
        CHECK(m.cols() == m2.cols()); // cols matches

        bool ok=true;
        for (r = 0; r < 10; r++) {
            for (c = 0; c < 40; c++) {
                ok = ok && ((m[r])[c] == (m2[r])[c]);
            }
        }

        CHECK(ok); // elements match

        INFO("check matrix construction from empty matrix");
        Matrix empty1;
        Matrix empty2(empty1);
    }

    SECTION("check matrix copy operator works.")
    {
        Matrix m(10,40);
        int r=0;
        int c=0;
        for (r = 0; r < 10; r++) {
            for (c = 0; c < 40; c++) {
                m[r][c] = 99;
            }
        }

        Matrix m2;
        m2=m;
        CHECK(m.rows() == m2.rows()); // rows matches
        CHECK(m.cols() == m2.cols()); // cols matches

        bool ok=true;
        for (r = 0; r < 10; r++) {
            for (c = 0; c < 40; c++) {
                ok = ok && (m[r][c] == m2[r][c]);
            }
        }
        CHECK(ok); // elements match
    }

    SECTION("check bottle.")
    {
        INFO("check bottle compatibility...");
        Bottle b("2 3 (0.0 1.1 2.2 3.3 4.4 5.5)");
        Matrix m(6,1);
        DummyConnector con;
        b.write(con.getWriter());
        m.read(con.getReader());
        CHECK(m.rows() == (size_t) 2); // row size correct
        CHECK(m.cols() == (size_t) 3); // col size correct
        CHECK((m[1][2]>5 && m[1][2]<6)); // content is sane
    }

    SECTION("check submatrix.")
    {
        INFO("check function Matrix::submatrix works...");
        const size_t R=10;
        const size_t C=20;
        Matrix m(R,C);

        size_t r=0;
        size_t c=0;
        int kk=0;
        for (r = 0; r < R; r++) {
            for (c = 0; c < C; c++) {
                m[r][c] = kk++;
            }
        }

        INFO("extracting submatrix...");
        size_t r1=5;
        size_t r2=8;
        size_t c1=4;
        size_t c2=8;
        Matrix m2=m.submatrix(r1, r2, c1, c2);

        CHECK(r2-r1+1 == m2.rows()); // rows matches
        CHECK(c2-c1+1 == m2.cols()); // cols matches

        kk=r1*C+c1;
        bool ok=true;
        for(r=0; r<m2.rows(); r++)
        {
            int cc=kk;
            for(c=0;c<m2.cols();c++)
            {
                if (m2[r][c] != cc++) {
                    ok = false;
                }
            }
            kk+=C;
        }

        CHECK(ok); // elements match

        INFO("extracting full size matrix...");
        Matrix m3=m.submatrix(0, R-1, 0, C-1);
        CHECK(R == m3.rows()); // rows matches
        CHECK(C == m3.cols()); // cols matches

        kk=0;
        ok=true;
        for(r=0; r<m3.rows(); r++)
        {
            int cc=kk;
            for(c=0;c<m3.cols();c++)
            {
                if (m3[r][c] != cc++) {
                    ok = false;
                }
            }
            kk+=C;
        }
        CHECK(ok); // elements match
    }

    SECTION("check gsl.")
    {
        Matrix a(5, 5);
        Matrix b;
        b = a;
        CHECK(checkConsistency(a)); // gsldata consistent after creation
        CHECK(checkConsistency(b)); // gsldata consistent after copy
        b.resize(100, 100);
        CHECK(checkConsistency(b)); // gsldata consistent after resize

        Matrix s = a.submatrix(1, 1, 2, 2);
        checkConsistency(s);
        CHECK(checkConsistency(s)); // gsldata consistent for submatrix
        Matrix c = a;
        CHECK(checkConsistency(c)); // gsldata consistent after init
    }

    SECTION("check resize.")
    {
        Matrix ones;
        Matrix eye;
        Matrix resized;
        ones.resize(10, 10);
        ones=1.1; //set all values to 1.1

        resized=ones;
        resized.resize(12, 15);

        bool ok=true;
        for (unsigned int r = 0; r < 10; r++) {
            for (unsigned int c = 0; c < 10; c++) {
                ok = ok && (resized[r][c] == 1.1);
            }
        }

        CHECK(ok); // resize(int r, int c) keeps old values [1]

        eye.resize(5, 5);
        eye=0.0;

        for (unsigned int r=0; r<5; r++)
        {
            eye(r,r)=1.0;
        }

        resized=eye;

        resized.resize(6, 5);

        ok=true;
        for (unsigned int r = 0; r < 5; r++) {
            for (unsigned int c = 0; c < 5; c++) {
                ok = ok && (resized[r][c] == eye[r][c]);
            }
        }

        CHECK(ok); // resize(int r, int c) keeps old values [2]

        resized=ones;
        resized.resize(3, 5);
        ok=true;
        for (unsigned int r = 0; r < 3; r++) {
            for (unsigned int c = 0; c < 5; c++) {
                ok = ok && (resized[r][c] == ones[r][c]);
            }
        }

        CHECK(ok); // resizing to smaller size keeps old values [1]

        resized=eye;
        resized.resize(3,5);

        ok=true;
        for (unsigned int r = 0; r < 3; r++) {
            for (unsigned int c = 0; c < 5; c++) {
                ok = ok && (resized[r][c] == eye[r][c]);
            }
        }

        CHECK(ok); // resizing to smaller size keeps old values [2]

    }

    SECTION("check matrix format conforms to network standard...")
    {
        Matrix m;
        size_t rr = 10;
        size_t cc = 5;
        makeTestMatrix(m,rr,cc);

        BufferedConnectionWriter writer;
        m.write(writer);
        std::string s = writer.toString();
        Bottle bot;
        bot.fromBinary(s.c_str(),s.length());
        CHECK((size_t) bot.get(0).asInt32() == rr); // row count matches
        CHECK((size_t) bot.get(1).asInt32() == cc); // column count matches
        Bottle *lst = bot.get(2).asList();
        CHECK(lst!=nullptr); // have data
        if (!lst) {
            return;
        }
        CHECK(lst->size() == (rr*cc)); // data length matches
        if (lst->size() != (rr * cc)) {
            return;
        }
        bool ok = true;
        for (int i=0; i<(int)(rr*cc); i++) {
            double v = lst->get(i).asFloat64();
            if (fabs(v-i)>0.01) {
                ok = false;
                CHECK(v == Approx(i)); // "cell matches")
                break;
            }
        }
        CHECK(ok); // data matches
    }

    SECTION("checking portable-pair serialization...")
    {
        // potential problem reported by Miguel Sarabia Del Castillo

        Matrix m;
        size_t rr = 10;
        size_t cc = 5;
        makeTestMatrix(m,rr,cc);

        double value = 3.14;

        yarp::os::PortablePair<yarp::sig::Matrix, yarp::os::Value> msg, msg2;
        msg.head = m;
        msg.body = yarp::os::Value(value);

        DummyConnector con;
        msg.write(con.getWriter());
        ConnectionReader& reader = con.getReader();
        msg2.read(reader);
        CHECK(msg.head.rows() == msg2.head.rows()); // matrix row match
        CHECK(msg.head.cols() == msg2.head.cols()); // matrix col match
        CHECK(msg.body.asFloat64()== Approx(msg2.body.asFloat64())); // "value match"

        Bottle bot;
        bot.read(msg);
        Bottle *bot1 = bot.get(0).asList();
        Bottle *bot2 = bot.get(1).asList();
        CHECK((bot1!=nullptr&&bot2!=nullptr)); // got head/body
        if (bot1 == nullptr || bot2 == nullptr) {
            return;
        }
        CHECK((size_t) bot1->get(0).asInt32() == rr); // row count matches
        CHECK((size_t) bot1->get(1).asInt32() == cc); // column count matches
        Bottle *lst = bot1->get(2).asList();
        CHECK(lst!=nullptr); // have data
        if (!lst) {
            return;
        }
        CHECK(lst->size() == (rr*cc)); // data length matches
        CHECK(bot2->get(0).asFloat64() == Approx(value)); // "value match"
    }

    SECTION("check data() when matrix is empty...")
    {
        Matrix m;
        m.resize(0,0);
        CHECK(m.data()==nullptr); // size 0x0 => null data()
        m.resize(0,5);
        CHECK(m.data()==nullptr); // size 0x5 => null data()
        m.resize(5,0);
        CHECK(m.data()==nullptr); // size 5x0 => null data()
        m.resize(5,5);
        CHECK(m.data()!=nullptr); // size 5x5 => non-null data()
        // This is *not* redundant with earlier test
        m.resize(0,0);
        CHECK(m.data()==nullptr); // size 0x0 => null data()
    }

    SECTION("check toString")
    {
        {
            INFO("testing toString");
            Matrix m;
            m.resize(10,10);
            std::string strToCheck = "\
 0.000000\t 0.100000\t 0.200000\t 0.300000\t 0.400000\t 0.500000\t 0.600000\t 0.700000\t 0.800000\t 0.900000\n\
 1.000000\t 1.100000\t 1.200000\t 1.300000\t 1.400000\t 1.500000\t 1.600000\t 1.700000\t 1.800000\t 1.900000\n\
 2.000000\t 2.100000\t 2.200000\t 2.300000\t 2.400000\t 2.500000\t 2.600000\t 2.700000\t 2.800000\t 2.900000\n\
 3.000000\t 3.100000\t 3.200000\t 3.300000\t 3.400000\t 3.500000\t 3.600000\t 3.700000\t 3.800000\t 3.900000\n\
 4.000000\t 4.100000\t 4.200000\t 4.300000\t 4.400000\t 4.500000\t 4.600000\t 4.700000\t 4.800000\t 4.900000\n\
 5.000000\t 5.100000\t 5.200000\t 5.300000\t 5.400000\t 5.500000\t 5.600000\t 5.700000\t 5.800000\t 5.900000\n\
 6.000000\t 6.100000\t 6.200000\t 6.300000\t 6.400000\t 6.500000\t 6.600000\t 6.700000\t 6.800000\t 6.900000\n\
 7.000000\t 7.100000\t 7.200000\t 7.300000\t 7.400000\t 7.500000\t 7.600000\t 7.700000\t 7.800000\t 7.900000\n\
 8.000000\t 8.100000\t 8.200000\t 8.300000\t 8.400000\t 8.500000\t 8.600000\t 8.700000\t 8.800000\t 8.900000\n\
 9.000000\t 9.100000\t 9.200000\t 9.300000\t 9.400000\t 9.500000\t 9.600000\t 9.700000\t 9.800000\t 9.900000\
";

            for (size_t i=0; i<10; i++) {
                for (size_t j=0; j<10; j++) {
                    m(i, j) = (i + static_cast<double>(j)/10);
                }
            }
            CHECK(m.toString() == strToCheck); // string correctly formatted
        }

        {
            INFO("testing toString with custom end of row");
            Matrix m;
            m.resize(10,10);
            std::string strToCheck = "\
 0.000000\t 0.100000\t 0.200000\t 0.300000\t 0.400000\t 0.500000\t 0.600000\t 0.700000\t 0.800000\t 0.900000\t\
 1.000000\t 1.100000\t 1.200000\t 1.300000\t 1.400000\t 1.500000\t 1.600000\t 1.700000\t 1.800000\t 1.900000\t\
 2.000000\t 2.100000\t 2.200000\t 2.300000\t 2.400000\t 2.500000\t 2.600000\t 2.700000\t 2.800000\t 2.900000\t\
 3.000000\t 3.100000\t 3.200000\t 3.300000\t 3.400000\t 3.500000\t 3.600000\t 3.700000\t 3.800000\t 3.900000\t\
 4.000000\t 4.100000\t 4.200000\t 4.300000\t 4.400000\t 4.500000\t 4.600000\t 4.700000\t 4.800000\t 4.900000\t\
 5.000000\t 5.100000\t 5.200000\t 5.300000\t 5.400000\t 5.500000\t 5.600000\t 5.700000\t 5.800000\t 5.900000\t\
 6.000000\t 6.100000\t 6.200000\t 6.300000\t 6.400000\t 6.500000\t 6.600000\t 6.700000\t 6.800000\t 6.900000\t\
 7.000000\t 7.100000\t 7.200000\t 7.300000\t 7.400000\t 7.500000\t 7.600000\t 7.700000\t 7.800000\t 7.900000\t\
 8.000000\t 8.100000\t 8.200000\t 8.300000\t 8.400000\t 8.500000\t 8.600000\t 8.700000\t 8.800000\t 8.900000\t\
 9.000000\t 9.100000\t 9.200000\t 9.300000\t 9.400000\t 9.500000\t 9.600000\t 9.700000\t 9.800000\t 9.900000\
";

            for (size_t i=0; i<10; i++) {
                for (size_t j=0; j<10; j++) {
                    m(i, j) = (i + static_cast<double>(j)/10);
                }
            }
            CHECK(m.toString(-1, -1, "\t") == strToCheck); // string correctly formatted
        }


        {
            INFO("testing toString with custom end of row");
            Matrix m;
            m.resize(10,10);
            std::string strToCheck = "\
 0.0  0.1  0.2  0.3  0.4  0.5  0.6  0.7  0.8  0.9\n\
 1.0  1.1  1.2  1.3  1.4  1.5  1.6  1.7  1.8  1.9\n\
 2.0  2.1  2.2  2.3  2.4  2.5  2.6  2.7  2.8  2.9\n\
 3.0  3.1  3.2  3.3  3.4  3.5  3.6  3.7  3.8  3.9\n\
 4.0  4.1  4.2  4.3  4.4  4.5  4.6  4.7  4.8  4.9\n\
 5.0  5.1  5.2  5.3  5.4  5.5  5.6  5.7  5.8  5.9\n\
 6.0  6.1  6.2  6.3  6.4  6.5  6.6  6.7  6.8  6.9\n\
 7.0  7.1  7.2  7.3  7.4  7.5  7.6  7.7  7.8  7.9\n\
 8.0  8.1  8.2  8.3  8.4  8.5  8.6  8.7  8.8  8.9\n\
 9.0  9.1  9.2  9.3  9.4  9.5  9.6  9.7  9.8  9.9\
";

            for (size_t i=0; i<10; i++) {
                for (size_t j=0; j<10; j++) {
                    m(i, j) = (i + static_cast<double>(j)/10);
                }
            }
            CHECK(m.toString(1, 3) == strToCheck); // string correctly formatted
        }

    }

    NetworkBase::setLocalMode(false);
}
