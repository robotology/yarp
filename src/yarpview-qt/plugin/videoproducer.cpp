#include "videoproducer.h"

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


QAbstractVideoSurface* VideoProducer::videoSurface() const
{
    return m_surface;
}

void VideoProducer::setVideoSurface(QAbstractVideoSurface *surface)
{
    if (m_surface != surface && m_surface && m_surface->isActive()) {
        m_surface->stop();
    }
    m_surface = surface;

}

int VideoProducer::getWidth()
{
    if(!m_format){
        return 0;
    }
    return m_format->frameWidth();
}

int VideoProducer::getHeight()
{
    if(!m_format){
        return 0;
    }
    return m_format->frameHeight();
}

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
            m_format = new QVideoSurfaceFormat(frame->size(),frame->pixelFormat());
            qreal ratio = (qreal)frame->size().width() / (qreal)frame->size().height();
            if(ratio > 2){

                m_format->setFrameSize(frame->size().width(),
                                       frame->size().height() * 2);
            }

            bool b = m_surface->start(*m_format);
            if(b){
                qDebug("Surface STARTED");
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
