#include <QApplication>
#include "lagometer.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    app.setOrganizationName("Odizinne");
    app.setApplicationName("Lagometer");
    QGuiApplication::setWindowIcon(QIcon(":/icons/icon.png"));
    Lagometer l;
    return app.exec();
}
