#ifndef __CONTROLBOARDIMPL__
#define __CONTROLBOARDIMPL__

#include <yarp/dev/ControlBoardInterfaces.h>

#include <ace/Log_Msg.h>
#include <stdio.h>
#include <ace/OS.h>

template <class T>
inline void checkAndDestroy(T *p)
{
    if (p!=0)
        delete [] p;
}

class ControlBoardHelper
{
public:
    ControlBoardHelper(int n, const int *aMap, const double *angToEncs, const double *zs): zeros(0), 
        signs(0),
        axisMap(0),
        invAxisMap(0),
        angleToEncoders(0)
    {
        nj=n;
        alloc(n);
        
        memcpy(axisMap, aMap, sizeof(int)*nj);
        memcpy(zeros, zs, sizeof(double)*nj);
        memcpy(angleToEncoders, angToEncs, sizeof(double)*nj);
        // invert the axis map
   		memset (invAxisMap, 0, sizeof(int) * nj);
		int i;
        for (i = 0; i < nj; i++)
		{
			int j;
			for (j = 0; j < nj; j++)
			{
				if (axisMap[j] == i)
				{
					invAxisMap[i] = j;
					break;
				}
			}
		}


    }

    bool alloc(int n)
    {
        nj=n;
        if (nj<=0)
            return false;

        if (zeros==0)
            dealloc();

        zeros=new double [nj];
        signs=new double [nj];
        axisMap=new int [nj];
        invAxisMap=new int [nj];
        angleToEncoders=new double [nj];

        return true;
    }

    bool dealloc()
    {
        checkAndDestroy<double> (zeros);
        checkAndDestroy<double> (signs);
        checkAndDestroy<int> (axisMap);
        checkAndDestroy<int> (invAxisMap);
        checkAndDestroy<double> (angleToEncoders);
        return true;
    }

    inline int toHw(int axis)
    { return axisMap[axis]; }
    
    inline int toUser(int axis)
    { return invAxisMap[axis]; }

    //map a vector, no conversion
    inline void toUser(const double *hwData, double *user)
    {
        for (int k=0;k<nj;k++)
         user[toUser(k)]=hwData[k];
    }

    //map a vector, no conversion
    inline void toHw(const double *usr, double *hwData)
    {
        for (int k=0;k<nj;k++)
            hwData[toHw(k)]=usr[k];
    }

    inline void posA2E(double ang, int j, double &enc, int &k)
    {
        enc=(ang*angleToEncoders[j])+zeros[j];
        k=toHw(j);
    }

    inline void posE2A(double enc, int j, double &ang, int &k)
    {
        k=toUser(j);
        ang=(zeros[k]-enc)/angleToEncoders[k];
    }

    inline void velA2E(double ang, int j, double &enc, int &k)
    {
        k=toHw(j);
        enc=ang*angleToEncoders[j];
    }

    inline void velE2A(double enc, int j, double &ang, int &k)
    {
        k=toUser(j);
        ang=enc/angleToEncoders[k];
    }

    inline void accA2E(double ang, int j, double &enc, int &k)
    {
        velA2E(ang, j, enc, k);
    }

    inline void accE2A(double enc, int j, double &ang, int &k)
    {
        velE2A(enc, j, ang, k);
    }

    inline double velE2A(double enc, int j)
    {
        int k=toUser(j);
        return enc/angleToEncoders[k];
    }

    inline double accE2A(double enc, int j)
    {
        return velE2A(enc, j);
    }

    //map a vector, convert from angles to encoders
    inline void posA2E(const double *ang, double *enc)
    {
        double tmp;
        int index;
        for(int j=0;j<nj;j++)
        {
            posA2E(ang[j], j, tmp, index);
            enc[index]=tmp;
        }
    }

    //map a vector, convert from encoders to angles
    inline void posE2A(const double *enc, double *ang)
    {
        double tmp;
        int index;
        for(int j=0;j<nj;j++)
        {
            posE2A(enc[j], j, tmp, index);
            ang[index]=tmp;
        }
    }

