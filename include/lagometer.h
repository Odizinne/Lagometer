#ifndef LAGOMETER_H
#define LAGOMETER_H

#include <QWidget>
#include <QQmlApplicationEngine>
#include <QSystemTrayIcon>
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
    void configureTrayIcon();
};

#endif // LAGOMETER_H
