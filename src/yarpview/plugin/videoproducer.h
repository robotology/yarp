/*
 * Copyright (C) 2006-2021 Istituto Italiano di Tecnologia (IIT)
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

#ifndef VIDEOPRODUCER_H
#define VIDEOPRODUCER_H

#include <QObject>
#include <QMutex>
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
    QString getPixelAsStr(int x, int y);

    QAbstractVideoSurface *videoSurface() const;
    void setVideoSurface(QAbstractVideoSurface *surface);

    int getWidth();
    int getHeight();

private:
    QAbstractVideoSurface *m_surface;
    QVideoSurfaceFormat *m_format;
    QVideoFrame *m_frame;  // Stores the value of the current video frame to allow "color picking"
    QMutex mutex;

signals:
    void resizeWindowRequest();

public slots:
    void onNewVideoContentReceived(QVideoFrame *frame);


};

#endif // VIDEOPRODUCER_H
