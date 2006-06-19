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

bool printDebug(const char *string)
{
    fprintf(stderr, "%s\n", string);
    return true;
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
            ImplementPositionControl(DERIVED *y)
            {
                iPosition = dynamic_cast<IPositionControlRaw *> (y);
                helper = 0;        
                temp=0;
            }
            
            /**
            * Destructor. Perform uninitialize if needed.
            */
            virtual ~ImplementPositionControl()
            {
                if (helper!=0) uninitialize();
            }
            
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

template <class DERIVED, class IMPLEMENT> bool ImplementPositionControl<DERIVED, IMPLEMENT>::positionMove(int j, double v)
{
    fprintf(stderr, "Calling \n");
    return iPosition->positionMoveRaw(j, v);
}

template <class DERIVED, class IMPLEMENT> bool ImplementPositionControl<DERIVED, IMPLEMENT>::setPositionMode()
{
    ACE_OS::printf ("called setPositionMode in ImplementPositionControl\n");
    if (helper==0) return false;
    iPosition->setPositionModeRaw();
    return true;
}

template <class DERIVED, class IMPLEMENT> bool ImplementPositionControl<DERIVED, IMPLEMENT>::positionMove(const double *refs)
{
    castToMapper(helper)->posA2E(refs, temp);
    
    return iPosition->positionMoveRaw(temp);
}

template <class DERIVED, class IMPLEMENT> bool ImplementPositionControl<DERIVED, IMPLEMENT>::relativeMove(int j, double delta)
{
    int k;
    double enc;
    castToMapper(helper)->velA2E(delta, j, enc, k);
    
    return iPosition->relativeMoveRaw(k,enc);
}

template <class DERIVED, class IMPLEMENT> bool ImplementPositionControl<DERIVED, IMPLEMENT>::relativeMove(const double *deltas)
{
    castToMapper(helper)->velA2E(deltas, temp);
    
    return iPosition->relativeMoveRaw(temp);
}

template <class DERIVED, class IMPLEMENT> bool ImplementPositionControl<DERIVED, IMPLEMENT>::checkMotionDone(int j, bool *flag)
{
    int k=castToMapper(helper)->toHw(j);
    
    return iPosition->checkMotionDoneRaw(k,flag);
}

template <class DERIVED, class IMPLEMENT> bool ImplementPositionControl<DERIVED, IMPLEMENT>::checkMotionDone(bool *flag)
{
    return iPosition->checkMotionDoneRaw(flag);
}

template <class DERIVED, class IMPLEMENT> bool ImplementPositionControl<DERIVED, IMPLEMENT>::setRefSpeed(int j, double sp)
{
    int k;
    double enc;
    castToMapper(helper)->velA2E(sp, j, enc, k);
    return iPosition->setRefSpeedRaw(k, enc);
}

template <class DERIVED, class IMPLEMENT> bool ImplementPositionControl<DERIVED, IMPLEMENT>::setRefSpeeds(const double *spds)
{
    castToMapper(helper)->velA2E(spds, temp);
    
    return iPosition->setRefSpeedsRaw(temp);
}

template <class DERIVED, class IMPLEMENT> bool ImplementPositionControl<DERIVED, IMPLEMENT>::setRefAcceleration(int j, double acc)
{
    int k;
    double enc;
    
    castToMapper(helper)->accA2E(acc, j, enc, k);
    return iPosition->setRefAccelerationRaw(k, enc);
}

template <class DERIVED, class IMPLEMENT> bool ImplementPositionControl<DERIVED, IMPLEMENT>::setRefAccelerations(const double *accs)
{
    castToMapper(helper)->accA2E(accs, temp);
    
    return iPosition->setRefAccelerationsRaw(temp);
}

template <class DERIVED, class IMPLEMENT> bool ImplementPositionControl<DERIVED, IMPLEMENT>::getRefSpeed(int j, double *ref)
{
    int k;
    double enc;
    k=castToMapper(helper)->toHw(j);
    
    bool ret = iPosition->getRefSpeedRaw(k, &enc);
    
    *ref=(castToMapper(helper)->velE2A(enc, k));
    
    return true;
}

template <class DERIVED, class IMPLEMENT> bool ImplementPositionControl<DERIVED, IMPLEMENT>::getRefSpeeds(double *spds)
{
    bool ret=iPosition->getRefSpeedsRaw(temp);
    castToMapper(helper)->velE2A(temp, spds);
    return true;
}

template <class DERIVED, class IMPLEMENT> bool ImplementPositionControl<DERIVED, IMPLEMENT>::getRefAccelerations(double *accs)
{
    bool ret=iPosition->getRefSpeedsRaw(temp);
    castToMapper(helper)->accE2A(temp, accs);
    return true;
}

template <class DERIVED, class IMPLEMENT> bool ImplementPositionControl<DERIVED, IMPLEMENT>::getRefAcceleration(int j, double *acc)
{
    int k;
    double enc;
    k=castToMapper(helper)->toHw(j);
    bool ret = iPosition->getRefAccelerationRaw(k, &enc);
    
    *acc=castToMapper(helper)->accE2A(enc, k);
    
    return ret;
}

template <class DERIVED, class IMPLEMENT> bool ImplementPositionControl<DERIVED, IMPLEMENT>::stop(int j)
{
    int k;
    k=castToMapper(helper)->toHw(j);
    
    return iPosition->stopRaw(k);
}

template <class DERIVED, class IMPLEMENT> bool ImplementPositionControl<DERIVED, IMPLEMENT>::stop()
{
    return iPosition->stopRaw();
}

template <class DERIVED, class IMPLEMENT> bool ImplementPositionControl<DERIVED, IMPLEMENT>::getAxes(int *axis) const
{
    if (helper==0)
        return false;
    else
        (*axis)=castToMapper(helper)->axes();
    
    return true;     
}

template <class DERIVED, class IMPLEMENT> 
bool ImplementPositionControl<DERIVED, IMPLEMENT>:: initialize (int size, int *amap, double *enc, double *zos)
{
    if (helper!=0)
        return false;
    
    helper=(void *)(new ControlBoardHelper(size, amap, enc, zos));
    temp=new double [size];
    
    return true;
}

/**
* Clean up internal data and memory.
* @return true if uninitialization is executed, false otherwise.
*/
template <class DERIVED, class IMPLEMENT> 
bool ImplementPositionControl<DERIVED, IMPLEMENT>::uninitialize ()
{
    if (helper!=0)
        delete castToMapper(helper);
    
    delete [] temp;
    
    helper=0;
    temp=0;
    return true;
}



#if 0


       /** 
        * Default implementation of the IVelocityControl interface. This template class can
        * be used to easily provide an implementation of IVelocityControl. It takes two
        * arguments, the class it is derived from and the class it is implementing, typically
        * IVelocityControl (which should probably be removed from the template arguments).
        * <IMPLEMENT> makes only explicit that the class is implementing IVelocityControl and
        * appears in the inheritance list of the derived class.
        */
        template <class DERIVED, class IMPLEMENT> class ImplementVelocityControl : public IMPLEMENT
        {
        protected:
            IVelocityControlRaw *x;
            bool set;
            int nj;
            int *axisMap;
            int *invAxisMap;
            double *angleToEncoder;
            double *signs;
            
        public:
        /**
        * Constructor.
        * @param y is the pointer to the class instance inheriting from this 
        *  implementation.
            */
            ImplementVelocityControl(DERIVED *y)
            {
                x = dynamic_cast<IVelocityControlRaw *> (y);
                nj = 0;
                axisMap = NULL;
                invAxisMap = NULL;
                angleToEncoder = NULL;
                signs = NULL;
                set = false;
            }
            
            /**
            * Destructor. Perform uninitialize if needed.
            */
            virtual ~ImplementVelocityControl()
            {
                if (set) uninitialize();
            }
            
            /**
            * Initialize the internal data and alloc memory.
            * @param size is the number of controlled axes the driver deals with.
            * @param amap is a lookup table mapping axes onto physical drivers.
            * @param enc is an array containing the encoder to angles conversion factors.
            * @param sgns is an array containing the signs of the encoder readings with
            *  respect to the control/output values of the driver.
            * @return true if initialized succeeded, false if it wasn't executed, or assert.
            */
            bool initialize (int size, int *amap, double *enc, double *sgns)
            {
                if (set)
                    return false;
                
                ACE_ASSERT(size > 0);
                nj = size;
                axisMap = new int[nj];
                ACE_ASSERT (axisMap != NULL);
                ACE_OS::memcpy(axisMap, amap, sizeof(int)*nj);
                invAxisMap = new int[nj];
                ACE_ASSERT (invAxisMap != NULL);
                
                int i, j;
                for (i = 0; i < nj; i++)
                    for (j = 0; j < nj; j++)
                    {
                        if (axisMap[j] == i)
                        {
                            invAxisMap[i] = j;
                            break;
                        }
                    }
                    
                    angleToEncoder = new double[nj];
                    ACE_ASSERT (angleToEncoder != NULL);
                    ACE_OS::memcpy(angleToEncoder, enc, sizeof(double)*nj);
                    signs = new double[nj];
                    ACE_ASSERT (signs != NULL);
                    ACE_OS::memcpy(signs, sgns, sizeof(double)*nj);
                    
                    set = true;
                    return true;
            }
            
            /**
            * Clean up internal data and memory.
            * @return true if uninitialization is executed, false otherwise.
            */
            bool uninitialize ()
            {
                if (!set)
                    return false;
                
                if (axisMap != NULL) delete[] axisMap;
                if (invAxisMap != NULL) delete[] invAxisMap;
                if (angleToEncoder != NULL) delete[] angleToEncoder;
                if (signs != NULL) delete[] signs;
                
                set = false;
                return true;
            }
            
            /**
            * Get the number of controlled axes. This command asks the number of controlled
            * axes for the current physical interface.
            * @return the number of controlled axes.
            */
            virtual int getAxes() const
            {
                if (!set) 
                    return -1;
                return nj;     
            }
            
            virtual bool setVelocityMode() 
            {
                ACE_OS::printf ("called setVelocityMode in ImplementVelocityControl\n");
                x->setVelocityModeRaw();
                return true; 
            }
            
            virtual bool velocityMove(int j, double sp);
            virtual bool velocityMove(const double *sp);
            virtual bool setRefAcceleration(int j, double acc) { return false; }
            virtual bool setRefAccelerations(const double *accs) { return false; }
            virtual bool getRefAcceleration(int j, double *acc) { return false; }
            virtual bool getRefAccelerations(double *accs) { return false; }
            virtual bool stop(int j) { return false; }
            virtual bool stop() { return false; }
        };
    }
}



#endif