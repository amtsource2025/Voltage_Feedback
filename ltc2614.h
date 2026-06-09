#ifndef LTC2614_H
#define LTC2614_H

#include <QObject>

#include <iostream>
#include <fstream>
#include <stdexcept>
#include <string>
#include <cstring>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/spi/spidev.h>

#define SPI_DEVICE "/dev/spidev2.1"
#define SPI_DEVICE_2 "/dev/spidev1.2"
#define SPI_BITS_PER_WORD 8
#define SPI_SPEED 1000000
#define ARRAY_SIZE(a) (sizeof(a) / sizeof((a)[0]))
class ltc2614 : public QObject
{
    Q_OBJECT
public:
    explicit ltc2614(QObject *parent = nullptr);
    ~ltc2614();
    void openDevice();
    void openDigDevice();
    void openType550xDevice();
    void writeDAC(int channel,uint16_t value);
    void writeDAC550x(int channel,uint16_t value);
    void writeDAC7811(uint16_t value);
    //uint16_t value=0x0000;

private:
    std::string device;
    int fd;
    int fd_type550;
int fd_dac7811;
uint32_t speed;
    uint8_t bits;
    uint32_t type550x_speed;
        uint8_t type550x_bits;

signals:

};

#endif // LTC2614_H


