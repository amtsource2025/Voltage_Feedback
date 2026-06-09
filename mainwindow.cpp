#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QMessageBox>
#include <QPen>
#include <QColor>
#include <QFile>
#include <QDir>
#include <QTextStream>
#include <QDateTime>
#include <qwt_plot_canvas.h>
#include <qwt_text.h>
#include <QDebug>
#include <qwt_plot_picker.h>
#include <qwt_text.h>
#include <QVector>
#include <QPointF>
#include <QSettings>                   // ← HP.NO persistence

class SweepTracker : public QwtPlotPicker
{
public:
    // pass pointers to the live data vectors so it can snap to real points
    SweepTracker(QWidget *canvas,
                 const QVector<QPointF> *dataCh2,
                 const QVector<QPointF> *dataCh3,
                 const QVector<QPointF> *dataZ,
                 bool isImpedancePlot = false)
        : QwtPlotPicker(canvas)
        , m_ch2(dataCh2), m_ch3(dataCh3), m_dataZ(dataZ)
        , m_isZ(isImpedancePlot)
    {
        setTrackerMode(QwtPicker::AlwaysOn);
        setRubberBand(QwtPicker::CrossRubberBand);
        setStateMachine(new QwtPickerTrackerMachine());
        setRubberBandPen(QPen(QColor(180, 180, 180, 160), 1, Qt::DashLine));
    }

protected:
    QwtText trackerTextF(const QPointF &pos) const override
    {
        // Find nearest point in the relevant dataset
        const QVector<QPointF> *primary = m_isZ ? m_dataZ : m_ch2;
        if (!primary || primary->isEmpty())
            return QwtText();

        // Snap to nearest frequency
        int nearest = 0;
        double minDist = std::abs((*primary)[0].x() - pos.x());
        for (int i = 1; i < primary->size(); ++i) {
            double d = std::abs((*primary)[i].x() - pos.x());
            if (d < minDist) { minDist = d; nearest = i; }
        }

        const QPointF &pt = (*primary)[nearest];
        QString text;

        if (m_isZ) {
            // Impedance plot: show freq + impedance
            text = QString("<b>Freq:</b> %1 kHz<br>"
                           "<b>Impedance:</b> %2 V/I")
                       .arg(pt.x(), 0, 'f', 3)
                       .arg(pt.y(), 0, 'f', 4);
        } else {
            // Main plot: show freq + CH2 + CH3
            double ch3Val = 0.0;
            if (m_ch3 && nearest < m_ch3->size())
                ch3Val = (*m_ch3)[nearest].y();

            text = QString("<b>Freq:</b> %1 kHz<br>"
                           "<b>CH2:</b> %2 counts<br>"
                           "<b>CH3:</b> %3 counts")
                       .arg(pt.x(), 0, 'f', 3)
                       .arg(pt.y(), 0, 'f', 1)
                       .arg(ch3Val, 0, 'f', 1);
        }

        QwtText label(text, QwtText::RichText);
        label.setBackgroundBrush(QBrush(QColor(30, 30, 30, 210)));
        label.setBorderPen(QPen(QColor(100, 180, 255), 1));
        label.setBorderRadius(4);
        QFont f; f.setPointSize(9);
        label.setFont(f);
        label.setColor(Qt::white);
        return label;
    }

private:
    const QVector<QPointF> *m_ch2   = nullptr;
    const QVector<QPointF> *m_ch3   = nullptr;
    const QVector<QPointF> *m_dataZ = nullptr;
    bool m_isZ = false;
};

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setWindowTitle("Frequency Tuning");

    ui->lineEditMinFreq->setText(QString::number(m_minFreq));
    ui->lineEditMaxFreq->setText(QString::number(m_maxFreq));

    // ── Load persisted HP.NO ──────────────────────────────────────────────
    QSettings settings("AdcTuning", "AdcTuning");
    m_hpNumber = settings.value("hpNumber", "").toString();
    ui->lineEditHPNo->setText(m_hpNumber);

    // ── Populate ADC channel combo ────────────────────────────────────────
    ui->comboADCChannel->addItem("FS (0x97)");
    ui->comboADCChannel->addItem("Phaco (0xA7)");
    ui->comboADCChannel->addItem("Sensor (0xD7)");
    ui->comboADCChannel->setCurrentIndex(1);   // default: Phaco (0xA7)

    // ── ADC reader ────────────────────────────────────────────────────────
    m_adc = new AdcReader(this);
    connect(m_adc, &AdcReader::sampleReady,   this, &MainWindow::onSampleReady);
    connect(m_adc, &AdcReader::sweepFinished, this, &MainWindow::onSweepFinished);
    connect(m_adc, &AdcReader::errorOccurred, this, &MainWindow::onError);

    // ── Button / line-edit connections ────────────────────────────────────
    connect(ui->pushButtonSweep, &QPushButton::clicked,       this, &MainWindow::onStartSweep);
    connect(ui->pushButtonClear, &QPushButton::clicked,       this, &MainWindow::onClearPlot);
    connect(ui->butDAC,          &QPushButton::clicked,       this, &MainWindow::on_butDAC_clicked);
    connect(ui->pushButtonSetDac,&QPushButton::clicked,       this, &MainWindow::onSetDacManual);
