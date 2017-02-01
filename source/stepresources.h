/*************************************
 * Copyright (C) 2017 Michael Pearce *
 *************************************/

#ifndef STEPRESOURCES_H
#define STEPRESOURCES_H

/*!
 * \file stepresources.h
 * \brief Contains utility functions and definitions used for visually representing Step objects
*/

#include <QString>
#include <QColor>
#include <map>
#include "types.h"
#include "ifontawesome.h"

class QLabel;
class QAction;
class QMenu;

/*! Get the string representation of a StepType constant */
QString TypeToString(const StepType type);

/*! Get the FontAwesome character-code associated with a given StepType constant */
int TypeToFaIcon(const StepType type);

/*! Get the background-colour associated with a given StepType constant */
QColor TypeToBgColour(const StepType type);

/*!
 * Scale the label's font's size according to the size of of the label.
 * \param label A pointer to the label in question.
 * \param maxHeight Denotes whether to use the label's height or its width to determine the scaling.
 */
void UpdateLabelFontSize(QLabel *label, bool maxHeight = true);

/*! Sets the text of the label to be the FontAwesome icon associated with the character code faIcon */
void DrawFaIconToLabel(QLabel *label, const int faIcon, IFontAwesome *fontAwesome);

/*!
 * Adds all of the step types to a given menu.
 * \param menu The menu to which to add the step types.
 * \param fontAwesome A pointer to the IFontAwesome interface, used to generate icons for each step type.
 * \return A container mapping the menu action to the originating step type constant.
 */
std::map<QAction*, StepType> AddTypesToMenu(QMenu *menu, IFontAwesome *fontAwesome);

/*! One font-family to rule them all */
static const QString FontName("Century Gothic");

#endif // STEPRESOURCES_H
