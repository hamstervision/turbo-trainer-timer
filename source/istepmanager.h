#ifndef ISTEPMANAGER_H
#define ISTEPMANAGER_H

class IStepManager
{
public:
    virtual void notifyChange(bool redrawNeeded = false) = 0;
};

#endif // ISTEPMANAGER_H
