// Yo Emacs!  Parse this file in -*- C++ -*- mode.

//based on libdc1394 example code grab_gray_image.c
//and coriander

#include "FirewireCameras.h"
#include <memory.h>

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

const bool PRINTF_ON=false;

void FWCameras::SetAuto(bool auto_adjustment_on, int camera_num)
{
  if(!cam_num_ok(camera_num)) return;
  int val;
  if(auto_adjustment_on) val = 1;
  else val = 0;

  dc1394_auto_on_off( handle, camera_nodes[camera_num],
		      FEATURE_EXPOSURE, val);
  dc1394_auto_on_off( handle, camera_nodes[camera_num],
		      FEATURE_SHUTTER, val);
  dc1394_auto_on_off( handle, camera_nodes[camera_num],
		      FEATURE_GAIN, val);
  dc1394_auto_on_off( handle, camera_nodes[camera_num],
		      FEATURE_BRIGHTNESS, val);
}

///////////////////////////

void FWCameras::SetExposure(float exposure_in, int camera_num)
{
  if(!cam_num_ok(camera_num)) return;
  // exposure = 260;   //0-1023     //260
  cameras[camera_num].dstate.SetExposure(exposure_in);
  dc1394_set_exposure(handle, camera_nodes[camera_num],
		      (unsigned int) cameras[camera_num].dstate.GetExposure());
}

void FWCameras::SetShutter(float shutter_in, int camera_num)
{
  if(!cam_num_ok(camera_num)) return;
  // shutter = 532;    //2-532   (dark to bright)        //532
  cameras[camera_num].dstate.SetShutter(shutter_in);
  dc1394_set_shutter(handle, camera_nodes[camera_num],
		     (unsigned int) cameras[camera_num].dstate.GetShutter());
}

void FWCameras::SetGain(float gain_in, int camera_num)
{
  if(!cam_num_ok(camera_num)) return;
 //////////////////range in coriander                //default value shown in coriander
  //gain was 450 then 300 then 200
  //  gain = 220;//400 //730  //220-1023                        //562
  cameras[camera_num].dstate.SetGain(gain_in);
  dc1394_set_gain(handle, camera_nodes[camera_num],
		  (unsigned int) cameras[camera_num].dstate.GetGain());
}

void FWCameras::SetBrightness(float brightness_in, int camera_num)
{
  if(!cam_num_ok(camera_num)) return;
  // brightness = 0;   //0-255                           //0
  cameras[camera_num].dstate.SetBrightness(brightness_in);
  dc1394_set_brightness(handle, camera_nodes[camera_num],
			(unsigned int) cameras[camera_num].dstate.GetBrightness());
}


void FWCameras::SetColor(float u_b_in, float v_r_in, int camera_num)
{ 
  if(!cam_num_ok(camera_num)) return;
  //u_b_value = 63;   //0-63                            //50
  //  v_r_value = 30;   //0-63                            //50
  cameras[camera_num].dstate.SetColor(u_b_in, v_r_in);
  dc1394_set_white_balance(handle, camera_nodes[camera_num],
			   (unsigned int) cameras[camera_num].dstate.GetUBColor(), 
			   (unsigned int) cameras[camera_num].dstate.GetVRColor());
}

//////////////////////////////////

void FWCameras::SetExposure(int exposure_in, int camera_num)
{
  if(!cam_num_ok(camera_num)) return;
  // exposure = 260;   //0-1023     //260
  cameras[camera_num].dstate.SetExposure(exposure_in);
  dc1394_set_exposure(handle, camera_nodes[camera_num],
		      (unsigned int) cameras[camera_num].dstate.GetExposure());
}

void FWCameras::SetShutter(int shutter_in, int camera_num)
{
  if(!cam_num_ok(camera_num)) return;
  // shutter = 532;    //2-532   (dark to bright)        //532
  cameras[camera_num].dstate.SetShutter(shutter_in);
  dc1394_set_shutter(handle, camera_nodes[camera_num],
		     (unsigned int) cameras[camera_num].dstate.GetShutter());
}

void FWCameras::SetGain(int gain_in, int camera_num)
{
  if(!cam_num_ok(camera_num)) return;
 //////////////////range in coriander                //default value shown in coriander
  //gain was 450 then 300 then 200
  //  gain = 220;//400 //730  //220-1023                        //562
  cameras[camera_num].dstate.SetGain(gain_in);
  dc1394_set_gain(handle, camera_nodes[camera_num],
		  (unsigned int) cameras[camera_num].dstate.GetGain());
}

