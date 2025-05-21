#ifndef LAGOMETER_H
#define LAGOMETER_H

#include <QWidget>
#include <QQmlApplicationEngine>
#include <QSystemTrayIcon>
#include <QMenu>
#include <QAction>
#include "globalshortcut.h"

class Lagometer : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(bool windowVisible READ windowVisible WRITE setWindowVisible NOTIFY windowVisibleChanged)

public:
    Lagometer(QWidget *parent = nullptr);
    ~Lagometer();

    Q_INVOKABLE void manageShortcut(bool state);
    Q_INVOKABLE bool isShortcutPresent();
    Q_INVOKABLE QRect availablePrimaryScreenGeometry() const;

    void setWindowVisible(const bool &status)
    {
        if (status != m_windowVisible) {
            m_windowVisible = status;
            emit windowVisibleChanged();
        }
    }

    bool windowVisible() const
    {
        return m_windowVisible;
    }

signals:
    void windowVisibleChanged();
    void requestShowWindow();
    void requestHideWindow();
    void requestSettingsWindow();
    void requestClearChart();

private slots:
    void updateToggleActionText();
    void trayIconActivated(QSystemTrayIcon::ActivationReason reason);
    void togglePingService();
    void updatePauseResumeText();

private:
    QQmlApplicationEngine *flyoutEngine;
    GlobalShortcut *shortcut;
    QSystemTrayIcon *trayIcon;
    QMenu *trayMenu;
    QAction *quitAction;
    QAction *toggleAction;
    QAction *settingsAction;
    QAction *pauseResumeAction;
    QAction *clearChartAction;

    void configureTrayIcon();
    void toggleWindow();
    void showSettingsPage();

    bool m_windowVisible;
};

#endif // LAGOMETER_H
