// SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
// SPDX-License-Identifier: BSD-3-Clause

%define RESET_CONSTANTS_IN_TO_MATLAB
#undef mxCreateDoubleMatrixHasComplexFlagParam
#undef mxCreateLogicalMatrixHasComplexFlagParam
%enddef

%define TO_MATLAB(matlabType,mxArrayInitializer)

    // Convert to a Matlab vector
    mxArray * toMatlab() const
    {
        // create Matlab vector and map it to pointer 'd'
        size_t selfDim = self->size();
        #if mxArrayInitializer ## HasComplexFlagParam
          mxArray *p  = mxArrayInitializer(selfDim, 1, mxREAL);
        #else
          mxArray *p  = mxArrayInitializer(selfDim, 1);
        #endif
        matlabType* d = static_cast<matlabType*>(mxGetData(p));

        // copy items from 'self' to 'd'
        for(size_t i=0; i<selfDim; i++)
        {
            d[i] = static_cast<matlabType>(self->operator[](i));
        }
        return p;
    }

%enddef


%define FROM_MATLAB(matlabType,cppType,mxArrayInitializer,vectorClass)

    // Convert from a Matlab vector
    void fromMatlab(mxArray * in)
    {
        // check size
        const mwSize * dims = mxGetDimensions(in);
        size_t selfDim = self->size();
        size_t matlabVecDim = (dims[0] == 1 ? dims[1] : dims[0]);

        if (matlabVecDim == selfDim)
        {
            // map Matlab vector to pointer 'd'
            matlabType* d = static_cast<matlabType*>(mxGetData(in));

            // copy items from 'd' to 'self'
            for(size_t i=0; i<selfDim; i++)
            {
                self->operator[](i) = static_cast<cppType>(d[i]);
            }
            return;
        } else {
            mexErrMsgIdAndTxt("yarp:vectorClass:wrongDimension",
              "Wrong vector size. Matlab size: %d. vectorClass size: %d", matlabVecDim, selfDim);
        }
    }

%enddef


%define RESET(resetValue)

    // Reset values
    void zero()
    {
        for(size_t i=0; i < self->size(); i++)
        {
            self->operator[](i) = resetValue;
        }
        return;
    }

%enddef

RESET_CONSTANTS_IN_TO_MATLAB
#define mxCreateDoubleMatrixHasComplexFlagParam 1

%extend std::vector<double> {
    TO_MATLAB(double,mxCreateDoubleMatrix)
    FROM_MATLAB(double,double,mxCreateDoubleMatrix,DVector)
    RESET(0)
}

%extend std::vector<bool> {
    TO_MATLAB(bool,mxCreateLogicalMatrix)
    FROM_MATLAB(bool,bool,mxCreateLogicalMatrix,BVector)
    RESET(false)
}

%extend std::vector<int> {
    TO_MATLAB(double,mxCreateDoubleMatrix)
    FROM_MATLAB(double,int,mxCreateDoubleMatrix,IVector)
    RESET(0)
}

%extend yarp::sig::VectorOf<double> {
    TO_MATLAB(double,mxCreateDoubleMatrix)
    FROM_MATLAB(double,double,mxCreateDoubleMatrix,yarp::sig::VectorOf<double>)
    RESET(0)
}

RESET_CONSTANTS_IN_TO_MATLAB
