#ifndef MOTOR_H
#define MOTOR_H

#include <QObject>
#include <QTimer>
#include "hwhandler.h"

class motor : public QObject
{
    Q_OBJECT
public:
    explicit motor(QObject *parent = nullptr);
    void motor_ccw();
    void motor_cw();
    void motor_stop();

signals:

private slots:
    void motor_int();

private:
    hwhandler h;

};

#endif // MOTOR_H


