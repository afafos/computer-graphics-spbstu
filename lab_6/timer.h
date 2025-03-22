#pragma once

#include <ctime>

class Timer {
public:
    static Timer& GetInstance() {
        static Timer timerInstance;
        return timerInstance;
    };

    void Init() { init_time = clock(); };
    double Clock() {
        return (1.0 * clock() - init_time) / CLOCKS_PER_SEC;
    };
private:
    std::clock_t init_time;
};