//    connect(ui->lineEditMinFreq, &QLineEdit::editingFinished, this, &MainWindow::onMinFreqChanged);
//    connect(ui->lineEditMaxFreq, &QLineEdit::editingFinished, this, &MainWindow::onMaxFreqChanged);
    ui->lineEditMinFreq->installEventFilter(this);
    ui->lineEditMaxFreq->installEventFilter(this);
    ui->lineEditHPNo->installEventFilter(this);   // ← opens numeric keypad on tap

    // ── Tab widget connection (if it exists in UI) ─────────────────────────
    if (ui->tabWidget) {
        connect(ui->tabWidget, &QTabWidget::currentChanged, this, &MainWindow::onTabChanged);
    }

    setupPlot();
    setupImpedancePlot();

    // ── Hardware handler — owns the LTC2614 SPI DAC ───────────────────────
    m_hw = new hwhandler(this);

    // Set DAC to idle level at program start (val=100 → 5 V via convert_dac)
    m_dacPower = DAC_POWER_IDLE;
    m_hw->convert_dac(0x30, m_dacPower);
    ui->spinBoxDacPower->setValue(m_dacPower);   // seed spinbox to match

    // ── Wire AdcReader DAC callback to hwhandler::convert_dac ────────────
    // If user has manually overridden DAC, the sweep does NOT change it.
    m_adc->onSetDacPower = [this](int power) {
        if (m_dacManualOverride) return;   // user holds control — ignore sweep changes
        m_dacPower = power;
        m_hw->convert_dac(0x30, power);
        updateDacDisplay();
    };

    if (!m_adc->openHardware()) {
        ui->labelStatus->setText("Hardware open failed — check SPI/mem");
        ui->pushButtonSweep->setEnabled(false);
    } else {
        ui->labelStatus->setText("Hardware ready");
    }

    // ── Show initial DAC voltage (2.5 V idle) ─────────────────────────────
    updateDacDisplay();

    // ── Set impedance peak label if it exists ──────────────────────────────
    if (ui->labelPeakZFreq) {
        ui->labelPeakZFreq->setText("Peak Impedance: --");
    }
}

MainWindow::~MainWindow()
{
    m_adc->closeHardware();
    delete ui;
}

// ─────────────────────────────────────────────────────────────────────────────
uint8_t MainWindow::selectedChannelCmd() const
{
    switch (ui->comboADCChannel->currentIndex()) {
        case 0:  return ADS7841_CH0_FS;       // 0x97
        case 1:  return ADS7841_CH2_PHACO;    // 0xA7
        case 2:  return ADS7841_CH1_SENSOR;   // 0xD7
        default: return ADS7841_CH2_PHACO;
    }
}

