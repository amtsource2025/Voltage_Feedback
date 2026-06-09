#include "motor.h"

motor::motor(QObject *parent) : QObject(parent)
{
    motor_cw();
    QTimer::singleShot(1, this, &motor::motor_int);
}

void motor::motor_ccw()
{
    h.write_motor(0x01,0x02,40);
}

void motor::motor_cw()
{
    h.write_motor(0x01,0x03,40);
}

void motor::motor_stop()
{
    h.write_motor(0x00,0x00,0);
}

void motor::motor_int()
{
    motor_ccw();
}
