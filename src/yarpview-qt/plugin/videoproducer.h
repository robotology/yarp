/*
 * Copyright (C) 2014 iCub Facility - Istituto Italiano di Tecnologia
 * Author: Davide Perrone
 * Date: Feb 2014
 * email:   dperrone@aitek.it
 * website: www.aitek.it
 *
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */

#ifndef VIDEOPRODUCER_H
#define VIDEOPRODUCER_H

#include <QObject>
#include <QAbstractVideoSurface>
#include <QVideoSurfaceFormat>

/*! \class VideoProducer
    \brief The videoproducer class

    This class has a QAbstractVideoSurface where the video frame is rendered and used by the QML VideoOutput
*/
class VideoProducer : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QAbstractVideoSurface *videoSurface READ videoSurface WRITE setVideoSurface)
    Q_PROPERTY(int frameWidth READ getWidth)
    Q_PROPERTY(int frameHeight READ getHeight)

public:
    VideoProducer(QObject *parent = 0);
    ~VideoProducer();

    QAbstractVideoSurface *videoSurface() const;
    void setVideoSurface(QAbstractVideoSurface *surface);

    int getWidth();
    int getHeight();

private:
    QAbstractVideoSurface *m_surface;
    QVideoSurfaceFormat *m_format;

signals:

public slots:
    void onNewVideoContentReceived(QVideoFrame *frame);


};

#endif // VIDEOPRODUCER_H
