#ifndef VACCUM_H
#define VACCUM_H

#include <QObject>
#include <QVector>
#include <QTimer>

class Vaccum : public QObject
{
    Q_OBJECT
public:
    Vaccum();
    ~Vaccum();
    int convert(uint8_t channel);
    void initSPI();
    float stabilize();

private:
    QTimer *timer;
    int spi_fd;

};

#endif // VACCUM_H
