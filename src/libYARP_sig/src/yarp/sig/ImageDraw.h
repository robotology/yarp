/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef YARP_SIG_IMAGEDRAW_H
#define YARP_SIG_IMAGEDRAW_H

#include <cmath>

#include <yarp/sig/Image.h>


namespace yarp {
    namespace sig{
        /**
         * \ingroup sig_class
         *
         *  Very basic drawing functions, in case you don't have
         *  anything better available.
         */
        namespace draw {

            template <class T>
            void addSegment(ImageOf<T>& dest, const T& pix,
                            int x, int y, int dx, int dy) {
                const double vx = double(dx - x);
                const double vy = double(dy - y);
                // can't use fmax - not portable --paulfitz
                double vbigger = fabs((fabs(vy)>fabs(vx))?vy:vx);
                const int steps = int(2*vbigger);
                const double r = 1.0 / steps;
                for (int i = 0; i <= steps; i++) {
                    dest.safePixel(int(x+vx*i*r),int(y+vy*i*r)) = pix;
                }
            }

            template <class T>
            void addCircle(ImageOf<T>& dest, const T& pix,
                           int i, int j, int r) {
                float d, r2 = (float)(r*r);
                for (int ii=i-r; ii<=i+r; ii++) {
                    for (int jj=j-r; jj<=j+r; jj++) {
                        d = float((ii-i)*(ii-i)+(jj-j)*(jj-j));
                        if (d<=r2) {
                            dest.safePixel(ii,jj) = pix;
                        }
                    }
                }
            }

            template <class T>
            void addCrossHair(ImageOf<T>& dest, const T& pix,
                              int i, int j, int r) {
                for (int ii=i-r; ii<=i+r; ii++) {
                    for (int jj=j-r; jj<=j+r; jj++) {
                        if (ii==i||jj==j) {
                            dest.safePixel(ii,jj) = pix;
                        }
                    }
                }
            }

            template <class T>
            void addCircleOutline(ImageOf<T>& dest, const T& pix,
                                  int i, int j, int r) {
                float d, r2 = float(r*r), r2l = float((r-1.1)*(r-1.1));
                for (int ii=i-r; ii<=i+r; ii++) {
                    for (int jj=j-r; jj<=j+r; jj++) {
                        d = float((ii-i)*(ii-i)+(jj-j)*(jj-j));
                        if (d<=r2 && d>=r2l) {
                            dest.safePixel(ii,jj) = pix;
                        }
                    }
                }
            }

            template <class T>
            void addOvalOutline(ImageOf<T>& dest, const T& pix,
                                int i, int j, int h2, int w2) {
                float x, y;
                for (float th=0; th<2.0*3.14159; th+=0.01) {
                    x = j+w2*cos(th);
                    y = i+h2*sin(th);
                    dest.safePixel((int)y,(int)x) = pix;
                }
            }


            template <class T>
            void addRectangleOutline(ImageOf<T>& dest, const T& pix,
                                     int i, int j, int w, int h) {
                for (int ii=i-w; ii<=i+w; ii++) {
                    dest.safePixel(ii,j-h) = pix;
                    dest.safePixel(ii,j-h+1) = pix;
                    dest.safePixel(ii,j+h) = pix;
                    dest.safePixel(ii,j+h-1) = pix;
                }
                for (int jj=j-h; jj<=j+h; jj++) {
                    dest.safePixel(i-w,jj) = pix;
                    dest.safePixel(i-w+1,jj) = pix;
                    dest.safePixel(i+w,jj) = pix;
                    dest.safePixel(i+w-1,jj) = pix;
                }
            }

            /**
             * warning : i, j is x, y center of rectangle
             */
            template <class T>
            void addRectangle(ImageOf<T>& dest, const T& pix,
                              int i, int j, int w, int h) {
                for (int ii=i-w; ii<=i+w; ii++) {
                    for (int jj=j-h; jj<=j+h; jj++) {
                        dest.safePixel(ii,jj) = pix;
                    }
                }
            }

            template <class T>
            int applyThreshold(ImageOf<T>& src, ImageOf<T>& dest,
                               const T& thetalo, const T& thetahi,
                               const T& pix0, const T& pix1) {
                int h = src.height();
                int w = src.width();
                for (int i=0; i<h; i++) {
                    for (int j=0; j<w; j++) {
                        if (src(i,j)>=thetalo && src(i,j)<=thetahi) {
                            dest(i,j) = pix1;
                        } else {
                            dest(i,j) = pix0;
                        }
                    }
                }
                return 0;
            }

            template <class T>
            void setImagePixels(ImageOf<T>& src, const T& pix) {
                int h = src.height();
                int w = src.width();
                for (int i=0; i<h; i++) {
                    for (int j=0; j<w; j++) {
                        src(i,j) = pix;
                    }
                }
            }

#ifndef IMGFOR
#define IMGFOR(img,i,j) for (size_t i=0; i<(img).width(); i++) for (size_t j=0; j<(img).height(); j++)
#endif

        }
    }
}  // end namespace yarp::sig::draw

#endif // YARP_SIG_IMAGEDRAW_H
