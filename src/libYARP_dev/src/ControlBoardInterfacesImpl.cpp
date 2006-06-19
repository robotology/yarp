#include <yarp/dev/ControlBoardInterfacesImpl.h>

#include <yarp/EsdMotionControl.h>

template <class DERIVED, class IMPLEMENT> ImplementPositionControl<DERIVED, IMPLEMENT>::
ImplementPositionControl(DERIVED *y)
{
    iPosition = dynamic_cast<IPositionControlRaw *> (y);
    helper = 0;        
    temp=0;
}

template <class DERIVED, class IMPLEMENT> ImplementPositionControl<DERIVED, IMPLEMENT>::
~ImplementPositionControl()
{
    if (helper!=0) uninitialize();
}


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