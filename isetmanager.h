#ifndef ISETMANAGER_H
#define ISETMANAGER_H

#include <cstdint>

class Interval;

class ISetManager
{
public:
    virtual Interval* currentInterval() = 0;
    virtual Interval* nextInterval() = 0;
    virtual bool currentIteration(uint32_t &current, uint32_t &total) = 0;
};

#endif // ISETMANAGER_H
