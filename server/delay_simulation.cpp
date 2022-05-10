#include "delay_simulation.h"
#include <random>
#include <thread>
#include <chrono>
using namespace std;

void DelaySimulation::delay() {
    if(milliseconds_ != 0) {
        static mt19937 gen{ random_device{}() };
        uniform_int_distribution<> distrib(milliseconds_*0.9, milliseconds_*1.1);
        auto delay = distrib(gen);
        this_thread::sleep_for(chrono::milliseconds{delay});
    }
}

DelaySimulation g_delay_simulation;
