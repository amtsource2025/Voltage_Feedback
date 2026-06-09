/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 5.15.4
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QFrame>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QTabWidget>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QWidget *centralwidget;
    QTabWidget *tabWidget;
    QWidget *tabFeedback;
    QVBoxLayout *verticalLayoutFeedback;
    QFrame *plotFrame;
    QVBoxLayout *plotLayout;
    QWidget *tabImpedance;
    QVBoxLayout *verticalLayoutImpedance;
    QFrame *impedancePlotFrame;
    QVBoxLayout *impedancePlotLayout;
    QPushButton *butDAC;
    QLabel *labelDacTitle;
    QLabel *labelDacValue;
    QLabel *labelDacManual;
    QSpinBox *spinBoxDacPower;
    QPushButton *pushButtonSetDac;
    QLabel *labelHPNo;
    QLineEdit *lineEditHPNo;
    QLabel *labelADC;
    QComboBox *comboADCChannel;
    QLabel *labelMaxFreq;
    QLineEdit *lineEditMaxFreq;
    QLabel *labelMinFreq;
    QLineEdit *lineEditMinFreq;
    QPushButton *pushButtonSweep;
    QPushButton *pushButtonClear;
    QLabel *labelStatus;
    QLabel *labelPeakFreq;
    QLabel *labelPeakZFreq;
    QMenuBar *menubar;
    QStatusBar *statusbar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QString::fromUtf8("MainWindow"));
        MainWindow->resize(1280, 720);
        centralwidget = new QWidget(MainWindow);
        centralwidget->setObjectName(QString::fromUtf8("centralwidget"));
        tabWidget = new QTabWidget(centralwidget);
        tabWidget->setObjectName(QString::fromUtf8("tabWidget"));
        tabWidget->setGeometry(QRect(20, 40, 1031, 541));
        tabFeedback = new QWidget();
        tabFeedback->setObjectName(QString::fromUtf8("tabFeedback"));
        verticalLayoutFeedback = new QVBoxLayout(tabFeedback);
        verticalLayoutFeedback->setObjectName(QString::fromUtf8("verticalLayoutFeedback"));
        verticalLayoutFeedback->setContentsMargins(2, 2, 2, 2);
        plotFrame = new QFrame(tabFeedback);
        plotFrame->setObjectName(QString::fromUtf8("plotFrame"));
        plotFrame->setFrameShape(QFrame::StyledPanel);
        plotFrame->setFrameShadow(QFrame::Raised);
        plotLayout = new QVBoxLayout(plotFrame);
        plotLayout->setObjectName(QString::fromUtf8("plotLayout"));
        plotLayout->setContentsMargins(2, 2, 2, 2);

        verticalLayoutFeedback->addWidget(plotFrame);

        tabWidget->addTab(tabFeedback, QString());
        tabImpedance = new QWidget();
        tabImpedance->setObjectName(QString::fromUtf8("tabImpedance"));
        verticalLayoutImpedance = new QVBoxLayout(tabImpedance);
        verticalLayoutImpedance->setObjectName(QString::fromUtf8("verticalLayoutImpedance"));
        verticalLayoutImpedance->setContentsMargins(2, 2, 2, 2);
        impedancePlotFrame = new QFrame(tabImpedance);
        impedancePlotFrame->setObjectName(QString::fromUtf8("impedancePlotFrame"));
        impedancePlotFrame->setFrameShape(QFrame::StyledPanel);
        impedancePlotFrame->setFrameShadow(QFrame::Raised);
        impedancePlotLayout = new QVBoxLayout(impedancePlotFrame);
        impedancePlotLayout->setObjectName(QString::fromUtf8("impedancePlotLayout"));
        impedancePlotLayout->setContentsMargins(2, 2, 2, 2);

        verticalLayoutImpedance->addWidget(impedancePlotFrame);

        tabWidget->addTab(tabImpedance, QString());
        butDAC = new QPushButton(centralwidget);
        butDAC->setObjectName(QString::fromUtf8("butDAC"));
        butDAC->setGeometry(QRect(1340, 130, 91, 41));
        labelDacTitle = new QLabel(centralwidget);
        labelDacTitle->setObjectName(QString::fromUtf8("labelDacTitle"));
        labelDacTitle->setGeometry(QRect(1100, 170, 151, 18));
        labelDacValue = new QLabel(centralwidget);
        labelDacValue->setObjectName(QString::fromUtf8("labelDacValue"));
        labelDacValue->setGeometry(QRect(1100, 200, 131, 28));
        labelDacManual = new QLabel(centralwidget);
        labelDacManual->setObjectName(QString::fromUtf8("labelDacManual"));
        labelDacManual->setGeometry(QRect(1060, 60, 221, 20));
        spinBoxDacPower = new QSpinBox(centralwidget);
        spinBoxDacPower->setObjectName(QString::fromUtf8("spinBoxDacPower"));
        spinBoxDacPower->setGeometry(QRect(1070, 100, 81, 41));
        spinBoxDacPower->setMinimum(0);
        spinBoxDacPower->setMaximum(100);
        spinBoxDacPower->setValue(100);
        pushButtonSetDac = new QPushButton(centralwidget);
        pushButtonSetDac->setObjectName(QString::fromUtf8("pushButtonSetDac"));
        pushButtonSetDac->setGeometry(QRect(1170, 100, 91, 41));
        labelHPNo = new QLabel(centralwidget);
        labelHPNo->setObjectName(QString::fromUtf8("labelHPNo"));
        labelHPNo->setGeometry(QRect(1100, 260, 101, 21));
        lineEditHPNo = new QLineEdit(centralwidget);
        lineEditHPNo->setObjectName(QString::fromUtf8("lineEditHPNo"));
        lineEditHPNo->setGeometry(QRect(1100, 290, 91, 28));
        labelADC = new QLabel(centralwidget);
        labelADC->setObjectName(QString::fromUtf8("labelADC"));
        labelADC->setGeometry(QRect(1350, 230, 91, 18));
        comboADCChannel = new QComboBox(centralwidget);
        comboADCChannel->addItem(QString());
        comboADCChannel->addItem(QString());
        comboADCChannel->addItem(QString());
        comboADCChannel->setObjectName(QString::fromUtf8("comboADCChannel"));
        comboADCChannel->setGeometry(QRect(1350, 180, 91, 31));
        labelMaxFreq = new QLabel(centralwidget);
        labelMaxFreq->setObjectName(QString::fromUtf8("labelMaxFreq"));
        labelMaxFreq->setGeometry(QRect(1100, 350, 131, 20));
        lineEditMaxFreq = new QLineEdit(centralwidget);
        lineEditMaxFreq->setObjectName(QString::fromUtf8("lineEditMaxFreq"));
        lineEditMaxFreq->setGeometry(QRect(1100, 380, 81, 31));
        labelMinFreq = new QLabel(centralwidget);
        labelMinFreq->setObjectName(QString::fromUtf8("labelMinFreq"));
        labelMinFreq->setGeometry(QRect(1100, 430, 141, 20));
        lineEditMinFreq = new QLineEdit(centralwidget);
        lineEditMinFreq->setObjectName(QString::fromUtf8("lineEditMinFreq"));
        lineEditMinFreq->setGeometry(QRect(1100, 450, 91, 31));
        pushButtonSweep = new QPushButton(centralwidget);
        pushButtonSweep->setObjectName(QString::fromUtf8("pushButtonSweep"));
        pushButtonSweep->setGeometry(QRect(1100, 500, 111, 61));
        pushButtonClear = new QPushButton(centralwidget);
        pushButtonClear->setObjectName(QString::fromUtf8("pushButtonClear"));
        pushButtonClear->setGeometry(QRect(1100, 590, 111, 51));
        labelStatus = new QLabel(centralwidget);
        labelStatus->setObjectName(QString::fromUtf8("labelStatus"));
        labelStatus->setGeometry(QRect(30, 590, 631, 20));
        labelPeakFreq = new QLabel(centralwidget);
        labelPeakFreq->setObjectName(QString::fromUtf8("labelPeakFreq"));
        labelPeakFreq->setGeometry(QRect(30, 650, 621, 21));
        labelPeakZFreq = new QLabel(centralwidget);
        labelPeakZFreq->setObjectName(QString::fromUtf8("labelPeakZFreq"));
        labelPeakZFreq->setGeometry(QRect(30, 620, 631, 21));
        MainWindow->setCentralWidget(centralwidget);
        menubar = new QMenuBar(MainWindow);
        menubar->setObjectName(QString::fromUtf8("menubar"));
        menubar->setGeometry(QRect(0, 0, 1280, 22));
        MainWindow->setMenuBar(menubar);
        statusbar = new QStatusBar(MainWindow);
        statusbar->setObjectName(QString::fromUtf8("statusbar"));
        MainWindow->setStatusBar(statusbar);

        retranslateUi(MainWindow);

        tabWidget->setCurrentIndex(0);


        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QCoreApplication::translate("MainWindow", "Frequency Tuning", nullptr));
        tabWidget->setTabText(tabWidget->indexOf(tabFeedback), QCoreApplication::translate("MainWindow", "Feedback (Current & Voltage)", nullptr));
        tabWidget->setTabText(tabWidget->indexOf(tabImpedance), QCoreApplication::translate("MainWindow", "Impedance (Z = V/I)", nullptr));
        butDAC->setText(QCoreApplication::translate("MainWindow", "DAC Values", nullptr));
        labelDacTitle->setText(QCoreApplication::translate("MainWindow", "DAC Output (live):", nullptr));
        labelDacValue->setStyleSheet(QCoreApplication::translate("MainWindow", "QLabel { font-weight: bold; font-size: 11pt; color: #005580; border: 1px solid #aaa; border-radius: 4px; padding: 2px 4px; background: #eef6ff; }", nullptr));
        labelDacValue->setText(QCoreApplication::translate("MainWindow", "-- V  (---- counts)", nullptr));
        labelDacManual->setText(QCoreApplication::translate("MainWindow", "Set DAC Power (0-100%):", nullptr));
        spinBoxDacPower->setSuffix(QCoreApplication::translate("MainWindow", " %", nullptr));
        pushButtonSetDac->setStyleSheet(QCoreApplication::translate("MainWindow", "QPushButton { background-color: #005580; color: white; border-radius: 4px; font-weight: bold; } QPushButton:pressed { background-color: #003355; }", nullptr));
        pushButtonSetDac->setText(QCoreApplication::translate("MainWindow", "Set DAC", nullptr));
        labelHPNo->setText(QCoreApplication::translate("MainWindow", "HP.NO:", nullptr));
        lineEditHPNo->setPlaceholderText(QCoreApplication::translate("MainWindow", "e.g. HP054", nullptr));
        labelADC->setText(QCoreApplication::translate("MainWindow", "ADC Channel", nullptr));
        comboADCChannel->setItemText(0, QCoreApplication::translate("MainWindow", "FS (0x97)", nullptr));
        comboADCChannel->setItemText(1, QCoreApplication::translate("MainWindow", "Phaco (0xA7)", nullptr));
        comboADCChannel->setItemText(2, QCoreApplication::translate("MainWindow", "Sensor (0xD7)", nullptr));

        labelMaxFreq->setText(QCoreApplication::translate("MainWindow", "Max Freq (kHz)", nullptr));
        lineEditMaxFreq->setText(QCoreApplication::translate("MainWindow", "46.0", nullptr));
        labelMinFreq->setText(QCoreApplication::translate("MainWindow", "Min Freq (kHz)", nullptr));
        lineEditMinFreq->setText(QCoreApplication::translate("MainWindow", "38.0", nullptr));
        pushButtonSweep->setText(QCoreApplication::translate("MainWindow", "Start Measure", nullptr));
        pushButtonClear->setText(QCoreApplication::translate("MainWindow", "Clear Graph", nullptr));
        labelStatus->setText(QCoreApplication::translate("MainWindow", "Initialising...", nullptr));
        labelPeakFreq->setText(QCoreApplication::translate("MainWindow", "Peak Current: --", nullptr));
        labelPeakZFreq->setText(QCoreApplication::translate("MainWindow", "Peak Impedance: --", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
