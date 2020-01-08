/*
 * Copyright (C) 2006-2020 Istituto Italiano di Tecnologia (IIT)
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

#include "videoproducer.h"
#include <QVideoFrame>

VideoProducer::VideoProducer(QObject *parent) :
    QObject(parent)
{
    m_format = nullptr;
    m_surface = nullptr;
}

VideoProducer::~VideoProducer()
{
    if(m_format)
    {
        delete m_format;
        m_format = nullptr;
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
                                     m_format->frameHeight() != frame->size().height()))
        {
            m_surface->stop();
            delete m_format;
            m_format = nullptr;
        }

        if (!m_surface->isActive())
        {
            QSize s = frame->size();
            QVideoFrame::PixelFormat f = frame->pixelFormat();
            m_format = new QVideoSurfaceFormat(s, f);

            bool b = m_surface->start(*m_format);
            if (b)
            {
                qDebug("Surface STARTED! Dimensions: %dx%d -- PixelFormat: %d", s.width(), s.height(), (int)f);
            }
            else
            {
                qDebug("Surface START ERROR");
                delete m_format;
            }
            emit resizeWindowRequest();
        }
    }

    if (m_surface && m_format){
        bool b = m_surface->present(*frame);
        if (!b) {
            qWarning("Surface PRESENT ERROR");
        }
    }

}
