#ifndef VIDEOPRODUCER_H
#define VIDEOPRODUCER_H

#include <QObject>
#include <QAbstractVideoSurface>
#include <QVideoSurfaceFormat>

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
