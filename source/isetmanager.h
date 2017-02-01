/*************************************
 * Copyright (C) 2017 Michael Pearce *
 *************************************/

#ifndef ISETMANAGER_H
#define ISETMANAGER_H

#include <cstdint>

class Interval;

/*! An interface for querying information regarding the currently-running set. */
class ISetManager
{
public:
    //! Query the current interval during playback.
    virtual Interval* currentInterval() = 0;

    //! Query the next interval during playback.
    virtual Interval* nextInterval() = 0;

    //! Query whether or not the current interval is part of a loop and, if so, what iteration we are currently at.
    /*!
     * \brief Query what iteration we are currently on (if any).
     * \param current The current iteration we are on.
     * \param total The total number of iterations in this current loop.
     * \return true if we are currently in a loop during playback, false otherwise.
     */
    virtual bool currentIteration(uint32_t &current, uint32_t &total) = 0;
};

#endif // ISETMANAGER_H
