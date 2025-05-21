#include "lagometer.h"
#include "pingservice.h"
#include <QApplication>
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
    trayMenu = new QMenu(this);
    quitAction = new QAction("Quit", this);
    settingsAction = new QAction("Settings", this);
    toggleAction = new QAction("Show", this);

    connect(quitAction, &QAction::triggered, this, &QApplication::quit);
    connect(settingsAction, &QAction::triggered, this, &Lagometer::showSettingsPage);
    connect(toggleAction, &QAction::triggered, this, &Lagometer::toggleWindow);

    trayMenu->addAction(toggleAction);
    trayMenu->addAction(settingsAction);
    trayMenu->addSeparator();
    trayMenu->addAction(quitAction);
    trayIcon->setContextMenu(trayMenu);
    trayIcon->show();
}

void Lagometer::showSettingsPage() {

}

void Lagometer::toggleWindow() {

}