void FWCameras::SetBrightness(int brightness_in, int camera_num)
{
  if(!cam_num_ok(camera_num)) return;
  // brightness = 0;   //0-255                           //0
  cameras[camera_num].dstate.SetBrightness(brightness_in);
  dc1394_set_brightness(handle, camera_nodes[camera_num],
			(unsigned int) cameras[camera_num].dstate.GetBrightness());
}


void FWCameras::SetColor(int u_b_in, int v_r_in, int camera_num)
{ 
  if(!cam_num_ok(camera_num)) return;
  //u_b_value = 63;   //0-63                            //50
  //  v_r_value = 30;   //0-63                            //50
  cameras[camera_num].dstate.SetColor(u_b_in, v_r_in);
  dc1394_set_white_balance(handle, camera_nodes[camera_num],
			   (unsigned int) cameras[camera_num].dstate.GetUBColor(), 
			   (unsigned int) cameras[camera_num].dstate.GetVRColor());
}

///////////////////////////

bool FWCameras::init_firewire()
{
  int port=0; // port 0 is the first IEEE1394 card. We ONLY probe this one.
  //open ohci and assign a handle to it
  handle=dc1394_create_handle(port);
  if (handle==NULL)
    { 
      printf("Unable to aquire a raw1394 handle\n\n"
             "Please check \n"
	     "  - if the kernel modules `ieee1394',`raw1394' and `ohci1394' are loaded \n"
	     "  - if you have read/write access to /dev/raw1394\n\n");
      return(false);
    }

  //get the camera nodes and describe them as we find them
  // probe the IEEE1394 bus for DC camera:
  num_nodes = raw1394_get_nodecount(handle);
  camera_nodes = dc1394_get_camera_nodes(handle, &num_cameras, 1); //used to be 0,  0 not to show the cams.
  fflush(stdout); //added with ",1);"
  if (num_cameras<1)
    {
      printf("no cameras found!\n");
      dc1394_destroy_handle(handle);
      return(false);
    }

  if(camera_nodes[0]== (num_nodes-1))
    {
      printf( "\n"
	      "Sorry, your camera is the highest numbered node\n"
	      "of the bus, and has therefore become the root node.\n"
	      "The root node is responsible for maintaining \n"
	      "the timing of isochronous transactions on the IEEE \n"
	      "1394 bus.  However, if the root node is not cycle master \n"
	      "capable (it doesn't have to be), then isochronous \n"
	      "transactions will not work.  The host controller card is \n"
	      "cycle master capable, however, most cameras are not.\n"
	      "\n"
	      "The quick solution is to add the parameter \n"
	      "attempt_root=1 when loading the OHCI driver as a \n"
	      "module.  So please do (as root):\n"
	      "\n"
	      "   rmmod ohci1394\n"
	      "   insmod ohci1394 attempt_root=1\n"
	      "\n"
	      "for more information see the FAQ at \n"
	      "http://linux1394.sourceforge.net/faq.html#DCbusmgmt\n"
	      "\n");
      return(false);
    }
      
  if(PRINTF_ON) printf("Initialization is complete.\n");
  return(true);
}


void FWCameras::get_camera_info(int camera_num)
{
  int err;
  unsigned int channel, speed;
  int i;

  if(!cam_num_ok(camera_num)) {
    printf("camera number %d is not valid\n", camera_num);
    return;
  }

  // allocate memory space for all camera infos & download all infos:
  if(num_cameras > MAX_NUM_CAMERAS) 
    { 
      printf("the number of available cameras is greater than the maximum allowed number of cameras\n");
      printf("num_cameras > MAX_NUM_CAMERAS\n");
      printf("recompile if you want more than %d\n\n", MAX_NUM_CAMERAS);
      num_cameras = MAX_NUM_CAMERAS;
    }
      
  err=1;
  i = camera_num;
	  
  err*=dc1394_get_camera_misc_info(handle, camera_nodes[i], &(cameras[i].misc_info));
  err*=dc1394_get_camera_info(handle, camera_nodes[i], &(cameras[i].camera_info));
  err*=dc1394_get_camera_feature_set(handle, cameras[i].camera_info.id, &(cameras[i].feature_set));
  if (!err) printf("Could not get camera basic informations!\n");
  
  printf("\n\n");
  dc1394_print_feature_set(&(cameras[i].feature_set));
  printf("\n\n");

  if (dc1394_get_iso_channel_and_speed(handle, cameras[i].camera_info.id, 
				       &channel, &speed)!=DC1394_SUCCESS)
    printf("Can't get iso channel and speed\n");

  if (dc1394_set_iso_channel_and_speed(handle, cameras[i].camera_info.id, 
				       camera_num, speed)!=DC1394_SUCCESS)
    printf("Can't set iso channel and speed\n");

}     

