#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QVector>
#include <QPointF>
#include <QSettings>                   // ← HP.NO persistence
#include <qwt_plot.h>
#include <qwt_plot_curve.h>
#include <qwt_plot_grid.h>
#include <qwt_plot_marker.h>
#include <qwt_legend.h>
#include <qwt_text.h>
#include <qwt_scale_draw.h>
#include <qwt_plot_picker.h>
#include <qwt_plot_canvas.h>
#include <qwt_picker_machine.h>
#include <qwt_series_data.h>
#include "adcreader.h"
#include "hwhandler.h"
#include "keypaddialog.h"
#include <QEvent>
#include <QLineEdit>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void onStartSweep();
    void on_butDAC_clicked();
    void onClearPlot();
    void onSampleReady(AdcSample sample);
    void onSweepFinished();
    void onError(QString msg);
    void onMinFreqChanged();
    void onMaxFreqChanged();
    void onTabChanged(int index);
    void onHPNoChanged();              // ← HP.NO slot
    void onSetDacManual();             // ← manual DAC set from UI spinbox

private:
    Ui::MainWindow *ui;

    // ── Plot — three curves (two for main tab, one for impedance tab) ──────
    QwtPlot       *m_plot       = nullptr;   // Main plot (current & voltage)
    QwtPlot       *m_plotZ      = nullptr;   // Impedance plot
    QwtPlotCurve  *m_curveCh2   = nullptr;   // primary ADC channel (from combo)
    QwtPlotCurve  *m_curveCh3   = nullptr;   // CH3 0xE7 voltage counts
    QwtPlotCurve  *m_curveZ     = nullptr;   // Impedance ratio (V/I)
    QwtPlotMarker *m_peakMarker = nullptr;   // CH2 peak vertical line
    QwtPlotMarker *m_peakMarkerZ= nullptr;   // Impedance peak vertical line
    QwtPlotGrid   *m_grid       = nullptr;
    QwtPlotGrid   *m_gridZ      = nullptr;
    QwtLegend     *m_legend     = nullptr;
    QwtLegend     *m_legendZ    = nullptr;
    // ── Crosshair tracker ─────────────────────────────────────────────────
    QwtPlotPicker *m_pickerMain = nullptr;
    QwtPlotPicker *m_pickerZ    = nullptr;

    QVector<QPointF> m_dataCh2;
    QVector<QPointF> m_dataCh3;
    QVector<QPointF> m_dataZ;

    // ── ADC reader ────────────────────────────────────────────────────────
    AdcReader   *m_adc     = nullptr;
    hwhandler   *m_hw      = nullptr;   // owns LTC2614 SPI DAC
    bool         m_running = false;
    int          m_dacPower         = DAC_POWER_IDLE;  // last power% sent to LTC2614
    bool         m_dacManualOverride = false;           // true = user has locked DAC manually
    int          m_sweepCounter     = 0;                // counts completed sweeps for filenames

    double m_minFreq  = 38.0;
    double m_maxFreq  = 46.0;
    double m_peakFreq = 0.0;
    double m_peakVal  = 0.0;
    double m_peakZFreq = 0.0;
    double m_peakZVal  = 0.0;

    QString m_hpNumber;                // ← HP.NO stored here

    // Returns the SPI command byte for the currently selected ADC channel
    uint8_t selectedChannelCmd() const;

    void setupPlot();
    void setupImpedancePlot();
    void updatePeakMarker();
    void updatePeakMarkerZ();
    void saveToFile();
    void calculateImpedance();
    void updateDacDisplay();          // ← refreshes labelDacValue from m_adc->dacCount()

    bool eventFilter(QObject *obj, QEvent *event) override;
    void openKeypadFor(QLineEdit *lineEdit, double &targetVar, const QString &label);
};

#endif // MAINWINDOW_H
