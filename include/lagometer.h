#ifndef LAGOMETER_H
#define LAGOMETER_H

#include <QWidget>
#include <QQmlApplicationEngine>
#include <QSystemTrayIcon>
#include <QMenu>
#include <QAction>
#include "globalshortcut.h"

QT_BEGIN_NAMESPACE

class Lagometer : public QWidget
{
    Q_OBJECT

public:
    Lagometer(QWidget *parent = nullptr);
    ~Lagometer();

private:
    QQmlApplicationEngine *flyoutEngine;
    GlobalShortcut *shortcut;
    QSystemTrayIcon *trayIcon;
    QMenu *trayMenu;
    QAction *quitAction;
    QAction *toggleAction;
    QAction *settingsAction;

    void configureTrayIcon();
    void toggleWindow();
    void showSettingsPage();
};

#endif // LAGOMETER_H