bool FWCameras::start_firewire(int camera_num)
{
   /***********************************************************
 dc1394_setup_capture

 Sets up both the camera and the cameracapture structure
 to be used other places.

 Returns DC1394_SUCCESS on success, DC1394_FAILURE otherwise

 NOTE: it is important to call dc1394_release_camera 
       to free memory allocated by this routine- if you
       don't, your application WILL leak memory
   ************************************************************/

   if((camera_num<0)||(camera_num>=GetNumberOfCameras())){
      printf("camera %d is not valid and so no attempt will be made to start it!\n", camera_num);
      return false;
   }
  
   if(!dma_on)
      {
         if (dc1394_setup_capture(handle,camera_nodes[camera_num],
                                  camera_num, /* channel */ 
                                  FORMAT_VGA_NONCOMPRESSED,
                                  MODE_640x480_MONO,
                                  SPEED_400,
                                  FRAMERATE_30,  //FRAMERATE_7_5, cck 05-05-02
                                  &(cameras[camera_num].capture))!=DC1394_SUCCESS) 
            {
               printf( "\n"
                       "unable to setup camera %d:\n"
                       "  - the camera may already be in use\n"
                       "  - the requested video mode, framerate or format,\n"
                       "    as specified on line %d of file \"%s\",\n"
                       "    may not be supported by the camera\n\n",
                       camera_num, __LINE__,__FILE__);
               //this command seems to seg fault here, not having it may lead to a memory leak
               //dc1394_release_camera(handle, &(cameras[camera_num].capture));
               return false;
            }
      } 
   else
      {	  
         if (access("/dev/video1394/0", R_OK) == 0)
            dma_device_file = "/dev/video1394/0";
         else if (access("/dev/video1394", R_OK) == 0)
            dma_device_file = "/dev/video1394";
         else 
            {
               fprintf(stderr, "******************************************************\n");
               fprintf(stderr, "Failed to open device file /dev/video1394\n");
               fprintf(stderr, "\n");
               fprintf(stderr, "    Have you setup /dev/video1394 as root?\n");
               fprintf(stderr, "\n");
               fprintf(stderr, "        mkdir /dev/video1394\n");
               fprintf(stderr, "        mknod -m 666 /dev/video1394/0 c 171 16\n");
               fprintf(stderr, "\n");
               fprintf(stderr, "******************************************************\n");
               return(false);
            }

         int num_dma_buffers = 4;
         int drop_frames = 1;
         int do_extra_buffering = 0; //1 introduces high latency when it's turned on

         if(dc1394_dma_setup_capture(handle, camera_nodes[camera_num],
                                     camera_num, 
                                     FORMAT_VGA_NONCOMPRESSED,
                                     MODE_640x480_MONO,
                                     SPEED_400,				 
                                     FRAMERATE_30, 
                                     num_dma_buffers,
                                     //do_extra_buffering, //removed for libdc1394-11-dev
                                     drop_frames, 
                                     dma_device_file,
                                     &(cameras[camera_num].capture))!=DC1394_SUCCESS)
            {
               printf("\n" 
                      "unable to setup camera %d:\n"
                      "  - the camera may already be in use\n"
                      "  - the requested video mode, framerate or format,\n"
                      "    as specified on line %d of file \"%s\",\n"
                      "    may not be supported by the camera\n\n",
                      camera_num, __LINE__,__FILE__);
               //this command seems to seg fault here, not having it may lead to a memory leak
               //dc1394_release_camera(handle, &(cameras[camera_num].capture));
               return false;
            }

      }
   /*
   //not sure if this is needed
   //dragonfly cameras may only support one mode anyway
   // set trigger mode
  
   if( dc1394_set_trigger_mode(handle, cameras[camera_num].capture.node, TRIGGER_MODE_0)
   != DC1394_SUCCESS)
   {
   printf("unable to set camera trigger mode\n");
   dc1394_release_camera(handle,&(cameras[camera_num].capture));
   dc1394_destroy_handle(handle);
   exit(1);
   }
   */


   // Start isochronous data transmission
   if (dc1394_start_iso_transmission(handle, cameras[camera_num].capture.node)
       !=DC1394_SUCCESS) 
      {
         printf("unable to start camera iso transmission\n");
         if(handle!=NULL) {
            dc1394_release_camera(handle, &(cameras[camera_num].capture));
         }
         return false;
      }

   return true;
}


