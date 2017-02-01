/*************************************
 * Copyright (C) 2017 Michael Pearce *
 *************************************/

#ifndef IFONTAWESOME_H
#define IFONTAWESOME_H

#include "QtAwesome/QtAwesome.h"

/*! An interface for accessing FontAwesome */

class IFontAwesome
{
public:
    /*! Get the icon that matches the specified input character.
     * \param character The FontAwesome character-code. See QtAwesome.h for a list.
     * \return An icon representation of the desired FontAwesome character
     */
    virtual QIcon faIcon(const int character) = 0;

    /*! Get the FontAwesome font at a given pixel size.
     * \param size The desired size (in pixels) of the font.
     * \return The FontAwesome font at the desired pixel height.
     */
    virtual QFont faFont(const int size) = 0;
};

#endif // IFONTAWESOME_H
