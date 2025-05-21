#include "pingservice.h"
#include <QHostAddress>
#include <QHostInfo>
#include <QDebug>
#include <QtConcurrent/QtConcurrentRun>

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
    m_interval(1000),
    m_timeout(1000)
{
    connect(&m_pingTimer, &QTimer::timeout, this, &PingService::doPing);

    // Connect the future watcher's finished signal
    connect(&m_futureWatcher, &QFutureWatcher<PingResult>::finished, this, [this]() {
        PingResult result = m_futureWatcher.result();

        if (result.success) {
            // Successful ping
            m_lastPingTime = result.time;
        } else {
            // For errors, log the error
            emit pingError(result.errorString);

            // Always use 0 for the chart when ping fails
            m_lastPingTime = 0.0;
        }

        // Always emit the ping time change, whether successful or not
        emit lastPingTimeChanged(m_lastPingTime);
    });
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

void PingService::setTimeout(int msec)
{
    if (m_timeout != msec) {
        m_timeout = msec;
        emit timeoutChanged(msec);
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
    // Only start a new ping if we're not already waiting for one
    if (!m_futureWatcher.isRunning()) {
        QFuture<PingResult> future = QtConcurrent::run(sendIcmpPing, m_targetHost, m_timeout);
        m_futureWatcher.setFuture(future);
    }
}

PingResult PingService::sendIcmpPing(const QString &host, int timeoutMs)
{
    PingResult result;
    result.success = false;
    result.time = 0.0; // Default to 0 for timeouts/errors

    HANDLE handle = IcmpCreateFile();
    if (handle == INVALID_HANDLE_VALUE) {
        result.errorString = "Failed to create ICMP handle";
        return result;
    }

    QHostInfo info = QHostInfo::fromName(host);
    if (info.addresses().isEmpty()) {
        IcmpCloseHandle(handle);
        result.errorString = "Host not found";
        return result;
    }

    QString ipStr = info.addresses().first().toString();

    IN_ADDR ipAddr;
    if (InetPton(AF_INET, ipStr.toStdWString().c_str(), &ipAddr) != 1) {
        IcmpCloseHandle(handle);
        result.errorString = "Invalid IP address";
        return result;
    }
    DWORD ip = ipAddr.S_un.S_addr;

    char data[] = "QtPing";
    DWORD replySize = sizeof(ICMP_ECHO_REPLY) + sizeof(data);
    void* replyBuffer = malloc(replySize);
    if (!replyBuffer) {
        IcmpCloseHandle(handle);
        result.errorString = "Memory allocation failed";
        return result;
    }

    DWORD pingResult = IcmpSendEcho(handle, ip, data, sizeof(data), nullptr,
                                    replyBuffer, replySize, timeoutMs);

    if (pingResult > 0) {
        auto* reply = reinterpret_cast<PICMP_ECHO_REPLY>(replyBuffer);
        if (reply->Status == IP_SUCCESS) {
            // Success
            result.success = true;
            result.time = static_cast<double>(reply->RoundTripTime);
            result.errorString = "";
        } else {
            // Got a reply but with error status
            switch (reply->Status) {
            case IP_DEST_NET_UNREACHABLE:   result.errorString = "Destination network unreachable"; break;
            case IP_DEST_HOST_UNREACHABLE:  result.errorString = "Destination host unreachable"; break;
            case IP_DEST_PROT_UNREACHABLE:  result.errorString = "Destination protocol unreachable"; break;
            case IP_DEST_PORT_UNREACHABLE:  result.errorString = "Destination port unreachable"; break;
            case IP_REQ_TIMED_OUT:          result.errorString = "Request timed out"; break;
            default:                        result.errorString = "Ping error: " + QString::number(reply->Status); break;
            }
        }
    } else {
        // No reply - likely timeout or other error
        DWORD errorCode = GetLastError();
        if (errorCode == IP_REQ_TIMED_OUT) {
            result.errorString = "Request timed out";
        } else {
            result.errorString = "Ping failed with error: " + QString::number(errorCode);
        }
    }

    free(replyBuffer);
    IcmpCloseHandle(handle);
    return result;
}