void FWCameras::shutdown_camera( int camera_num )
{
   if(!cam_num_ok(camera_num)) {
      printf("camera %d was not properly started,\nso no attempt will be made to shut it down.\n", camera_num);
      return;
   }

   if(handle==NULL) {
      printf("Failure when trying to shutdown camera %d\n", camera_num);
      printf("handle == NULL, so it was previously released or never initialized\n");
   }

   printf("trying to stop iso_transmission of camera #%d...", camera_num);

   if (dc1394_stop_iso_transmission(handle, cameras[camera_num].capture.node)
       !=DC1394_SUCCESS) 
      {
         printf("couldn't stop iso_transmission of camera #%d!\n", camera_num);
      } 
   else 
      {
         printf("stopped.\n");
      }

   if(!dma_on)
      {
         printf("trying to release camera #%d, no dma...", camera_num);
         dc1394_release_camera(handle, &(cameras[camera_num].capture));
         printf("released.\n");
      }
   else
      {
         printf("trying to release camera #%d, with dma...", camera_num);
         dc1394_dma_unlisten(handle, &(cameras[camera_num].capture));
         dc1394_dma_release_camera(handle, &(cameras[camera_num].capture));
         printf("released.\n");
      }
}

void FWCameras::shutdown_firewire()
{
  if(a_camera_is_ready())
    {
      printf("\nshutdown_firewire:\n");
      printf("shutdown %d firewire camera(s):\n", num_cameras);
      
      int c;
      for(c = 0; c<num_cameras; c++)
	{
	  printf("trying to shutdown camera #%d:\n", c);
	  if(handle!=NULL) shutdown_camera(c); 
	  printf("camera #%d has been shutdown successfully.\n", c);
	}
      printf("all cameras have been shutdown.\n");
    } else { 
      printf("no cameras need to be shutdown,\n"
	     "since none are currently valid and started\n");
    }
      
  printf("now trying to destroy handle for dc1394...");
  if(handle != NULL) {
    dc1394_destroy_handle(handle);
    printf("destroyed.\n");
  } else printf("handle is already NULL\n"
		"apparently, firewire was not properly initialized\n");

  printf("firewire shutdown complete.\n\n");
  init_variables(dma_on);
}

bool FWCameras::capture_a_frame(int camera_num)
{
  if(!cam_num_ok(camera_num)) return(false);

  if(PRINTF_ON) printf("capture_a_frame from camera %d\n", camera_num);

  if(!dma_on)
    {
      if(PRINTF_ON) printf("dc1394_single_capture(%d, %d)\n", handle, &(cameras[camera_num].capture));
      if (dc1394_single_capture(handle, &(cameras[camera_num].capture))!=DC1394_SUCCESS) 
	{
	  printf("unable to capture a frame from camera #%d\n", camera_num);
	  printf("shutting down firewire...\n");
	  shutdown_firewire();
	  printf("done.\n");
	  exit(1);
	}
    } else
      {
	if(PRINTF_ON) printf("executing dc1394_dma_single_capture\n");
	if (dc1394_dma_single_capture(&(cameras[camera_num].capture))!=DC1394_SUCCESS) 
	{
	  printf("unable to capture a frame from camera #%d\n", camera_num);
	  printf("shutting down firewire...\n");
	  shutdown_firewire();
	  printf("done.\n");
	  exit(1);
	}
	if(PRINTF_ON) printf("completed dc1394_dma_single_capture\n");	
      }

  return(true);
}

bool FWCameras::release_capture_buffer(dc1394_cameracapture * camera, int camera_num)
{
  if(dma_on) dc1394_dma_done_with_buffer(&(cameras[camera_num].capture));
  return(true);
}

bool FWCameras::Capture(unsigned char *imout, int camera_num)
{
  if(!cam_num_ok(camera_num)) return(false);

  unsigned char * buf;
  int buf_w, buf_h, buf_length;
  switch(capture_size)
    {
    case _320x240:
      buf_w=320; buf_h=240;
      buf_length = buf_w*buf_h*3;
      buf = new unsigned char[buf_length];
      capture_320x240_color_image(buf, camera_num);
      memcpy(imout, (char *) buf, buf_length);
      delete [] buf;
      break;
    case _160x120:
      buf_w=160; buf_h=120;
      buf_length = buf_w*buf_h*3;
      buf = new unsigned char[buf_length];
      capture_color_image_3x3(buf, camera_num);
      memcpy(imout, (char *) buf, buf_length);
      delete [] buf;
      break;
    case _160x120_5x5:
      buf_w=160; buf_h=120;
      buf_length = buf_w*buf_h*3;
      buf = new unsigned char[buf_length];
      capture_color_image_5x5(buf, camera_num);
      memcpy(imout, (char *) buf, buf_length);
      delete [] buf;
      break;
    case _160x120_3x3:
      buf_w=160; buf_h=120;
      buf_length = buf_w*buf_h*3;
      buf = new unsigned char[buf_length];
      capture_color_image_3x3(buf, camera_num);
      memcpy(imout, (char *) buf, buf_length);
      delete [] buf;
      break;
    default:
      buf_w=320; buf_h=240;
      buf_length = buf_w*buf_h*3;
      buf = new unsigned char[buf_length];
      capture_320x240_color_image(buf, camera_num);
      memcpy(imout, (char *) buf, buf_length);
      delete [] buf;
      break;
    }

  return true;
}