// ─────────────────────────────────────────────────────────────────────────────
void MainWindow::setupPlot()
{
    m_plot = new QwtPlot(this);
    m_plot->setTitle("Frequency vs Feedback");
    m_plot->setAxisTitle(QwtPlot::xBottom, "Frequency (kHz)");
    m_plot->setAxisTitle(QwtPlot::yLeft,   "ADC counts (0-4096)");
    m_plot->setAxisScale(QwtPlot::xBottom, m_minFreq, m_maxFreq);
    m_plot->setAxisScale(QwtPlot::yLeft,   0.0, 4095.0);

    if (auto *c = qobject_cast<QwtPlotCanvas*>(m_plot->canvas()))
        c->setStyleSheet("background: white;");

    m_grid = new QwtPlotGrid();
    m_grid->setPen(QPen(Qt::lightGray, 0.5, Qt::DotLine));
    m_grid->attach(m_plot);

    m_legend = new QwtLegend();
    m_plot->insertLegend(m_legend, QwtPlot::BottomLegend);

    m_curveCh2 = new QwtPlotCurve("CH2 Phaco (0xA7)");
    m_curveCh2->setPen(QPen(Qt::blue, 1.5));
    m_curveCh2->setYAxis(QwtPlot::yLeft);
    m_curveCh2->setRenderHint(QwtPlotItem::RenderAntialiased, true);
    m_curveCh2->attach(m_plot);

    m_curveCh3 = new QwtPlotCurve("CH3 Voltage (0xE7)");
    m_curveCh3->setPen(QPen(Qt::red, 1.5));
    m_curveCh3->setYAxis(QwtPlot::yLeft);
    m_curveCh3->setRenderHint(QwtPlotItem::RenderAntialiased, true);
    m_curveCh3->attach(m_plot);

    m_peakMarker = new QwtPlotMarker();
    m_peakMarker->setLineStyle(QwtPlotMarker::VLine);
    m_peakMarker->setLinePen(QPen(Qt::darkGreen, 1.5, Qt::DashLine));
    QwtText peakLabel("Peak");
    peakLabel.setColor(Qt::darkGreen);
    m_peakMarker->setLabel(peakLabel);
    m_peakMarker->attach(m_plot);

    ui->plotFrame->layout()->addWidget(m_plot);

    // Crosshair tracker for main plot
    m_pickerMain = new SweepTracker(
        m_plot->canvas(),
        &m_dataCh2, &m_dataCh3, &m_dataZ,
        false   // not impedance plot
    );

}

// ─────────────────────────────────────────────────────────────────────────────
void MainWindow::setupImpedancePlot()
{
    m_plotZ = new QwtPlot(this);
    m_plotZ->setTitle("Impedance (V/I) vs Frequency");
    m_plotZ->setAxisTitle(QwtPlot::xBottom, "Frequency (kHz)");
    m_plotZ->setAxisTitle(QwtPlot::yLeft,   "Impedance (V/I Ratio)");
    m_plotZ->setAxisScale(QwtPlot::xBottom, m_minFreq, m_maxFreq);
    m_plotZ->setAxisScale(QwtPlot::yLeft,   0.0, 10.0);

    if (auto *c = qobject_cast<QwtPlotCanvas*>(m_plotZ->canvas()))
        c->setStyleSheet("background: white;");

    m_gridZ = new QwtPlotGrid();
    m_gridZ->setPen(QPen(Qt::lightGray, 0.5, Qt::DotLine));
    m_gridZ->attach(m_plotZ);

    m_legendZ = new QwtLegend();
    m_plotZ->insertLegend(m_legendZ, QwtPlot::BottomLegend);

    m_curveZ = new QwtPlotCurve("Impedance (V/I)");
    m_curveZ->setPen(QPen(Qt::magenta, 2.0));
    m_curveZ->setYAxis(QwtPlot::yLeft);
    m_curveZ->setRenderHint(QwtPlotItem::RenderAntialiased, true);
    m_curveZ->attach(m_plotZ);

    m_peakMarkerZ = new QwtPlotMarker();
    m_peakMarkerZ->setLineStyle(QwtPlotMarker::VLine);
    m_peakMarkerZ->setLinePen(QPen(Qt::darkYellow, 1.5, Qt::DashLine));
    QwtText peakLabelZ("Peak Z");
    peakLabelZ.setColor(Qt::darkYellow);
    m_peakMarkerZ->setLabel(peakLabelZ);
    m_peakMarkerZ->attach(m_plotZ);

    // Add the plot to the impedance frame if it exists, otherwise create a new frame
    if (ui->impedancePlotFrame) {
        ui->impedancePlotFrame->layout()->addWidget(m_plotZ);
    } else {
        // If the UI doesn't have impedancePlotFrame yet, we'll create a new window or tab later
        // For now, just show a message
        qDebug() << "Impedance plot frame not found in UI - impedance plot will not be visible";
    }

    // Crosshair tracker for impedance plot
    m_pickerZ = new SweepTracker(
        m_plotZ->canvas(),
        &m_dataCh2, &m_dataCh3, &m_dataZ,
        true    // impedance plot
    );
}

