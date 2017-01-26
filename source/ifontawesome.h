#ifndef IFONTAWESOME_H
#define IFONTAWESOME_H

#include "QtAwesome/QtAwesome.h"

class IFontAwesome
{
public:
    virtual QIcon faIcon(const int character) = 0;
    virtual QFont faFont(const int size) = 0;
};

#endif // IFONTAWESOME_H