void FWCameras::capture_320x240_color_image(unsigned char * & buff_out, int camera_num )
{
  if(!cam_num_ok(camera_num)) return;
  capture_a_frame(camera_num);

  int frame_width, frame_height;
  frame_width = cameras[camera_num].capture.frame_width;
  frame_height = cameras[camera_num].capture.frame_height;
  unsigned char * buffer;
  buffer = (unsigned char *) (cameras[camera_num].capture.capture_buffer);

  if(PRINTF_ON) printf("frame_height = %d, frame_width = %d \n", frame_height, frame_width);
  
  int dh, dw;
  //  dh = 4; dw = 4;
  dh = 2; dw = 2;

  int tw,th; th = frame_height/dh; tw = frame_width/dw;
  int sz = th*tw*3;

  int r_int, g_int, b_int;
  unsigned char r,g,b;
  int h,w;

  int index;
  //set to 0
  int temp_width, temp_height, temp_size; 
  temp_width = 320; 
  temp_height = 240; 
  temp_size = temp_width * temp_height * 3;
  for(index=0; index<temp_size; index++) 
     buff_out[index] = 0;

  for(h = 2; h < (frame_height-1); h=h+dh)
     {
        for(w = 2; w < (frame_width-1); w=w+dw)
           {
              index = ( (w/dw) + ((h/dh)*tw) ) * 3; 
	  
              buff_out[index] = buffer[(h*frame_width)+w];
	
              g_int = (  buffer[(h*frame_width)+(w-1)] +   buffer[(h*frame_width)+(w+1)] + 
                         buffer[((h-1)*frame_width)+w] +   buffer[((h+1)*frame_width)+w])/4;
              buff_out[index+1] = (unsigned char) g_int;

              b_int = (  buffer[((h-1)*frame_width)+(w-1)] +   buffer[((h+1)*frame_width)+(w+1)] + 
                         buffer[((h-1)*frame_width)+(w+1)] +   buffer[((h+1)*frame_width)+(w-1)])/4;
              buff_out[index+2] = (unsigned char) b_int;
           }
     }

  release_capture_buffer(&(cameras[camera_num].capture), camera_num);
}

