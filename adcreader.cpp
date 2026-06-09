#include "adcreader.h"
#include <QDebug>

AdcReader::AdcReader(QObject *parent) : QObject(parent) {}

AdcReader::~AdcReader()
{
    closeHardware();
}

// ─────────────────────────────────────────────────────────────────────────────
bool AdcReader::openHardware()
{
    m_spiFd = open(ADS7841_SPI_DEV, O_RDWR);
    if (m_spiFd < 0) {
        emit errorOccurred(QString("Cannot open %1").arg(ADS7841_SPI_DEV));
        return false;
    }

    uint8_t  mode  = 0;
    uint8_t  bits  = ADS7841_SPI_BITS;
    uint32_t speed = ADS7841_SPI_SPEED;

    ioctl(m_spiFd, SPI_IOC_WR_MODE,          &mode);
    ioctl(m_spiFd, SPI_IOC_RD_MODE,          &mode);
    ioctl(m_spiFd, SPI_IOC_WR_BITS_PER_WORD, &bits);
    ioctl(m_spiFd, SPI_IOC_RD_BITS_PER_WORD, &bits);
    ioctl(m_spiFd, SPI_IOC_WR_MAX_SPEED_HZ,  &speed);
    ioctl(m_spiFd, SPI_IOC_RD_MAX_SPEED_HZ,  &speed);

    m_memFd = open("/dev/mem", O_RDWR | O_SYNC);
    if (m_memFd < 0) {
        emit errorOccurred("Cannot open /dev/mem");
        close(m_spiFd); m_spiFd = -1;
        return false;
    }

    m_mapBase = mmap(nullptr, FPGA_MAP_SIZE, PROT_READ | PROT_WRITE,
                     MAP_SHARED, m_memFd, FPGA_BASE_ADDR & ~FPGA_MAP_MASK);
    if (m_mapBase == MAP_FAILED) {
        emit errorOccurred("mmap failed");
        close(m_memFd); m_memFd = -1;
        close(m_spiFd); m_spiFd = -1;
        return false;
    }
    m_mapDev = static_cast<uint8_t*>(m_mapBase) + (FPGA_BASE_ADDR & FPGA_MAP_MASK);

    qDebug() << "HW open OK — SPI fd=" << m_spiFd;
    return true;
}

// ─────────────────────────────────────────────────────────────────────────────
void AdcReader::closeHardware()
{
    if (m_mapBase && m_mapBase != MAP_FAILED) {
        munmap(m_mapBase, FPGA_MAP_SIZE);
        m_mapBase = nullptr;
        m_mapDev  = nullptr;
    }
    if (m_memFd >= 0) { close(m_memFd); m_memFd = -1; }
    if (m_spiFd >= 0) { close(m_spiFd); m_spiFd = -1; }
}

// ─────────────────────────────────────────────────────────────────────────────
void AdcReader::writeReg(uint32_t offset, uint16_t val)
{
    *reinterpret_cast<volatile uint16_t*>(
        static_cast<uint8_t*>(m_mapDev) + offset) = val;
}

// ── ADS7841: send 3 bytes, 12-bit left-aligned result → >>3 ─────────────────
uint16_t AdcReader::spiRead(uint8_t cmd)
{
    if (m_spiFd < 0) return 0;

    uint8_t tx[3] = { cmd, 0x00, 0x00 };
    uint8_t rx[3] = { 0,   0,    0    };

    struct spi_ioc_transfer tr;
    memset(&tr, 0, sizeof(tr));
    tr.tx_buf        = reinterpret_cast<unsigned long>(tx);
    tr.rx_buf        = reinterpret_cast<unsigned long>(rx);
    tr.len           = 3;
    tr.speed_hz      = ADS7841_SPI_SPEED;
    tr.bits_per_word = ADS7841_SPI_BITS;

    if (ioctl(m_spiFd, SPI_IOC_MESSAGE(1), &tr) < 0)
        return 0;

    return static_cast<uint16_t>(((rx[1] << 8) | rx[2]) >> 3);
}

// ─────────────────────────────────────────────────────────────────────────────
void AdcReader::phaco_on(uint16_t fsCount)
{
    writeReg(REG_PHACO_ONOFF, static_cast<uint16_t>((fsCount << 1) | 0x01));
}

void AdcReader::phaco_off()
{
    writeReg(REG_PHACO_ONOFF, 0x0000);
    writeReg(REG_FREQ_COUNT,  0x0000);
}

void AdcReader::emitTuneStart()
{
    writeReg(REG_TUNE_REQ, TUNE_REQUEST_MASK);
}

void AdcReader::emitTuneStop()
{
    writeReg(REG_TUNE_REQ, 0x0000);
}

void AdcReader::setFreqCount(uint16_t cnt)
{
    writeReg(REG_FREQ_COUNT, cnt);
}

// ─────────────────────────────────────────────────────────────────────────────
//  Sweep: countHigh (low kHz) → countLow (high kHz)
//  ch2Cmd: SPI command byte for the primary channel (selected via combo box)
//  Always reads CH3 (0xE7) as the secondary voltage channel
// ─────────────────────────────────────────────────────────────────────────────
void AdcReader::runSweep(int countHigh, int countLow, int samplesPerStep,
                         uint8_t ch2Cmd)
{
    if (m_spiFd < 0 || m_mapDev == nullptr) {
        emit errorOccurred("Hardware not open");
        return;
    }

    writeReg(REG_FS_COUNT,    static_cast<uint16_t>(countHigh));
    writeReg(REG_PULSE_COUNT, static_cast<uint16_t>(500));
    writeReg(REG_PDM_MODE,    PDM_CONTINUOUS);
    phaco_on(static_cast<uint16_t>(countHigh));
    emitTuneStart();

    // ── Raise LTC2614 DAC to 5 V (100%) only during active sweep ─────────
    if (onSetDacPower) onSetDacPower(DAC_POWER_ACTIVE);

    usleep(10000);

    for (int cnt = countHigh; cnt >= countLow; cnt -= 2) {
        setFreqCount(static_cast<uint16_t>(cnt));
        usleep(300);

        double sum2 = 0.0, sum3 = 0.0;
        for (int s = 0; s < samplesPerStep; ++s) {
            sum2 += spiRead(ch2Cmd);
            sum3 += spiRead(ADS7841_CH3_VOLTAGE);
        }

        AdcSample sample;
        sample.freqKHz    = 100000.0 / static_cast<double>(cnt);
        sample.ch2_counts = sum2 / samplesPerStep;
        sample.ch3_counts = sum3 / samplesPerStep;

        emit sampleReady(sample);
        QCoreApplication::processEvents();
    }

    emitTuneStop();
    phaco_off();

    // ── Lower LTC2614 DAC back to idle (~3.05 V, power=0%) after sweep ───
    if (onSetDacPower) onSetDacPower(DAC_POWER_IDLE);

    emit sweepFinished();
}
