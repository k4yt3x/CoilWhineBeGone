#include "cpucycleburner.h"

#include <process.h>
#include <signal.h>

#include <QDebug>

CpuCycleBurner::CpuCycleBurner() : m_isRunning(false), m_utilizationPercent(50) {
    signal(SIGINT, CpuCycleBurner::signalHandler);
}

CpuCycleBurner::~CpuCycleBurner() {
    stop();
}

void CpuCycleBurner::signalHandler(int signum) {
    qDebug() << "\nInterrupt signal (" << signum << ") received. Stopping threads...";
}

void CpuCycleBurner::threadProc(int threadId) {
    while (m_isRunning) {
        auto start = std::chrono::high_resolution_clock::now();

        // Calculate active and idle time based on utilization percentage
        int activeTimeMs = m_utilizationPercent.load();
        int idleTimeMs = 100 - activeTimeMs;

        // Active time: Burn CPU cycles
        while (std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - start)
                   .count() < activeTimeMs) {
            volatile int dummy = 0;
            dummy++;
        }

        // Sleep $idleTimeMs in every 100 ms
        if (idleTimeMs > 0) {
            std::this_thread::sleep_for(std::chrono::milliseconds(idleTimeMs));
        }
    }

    qDebug() << "Thread " << threadId << " stopping.";
}

void CpuCycleBurner::start() {
    std::unique_lock<std::mutex> lock(m_startStopMutex);
    if (m_isRunning) {
        qDebug() << "CpuCycleBurner is already running.";
        return;
    }

    m_isRunning = true;

    unsigned int numCores = std::thread::hardware_concurrency();
    if (numCores == 0) {
        qDebug() << "Failed to determine the number of cores. Defaulting to 1 thread.";
        numCores = 1;
    }

    qDebug() << "Starting " << numCores << " threads...";

    for (unsigned int i = 0; i < numCores; ++i) {
        m_threads.emplace_back(&CpuCycleBurner::threadProc, this, i);
    }
}

void CpuCycleBurner::stop() {
    {
        std::unique_lock<std::mutex> lock(m_startStopMutex);
        if (!m_isRunning) {
            qDebug() << "CpuCycleBurner is not running.";
            return;
        }

        m_isRunning = false;
    }

    m_startStopCv.notify_all();

    for (auto& thread : m_threads) {
        if (thread.joinable()) {
            thread.join();
        }
    }

    m_threads.clear();
}

void CpuCycleBurner::setUtilizationPercent(int utilizationPercent) {
    if (utilizationPercent < 0) {
        utilizationPercent = 0;
    } else if (utilizationPercent > 100) {
        utilizationPercent = 100;
    }
    m_utilizationPercent.store(utilizationPercent);
    qDebug() << "CPU utilization set to " << m_utilizationPercent.load() << "%.";
}

bool CpuCycleBurner::isRunning() const {
    return m_isRunning.load();
}
