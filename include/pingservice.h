#ifndef PINGSERVICE_H
#define PINGSERVICE_H

#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define _WIN32_WINNT 0x0601
#define WIN32_LEAN_AND_MEAN

#include <winsock2.h>
#include <windows.h>
#include <ws2tcpip.h>
#include <iphlpapi.h>
#include <icmpapi.h>

#include <QObject>
#include <QString>
#include <QTimer>
#include <QElapsedTimer>
#include <QHostInfo>
#include <QFuture>
#include <QFutureWatcher>

// Define result structure for ping
struct PingResult {
    double time;         // Round trip time in ms (0 for timeout, -1 for other errors)
    bool success;        // Was the ping successful?
    QString errorString; // Error message if unsuccessful
};

class PingService : public QObject
{
    Q_OBJECT
    Q_PROPERTY(double lastPingTime READ lastPingTime NOTIFY lastPingTimeChanged)
    Q_PROPERTY(bool isRunning READ isRunning NOTIFY isRunningChanged)
    Q_PROPERTY(QString targetHost READ targetHost WRITE setTargetHost NOTIFY targetHostChanged)
    Q_PROPERTY(int interval READ interval WRITE setInterval NOTIFY intervalChanged)
    Q_PROPERTY(int timeout READ timeout WRITE setTimeout NOTIFY timeoutChanged)

public:
    static PingService* getInstance();

    double lastPingTime() const { return m_lastPingTime; }
    bool isRunning() const { return m_isRunning; }
    QString targetHost() const { return m_targetHost; }
    int interval() const { return m_interval; }
    int timeout() const { return m_timeout; }

    void setTargetHost(const QString &host);
    void setInterval(int msec);
    void setTimeout(int msec);

    Q_INVOKABLE void startPinging();
    Q_INVOKABLE void stopPinging();
    Q_INVOKABLE void singlePing();

signals:
    void lastPingTimeChanged(double pingTime);
    void isRunningChanged(bool running);
    void targetHostChanged(QString host);
    void intervalChanged(int interval);
    void timeoutChanged(int timeout);
    void pingError(QString errorMessage);

private:
    explicit PingService(QObject *parent = nullptr);
    PingService(const PingService&) = delete;
    PingService& operator=(const PingService&) = delete;

    static PingService* m_instance;

    QTimer m_pingTimer;
    double m_lastPingTime;
    bool m_isRunning;
    QString m_targetHost;
    int m_interval;
    int m_timeout;
    QFutureWatcher<PingResult> m_futureWatcher;

    void doPing();
    static PingResult sendIcmpPing(const QString &host, int timeoutMs);
};

#endif // PINGSERVICE_H
