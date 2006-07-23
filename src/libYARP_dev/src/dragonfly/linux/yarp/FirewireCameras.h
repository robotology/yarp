// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-


// By cckemp mit edu
// Modded by paulfitz mit edu
// re-adapted for YARP2, by nat (almost nothing done)

#ifndef FIREWIRECAMERAS
#define FIREWIRECAMERAS

#include "GlobalMathHelper.h"

extern "C"{
#include <libdc1394/dc1394_control.h>
#include <libraw1394/raw1394.h>
}

#include "daq_state.h"

#define MAX_NUM_CAMERAS 2


//need to fix this 20040908 cckemp
//no cameras need to be shutdown,
//since none are currently valid and started
//free(): invalid pointer 0x40247620!
//now trying to destroy handle for dc1394...destroyed.
//firewire shutdown complete.

class CCKCamera
{
public:
  
    dc1394_camerainfo camera_info;
    dc1394_feature_set feature_set;
    dc1394_miscinfo misc_info;
    dc1394_cameracapture capture;
 
    DAQState dstate;
};

//shutter influences motion blur and image brightness
//gain and shutter have a large impact on image brightness
//brightness has a smaller impact on image brightness 
//gain seems to introduce the most noise for image brightness increase
           
enum CaptureType{ _320x240, _160x120, _160x120_3x3, _160x120_5x5};

class FWCameras
{
public:

    ~FWCameras() {}

    FWCameras(bool dma_on_in=true)
    {}

    bool Restart(bool dma_on_in=true) {
        Shutdown();
        init_cameras(dma_on_in);
        return(a_camera_is_ready());
    }
    void Shutdown() { shutdown_firewire(); }

    int GetNumberOfCameras() {return(num_cameras);}

    int HowManyCamerasAreReady() {
        int n=0;
        for(int c=0; c<GetNumberOfCameras(); c++)
            if(cam_num_ok(c)) n++;
        return(n);
    }

    int getBufferLength(int camera_num=0);
    int getX(int camera_num=0);
    int getY(int camera_num=0);

    bool CameraIsReady(int camera_num=0){
        return(cam_num_ok(camera_num));
    }

    bool Capture(unsigned char *im, int camera_num=0);
		     
    void SetCaptureSize(CaptureType capture_size_in) { capture_size = capture_size_in; }
    void SetJpgQuality(int jpg_quality_in) { jpg_quality = jpg_quality_in; }
    int GetJpgQuality() { return(jpg_quality); }

    void SetAuto(bool auto_adjustment_on, int camera_num=0);
    void SetToDefaults(int camera_num = 0) {
        DAQState ds;
        Set(ds, camera_num, true);
    }

    void SetExposure(int exposure_in, int camera_num=0);
    void SetShutter(int shutter_in, int camera_num=0);
    void SetGain(int gain_in, int camera_num=0);
    void SetBrightness(int brightness_in, int camera_num=0);
    void SetColor(int u_b_in, int v_r_in, int camera_num=0);
  
    void SetExposure(float exposure_in, int camera_num=0);
    void SetShutter(float shutter_in, int camera_num=0);
    void SetGain(float gain_in, int camera_num=0);
    void SetBrightness(float brightness_in, int camera_num=0);
    void SetColor(float u_b_in, float v_r_in, int camera_num=0);

    float GetFExposure(int camera_num=0) { return(cameras[camera_num].dstate.GetFExposure()); }
    float GetFShutter(int camera_num=0) { return(cameras[camera_num].dstate.GetFShutter()); }
    float GetFGain(int camera_num=0) { return(cameras[camera_num].dstate.GetFGain());}
    float GetFBrightness(int camera_num=0) { return(cameras[camera_num].dstate.GetFBrightness()); }
    float GetFUBColor(int camera_num=0) { return(cameras[camera_num].dstate.GetFUBColor()); }
    float GetFVRColor(int camera_num=0) { return(cameras[camera_num].dstate.GetFVRColor()); }

