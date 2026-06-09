#include "ltc2614.h"

#include<QDebug>

static const char *ltc2604_device = "/dev/spidev2.1";
static const char *ltc2604_type550xdevice = "/dev/spidev2.1";

//static const char *dac7811_device = "/dev/spidev1.2";

ltc2614::ltc2614(QObject *parent) : QObject(parent)
{
   // device=SPI_DEVICE;
    bits=SPI_BITS_PER_WORD;
    speed=SPI_SPEED;
    type550x_bits=SPI_BITS_PER_WORD;
    type550x_speed=SPI_SPEED;
    openDevice();
  //  openDigDevice();
    openType550xDevice();


}

ltc2614::~ltc2614() {
    close(fd);
    close(fd_dac7811);
    close(fd_type550);
}

//void ltc2614::writeDAC(int channel,uint16_t value) {

   //qDebug()<<"the count from hwhandler to ltc2614"<<value;
   // uint8_t tx[4];
   // uint8_t rx[4] = {0};

    //tx[0] = 0x00;

    // First byte is 0x31
   // tx[1] = channel;

    // Second byte contains bits 13-6 of the value
    //tx[2] = (value >> 8) & 0x00FF;//HHEXA

    // Third byte contains bits 5-0 of the value, shifted left by 2, with 2 don't-care bits at the end
    //tx[3] = (value) & 0x00FF;//LHEXA

   // struct spi_ioc_transfer tr = {
    //    .tx_buf = (unsigned long)tx,
     //   .rx_buf = (unsigned long)rx,
      //  .len = 4,
        //.speed_hz = speed,
      //  .bits_per_word = bits,
   // };


    //if (ioctl(fd, SPI_IOC_MESSAGE(1), &tr) < 1) {
       // throw std::runtime_error("Failed to send SPI message");
    //}
    
    //if (ioctl(fd, SPI_IOC_MESSAGE(1), &tr) < 1) {
     //   throw std::runtime_error("Failed to send SPI message");
    //}

//}
void ltc2614::writeDAC(int channel,uint16_t value) {

    uint8_t tx[4];
    uint8_t rx[4] = {0};

    tx[0] = 0x00;
    tx[1] = channel;
    tx[2] = (value >> 8) & 0x00FF;
    tx[3] = (value) & 0x00FF;

    struct spi_ioc_transfer tr = {
        .tx_buf = (unsigned long)tx,
        .rx_buf = (unsigned long)rx,
        .len = 4,
        .speed_hz = speed,
        .bits_per_word = bits,
    };

    qDebug() << "[SPI] channel=" << channel
             << "value=" << value
             << "tx=" << tx[0] << tx[1] << tx[2] << tx[3];

    if (ioctl(fd, SPI_IOC_MESSAGE(1), &tr) < 1) {
        throw std::runtime_error("Failed to send SPI message");
    }
    // REMOVED second duplicate ioctl call
}
void ltc2614::writeDAC550x(int channel, uint16_t value)
{
    uint8_t tx[4];
       uint8_t rx[4] = {0};

       tx[0] = 0x00;

       // First byte is 0x31
       tx[1] = channel;

       // Second byte contains bits 13-6 of the value
       tx[2] = (value >> 6) & 0xFF;

       // Third byte contains bits 5-0 of the value, shifted left by 2, with 2 don't-care bits at the end
       tx[3] = (value << 2) & 0xFC;

       struct spi_ioc_transfer tr = {
           .tx_buf = (unsigned long)tx,
           .rx_buf = (unsigned long)rx,
           .len = 4,
           .speed_hz = type550x_speed,
           .bits_per_word = type550x_bits,
       };

      // qDebug()<<tx[0]<<tx[1]<<tx[2]<<tx[3]<<fd;

       if (ioctl(fd_type550, SPI_IOC_MESSAGE(1), &tr) < 1) {
           throw std::runtime_error("Failed to send SPI message");
       }
}

