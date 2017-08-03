%extend std::vector<int> {
    // Convert to a Matlab vector
    mxArray * toMatlab() const
    {
        // create Matlab vector and map it to pointer 'd'
        size_t selfDim = self->size();
        mxArray *p  = mxCreateDoubleMatrix(selfDim, 1, mxREAL);
        double* d = static_cast<double*>(mxGetData(p));

        // copy items from 'self' to 'd'
        const int* selfData = self->data();
        for(size_t i=0; i<selfDim; i++)
        {
            d[i] = selfData[i];
        }
        return p;
    }

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
            double* d = static_cast<double*>(mxGetData(in));

            // copy items from 'd' to 'self'
            int* selfData = self->data();
            for(size_t i=0; i<selfDim; i++)
            {
                selfData[i] = static_cast<int>(d[i]);
            }
            return;
        } else {
            mexErrMsgIdAndTxt("yarp:IVector:wrongDimension",
              "Wrong vector size. Matlab size: %d. IVector size: %d", matlabVecDim, selfDim);
        }
    }

    // Reset values
    void zero()
    {
        int* selfData = self->data();
        for(size_t i=0; i < self->size(); i++)
        {
            selfData[i] = 0;
        }
        return;
    }
}
