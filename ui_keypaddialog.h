/********************************************************************************
** Form generated from reading UI file 'keypaddialog.ui'
**
** Created by: Qt User Interface Compiler version 5.15.4
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_KEYPADDIALOG_H
#define UI_KEYPADDIALOG_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QDialog>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>

QT_BEGIN_NAMESPACE

class Ui_KeypadDialog
{
public:
    QPushButton *pushButton;
    QPushButton *pushButton_2;
    QPushButton *pushButton_3;
    QPushButton *pushButton_4;
    QPushButton *pushButton_5;
    QPushButton *pushButton_6;
    QPushButton *pushButton_7;
    QPushButton *pushButton_8;
    QPushButton *pushButton_9;
    QPushButton *pushButton_10;
    QPushButton *pushButton_11;
    QPushButton *pushButton_12;
    QLineEdit *display;

    void setupUi(QDialog *KeypadDialog)
    {
        if (KeypadDialog->objectName().isEmpty())
            KeypadDialog->setObjectName(QString::fromUtf8("KeypadDialog"));
        KeypadDialog->resize(209, 204);
        pushButton = new QPushButton(KeypadDialog);
        pushButton->setObjectName(QString::fromUtf8("pushButton"));
        pushButton->setGeometry(QRect(20, 50, 31, 25));
        pushButton_2 = new QPushButton(KeypadDialog);
        pushButton_2->setObjectName(QString::fromUtf8("pushButton_2"));
        pushButton_2->setGeometry(QRect(80, 50, 31, 25));
        pushButton_3 = new QPushButton(KeypadDialog);
        pushButton_3->setObjectName(QString::fromUtf8("pushButton_3"));
        pushButton_3->setGeometry(QRect(140, 50, 31, 25));
        pushButton_4 = new QPushButton(KeypadDialog);
        pushButton_4->setObjectName(QString::fromUtf8("pushButton_4"));
        pushButton_4->setGeometry(QRect(140, 90, 31, 25));
        pushButton_5 = new QPushButton(KeypadDialog);
        pushButton_5->setObjectName(QString::fromUtf8("pushButton_5"));
        pushButton_5->setGeometry(QRect(140, 130, 31, 25));
        pushButton_6 = new QPushButton(KeypadDialog);
        pushButton_6->setObjectName(QString::fromUtf8("pushButton_6"));
        pushButton_6->setGeometry(QRect(80, 130, 31, 25));
        pushButton_7 = new QPushButton(KeypadDialog);
        pushButton_7->setObjectName(QString::fromUtf8("pushButton_7"));
        pushButton_7->setGeometry(QRect(20, 130, 31, 25));
        pushButton_8 = new QPushButton(KeypadDialog);
        pushButton_8->setObjectName(QString::fromUtf8("pushButton_8"));
        pushButton_8->setGeometry(QRect(80, 90, 31, 25));
        pushButton_9 = new QPushButton(KeypadDialog);
        pushButton_9->setObjectName(QString::fromUtf8("pushButton_9"));
        pushButton_9->setGeometry(QRect(10, 170, 61, 25));
        pushButton_10 = new QPushButton(KeypadDialog);
        pushButton_10->setObjectName(QString::fromUtf8("pushButton_10"));
        pushButton_10->setGeometry(QRect(20, 90, 31, 25));
        pushButton_11 = new QPushButton(KeypadDialog);
        pushButton_11->setObjectName(QString::fromUtf8("pushButton_11"));
        pushButton_11->setGeometry(QRect(80, 170, 31, 25));
        pushButton_12 = new QPushButton(KeypadDialog);
        pushButton_12->setObjectName(QString::fromUtf8("pushButton_12"));
        pushButton_12->setGeometry(QRect(120, 170, 61, 25));
        display = new QLineEdit(KeypadDialog);
        display->setObjectName(QString::fromUtf8("display"));
        display->setGeometry(QRect(70, 10, 51, 25));

        retranslateUi(KeypadDialog);

        QMetaObject::connectSlotsByName(KeypadDialog);
    } // setupUi

    void retranslateUi(QDialog *KeypadDialog)
    {
        KeypadDialog->setWindowTitle(QCoreApplication::translate("KeypadDialog", "Dialog", nullptr));
        pushButton->setText(QCoreApplication::translate("KeypadDialog", "1", nullptr));
        pushButton_2->setText(QCoreApplication::translate("KeypadDialog", "2", nullptr));
        pushButton_3->setText(QCoreApplication::translate("KeypadDialog", "3", nullptr));
        pushButton_4->setText(QCoreApplication::translate("KeypadDialog", "6", nullptr));
        pushButton_5->setText(QCoreApplication::translate("KeypadDialog", "9", nullptr));
        pushButton_6->setText(QCoreApplication::translate("KeypadDialog", "8", nullptr));
        pushButton_7->setText(QCoreApplication::translate("KeypadDialog", "7", nullptr));
        pushButton_8->setText(QCoreApplication::translate("KeypadDialog", "5", nullptr));
        pushButton_9->setText(QCoreApplication::translate("KeypadDialog", "Enter", nullptr));
        pushButton_10->setText(QCoreApplication::translate("KeypadDialog", "4", nullptr));
        pushButton_11->setText(QCoreApplication::translate("KeypadDialog", "0", nullptr));
        pushButton_12->setText(QCoreApplication::translate("KeypadDialog", "Clear", nullptr));
    } // retranslateUi

};

namespace Ui {
    class KeypadDialog: public Ui_KeypadDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_KEYPADDIALOG_H