void FWCameras::capture_color_image_5x5(unsigned char * & buff_out, int camera_num )
{ 
  if(!cam_num_ok(camera_num)) return;
  static unsigned char temp_buff [320*240*3];
  capture_a_frame(camera_num);
 
  int frame_width, frame_height;
  frame_width = cameras[camera_num].capture.frame_width;
  frame_height = cameras[camera_num].capture.frame_height;
  unsigned char * buffer;
  buffer = (unsigned char *) (cameras[camera_num].capture.capture_buffer);

  if(PRINTF_ON) printf("frame_height = %d, frame_width = %d \n", frame_height, frame_width);
  
  int dh, dw;
  dh = 2; dw = 2;
   
  int tw,th; th = frame_height/dh; tw = frame_width/dw;
  int sz = th*tw*3;

  int r_int, g_int, b_int;
  unsigned char r,g,b;
  int h,w;

  int index;
  int temp_width, temp_height, temp_size; 
  temp_width = 320; temp_height = 240; temp_size = temp_width * temp_height * 3;
  //set to 0
  for(index=0; index<temp_size; index++) temp_buff[index] = 0;


  for(h = 2; h < (frame_height-1); h=h+dh)
    {
      for(w = 2; w < (frame_width-1); w=w+dw)
	{
	  index = ( (w/dw) + ((h/dh)*tw) ) * 3; 
	  
	  temp_buff[index] = buffer[(h*frame_width)+w];
	
	  g_int = (  buffer[(h*frame_width)+(w-1)] +   buffer[(h*frame_width)+(w+1)] + 
		     buffer[((h-1)*frame_width)+w] +   buffer[((h+1)*frame_width)+w])/4;
	  temp_buff[index+1] = (unsigned char) g_int;

	  b_int = (  buffer[((h-1)*frame_width)+(w-1)] +   buffer[((h+1)*frame_width)+(w+1)] + 
		     buffer[((h-1)*frame_width)+(w+1)] +   buffer[((h+1)*frame_width)+(w-1)])/4;
	  temp_buff[index+2] = (unsigned char) b_int;
	}
    }

  release_capture_buffer(&(cameras[camera_num].capture), camera_num);

  /////////////////////////////////////////////
  //perform a 5x5 separable convolution and downsample by 2

  int out_width, out_height, out_size;
  out_width = 160; out_height = 120; out_size = out_width * out_height * 3;

  for(index=0; index < out_size; index++) buff_out[index] = 0;
  
  int * row;
  row = new int [out_width*3];
  for(index=0; index < out_width*3; index++) row[index]=0;

  int n; int x; int y;
  int s = (temp_width*3);
 
  for(h = 0; h < temp_height; h++)
    {
      index = h*(temp_width*3);
      for(w = 2; w < out_width-2; w++)
	{	  
	  x = 3 * w;
	  n = index + (2*x);
	  row[x+0] = (temp_buff[n-6] + (temp_buff[n-3]*4) + (temp_buff[n+0]*6) + 
		      (temp_buff[n+3]*4) + temp_buff[n+6])/16;
	  row[x+1] = (temp_buff[n-5] + (temp_buff[n-2]*4) + (temp_buff[n+1]*6) + 
		      (temp_buff[n+4]*4) + temp_buff[n+7])/16;
	  row[x+2] = (temp_buff[n-4] + (temp_buff[n-1]*4) + (temp_buff[n+2]*6) + 
		      (temp_buff[n+5]*4) + temp_buff[n+8])/16;
	}

      for(w = 2; w < out_width-2; w++)
	{
	  x = 3 * w;
	  n = index + x;
	  temp_buff[n+0] = (unsigned char) row[x+0];
	  temp_buff[n+1] = (unsigned char) row[x+1];
	  temp_buff[n+2] = (unsigned char) row[x+2];
	}
    }  

  delete [] row;

  ////////////
  
  int * col;
  col = new int[out_height*3];
  for(index=0; index<out_height*3; index++) col[index]=0;

  for(w = 2; w < out_width-2; w++)
    {
      x = 3*w;
      for(h = 2; h < out_height-2; h++)
	{
	  y = 3*h;
	  n = ((2*h)*s) + x;
      	  col[y+0] = (temp_buff[(n-(2*s))+0] +(temp_buff[(n-s)+0]*4) + (temp_buff[n+0]*6) + 
		      (temp_buff[(n+s)+0]*4) + temp_buff[(n+(2*s))+0])/16;
	  col[y+1] = (temp_buff[(n-(2*s))+1] + (temp_buff[(n-s)+1]*4) + (temp_buff[n+1]*6) + 
		      (temp_buff[(n+s)+1]*4) + temp_buff[(n+(2*s))+1])/16;
	  col[y+2] = (temp_buff[(n-(2*s))+2] + (temp_buff[(n-s)+2]*4) + (temp_buff[n+2]*6) + 
		      (temp_buff[(n+s)+2]*4) + temp_buff[(n+(2*s))+2])/16;
	}
 
      for(h = 2; h < out_height-2; h++)
	{
	  y = 3*h;
	  n = (h*out_width*3)+x;
	  buff_out[n+0] = (unsigned char) col[y+0];
	  buff_out[n+1] = (unsigned char) col[y+1];
	  buff_out[n+2] = (unsigned char) col[y+2];
	}
    }
  
  delete [] col;
}