// ─────────────────────────────────────────────────────────────────────────────
void MainWindow::onStartSweep()
{
    if (m_running) return;

    onMinFreqChanged();
    onMaxFreqChanged();

    m_dataCh2.clear();
    m_dataCh3.clear();
    m_dataZ.clear();
    m_peakFreq = 0.0;
    m_peakVal  = 0.0;
    m_peakZFreq = 0.0;
    m_peakZVal  = 0.0;

    QString chLabel = QString("CH2 %1").arg(ui->comboADCChannel->currentText());
    m_curveCh2->setTitle(chLabel);

    m_curveCh2->setSamples(m_dataCh2);
    m_curveCh3->setSamples(m_dataCh3);
    m_curveZ->setSamples(m_dataZ);

    m_plot->setAxisScale(QwtPlot::xBottom, m_minFreq, m_maxFreq);
    if (m_plotZ) {
        m_plotZ->setAxisScale(QwtPlot::xBottom, m_minFreq, m_maxFreq);
        m_plotZ->replot();
    }
    m_plot->replot();

    m_running = true;
    ui->pushButtonSweep->setEnabled(false);
    ui->labelStatus->setText("Sweeping...");

    // DAC will be raised to 5 V inside runSweep(); reflect that immediately
    // (the actual write happens in AdcReader, we update label after the call)
    int countHigh = static_cast<int>(100000.0 / m_minFreq);
    int countLow  = static_cast<int>(100000.0 / m_maxFreq);
    m_adc->runSweep(countHigh, countLow, SAMPLES_PER_STEP, selectedChannelCmd());
}

// ─────────────────────────────────────────────────────────────────────────────
void MainWindow::calculateImpedance()
{
    m_dataZ.clear();
    m_peakZFreq = 0.0;
    m_peakZVal = 0.0;

    int n = qMin(m_dataCh2.size(), m_dataCh3.size());
    for (int i = 0; i < n; ++i) {
        double freq = m_dataCh2[i].x();
        double current = m_dataCh2[i].y();
        double voltage = m_dataCh3[i].y();

        double impedance = 0.0;
        if (current > 0.01) {
            impedance = voltage / current;
        } else {
            impedance = 0.0;
        }

        m_dataZ.append(QPointF(freq, impedance));

        if (impedance > m_peakZVal) {
            m_peakZVal = impedance;
            m_peakZFreq = freq;
        }
    }

    if (m_curveZ) {
        m_curveZ->setSamples(m_dataZ);
        updatePeakMarkerZ();

        // Auto-scale Y-axis for impedance plot
        if (m_dataZ.size() > 0 && m_plotZ) {
            double maxZ = 0.0;
            for (const auto& point : m_dataZ) {
                if (point.y() > maxZ) maxZ = point.y();
            }
            if (maxZ > 0) {
                m_plotZ->setAxisScale(QwtPlot::yLeft, 0.0, maxZ * 1.1);
            }
        }
    }
}

