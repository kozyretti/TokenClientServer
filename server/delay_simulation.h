#pragma once

class DelaySimulation
{
    unsigned int milliseconds_ = 0;

public:
    void init(unsigned int milliseconds) {
        milliseconds_ = milliseconds;
    }

    void delay();
};

extern DelaySimulation g_delay_simulation;