    inline void velA2E(const double *ang, double *enc)
    {
        double tmp;
        int index;
        for(int j=0;j<nj;j++)
        {
            velA2E(ang[j], j, tmp, index);
            enc[index]=tmp;
        }
    }

    inline void velE2A(const double *enc, double *ang)
    {
        double tmp;
        int index;
        for(int j=0;j<nj;j++)
        {
            velE2A(enc[j], j, tmp, index);
            ang[index]=tmp;
        }
    }

    inline void accA2E(const double *ang, double *enc)
    {
        double tmp;
        int index;
        for(int j=0;j<nj;j++)
        {
            accA2E(ang[j], j, tmp, index);
            enc[index]=tmp;
        }
    }

    inline void accE2A(const double *enc, double *ang)
    {
        double tmp;
        int index;
        for(int j=0;j<nj;j++)
        {
            accE2A(enc[j], j, tmp, index);
            ang[index]=tmp;
        }
    }

    inline int axes()
    { return nj; }
        

 	int nj;

	double *zeros;
	double *signs;
	int *axisMap;
	int *invAxisMap;
	double *angleToEncoders;
};

inline ControlBoardHelper *castToMapper(void *p)
{ return static_cast<ControlBoardHelper *>(p); }

/** 
* Default implementation of the IPositionControl interface. This template class can
* be used to easily provide an implementation of IPositionControl. It takes two
* arguments, the class it is derived from and the class it is implementing, typically
* IPositionControl (which should probably be removed from the template arguments).
* <IMPLEMENT> makes only explicit that the class is implementing IPositionControl and
* appears in the inheritance list of the derived class.
*/
template <class DERIVED, class IMPLEMENT> class ImplementPositionControl : public IMPLEMENT
{
        protected:
            yarp::dev::IPositionControlRaw *iPosition;
            void *helper;
            double *temp;
            
        public:
        /**
        * Constructor.
        * @param y is the pointer to the class instance inheriting from this 
        *  implementation.
            */
            ImplementPositionControl(DERIVED *y);
            
            /**
            * Destructor. Perform uninitialize if needed.
            */
            virtual ~ImplementPositionControl();
            
            /**
            * Initialize the internal data and alloc memory.
            * @param size is the number of controlled axes the driver deals with.
            * @param amap is a lookup table mapping axes onto physical drivers.
            * @param enc is an array containing the encoder to angles conversion factors.
            * @param zos is an array containing the zeros of the encoders.
            *  respect to the control/output values of the driver.
            * @return true if initialized succeeded, false if it wasn't executed, or assert.
            */
            bool initialize (int size, int *amap, double *enc, double *zos);
            
            /**
            * Clean up internal data and memory.
            * @return true if uninitialization is executed, false otherwise.
            */
            bool uninitialize ();
            
            /**
            * Get the number of controlled axes. This command asks the number of controlled
            * axes for the current physical interface.
            * @return the number of controlled axes.
            */
            virtual bool getAxes(int *axis) const;
            
            /** 
            * Set position mode. This command
            * is required by control boards implementing different
            * control methods (e.g. velocity/torque), in some cases
            * the implementation can be left empty.
            * @return true/false on success failure
            */
            virtual bool setPositionMode();
            virtual bool positionMove(int j, double ref);
            virtual bool positionMove(const double *refs);
            virtual bool relativeMove(int j, double delta);
            virtual bool relativeMove(const double *deltas);
            virtual bool checkMotionDone(bool *flag);
            virtual bool checkMotionDone(int j, bool *flag);
            virtual bool setRefSpeed(int j, double sp);
            virtual bool setRefSpeeds(const double *spds);
            virtual bool setRefAcceleration(int j, double acc);
            virtual bool setRefAccelerations(const double *accs);
            virtual bool getRefSpeed(int j, double *ref);
            virtual bool getRefSpeeds(double *spds);
            virtual bool getRefAcceleration(int j, double *acc);
            virtual bool getRefAccelerations(double *accs);
            virtual bool stop(int j);
            virtual bool stop();
};  

#endif