// ─────────────────────────────────────────────────────────────────────────────
void MainWindow::onSampleReady(AdcSample sample)
{
    double ch2  = sample.ch2_counts;
    double ch3  = sample.ch3_counts;
    double freq = sample.freqKHz;

    m_dataCh2.append(QPointF(freq, ch2));
    m_dataCh3.append(QPointF(freq, ch3));

    if (ch2 > m_peakVal) { m_peakVal = ch2; m_peakFreq = freq; }

    m_curveCh2->setSamples(m_dataCh2);
    m_curveCh3->setSamples(m_dataCh3);

    calculateImpedance();

    updatePeakMarker();
    m_plot->replot();
    if (m_plotZ) m_plotZ->replot();

    ui->labelStatus->setText(
        QString("Sweeping... %1 kHz | CH2=%2  CH3=%3 | Z=%4")
            .arg(freq, 0, 'f', 2)
            .arg(ch2,  0, 'f', 0)
            .arg(ch3,  0, 'f', 0)
            .arg(m_dataZ.size() > 0 ? m_dataZ.last().y() : 0, 0, 'f', 3));
}

// ─────────────────────────────────────────────────────────────────────────────
void MainWindow::onSweepFinished()
{
    m_running = false;
    ui->pushButtonSweep->setEnabled(true);

    ui->labelStatus->setText(
        QString("Sweep done. Peak CH2: %1 counts @ %2 kHz | Peak Z: %3 @ %4 kHz")
            .arg(m_peakVal,  0, 'f', 0)
            .arg(m_peakFreq, 0, 'f', 2)
            .arg(m_peakZVal, 0, 'f', 3)
            .arg(m_peakZFreq, 0, 'f', 2));

    ui->labelPeakFreq->setText(
        QString("Peak Current: %1 kHz (%2 counts)")
            .arg(m_peakFreq, 0, 'f', 2)
            .arg(m_peakVal, 0, 'f', 0));

    if (ui->labelPeakZFreq) {
        ui->labelPeakZFreq->setText(
            QString("Peak Impedance: %1 kHz (Z = %2)")
                .arg(m_peakZFreq, 0, 'f', 2)
                .arg(m_peakZVal, 0, 'f', 3));
    }

    saveToFile();
}

// ─────────────────────────────────────────────────────────────────────────────
void MainWindow::on_butDAC_clicked()
{
    ui->labelStatus->setText("DAC Values button pressed");
}

// ─────────────────────────────────────────────────────────────────────────────
// onSetDacManual: called when user presses "Set DAC" button.
// Reads the spinbox value (0–100 %), writes it immediately to the LTC2614,
// and sets m_dacManualOverride so the sweep loop won't overwrite it.
// ─────────────────────────────────────────────────────────────────────────────
void MainWindow::onSetDacManual()
{
    int power = ui->spinBoxDacPower->value();   // 0–100 %
    m_dacManualOverride = true;                 // lock out sweep-driven changes
    m_dacPower = power;
    m_hw->convert_dac(0x30, power);
    updateDacDisplay();

    // Compute display voltage using convert_dac formula
    uint16_t dacVal = static_cast<uint16_t>(39321 + power * 249.03);
    if (dacVal > 64224) dacVal = 64224;
    double volts = static_cast<double>(dacVal) / 64224.0 * 5.0;

    ui->labelStatus->setText(
        QString("DAC manually set to %1%  (%2 V)  — sweep will NOT override this")
            .arg(power)
            .arg(volts, 0, 'f', 2));
}

// ─────────────────────────────────────────────────────────────────────────────
void MainWindow::onClearPlot()
{
    m_dataCh2.clear();
    m_dataCh3.clear();
    m_dataZ.clear();
    m_curveCh2->setSamples(m_dataCh2);
    m_curveCh3->setSamples(m_dataCh3);
    if (m_curveZ) m_curveZ->setSamples(m_dataZ);
    m_peakMarker->setXValue(0);
    if (m_peakMarkerZ) m_peakMarkerZ->setXValue(0);
    m_plot->replot();
    if (m_plotZ) m_plotZ->replot();
    ui->labelStatus->setText("Cleared");
    ui->labelPeakFreq->setText("Peak Current: --");
    if (ui->labelPeakZFreq) ui->labelPeakZFreq->setText("Peak Impedance: --");
}