void FWCameras::capture_color_image_3x3(unsigned char * & buff_out, int camera_num)
{
  if(!cam_num_ok(camera_num)) return;
  static unsigned char temp_buff [320*240*3];
  if(PRINTF_ON) printf("capture_a_frame\n");
  capture_a_frame(camera_num);
 
  if(PRINTF_ON) printf("capture_a_frame completed!\n");

  int frame_width, frame_height;
  frame_width = cameras[camera_num].capture.frame_width;
  frame_height = cameras[camera_num].capture.frame_height;
  unsigned char * buffer;
  buffer = (unsigned char *) (cameras[camera_num].capture.capture_buffer);

  if(PRINTF_ON) printf("frame_height = %d, frame_width = %d \n", frame_height, frame_width);
  
  int dh, dw;
  dh = 2; dw = 2;
   
  int tw,th; th = frame_height/dh; tw = frame_width/dw;
  int sz = th*tw*3;

  int r_int, g_int, b_int;
  unsigned char r,g,b;
  int h,w;

  int index;
  int temp_width, temp_height, temp_size; 
  temp_width = 320; temp_height = 240; temp_size = temp_width * temp_height * 3;
  //set to 0
  for(index=0; index<temp_size; index++) temp_buff[index] = 0;

  for(h = 2; h < (frame_height-1); h=h+dh)
    {
      for(w = 2; w < (frame_width-1); w=w+dw)
	{
	  index = ( (w/dw) + ((h/dh)*tw) ) * 3; 
	  
	  temp_buff[index] = buffer[(h*frame_width)+w];
	
	  g_int = (  buffer[(h*frame_width)+(w-1)] +   buffer[(h*frame_width)+(w+1)] + 
		     buffer[((h-1)*frame_width)+w] +   buffer[((h+1)*frame_width)+w])/4;
	  temp_buff[index+1] = (unsigned char) g_int;

	  b_int = (  buffer[((h-1)*frame_width)+(w-1)] +   buffer[((h+1)*frame_width)+(w+1)] + 
		     buffer[((h-1)*frame_width)+(w+1)] +   buffer[((h+1)*frame_width)+(w-1)])/4;
	  temp_buff[index+2] = (unsigned char) b_int;
	}
    }

  release_capture_buffer(&(cameras[camera_num].capture), camera_num);

  /////////////////////////////////////////////
  //perform a 3x3 separable convolution and downsample by 2

  int out_width, out_height, out_size;
  out_width = 160; out_height = 120; out_size = out_width * out_height * 3;

  for(index=0; index < out_size; index++) buff_out[index] = 0;
  
  int * row;
  row = new int [out_width*3];
  for(index=0; index < out_width*3; index++) row[index]=0;

  int n; int x; int y;
  int s = (temp_width*3);
 
  for(h = 0; h < temp_height; h++)
    {
      index = h*(temp_width*3);
      for(w = 1; w < out_width-1; w++)
	{	  
	  x = 3 * w;
	  n = index + (2*x);
	  row[x+0] = (temp_buff[n-3] + (temp_buff[n]*2) + temp_buff[n+3])/4;
	  row[x+1] = (temp_buff[n-2] + (temp_buff[n+1]*2) + temp_buff[n+4])/4;
	  row[x+2] = (temp_buff[n-1] + (temp_buff[n+2]*2) + temp_buff[n+5])/4;
	}

      for(w = 1; w < out_width-1; w++)
	{
	  x = 3 * w;
	  n = index + x;
	  temp_buff[n+0] = (unsigned char) row[x+0];
	  temp_buff[n+1] = (unsigned char) row[x+1];
	  temp_buff[n+2] = (unsigned char) row[x+2];
	}
    }  

  delete [] row;

  ////////////
  
  int * col;
  col = new int [out_height*3];
  for(index=0; index<out_height*3; index++) col[index]=0;

  for(w = 1; w < out_width-1; w++)
    {
      x = 3*w;
      for(h = 1; h < out_height-1; h++)
	{
	  y = 3*h;
	  n = ((2*h)*s) + x;
      	  col[y+0] = (temp_buff[(n-s)+0] + (temp_buff[n+0]*2) + temp_buff[(n+s)+0])/4;
	  col[y+1] = (temp_buff[(n-s)+1] + (temp_buff[n+1]*2) + temp_buff[(n+s)+1])/4;
	  col[y+2] = (temp_buff[(n-s)+2] + (temp_buff[n+2]*2) + temp_buff[(n+s)+2])/4;
	}
      
      for(h = 1; h < out_height-1; h++)
         {
            y = 3*h;
            n = (h*out_width*3)+x;
            buff_out[n+0] = (unsigned char) col[y+0];
            buff_out[n+1] = (unsigned char) col[y+1];
            buff_out[n+2] = (unsigned char) col[y+2];
         }
    }
  
  delete [] col;  
}

