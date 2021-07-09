/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2009, The MathWorks, Inc.
 * SPDX-License-Identifier: BSD-3-Clause
 */

#define S_FUNCTION_LEVEL 2
#define S_FUNCTION_NAME  sfun_time

#define TIME_SCALE_FACTOR(S) ssGetSFcnParam(S,0)

/*
 * Need to include simstruc.h for the definition of the SimStruct and
 * its associated macro definitions.
 */
#include "simstruc.h"

/*
 *  Include the standard ANSI C header for handling time functions:
 *  ---------------------------------------------------------------
 */
#include <time.h>

static void mdlInitializeSizes(SimStruct *S)
{

   ssSetNumSFcnParams(S, 1);  /* Number of expected parameters */

   if (ssGetNumSFcnParams(S) != ssGetSFcnParamsCount(S)) return;

    ssSetNumContStates(S, 0);
    ssSetNumDiscStates(S, 0);

   if (!ssSetNumInputPorts(S, 0)) return;

   if (!ssSetNumOutputPorts(S, 0)) return;

   ssSetNumSampleTimes(S, 1);
   ssSetNumRWork(S, 1);
   ssSetNumIWork(S, 0);
   ssSetNumPWork(S, 0);
   ssSetNumModes(S, 0);
   ssSetNumNonsampledZCs(S, 0);
   ssSetOptions(S, 0);
}

#define MDL_INITIALIZE_SAMPLE_TIMES
static void mdlInitializeSampleTimes(SimStruct *S)
{
   ssSetSampleTime(S, 0, CONTINUOUS_SAMPLE_TIME);
   ssSetOffsetTime(S, 0, 0.0);
}

#define MDL_START
static void mdlStart(SimStruct *S)
{
    ssSetRWorkValue(S,0,ssGetTStart(S));
}

static void mdlOutputs(SimStruct *S, int_T tid)
{
   real_T             t_previousSimTime = ssGetRWorkValue(S,0);
   const real_T      *scaleFactor = mxGetPr(TIME_SCALE_FACTOR(S));
   time_T             t_SimTime = ssGetT(S);
   real_T             t_diff = 0.0;
   real_T             dt;
   real_T             t_current;
   real_T             t_0;
   real_T             t_previous;
   real_T             t_elapsed;

   /* Desired Delta time */
   dt = (t_SimTime - t_previousSimTime) * (scaleFactor[0]);
   /* Get clock time at the beginning of this step*/
   t_previous = (real_T)clock()/CLOCKS_PER_SEC;
   t_0 = (real_T)clock()/CLOCKS_PER_SEC;

   /* Wait to reach the desired time */
   while (t_diff<dt){
        t_current = (real_T) clock()/CLOCKS_PER_SEC;
        /* Look for wrapup */
        if (t_current<t_previous){
            t_elapsed = t_previous - t_0;
            t_0 = (real_T) clock()/CLOCKS_PER_SEC - t_elapsed;
        }
        t_diff = t_current - t_0;
        t_previous = t_current;
   }

   /* Store current time to be used in next time step*/
   ssSetRWorkValue(S, 0, t_SimTime);
}

static void mdlTerminate(SimStruct *S)
{
    UNUSED_ARG(S); /* unused input argument */
}

/*
 *  Required S-function trailer:
 *  ----------------------------
 */
#ifdef  MATLAB_MEX_FILE    /* Is this file being compiled as a MEX-file? */
#include "simulink.c"      /* MEX-file interface mechanism */
#else
#include "cg_sfun.h"       /* Code generation registration function */
#endif