    void Set(DAQState & dstate_in, int camera_num = 0, bool force = false)
    {
        if(force)
            {	
                SetExposure(dstate_in.exposure, camera_num);
                SetShutter(dstate_in.shutter, camera_num);
                SetGain(dstate_in.gain, camera_num);
                SetBrightness(dstate_in.brightness, camera_num);
                SetColor(dstate_in.u_b_value, dstate_in.v_r_value, camera_num);
            }
        else
            {
                if(dstate_in.exposure != cameras[camera_num].dstate.exposure) 
                    SetExposure(dstate_in.exposure, camera_num);
                if(dstate_in.shutter != cameras[camera_num].dstate.shutter) 
                    SetShutter(dstate_in.shutter, camera_num);
                if(dstate_in.gain != cameras[camera_num].dstate.gain) 
                    SetGain(dstate_in.gain, camera_num);
                if(dstate_in.brightness != cameras[camera_num].dstate.brightness) 
                    SetBrightness(dstate_in.brightness, camera_num);
                if((dstate_in.u_b_value != cameras[camera_num].dstate.u_b_value)||
                   (dstate_in.v_r_value!=cameras[camera_num].dstate.v_r_value)) 
                    SetColor(dstate_in.u_b_value, dstate_in.v_r_value, camera_num);
            }
    }
  
    int GetExposure(int camera_num=0) { return(cameras[camera_num].dstate.GetExposure()); }
    int GetShutter(int camera_num=0) { return(cameras[camera_num].dstate.GetShutter()); }
    int GetGain(int camera_num=0) { return(cameras[camera_num].dstate.GetGain()); }
    int GetBrightness(int camera_num=0) { return(cameras[camera_num].dstate.GetBrightness()); }
    int GetUBColor(int camera_num=0) { return(cameras[camera_num].dstate.GetUBColor()); }
    int GetVRColor(int camera_num=0) { return(cameras[camera_num].dstate.GetVRColor()); }

    // set the size of the frame, currently only a limited number of options are available
    // (320x240 and 160x120), the function returns false if asked to produce an image of a 
    // different format. Future versions of the driver might support more choices.
    bool SetSize(int x, int y);

    void init_cameras(bool dma_on_in=true);
private:

    raw1394handle_t handle;
    nodeid_t *camera_nodes; 
    int num_nodes;
    int num_cameras;
    bool dma_on;
    char *dma_device_file;
    bool fw_ready[MAX_NUM_CAMERAS];
    CCKCamera cameras[MAX_NUM_CAMERAS];
    CaptureType capture_size;
    int jpg_quality;

    void set_nothing_ready(){
        for(int i=0; i<MAX_NUM_CAMERAS; i++) {
            fw_ready[i] = false;
        }
    }

    bool a_camera_is_ready() {
        bool any_ready = false;
        for(int c=0; c<GetNumberOfCameras(); c++)
            any_ready = any_ready || cam_num_ok(c);
        return(any_ready);
    }

    bool cam_num_ok(int camera_num) {
        if((camera_num>=0)&&(camera_num<GetNumberOfCameras())){
            return(fw_ready[camera_num]);
        }
        return(false);
    }

    void init_variables(bool dma_on_in=true){
        capture_size = _320x240;
        jpg_quality = 80;
        handle = NULL;
        camera_nodes = NULL;
        num_nodes = 0;
        num_cameras = 0;
        dma_on = dma_on_in;
        dma_device_file = NULL;
        set_nothing_ready();
    }

    void shutdown_camera( int camera_num = 0 );
    bool capture_a_frame(int camera_num = 0);
    bool release_capture_buffer(dc1394_cameracapture * camera, int camera_num = 0);
    void get_info_for_cameras();
    void get_camera_info(int camera_num = 0);
    bool init_firewire();
    bool start_firewire(int camera_num=0);
    void shutdown_firewire();
    void capture_320x240_color_image(unsigned char * & buff_out, int camera_num = 0 );
    void capture_color_image_5x5(unsigned char * & buff_out, int camera_num = 0 );
    void capture_color_image_3x3(unsigned char * & buff_out,  int camera_num = 0 );
    //  void capture_color_image(YARPImageOf<YarpPixelRGB> & grabbed_image_out, int camera_num = 0 );

    //  void capture_mono_image(YARPImageOf<YarpPixelMono> &grabbed_image_out, int camera_num = 0);
};

#endif
