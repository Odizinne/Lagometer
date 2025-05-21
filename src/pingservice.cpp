#include "pingservice.h"
#include <QHostAddress>
#include <QHostInfo>
#include <QDebug>

PingService* PingService::m_instance = nullptr;

PingService* PingService::getInstance()
{
    if (!m_instance) {
        m_instance = new PingService();
    }
    return m_instance;
}

PingService::PingService(QObject *parent)
    : QObject(parent),
    m_lastPingTime(0.0),
    m_isRunning(false),
    m_targetHost("8.8.8.8"),
    m_interval(1000)
{
    connect(&m_pingTimer, &QTimer::timeout, this, &PingService::doPing);
}

void PingService::setTargetHost(const QString &host)
{
    if (m_targetHost != host) {
        m_targetHost = host;
        emit targetHostChanged(host);
    }
}

void PingService::setInterval(int msec)
{
    if (m_interval != msec) {
        m_interval = msec;

        if (m_isRunning) {
            m_pingTimer.setInterval(msec);
        }

        emit intervalChanged(msec);
    }
}

void PingService::startPinging()
{
    if (!m_isRunning) {
        m_isRunning = true;
        m_pingTimer.setInterval(m_interval);
        m_pingTimer.start();
        emit isRunningChanged(true);
        doPing(); // First ping immediately
    }
}

void PingService::stopPinging()
{
    if (m_isRunning) {
        m_pingTimer.stop();
        m_isRunning = false;
        emit isRunningChanged(false);
    }
}

void PingService::singlePing()
{
    doPing();
}

void PingService::doPing()
{
    double time = sendIcmpPing(m_targetHost, 1000);
    m_lastPingTime = time;

    if (time > 0.0) {
        emit lastPingTimeChanged(time);
    } else {
        emit pingError("Ping failed");
        emit lastPingTimeChanged(0.0);
    }
}

double PingService::sendIcmpPing(const QString &host, int timeoutMs)
{
    HANDLE handle = IcmpCreateFile();
    if (handle == INVALID_HANDLE_VALUE)
        return 0.0;

    QHostInfo info = QHostInfo::fromName(host);
    if (info.addresses().isEmpty()) {
        IcmpCloseHandle(handle);
        return 0.0;
    }

    QString ipStr = info.addresses().first().toString();

    IN_ADDR ipAddr;
    if (InetPton(AF_INET, ipStr.toStdWString().c_str(), &ipAddr) != 1) {
        IcmpCloseHandle(handle);
        return 0.0;
    }
    DWORD ip = ipAddr.S_un.S_addr;

    char data[] = "QtPing";
    DWORD replySize = sizeof(ICMP_ECHO_REPLY) + sizeof(data);
    void* replyBuffer = malloc(replySize);
    if (!replyBuffer) {
        IcmpCloseHandle(handle);
        return 0.0;
    }

    DWORD result = IcmpSendEcho(handle, ip, data, sizeof(data), nullptr,
                                replyBuffer, replySize, timeoutMs);

    double time = 0.0;
    if (result > 0) {
        auto* reply = reinterpret_cast<PICMP_ECHO_REPLY>(replyBuffer);
        if (reply->Status == IP_SUCCESS)
            time = static_cast<double>(reply->RoundTripTime);
    }

    free(replyBuffer);
    IcmpCloseHandle(handle);
    return time;
}
