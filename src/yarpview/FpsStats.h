#ifndef __FPSSTATS__
#define __FPSSTATS__

class FpsStats
{
public:
    double average;
    double max;
    double min;
    unsigned int iterations;
    double now;
    double prev;
    double accDt;
    
    FpsStats()
    {
        reset();
    }

    void reset()
    {
        average=0;
        max=0;
        min=1e20;
        iterations=0;
        prev=0;
        now=0;
        accDt=0;
    }

    void update(double nt)
    {
        now=nt;
        if (iterations>0)
        {
            double tmp=now-prev;
            accDt+=tmp;
            if (tmp>max)
                max=tmp;
            if (tmp<min)
                min=tmp;
        }

        prev=nt;

        iterations++;
    }

    void getStats(double &av, double &m, double &M)
    {
        if (iterations<2)
        {
            av=0;
            m=0;
            M=0;
        }
        else
        {
            av=accDt/(iterations-1);
            m=min;
            M=max;
        }
    }
    
    void getStats(double &av)
    {
        if (iterations<2)
            av=0;
        else
            av=accDt/(iterations-1);
    }
};

#endif