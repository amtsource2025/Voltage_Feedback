//#ifndef KEYPADDIALOG_H
//#define KEYPADDIALOG_H

//#include <QDialog>

//namespace Ui {
//class KeypadDialog;
//}

//class KeypadDialog : public QDialog {
//    Q_OBJECT

//public:
//    explicit KeypadDialog(QWidget *parent = nullptr);
//    ~KeypadDialog();

//    QString getValue() const;

//private slots:
//    void onButtonClicked();

//private:
//    Ui::KeypadDialog *ui;
//    QString inputValue;
//};

//#endif // KEYPADDIALOG_H

#ifndef KEYPADDIALOG_H
#define KEYPADDIALOG_H

#include <QDialog>
#include <QLineEdit>
#include <QPushButton>
#include <QGridLayout>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>

class KeypadDialog : public QDialog
{
    Q_OBJECT

public:
    enum Mode { Numeric, Alphanumeric };

    explicit KeypadDialog(const QString &title,
                          const QString &currentValue,
                          Mode           mode   = Numeric,
                          QWidget       *parent = nullptr);

    QString value() const { return m_display->text(); }

private slots:
    void onKeyPressed();
    void onBackspace();
    void onClear();
    void onOK();
    void onCancel();

private:
    void buildNumericPad  (QGridLayout *grid);
    void buildAlphaPad    (QGridLayout *grid);
    QPushButton *makeBtn  (const QString &label, int minH = 60);

    QLineEdit *m_display = nullptr;
};

#endif // KEYPADDIALOG_H