// ─────────────────────────────────────────────────────────────────────────────
void MainWindow::onError(QString msg)
{
    m_running = false;
    ui->pushButtonSweep->setEnabled(true);
    ui->labelStatus->setText("Error: " + msg);
    QMessageBox::critical(this, "Hardware Error", msg);
}

// HP.NO persistence is handled directly inside eventFilter (alphanumeric keypad).
void MainWindow::onHPNoChanged() {}

// ─────────────────────────────────────────────────────────────────────────────
bool MainWindow::eventFilter(QObject *obj, QEvent *event)
{
    if (event->type() == QEvent::MouseButtonPress) {
        if (obj == ui->lineEditMinFreq) {
            openKeypadFor(ui->lineEditMinFreq, m_minFreq, "Min Frequency (kHz)");
            return true;   // consume the event — don't let QLineEdit get focus
        }
        if (obj == ui->lineEditMaxFreq) {
            openKeypadFor(ui->lineEditMaxFreq, m_maxFreq, "Max Frequency (kHz)");
            return true;
        }
        if (obj == ui->lineEditHPNo) {
            // Open alphanumeric keypad for HP number entry
            KeypadDialog dlg("Enter HP Number", m_hpNumber,
                             KeypadDialog::Alphanumeric, this);
            if (dlg.exec() == QDialog::Accepted) {
                QString val = dlg.value().trimmed();
                if (val != m_hpNumber) {
                    m_hpNumber = val;
                    ui->lineEditHPNo->setText(m_hpNumber);
                    QSettings settings("AdcTuning", "AdcTuning");
                    settings.setValue("hpNumber", m_hpNumber);
                }
            }
            return true;
        }
    }
    return QMainWindow::eventFilter(obj, event);
}

void MainWindow::openKeypadFor(QLineEdit *lineEdit, double &targetVar, const QString &label)
{
    KeypadDialog dlg(label, lineEdit->text(), KeypadDialog::Numeric, this);
    if (dlg.exec() == QDialog::Accepted) {
        bool ok;
        double v = dlg.value().toDouble(&ok);
        if (ok && v > 0) {
            targetVar = v;
            lineEdit->setText(QString::number(v));

            // ── UPDATE BOTH PLOT AXES IMMEDIATELY ─────────────────────────
                        if (m_plot) {
                            m_plot->setAxisScale(QwtPlot::xBottom, m_minFreq, m_maxFreq);
                            m_plot->replot();
                        }
                        if (m_plotZ) {
                            m_plotZ->setAxisScale(QwtPlot::xBottom, m_minFreq, m_maxFreq);
                            m_plotZ->replot();
                        }
        }
    }
}

// Keep these as no-ops or remove them — they're no longer called:
void MainWindow::onMinFreqChanged() {}
void MainWindow::onMaxFreqChanged() {}

// ─────────────────────────────────────────────────────────────────────────────
void MainWindow::onTabChanged(int index)
{
    if (index == 0 && m_plot) {
        m_plot->replot();
    } else if (index == 1 && m_plotZ) {
        m_plotZ->replot();
    }
}

// ─────────────────────────────────────────────────────────────────────────────
void MainWindow::updatePeakMarker()
{
    if (m_peakFreq > 0) m_peakMarker->setXValue(m_peakFreq);
}

void MainWindow::updatePeakMarkerZ()
{
    if (m_peakZFreq > 0 && m_peakMarkerZ) m_peakMarkerZ->setXValue(m_peakZFreq);
}

