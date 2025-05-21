#include "globalshortcut.h"
#include <QGuiApplication>
#include <QtDebug>
#include <windows.h>

#define HOTKEY_ID 1

GlobalShortcut::GlobalShortcut(QObject *parent)
    : QObject(parent), registered(false)
{
    QGuiApplication::instance()->installNativeEventFilter(this);
    registerShortcut();
}

GlobalShortcut::~GlobalShortcut()
{
    unregisterShortcut();
    QGuiApplication::instance()->removeNativeEventFilter(this);
}

void GlobalShortcut::registerShortcut()
{
    if (registered)
        return;

    UINT modifiers = MOD_CONTROL | MOD_ALT;
    UINT vk = 'L';

    if (RegisterHotKey(nullptr, HOTKEY_ID, modifiers, vk)) {
        registered = true;
    } else {
        qWarning() << "Failed to register global shortcut";
    }
}

void GlobalShortcut::unregisterShortcut()
{
    if (!registered)
        return;

    UnregisterHotKey(nullptr, HOTKEY_ID);
    registered = false;
}

bool GlobalShortcut::nativeEventFilter(const QByteArray &eventType, void *message, qintptr *result)
{
    Q_UNUSED(eventType);
    Q_UNUSED(result);

    MSG* msg = static_cast<MSG*>(message);
    if (msg->message == WM_HOTKEY && msg->wParam == HOTKEY_ID) {
        emit activated();
        return true;
    }
    return false;
}
