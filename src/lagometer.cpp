#include "lagometer.h"
#include "pingservice.h"
#include <QApplication>
#include <QQmlContext>
#include <QIcon>
#include <QFile>
#include <QStandardPaths>
#include <QDir>
#include <QScreen>

Lagometer::Lagometer(QWidget *parent)
    : QWidget(parent)
    , flyoutEngine(new QQmlApplicationEngine(this))
    , shortcut(new GlobalShortcut(this))
    , trayIcon(new QSystemTrayIcon(this))
{
    connect(this, &Lagometer::windowVisibleChanged, this, &Lagometer::updateToggleActionText);
    configureTrayIcon();

    flyoutEngine->rootContext()->setContextProperty("globalShortcut", shortcut);
    flyoutEngine->rootContext()->setContextProperty("lagometer", this);
    qmlRegisterSingletonInstance<PingService>("Odizinne.Ping", 1, 0, "PingService", PingService::getInstance());
    flyoutEngine->loadFromModule("Odizinne.Lagometer", "Main");

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
    emit requestSettingsWindow();
}

void Lagometer::toggleWindow() {
    if (m_windowVisible) {
        emit requestHideWindow();
    } else {
        emit requestShowWindow();
    }
}

void Lagometer::updateToggleActionText() {
    toggleAction->setText(m_windowVisible ? "Hide" : "Show");
}

bool Lagometer::isShortcutPresent()
{
    QString shortcutName = "Lagometer.lnk";
    QString startupPath = QStandardPaths::writableLocation(QStandardPaths::ApplicationsLocation) + QDir::separator() + "Startup";
    QString shortcutPath = startupPath + QDir::separator() + shortcutName;
    return QFile::exists(shortcutPath);
}

void Lagometer::manageShortcut(bool state)
{
    QString shortcutName = "Lagometer.lnk";
    QString applicationPath = QCoreApplication::applicationFilePath();
    QString startupPath = QStandardPaths::writableLocation(QStandardPaths::ApplicationsLocation) + QDir::separator() + "Startup";
    QString shortcutPath = startupPath + QDir::separator() + shortcutName;

    if (state) {
        QFile::link(applicationPath, shortcutPath);
    } else {
        QFile::remove(shortcutPath);
    }
}

QRect Lagometer::availablePrimaryScreenGeometry() const {
    QScreen* screen = QGuiApplication::primaryScreen();
    return screen ? screen->availableGeometry() : QRect();
}
