/*
 * Copyright (C) 2014 iCub Facility - Istituto Italiano di Tecnologia
 * Author: Davide Perrone
 * Date: Feb 2014
 * email:   dperrone@aitek.it
 * website: www.aitek.it
 *
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */

#include "videoproducer.h"
#include <QVideoFrame>

VideoProducer::VideoProducer(QObject *parent) :
    QObject(parent)
{
    m_format = NULL;
    m_surface = NULL;
}

VideoProducer::~VideoProducer()
{
    if(m_format){
        delete m_format;
    }
}


/*! \brief returns the abstract surface
 *
 *  this property is called by the qml videooutput to get the render surface
 */
QAbstractVideoSurface* VideoProducer::videoSurface() const
{
    return m_surface;
}

/*! \brief sets the abstract surface
 *
 *  this property is called by the qml videooutput and set the abstract surface to its surface
 */
void VideoProducer::setVideoSurface(QAbstractVideoSurface *surface)
{
    if (m_surface != surface && m_surface && m_surface->isActive()) {
        m_surface->stop();
    }
    m_surface = surface;

}

/*! \brief returns the width of the surface
 *  \return the width
 */
int VideoProducer::getWidth()
{
    if(!m_format){
        return 0;
    }
    int w = m_format->frameWidth();
    return w;
}

/*! \brief returns the height of the surface
 *  \return the height
 */
int VideoProducer::getHeight()
{
    if(!m_format){
        return 0;
    }
    int h = m_format->frameHeight();
    return h;
}

/*! \brief This gets the frame and presents it to the abstract surface
 *  \param frame the video frame
 */
void VideoProducer::onNewVideoContentReceived(QVideoFrame *frame)
{
    if (m_surface){
        if(m_surface->isActive() && (m_format->frameWidth() != frame->size().width() ||
                                     m_format->frameHeight() != frame->size().height())){
            m_surface->stop();
            delete m_format;
            m_format = NULL;
        }

        if(!m_surface->isActive()){
            QSize s = frame->size();
            QVideoFrame::PixelFormat f = frame->pixelFormat();
            m_format = new QVideoSurfaceFormat(s,f);
            qreal ratio = (qreal)s.width() / (qreal)s.height();
            if(ratio > 2){

                m_format->setFrameSize(s.width(),
                                       s.height() * 2);
            }

            bool b = m_surface->start(*m_format);
            if(b){
                qDebug("Surface STARTED! Dimensions: %dx%d -- PixelFormat: %d",s.width(),s.height(),(int)f);
            }else{
                qDebug("Surface START ERROR");
                delete m_format;
            }
        }
    }

    if (m_surface && m_format){
        bool b = m_surface->present(*frame);
    }

}
