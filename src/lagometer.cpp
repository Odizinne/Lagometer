#include "lagometer.h"
#include "pingservice.h"
#include <QQmlContext>
#include <QIcon>

Lagometer::Lagometer(QWidget *parent)
    : QWidget(parent)
    , flyoutEngine(new QQmlApplicationEngine(this))
    , shortcut(new GlobalShortcut(this))
    , trayIcon(new QSystemTrayIcon(this))
{
    flyoutEngine->rootContext()->setContextProperty("globalShortcut", shortcut);
    qmlRegisterSingletonInstance<PingService>("Odizinne.Ping", 1, 0, "PingService", PingService::getInstance());
    flyoutEngine->loadFromModule("Odizinne.Lagometer", "Main");

    configureTrayIcon();
}

Lagometer::~Lagometer()
{
}

void Lagometer::configureTrayIcon() {
    trayIcon->setIcon(QIcon(":/icons/icon.png"));
    trayIcon->show();
}
