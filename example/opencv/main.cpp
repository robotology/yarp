
#include <cv.h>
#include <cvaux.h>
#include <highgui.h>

#include <yarp/sig/all.h>

using namespace yarp::sig;
using namespace yarp::sig::draw;


int main(int argc, char *argv[]) {
  printf("Show a circle for max 3 seconds...\n");
  ImageOf<PixelRgb> yarpImage;
  yarpImage.resize(300,200);
  addCircle(yarpImage,PixelRgb(255,0,0),
	    yarpImage.width()/2,yarpImage.height()/2,
	    yarpImage.height()/4);
  addCircle(yarpImage,PixelRgb(255,50,50),
	    yarpImage.width()/2,yarpImage.height()/2,
	    yarpImage.height()/5);

  IplImage *cvImage = cvCreateImage(cvSize(yarpImage.width(),  
					   yarpImage.height()), 
				    IPL_DEPTH_8U, 3 );
  
  cvCvtColor((IplImage*)yarpImage.getIplImage(), cvImage, CV_RGB2BGR);

  cvNamedWindow("test",1);
  cvShowImage("test",cvImage);
  cvWaitKey(3000);

  cvDestroyWindow("test");

  cvReleaseImage(&cvImage);
  printf("...done\n");
  return 0;
}
