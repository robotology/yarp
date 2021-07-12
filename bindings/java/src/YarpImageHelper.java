/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
 */

package yarp.matlab;

import yarp.PixelRgb;
import yarp.ImageRgb;
import yarp.ImageFloat;
import yarp.PixelRgbFloat;
import yarp.ImageRgbFloat;
import yarp.charArray;

/**
 * Helper class to convert YARP image to Matlab matrices (images).
 */
public class YarpImageHelper
{
    short [][][] sarray1;
    short [][] array2d;
    short [][] arrayfull;
    float [][] arrFltFull;
    int width;
    int height;

    /**
     * Constructor, specify height and width of the image you are going to
     * convert.
     * @param h image height
     * @param w image width
     */
    public YarpImageHelper(int h, int w)
    {
        sarray1 = new short [h][w][3];
        array2d = new short [h][w];
        arrayfull = new short [h][3*w];
        arrFltFull = new float [h][w];
        height=h;
        width=w;
    }

    /**
     * Converts a YARP image to grayscale.
     * Returns a height by width two dimensional array. This array is
     * compatible with a Matlab matrix.
     * @param img input image
     * @return the two dimensional array which contains the image
     */
    public short [][] getMonoMatrix(ImageRgb img)
    {
        for(int r=0; r<img.height(); r++)
            for(int c=0; c<img.width(); c++)
                {
                    PixelRgb p=img.pixel(c,r);
                    double temp= ( 0.3*(double)p.getR()+
                                   0.59*(double)p.getG()+
                                   0.11*(double)p.getB());

                    array2d[r][c]=(short)(temp+0.5);
                }

        return array2d;
    }

    /**
     * Converts a YARP image to a 3D array.
     * This array is compatible with a [HxWx3] Matlab matrix. Note:
     * passing 3d arrays from/to Matlab/Java is SLOW (at least on
     * Matlab 6.5); use getB/getR/getG or get2DMatrix.
     * @param img input image
     * @return the three dimensional array which contains the image
     */
    public short [][][] get3DMatrix(ImageRgb img)
    {
        for(int r=0; r<img.height(); r++)
            for(int c=0; c<img.width(); c++)
                {
                    PixelRgb p=img.pixel(c,r);
                    sarray1[r][c][0] = (short) p.getR();
                    sarray1[r][c][1] = (short) p.getG();
                    sarray1[r][c][2] = (short) p.getB();
                }

        return sarray1;
    }

    /**
     * Access a YARP image by planes.
     * Returns a 2D array which contains the red plane, this is a
     * [HxW] array compatible with Matlab.
     * @param img input image
     * @return the two dimensional array which contains the red plane
     */
    public short [][] getR(ImageRgb img)
    {
        for(int r=0; r<img.height(); r++)
            for(int c=0; c<img.width(); c++)
                {
                    PixelRgb p=img.pixel(c,r);
                    array2d[r][c] = (short) p.getR();
                }
        return array2d;
    }

    /**
     * Access a YARP image by planes.
     * Returns a 2D array which contains the green plane, this is a
     * [HxW] array compatible with Matlab.
     * @param img input image
     * @return the two dimensional array which contains the green plane
     */
    public short [][] getG(ImageRgb img)
    {
        for(int r=0; r<img.height(); r++)
            for(int c=0; c<img.width(); c++)
                {
                    PixelRgb p=img.pixel(c,r);
                    array2d[r][c] = (short) p.getG();
                }
        return array2d;
    }

    /**
     * Access a YARP image by planes.
     * Returns a 2D array which contains the blue plane, this is a
     * [HxW] array compatible with Matlab.
     * @param img input image
     * @return the two dimensional array which contains the blue plane
     */
    public short [][] getB(ImageRgb img)
    {
        for(int r=0; r<img.height(); r++)
            for(int c=0; c<img.width(); c++)
                {
                    PixelRgb p=img.pixel(c,r);
                    array2d[r][c] = (short) p.getB();
                }
        return array2d;
    }

    /**
     * Converts a color YARP image to a two dimensional array.
     * Returns a [Hx3*W] array which contains the 'justaposition' of the
     * three color planes of the image. This array can be copied into a
     * Matlab matrix:
     * OUT=[R|G|B]
     * where R, G, B are three [HxW] matrices formed by the color
     * planes. From OUT you can create a color image [HxWx3] by typing:
     * IMG=uint8(H,W,3);
     * IMG(:,:,1)=OUT(:,1:W);
     * IMG(:,:,2)=OUT(:,W+1:2*W);
     * IMG(:,:,3)=OUT(:,2*W+1:3*W);
     *
     * @param img input image
     * @return output array
     */
    public short [][] get2DMatrix(ImageRgb img)
    {
        for(int r=0; r<img.height(); r++)
            for(int c=0; c<img.width(); c++)
                {
                    PixelRgb p=img.pixel(c,r);
                    arrayfull[r][c] = (short) p.getR();
                    arrayfull[r][c+width] = (short) p.getG();
                    arrayfull[r][c+2*width] = (short) p.getB();
                }

        return arrayfull;
    }

