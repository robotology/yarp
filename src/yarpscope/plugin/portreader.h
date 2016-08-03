#ifndef PORTREADER_H
#define PORTREADER_H

#include <QObject>

class PortReader : public QObject
{
    Q_OBJECT
public:
    explicit PortReader(QObject *parent = 0);
    static PortReader *instance();

    void acquireData(const QString &remotePortName,
                         int index,
                         const QString &localPortName,
                         const QString &carrier,
                         bool persistent);

private:
    static PortReader *self;
signals:

public slots:

};

#endif // PORTREADER_H