#if 0
void FWCameras::capture_mono_image(YARPImageOf<YarpPixelMono> &grabbed_image_out, int camera_num)
{
  if(!cam_num_ok(camera_num)) return;
  capture_a_frame(camera_num);
  int frame_width, frame_height;
  frame_width = cameras[camera_num].capture.frame_width;
  frame_height = cameras[camera_num].capture.frame_height;
  unsigned char * buffer;
  buffer = (unsigned char *) (cameras[camera_num].capture.capture_buffer);

  if(PRINTF_ON) printf("frame_height = %d, frame_width = %d \n", frame_height, frame_width);
  
  int dh, dw;
  dh = 4;
  dw = 4;

  grabbed_image_out.Resize(frame_width/dw, frame_height/dh);
  
  int r_int, g_int, b_int, t_int;
  unsigned char r,g,b;
  int h,w;

  for(h = 2; h < (frame_height-1); h=h+dh)
    {
      for(w = 2; w < (frame_width-1); w=w+dw)
	{
	  r = buffer[(h*frame_width)+w];
	  g_int = (buffer[(h*frame_width)+(w-1)] + buffer[(h*frame_width)+(w+1)] + 
		   buffer[((h-1)*frame_width)+w] + buffer[((h+1)*frame_width)+w])/4;
	  g = (unsigned char) g_int;
	  b_int = (buffer[((h-1)*frame_width)+(w-1)] + buffer[((h+1)*frame_width)+(w+1)] + 
		   buffer[((h-1)*frame_width)+(w+1)] + buffer[((h+1)*frame_width)+(w-1)])/4;
	  b = (unsigned char) b_int;
	  t_int = (r+g+b)/3;
	  
	  grabbed_image_out.Pixel(w/dw,h/dh) = (unsigned char) t_int; 
	}
    }

  release_capture_buffer(&(cameras[camera_num].capture), camera_num);
}
#endif

int FWCameras::getX(int camera_num)
{
  int buf_w, buf_h, buf_length;

  if(!cam_num_ok(camera_num)) return 0;

  switch(capture_size)
    {
    case _320x240:
      buf_w=320; buf_h=240;
      buf_length = buf_w*buf_h*3;
      break;
    case _160x120:
      buf_w=160; buf_h=120;
      buf_length = buf_w*buf_h*3;
      break;
    case _160x120_5x5:
      buf_w=160; buf_h=120;
      buf_length = buf_w*buf_h*3;
      break;
    case _160x120_3x3:
      buf_w=160; buf_h=120;
      buf_length = buf_w*buf_h*3;
      break;
    default:
      buf_w=320; buf_h=240;
      buf_length = buf_w*buf_h*3;
      break;
    }

  return buf_w;
}

int FWCameras::getY(int camera_num)
{
  int buf_w, buf_h, buf_length;
  if(!cam_num_ok(camera_num)) return 0;

  switch(capture_size)
    {
    case _320x240:
      buf_w=320; buf_h=240;
      buf_length = buf_w*buf_h*3;
      break;
    case _160x120:
      buf_w=160; buf_h=120;
      buf_length = buf_w*buf_h*3;
      break;
    case _160x120_5x5:
      buf_w=160; buf_h=120;
      buf_length = buf_w*buf_h*3;
      break;
    case _160x120_3x3:
      buf_w=160; buf_h=120;
      buf_length = buf_w*buf_h*3;
      break;
    default:
      buf_w=320; buf_h=240;
      buf_length = buf_w*buf_h*3;
      break;
    }

  return buf_h;
}

int FWCameras::getBufferLength(int camera_num)
{
   int buf_w, buf_h, buf_length;
   if(!cam_num_ok(camera_num)) return(false);

   switch(capture_size)
      {
      case _320x240:
         buf_w=320; buf_h=240;
         buf_length = buf_w*buf_h*3;
         break;
      case _160x120:
         buf_w=160; buf_h=120;
         buf_length = buf_w*buf_h*3;
         break;
      case _160x120_5x5:
         buf_w=160; buf_h=120;
         buf_length = buf_w*buf_h*3;
         break;
      case _160x120_3x3:
         buf_w=160; buf_h=120;
         buf_length = buf_w*buf_h*3;
         break;
      default:
         buf_w=320; buf_h=240;
         buf_length = buf_w*buf_h*3;
         break;
      }

   return buf_length;
}

void FWCameras::init_cameras(bool dma_on_in)
{
   init_variables(dma_on_in);
   bool init_ok = init_firewire();
   if(init_ok) 
      {
         int num_camera=GetNumberOfCameras();
         fprintf(stderr, "Found %d cameras\n", num_cameras);
         for(int cam_num=0; cam_num<num_cameras; cam_num++) 
            {
               fw_ready[cam_num] = start_firewire(cam_num);
               if(cam_num_ok(cam_num))
                  {
                     if (PRINTF_ON)
                        {
                           printf("camera info for camera #%d\n", cam_num);
                           get_camera_info(cam_num);
                        }
                  } 
               else
                  {
                     printf("\ncamera %d will not be usable\n", cam_num);
                  }
            }
      }   
}