// ─────────────────────────────────────────────────────────────────────────────
// updateDacDisplay: converts the current DAC count to volts and refreshes the
// labelDacValue label.  Called from constructor, onSampleReady, onSweepFinished.
// Does NOT touch any hardware — purely a UI update.
// ─────────────────────────────────────────────────────────────────────────────
void MainWindow::updateDacDisplay()
{
    // convert_dac(0x30, power):
    //   dac_value = 39321 + power * 249.03
    //   voltage   = dac_value / 64224 * 5.0
    int      power  = m_dacPower;
    uint16_t dacVal = static_cast<uint16_t>(39321 + power * 249.03);
    if (dacVal > 64224) dacVal = 64224;
    double   volts  = static_cast<double>(dacVal) / 64224.0 * 5.0;

    bool    active = (power >= DAC_POWER_ACTIVE);
    QString color  = active ? "#cc0000" : "#005580";
    QString bg     = active ? "#fff0f0" : "#eef6ff";
    QString text   = QString("%1 V  (%2%)")
                         .arg(volts, 0, 'f', 2)
                         .arg(power);

    ui->labelDacValue->setText(text);
    ui->labelDacValue->setStyleSheet(
        QString("QLabel { font-weight: bold; font-size: 11pt; color: %1; "
                "border: 1px solid #aaa; border-radius: 4px; "
                "padding: 2px 4px; background: %2; }")
            .arg(color).arg(bg));
}

// ─────────────────────────────────────────────────────────────────────────────
void MainWindow::saveToFile()
{
    // ── Ensure /home/tune_alone/ directory exists ─────────────────────────
    QDir dir("/home/tune_alone");
    if (!dir.exists()) {
        if (!dir.mkpath(".")) {
            ui->labelStatus->setText("Save failed: cannot create /home/tune_alone/");
            return;
        }
    }

    ++m_sweepCounter;

    QString timestamp = QDateTime::currentDateTime().toString("yyyy-MM-dd_HH-mm-ss");

    // Filename: tune_alone/<HPNO>_sweep<N>_<timestamp>.txt
    QString fname;
    if (!m_hpNumber.isEmpty())
        fname = QString("%1_sweep%2_%3.txt").arg(m_hpNumber).arg(m_sweepCounter).arg(timestamp);
    else
        fname = QString("sweep%1_%2.txt").arg(m_sweepCounter).arg(timestamp);

    QString path = "/home/tune_alone/" + fname;

    QFile file(path);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        ui->labelStatus->setText("Save failed: " + path);
        return;
    }

    QTextStream out(&file);
    out << "Frequency Sweep — " << QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss") << "\n";
    out << "HP.NO: "    << (m_hpNumber.isEmpty() ? "(not set)" : m_hpNumber) << "\n";
    out << "Sweep No: " << m_sweepCounter << "\n";
    out << "Min Freq: " << m_minFreq << " kHz   Max Freq: " << m_maxFreq << " kHz\n";

    // DAC state at time of save
    uint16_t dacVal = static_cast<uint16_t>(39321 + m_dacPower * 249.03);
    if (dacVal > 64224) dacVal = 64224;
    double dacVolts = static_cast<double>(dacVal) / 64224.0 * 5.0;
    out << QString("DAC Power: %1%  (%2 V)%3\n")
               .arg(m_dacPower)
               .arg(dacVolts, 0, 'f', 2)
               .arg(m_dacManualOverride ? "  [manual override]" : "");

    out << QString("Peak CH2: %1 counts @ %2 kHz\n").arg(m_peakVal,0,'f',0).arg(m_peakFreq,0,'f',2);
    out << QString("Peak Impedance (V/I): %1 @ %2 kHz\n").arg(m_peakZVal,0,'f',3).arg(m_peakZFreq,0,'f',2);
    out << "----------------------------------------\n";
    out << "Freq_kHz\tCH2_counts\tCH3_counts\tImpedance(V/I)\n";

    int n = qMin(m_dataCh2.size(), m_dataCh3.size());
    for (int i = 0; i < n; ++i) {
        double ch2 = m_dataCh2[i].y();
        double ch3 = m_dataCh3[i].y();
        double impedance = (ch2 > 0.01) ? (ch3 / ch2) : 0.0;
        out << QString("%1\t%2\t%3\t%4\n")
               .arg(m_dataCh2[i].x(), 0, 'f', 3)
               .arg(ch2,              0, 'f', 1)
               .arg(ch3,              0, 'f', 1)
               .arg(impedance,        0, 'f', 4);
    }
    file.close();

    ui->labelStatus->setText(
        QString("Saved %1 points → %2").arg(n).arg(path));
}
