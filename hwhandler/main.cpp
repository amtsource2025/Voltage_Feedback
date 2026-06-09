#include <QCoreApplication>

#include "hwhandler.h"
#include "motor.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    motor m;

    return a.exec();
}