void ltc2614::writeDAC7811(uint16_t value)
{
//qDebug()<<"the writedata7811"<<value;
//    unsigned char Lhexa,Hhexa;
//       value |= 0x1000;

//       Lhexa=value&0x00FF;
//       Hhexa=(value>>8)&0x00FF;

//       char wr_buf[]={Hhexa, Lhexa};
//       write(fd_dac7811, wr_buf, ARRAY_SIZE(wr_buf));
//       printf("\r\n0x%x,0x%x,0x%x",Hhexa,Lhexa,value);
}

void ltc2614::openDevice() {
    fd = open(ltc2604_device,O_RDWR);

    uint8_t mode;
   qDebug()<<mode<<fd;

    if (fd < 0) {
        throw std::runtime_error("Failed to open SPI device");
    }

    // Set SPI mode
    if (ioctl(fd, SPI_IOC_WR_MODE, &mode) < 0) {
        throw std::runtime_error("Failed to set write SPI mode.");
    }

    if (ioctl(fd, SPI_IOC_RD_MODE, &mode) < 0) {
        throw std::runtime_error("Failed to set read SPI mode.");
    }

    // Set bits per word
    if (ioctl(fd, SPI_IOC_WR_BITS_PER_WORD, &bits) < 0) {
        throw std::runtime_error("Failed to set bits per word.");
    }

    if (ioctl(fd, SPI_IOC_RD_BITS_PER_WORD, &bits) < 0) {
        throw std::runtime_error("Failed to set bits per word.");
    }

    // Set max speed (Hz)
    if (ioctl(fd, SPI_IOC_WR_MAX_SPEED_HZ, &speed) < 0) {
        throw std::runtime_error("Failed to set max speed.");
    }

    if (ioctl(fd, SPI_IOC_RD_MAX_SPEED_HZ, &speed) < 0) {
        throw std::runtime_error("Failed to set max speed.");
    }

}

void ltc2614::openDigDevice()
{
//    fd_dac7811 = open(dac7811_device,O_RDWR);
//uint mode;
//           mode |= SPI_CPOL;
//           ioctl(fd_dac7811, SPI_IOC_WR_MODE, &mode);
//           ioctl(fd_dac7811, SPI_IOC_RD_MODE, &mode);
//           ioctl(fd_dac7811, SPI_IOC_WR_BITS_PER_WORD, &bits);
//           ioctl(fd_dac7811, SPI_IOC_RD_BITS_PER_WORD, &bits);
//           ioctl(fd_dac7811, SPI_IOC_WR_MAX_SPEED_HZ, &speed);
    //     ioctl(fd_dac7811, SPI_IOC_RD_MAX_SPEED_HZ, &speed);
}

void ltc2614::openType550xDevice()
{
    fd_type550 = open(ltc2604_type550xdevice,O_RDWR);

    uint8_t mode;
   qDebug()<<mode<<fd_type550;

    if (fd_type550 < 0) {
        throw std::runtime_error("Failed to open SPI device");
    }

    // Set SPI mode
    if (ioctl(fd_type550, SPI_IOC_WR_MODE, &mode) < 0) {
        throw std::runtime_error("Failed to set write SPI mode.");
    }

    if (ioctl(fd_type550, SPI_IOC_RD_MODE, &mode) < 0) {
        throw std::runtime_error("Failed to set read SPI mode.");
    }

    // Set bits per word
    if (ioctl(fd_type550, SPI_IOC_WR_BITS_PER_WORD, &type550x_bits) < 0) {
        throw std::runtime_error("Failed to set bits per word.");
    }

    if (ioctl(fd_type550, SPI_IOC_RD_BITS_PER_WORD, &type550x_bits) < 0) {
        throw std::runtime_error("Failed to set bits per word.");
    }

    // Set max speed (Hz)
    if (ioctl(fd_type550, SPI_IOC_WR_MAX_SPEED_HZ, &type550x_speed) < 0) {
        throw std::runtime_error("Failed to set max speed.");
    }

    if (ioctl(fd_type550, SPI_IOC_RD_MAX_SPEED_HZ, &type550x_speed) < 0) {
        throw std::runtime_error("Failed to set max speed.");
    }
}
