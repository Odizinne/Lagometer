#include <QApplication>
#include "lagometer.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    Lagometer l;
    return app.exec();
}
