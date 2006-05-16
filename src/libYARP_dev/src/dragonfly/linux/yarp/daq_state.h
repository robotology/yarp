// Yo Emacs!  Parse this file in -*- C++ -*- mode.

#ifndef DAQSTATE_INCLUDE
#define DAQSTATE_INCLUDE

// By cckemp mit edu
// Modded by paulfitz mit edu

#include "GlobalMathHelper.h"


class DAQState
{
public:
  bool flash_leds_on;  
  int exposure, shutter, gain, brightness, u_b_value, v_r_value;
  int emx, emn, smx, smn, gmx, gmn, bmx, bmn, ubmx, ubmn, vrmx, vrmn;

  DAQState()
  {
    flash_leds_on = false;

    //////////////////range in coriander                //default value shown in coriander
    exposure = 260;   //0-1023                          //260
    shutter = 532;    //2-532   (dark to bright)        //532
    gain = 350; //450 //700 for the shoe cam //220-1023                        //562
    brightness = 0;   //0-255                           //0
    u_b_value = 63;   //0-63                            //50
    v_r_value = 30;   //0-63                            //50

    emx = 1023;
    emn = 0;
    smx = 532;
    smn = 2;
    gmx = 1023;
    gmn = 220;
    bmx = 255;
    bmn = 0;
    ubmx = 63;
    ubmn = 0;
    vrmx = 63;
    vrmn = 0;
  }

  /////////////////////////////////////////////////

  inline int ftoi(float f, int mn, int mx)
  {
    return( myround((f*(mx-mn))+mn) );
  }

  inline float itof(int i, int mn, int mx)
  {
    return( (i-mn)/(mx-mn) );
  }

  /////////////////////////////////////////////////

  inline int GetUBColor(){return(u_b_value);}
  inline int GetVRColor(){return(v_r_value);}
  inline int GetBrightness(){return(brightness);}
  inline int GetGain(){return(gain);}
  inline int GetShutter(){return(shutter);}
  inline int GetExposure(){return(exposure);}

  /////////////////////////////////////////////////

  inline float GetFUBColor(){return(itof(u_b_value, ubmn, ubmx));}
  inline float GetFVRColor(){return(itof(v_r_value, vrmn, vrmx));}
  inline float GetFBrightness(){return(itof(brightness, bmn, bmx));}
  inline float GetFGain(){return(itof(gain, gmn, gmx));}
  inline float GetFShutter(){return(itof(shutter, smn, smx));}
  inline float GetFExposure(){return(itof(exposure, emn, emx));}
  
  /////////////////////////////////////////////////

  inline void SetColor(float u_b_in, float v_r_in)
  {
    SetColor(ftoi(u_b_in, ubmn, ubmx), ftoi(v_r_in, vrmn, vrmx));
  }
  
  inline void SetBrightness(float brightness_in)
  {
    SetBrightness(ftoi(brightness_in,  bmn, bmx));
  }
  
  inline void SetGain(float gain_in)
  {
    SetGain(ftoi(gain_in, gmn, gmx));
  }
  
  inline void SetShutter(float shutter_in)
  {
    SetShutter(ftoi(shutter_in, smn, smx));
  }
  
  inline void SetExposure(float exposure_in)
  {
    SetExposure(ftoi(exposure_in, emn, emx));
  }

  /////////////////////////////////////////////////

  inline void SetColor(int u_b_in, int v_r_in)
  {
    u_b_value = u_b_in;
    v_r_value = v_r_in;
    if(u_b_value < ubmn) u_b_value = ubmn;
    if(u_b_value > ubmx) u_b_value = ubmx;
    if(v_r_value < vrmn) v_r_value = vrmn;
    if(v_r_value > vrmx) v_r_value = vrmx;
  }

  inline void SetBrightness(int brightness_in)
  {
    brightness = brightness_in;
    if(brightness < bmn ) brightness = bmn;
    if(brightness > bmx) brightness = bmx;
  }

  inline void SetGain(int gain_in)
  {
    gain = gain_in;
    if(gain < gmn) gain = gmn;
    if(gain > gmx) gain = gmx;
  }

  inline void SetShutter(int shutter_in)
  {
    shutter = shutter_in;
    if(shutter > smx) shutter = smx;
    if(shutter < smn) shutter = smn;
  }

  inline void SetExposure(int exposure_in)
  {
    exposure = exposure_in;
    if(exposure < emn ) exposure = emn;
    if(exposure > emx) exposure = emx;
  }

};



#endif
