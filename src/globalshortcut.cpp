#include "globalshortcut.h"
#include <QGuiApplication>
#include <QtDebug>
#include <windows.h>
#include <QSettings>

#define HOTKEY_ID 1

GlobalShortcut::GlobalShortcut(QObject *parent)
    : QObject(parent), registered(false), shortcutKey("L")
{
    QGuiApplication::instance()->installNativeEventFilter(this);
    QSettings settings;
    shortcutKey = settings.value("UserSettings/shortcutKey", "L").toString();
    registerShortcut();
}

GlobalShortcut::~GlobalShortcut()
{
    unregisterShortcut();
    QGuiApplication::instance()->removeNativeEventFilter(this);
}

void GlobalShortcut::updateShortcut(const QString &key)
{
    if (key.isEmpty() || key == shortcutKey)
        return;

    // Unregister the old shortcut
    unregisterShortcut();

    // Update key and register the new shortcut
    shortcutKey = key;
    registerShortcut();
}

void GlobalShortcut::registerShortcut()
{
    if (registered)
        return;

    UINT modifiers = MOD_CONTROL | MOD_ALT;
    UINT vk = 0;

    // Convert the shortcut key to a virtual key code
    if (shortcutKey.length() == 1) {
        QChar ch = shortcutKey.at(0).toUpper();
        if (ch.isLetter()) {
            // For letters, we can just use the ASCII value
            vk = ch.toLatin1();
        } else if (ch.isDigit()) {
            // For digits 0-9
            vk = ch.toLatin1();
        } else {
            // Default to 'L' if we can't handle the character
            vk = 'L';
        }
    } else {
        // Default to 'L' for multi-character strings
        vk = 'L';
    }

    if (RegisterHotKey(nullptr, HOTKEY_ID, modifiers, vk)) {
        registered = true;
    } else {
        qWarning() << "Failed to register global shortcut with key:" << shortcutKey;
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
