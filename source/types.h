/*************************************
 * Copyright (C) 2017 Michael Pearce *
 *************************************/

#ifndef TYPES_H
#define TYPES_H

/*! \file types.h
 * Contains type definitions used throughout the application */

/*!
 * \brief Lists the default types of Steps available.
 */
enum class StepType
{
    WarmUp,
    Drill,
    Effort,
    Recovery,
    CoolDown,
    Loop
};

#endif // TYPES_H
