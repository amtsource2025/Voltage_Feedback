#ifndef ADCREADER_H
#define ADCREADER_H

#include <QObject>
#include <QCoreApplication>
#include <functional>
#include <stdint.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/spi/spidev.h>
#include <sys/mman.h>
#include <cstring>

// ── SPI device ──────────────────────────────────────────────────────────────
#define ADS7841_SPI_DEV     "/dev/spidev2.0"
#define ADS7841_SPI_SPEED   1000000
#define ADS7841_SPI_BITS    8

// ── ADS7841 channel command bytes ──────────────────────────────────────────
#define ADS7841_CH0_FS          0x97
#define ADS7841_CH2_PHACO       0xA7
#define ADS7841_CH1_SENSOR      0xD7
#define ADS7841_CH3_VOLTAGE     0xE7
#define ADC_FULLSCALE           4095

// ── FPGA registers ──────────────────────────────────────────────────────────
#define FPGA_BASE_ADDR      0x43C20000UL
#define FPGA_MAP_SIZE       0x1000UL
#define FPGA_MAP_MASK       (FPGA_MAP_SIZE - 1)

#define REG_PHACO_ONOFF     0x00
#define REG_FS_COUNT        0x02
#define REG_PULSE_COUNT     0x04
#define REG_PDM_MODE        0x06
#define REG_FREQ_COUNT      0x0C
#define REG_TUNE_REQ        0x0E

#define TUNE_REQUEST_MASK   0x8000
#define PDM_CONTINUOUS      0x01

// ── LTC2614 SPI DAC power levels (via hwhandler::convert_dac) ──────────────
// convert_dac(0x30, val) where val is 0-100 (percent).
// val=0   → dac_value=39321 → ~3.05 V  (idle / pre- / post-sweep)
// val=100 → dac_value=64224 → 5.00 V  (during active sweep only)
#define DAC_POWER_IDLE      100       // percent — ~3.05 V idle
#define DAC_POWER_ACTIVE    100     // percent — 5.00 V during sweep

// ── Sweep defaults ──────────────────────────────────────────────────────────
#define DEFAULT_COUNT_HIGH  2631    // 100000/2631 = 38.01 kHz
#define DEFAULT_COUNT_LOW   2174    // 100000/2174 = 46.00 kHz
#define SAMPLES_PER_STEP    128

// ── Sample carries raw 12-bit counts (0–4095) ───────────────────────────────
struct AdcSample {
    double freqKHz;
    double ch2_counts;    // primary channel selected via combo (0–4095)
    double ch3_counts;    // CH3 0xE7 voltage sensor            (0–4095)
};

class AdcReader : public QObject
{
    Q_OBJECT
public:
    explicit AdcReader(QObject *parent = nullptr);
    ~AdcReader();

    bool openHardware();
    void closeHardware();

    // Optional callback: called by runSweep to set DAC power level (0–100%).
    // Connect this to hwhandler::convert_dac(0x30, power) from the caller.
    // If not set, DAC is not touched (backward compatible).
    std::function<void(int power)> onSetDacPower;

    // ch2Cmd: ADS7841 command byte for the primary channel (from combo box)
    void runSweep(int countHigh, int countLow, int samplesPerStep,
                  uint8_t ch2Cmd = ADS7841_CH2_PHACO);

signals:
    void sampleReady(AdcSample sample);
    void sweepFinished();
    void errorOccurred(QString msg);

private:
    int      m_spiFd   = -1;
    int      m_memFd   = -1;
    void    *m_mapBase = nullptr;
    void    *m_mapDev  = nullptr;

    uint16_t spiRead(uint8_t cmd);
    void     writeReg(uint32_t offset, uint16_t val);
    void     phaco_on(uint16_t fsCount);
    void     phaco_off();
    void     emitTuneStart();
    void     emitTuneStop();
    void     setFreqCount(uint16_t cnt);
};

#endif // ADCREADER_H
