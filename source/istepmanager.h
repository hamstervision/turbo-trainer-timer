/*************************************
 * Copyright (C) 2017 Michael Pearce *
 *************************************/

#ifndef ISTEPMANAGER_H
#define ISTEPMANAGER_H

/*! An interfact for alerting the step model that a change has been made */
class IStepManager
{
public:
    /*! Notify the model that a change has been made
     * \param redrawNeeded Whether or not the change should result in a redraw.
     */
    virtual void notifyChange(bool redrawNeeded = false) = 0;
};

#endif // ISTEPMANAGER_H
