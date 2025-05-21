#include <QApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include "pingservice.h"
#include "globalshortcut.h"

int main(int argc, char *argv[])
{
    // Use QApplication instead of QGuiApplication for QtCharts
    QApplication app(argc, argv);
    QQmlApplicationEngine engine;

    GlobalShortcut *shortcut = new GlobalShortcut(&app);
    engine.rootContext()->setContextProperty("globalShortcut", shortcut);
    qmlRegisterSingletonInstance<PingService>("Odizinne.Ping", 1, 0, "PingService", PingService::getInstance());

    engine.loadFromModule("Odizinne.Lagometer", "Main");

    return app.exec();
}
