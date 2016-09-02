/*
* Author: Lorenzo Natale.
* Copyright (C) 2006 The Robotcub consortium
* CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
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
#include <yarp/gsl_compatibility.h>

#include <math.h>
#include <vector>

#include "TestList.h"

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

    bool threadInit()
    {
        success=false;
        return true;
    }

    void run()
    {
        Matrix m;

        int times=10;

        while(times--)
        {
            m.resize(4,4);
            int r=0;
            int c=0;
            for(r=0; r<4; r++)
                for (c=0; c<4; c++)
                    m[r][c]=99;

            portOut->write(m);
            Time::delay(0.1);

            m.resize(2,4);
            for(r=0; r<2; r++)
                for (c=0; c<4; c++)
                    m[r][c]=66;

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

    bool threadInit()
    {
        success=false;
        return true;
    }

    void run()
    {
        Matrix m;

        int times=10;
        bool ok=true;
        while(times--)
        {
            portIn->read(m);
            if ( (m.rows()!=4)||(m.cols()!=4))
                ok=false;


            portIn->read(m);

            if ( (m.rows()!=2)||(m.cols()!=4))
                ok=false;
        }

        success=ok;
    }

    Port *portIn;
    bool success;
};

class MatrixTest : public UnitTest {

    bool checkConsistency(Matrix &a)
    {
        gsl_matrix *tmp;

        yarp::gsl::GslMatrix tmpGSL(a);
        tmp=(gsl_matrix *)(tmpGSL.getGslMatrix());

        bool ret=true;
        if ((int)tmp->size1!=a.rows())
            ret=false;

        if ((int)tmp->size2!=a.cols())
            ret=false;

        if ((int)tmp->block->size!=a.cols()*a.rows())
            ret=false;

        if (tmp->data!=a.data())
            ret=false;

        if (tmp->block->data!=a.data())
            ret=false;

        return ret;
    }

public:
    virtual ConstString getName() { return "MatrixTest"; }

    void checkOperators()
    {
        report(0,"checking operator ==");

        Matrix M1(3,3);
        Matrix M2(3,3);

        M1=1;
        M2=1; //now we have to identical vectors

        bool ok=false;
        if (M1==M2)
            ok=true;

        M1=2;
        M2=1; //now vectors are different
        if (M1==M2)
            ok=false;

        checkTrue(ok, "operator== for matrix work");
    }

    void checkSendReceive()
    {
        report(0,"check matrix send/receive");
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

        checkTrue(senderThread->success, "Send matrix test");
        checkTrue(receiverThread->success, "Receive matrix test");

        delete receiverThread;
        delete senderThread;
    }

    void checkCopyCtor()
    {
        report(0,"check matrix copy constructor works...");
        Matrix m(10,40);
        int r=0;
        int c=0;
        for(r=0; r<10; r++)
        {
            for (c=0; c<40; c++)
                m[r][c]=1333;
        }

        Matrix m2(m);
        checkEqual(m.rows(),m2.rows(),"rows matches");
        checkEqual(m.cols(),m2.cols(),"cols matches");

        bool ok=true;
        for(r=0; r<10; r++)
            for (c=0; c<40; c++)
                ok=ok && ((m[r])[c]==(m2[r])[c]);

        checkTrue(ok,"elements match");

        report(0,"check matrix construction from empty matrix");
        Matrix empty1;
        Matrix empty2(empty1);
    }

    void checkCopy() {
        report(0,"check matrix copy operator works...");
        Matrix m(10,40);
        int r=0;
        int c=0;
        for(r=0; r<10; r++)
            for (c=0; c<40; c++)
                m[r][c]=99;

        Matrix m2;
        m2=m;
        checkEqual(m.rows(),m2.rows(),"rows matches");
        checkEqual(m.cols(),m2.cols(),"cols matches");

        bool ok=true;
        for(r=0; r<10; r++)
            for (c=0; c<40; c++)
                ok=ok && (m[r][c]==m2[r][c]);
        checkTrue(ok,"elements match");
    }

    void checkBottle() {
        report(0,"check bottle compatibility...");
        Bottle b("2 3 (0.0 1.1 2.2 3.3 4.4 5.5)");
        Matrix m(6,1);
        DummyConnector con;
        b.write(con.getWriter());
        m.read(con.getReader());
        checkEqual(m.rows(),2,"row size correct");
        checkEqual(m.cols(),3,"col size correct");
        checkTrue(m[1][2]>5 && m[1][2]<6, "content is sane");
    }

    void checkSubmatrix()
    {
        report(0,"check function Matrix::submatrix works...");
        const int R=10;
        const int C=20;
        Matrix m(R,C);

        int r=0;
        int c=0;
        int kk=0;
        for(r=0; r<R; r++)
            for (c=0; c<C; c++)
                m[r][c]=kk++;

        report(0,"extracting submatrix...");
        int r1=5;
        int r2=8;
        int c1=4;
        int c2=8;
        Matrix m2=m.submatrix(r1, r2, c1, c2);

        checkEqual(r2-r1+1,m2.rows(),"rows matches");
        checkEqual(c2-c1+1,m2.cols(),"cols matches");

        kk=r1*C+c1;
        bool ok=true;
        for(r=0; r<m2.rows(); r++)
        {
            int cc=kk;
            for(c=0;c<m2.cols();c++)
            {
                if (m2[r][c]!=cc++)
                    ok=false;
            }
            kk+=C;
        }

        checkTrue(ok,"elements match");

        report(0,"extracting full size matrix...");
        Matrix m3=m.submatrix(0, R-1, 0, C-1);
        checkEqual(R,m3.rows(),"rows matches");
        checkEqual(C,m3.cols(),"cols matches");

        kk=0;
        ok=true;
        for(r=0; r<m3.rows(); r++)
        {
            int cc=kk;
            for(c=0;c<m3.cols();c++)
            {
                if (m3[r][c]!=cc++)
                    ok=false;
            }
            kk+=C;
        }
        checkTrue(ok,"elements match");
    }

    void checkGsl()
    {
        Matrix a(5, 5);
        Matrix b;
        b = a;
        checkTrue(checkConsistency(a), "gsldata consistent after creation");
        checkTrue(checkConsistency(b), "gsldata consistent after copy");
        b.resize(100, 100);
        checkTrue(checkConsistency(b), "gsldata consistent after resize");

        Matrix s = a.submatrix(1, 1, 2, 2);
        checkConsistency(s);
        checkTrue(checkConsistency(s), "gsldata consistent for submatrix");
        Matrix c = a;
        checkTrue(checkConsistency(c), "gsldata consistent after init");
    }

    void checkResize()
    {
        Matrix ones;
        Matrix eye;
        Matrix resized;
        ones.resize(10, 10);
        ones=1.1; //set all values to 1.1

        resized=ones;
        resized.resize(12, 15);

        bool ok=true;
        for(unsigned int r=0; r<10; r++)
            for(unsigned int c=0; c<10; c++)
                ok=ok&&(resized[r][c]==1.1);

        checkTrue(ok,"resize(int r, int c) keeps old values [1]");

        eye.resize(5, 5);
        eye=0.0;

        for (unsigned int r=0; r<5; r++)
        {
            eye(r,r)=1.0;
        }

        resized=eye;

        resized.resize(6, 5);

        ok=true;
        for(unsigned int r=0; r<5; r++)
            for(unsigned int c=0; c<5; c++)
                ok=ok&&(resized[r][c]==eye[r][c]);

        checkTrue(ok,"resize(int r, int c) keeps old values [2]");

        resized=ones;
        resized.resize(3, 5);
        ok=true;
        for(unsigned int r=0; r<3; r++)
            for(unsigned int c=0; c<5; c++)
                ok=ok&&(resized[r][c]==ones[r][c]);

        checkTrue(ok,"resizing to smaller size keeps old values [1]");

        resized=eye;
        resized.resize(3,5);

        ok=true;
        for(unsigned int r=0; r<3; r++)
            for(unsigned int c=0; c<5; c++)
                ok=ok&&(resized[r][c]==eye[r][c]);

        checkTrue(ok,"resizing to smaller size keeps old values [2]");

    }

    void makeTestMatrix(Matrix& m, unsigned int rr, unsigned int cc) {
        m.resize((int)rr,(int)cc);
        for(unsigned int r=0; r<rr; r++) {
            for(unsigned int c=0; c<cc; c++) {
                m[r][c] = r*cc+c;
            }
        }
    }

    void checkFormat() {
        report(0,"check matrix format conforms to network standard...");

        Matrix m;
        unsigned int rr = 10;
        unsigned int cc = 5;
        makeTestMatrix(m,rr,cc);

        BufferedConnectionWriter writer;
        m.write(writer);
        ConstString s = writer.toString();
        Bottle bot;
        bot.fromBinary(s.c_str(),(int)s.length());
        checkEqual(bot.get(0).asInt(),rr,"row count matches");
        checkEqual(bot.get(1).asInt(),cc,"column count matches");
        Bottle *lst = bot.get(2).asList();
        checkTrue(lst!=NULL,"have data");
        if (!lst) return;
        checkEqual(lst->size(),(int)(rr*cc),"data length matches");
        if (lst->size()!=(int)(rr*cc)) return;
        bool ok = true;
        for (int i=0; i<(int)(rr*cc); i++) {
            double v = lst->get(i).asDouble();
            if (fabs(v-i)>0.01) {
                ok = false;
                checkEqualish(v,i,"cell matches");
                break;
            }
        }
        checkTrue(ok,"data matches");
    }

    void checkPair() {
        report(0,"checking portable-pair serialization...");
        // potential problem reported by Miguel Sarabia Del Castillo

        Matrix m;
        unsigned int rr = 10;
        unsigned int cc = 5;
        makeTestMatrix(m,rr,cc);

        double value = 3.14;

        yarp::os::PortablePair<yarp::sig::Matrix, yarp::os::Value> msg, msg2;
        msg.head = m;
        msg.body = yarp::os::Value(value);

        DummyConnector con;
        msg.write(con.getWriter());
        ConnectionReader& reader = con.getReader();
        msg2.read(reader);
        checkEqual(msg.head.rows(),msg2.head.rows(),"matrix row match");
        checkEqual(msg.head.cols(),msg2.head.cols(),"matrix col match");
        checkEqualish(msg.body.asDouble(),msg2.body.asDouble(),"value match");

        Bottle bot;
        bot.read(msg);
        Bottle *bot1 = bot.get(0).asList();
        Bottle *bot2 = bot.get(1).asList();
        checkTrue(bot1!=NULL&&bot2!=NULL,"got head/body");
        if (bot1==NULL&&bot2==NULL) return;
        checkEqual(bot1->get(0).asInt(),rr,"row count matches");
        checkEqual(bot1->get(1).asInt(),cc,"column count matches");
        Bottle *lst = bot1->get(2).asList();
        checkTrue(lst!=NULL,"have data");
        if (!lst) return;
        checkEqual(lst->size(),(int)(rr*cc),"data length matches");
        checkEqualish(bot2->get(0).asDouble(),value,"value match");
    }

    void checkEmpty() {
        report(0,"check data() when matrix is empty...");
        Matrix m;
        m.resize(0,0);
        checkTrue(m.data()==NULL, "size 0x0 => null data()");
        m.resize(0,5);
        checkTrue(m.data()==NULL, "size 0x5 => null data()");
        m.resize(5,0);
        checkTrue(m.data()==NULL, "size 5x0 => null data()");
        m.resize(5,5);
        checkTrue(m.data()!=NULL, "size 5x5 => non-null data()");
        // This is *not* redundant with earlier test
        m.resize(0,0);
        checkTrue(m.data()==NULL, "size 0x0 => null data()");
    }

    virtual void runTests() {
            Network::setLocalMode(true);
            checkCopyCtor();
            checkCopy();
            checkResize();
            checkSubmatrix();
            checkGsl();

            checkBottle();
            checkSendReceive();

            checkFormat();
            checkPair();

            checkEmpty();

            Network::setLocalMode(false);
        }
};

static MatrixTest theMatrixTest;

UnitTest& getMatrixTest() {
    return theMatrixTest;
}