    /**
     * Converts a floating point YARP image to a two dimensional array.
     * @param img input image
     * @return output array
     */
    public float [][] get2DMatrix(ImageFloat img)
    {
        for(int r=0; r<img.height(); r++)
            for(int c=0; c<img.width(); c++)
                {
                    arrFltFull[r][c] = img.getPixel(c,r);
                }

        return arrFltFull;
    }


    /**
     * Access a floating point YARP image by planes.
     * Returns a 2D array which contains the red plane, this is a
     * [HxW] array compatible with Matlab.
     * @param img input image
     * @return the two dimensional array which contains the red plane
     */
    public float [][] get2DMatrixRed(ImageRgbFloat img)
    {
        for(int r=0; r<img.height(); r++)
            for(int c=0; c<img.width(); c++)
                {
                    PixelRgbFloat p=img.pixel(c,r);
                    arrFltFull[r][c] = p.getR();
                }

        return arrFltFull;
    }

    /**
     * Access a floating point YARP image by planes.
     * Returns a 2D array which contains the green plane, this is a
     * [HxW] array compatible with Matlab.
     * @param img input image
     * @return the two dimensional array which contains the green plane
     */
    public float [][] get2DMatrixGreen(ImageRgbFloat img)
    {
        for(int r=0; r<img.height(); r++)
            for(int c=0; c<img.width(); c++)
                {
                    PixelRgbFloat p=img.pixel(c,r);
                    arrFltFull[r][c] = p.getG();
                }

        return arrFltFull;
    }


    /**
     * Access a floating point YARP image by planes.
     * Returns a 2D array which contains the blue plane, this is a
     * [HxW] array compatible with Matlab.
     * @param img input image
     * @return the two dimensional array which contains the blue plane
     */
    public float [][] get2DMatrixBlue(ImageRgbFloat img)
    {
        for(int r=0; r<img.height(); r++)
            for(int c=0; c<img.width(); c++)
                {
                    PixelRgbFloat p=img.pixel(c,r);
                    arrFltFull[r][c] = p.getB();
                }

        return arrFltFull;
    }


    /**
     * Fast conversion of a YARP image to a one dimensional array.
     * In MATLAB, USE: reshape(OUT, [height width pixelsize]); Extended function to work with padding (Vadim Tikhanoff)
     * \author Leo Pape
     * @param img input image
     * @return output array
     */
    public static short[] getRawImg(ImageRgb img) {
        int pixelsize = (int) img.getPixelSize();
        int width = (int) img.width();
        int height = (int) img.height();
        int imgsize = (int) img.getRawImageSize();
        int imgPixels = width * height * pixelsize;
        int pad = (int) img.getPadding();
        //short [] vec1ds = new short [imgsize];
        short [] vec1ds = new short [imgPixels];
        charArray car = charArray.frompointer(img.getRawImage());
        int index = 0;
        for(int r=0; r<height; r++)
        {
            for(int c=0; c<width; c++)
            {
                for(int p=0; p<pixelsize; p++)
                {
                    //vec1ds[(c * height) + r + (p * width * height)] = (short) car.getitem((r * width * pixelsize) + (c * pixelsize) + p);
                    vec1ds[(c * height) + r + (p * width * height)] = (short) car.getitem(index);
                    index ++;

                }
            }
            index += pad;
        }
        return vec1ds;
    }

    /**
    *Fast conversion from matlab image format (based on leo's structure) to a YARP image format
    * in MATLAB, the reshape function to modify the matrix to  be a 1d vector IN = reshape(label, [h*w*pixelSize 1]);
    * and, if not previously done cast it to int16 eg: tempImg = cast(IN,'int16');
    *
    * \author Vadim Tikhanoff
    * @param input array height int width int pixelSize int
    * @return output yarp image
    */
    public ImageRgb setRawImg(short [] vec1ds, int height, int width, int pixelSize){
        ImageRgb img = new ImageRgb();
        img.resize(width, height);
        img.zero();
        charArray tempImg = charArray.frompointer(img.getRawImage());
        for(int r=0; r<height; r++)
            for(int c=0; c<width; c++)
                for(int p=0; p<pixelSize; p++)
                    tempImg.setitem( (r * width * pixelSize) + (c * pixelSize) + p,  vec1ds[(c * height) + r + (p * width * height)] );
        return img;
    }
}
