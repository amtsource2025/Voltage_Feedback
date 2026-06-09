#include "vaccum.h"

#include <QDebug>
#include <QTimer>
#include <QFile>
#include <QProcess>
#include <stdint.h>
#include <cstdio>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/types.h>
#include <linux/spi/spidev.h>
#include <vector>
#include <iostream>
Vaccum::Vaccum()
{
    initSPI();

    //timer = new QTimer(this);
   // connect(timer, SIGNAL(timeout()), this, SLOT(stabilize()));
   // timer->start(100); // milliseconds
}

void Vaccum::initSPI()
{
    spi_fd = open("/dev/spidev2.0", O_RDWR);

    int mode;
    int bits = 8;
    int speed = 1000000;
    ioctl(spi_fd, SPI_IOC_WR_MODE, &mode);
    ioctl(spi_fd, SPI_IOC_RD_MODE, &mode);
    ioctl(spi_fd, SPI_IOC_WR_BITS_PER_WORD, &bits);
    ioctl(spi_fd, SPI_IOC_RD_BITS_PER_WORD, &bits);
    ioctl(spi_fd, SPI_IOC_WR_MAX_SPEED_HZ, &speed);
    ioctl(spi_fd, SPI_IOC_RD_MAX_SPEED_HZ, &speed);
}

Vaccum::~Vaccum()
{
    close(spi_fd);
}


float Vaccum::stabilize()
{
    int sum = 0;
    for(int i=0; i<25; i++)
    {
        sum += (int)convert(0xD7);
    }

    int avg = (sum/25) - 10;

    //vaccum
    float val = avg * 0.17;

    //pressure
    //float val = avg * 0.0236;

    //qDebug()<<val<<avg;

    return val;

}

int Vaccum::convert(uint8_t channel)
{
    uint8_t tx[3] = {channel, 0x00, 0x00};

    uint8_t rx[3] = {0x00, 0x00, 0x00};
    uint16_t sample = 0;
    uint16_t delay = 0;
    uint8_t bits = 8;
    uint32_t speed = 1000000;

    struct spi_ioc_transfer tr = {
        .tx_buf = (unsigned long)tx,
        .rx_buf = (unsigned long)rx,
        .len = 3,
        .speed_hz = speed,
        .delay_usecs = delay,
        .bits_per_word = bits,
    };

    /* send the cmd to start the conversion and read the result */
    ioctl(spi_fd, SPI_IOC_MESSAGE(1), &tr);
    //qDebug()<<rx[0]<<rx[1];

    sample =  (rx[2] + (rx[1] << 8)) >> 3;
   // qDebug()<<sample;

    return sample;
}
