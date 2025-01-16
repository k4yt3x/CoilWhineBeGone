#ifndef CPUCYCLEBURNER_H
#define CPUCYCLEBURNER_H

#include <atomic>
#include <condition_variable>
#include <mutex>
#include <thread>
#include <vector>

class CpuCycleBurner
{
public:
    CpuCycleBurner();
    ~CpuCycleBurner();

    void start();
    void stop();

    // New method to set CPU usage active time
    void setUtilizationPercent(int utilizationPercent);

private:
    static void signalHandler(int signum);
    void threadProc(int threadId);

    std::atomic<bool> m_isRunning;
    std::atomic<int> m_utilizationPercent;
    std::vector<std::thread> m_threads;
    std::mutex m_startStopMutex;
    std::condition_variable m_startStopCv;
};

#endif // CPUCYCLEBURNER_H
