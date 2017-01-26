#ifndef STEPRESOURCES_H
#define STEPRESOURCES_H

#include <QString>
#include <QColor>
#include <map>
#include "types.h"
#include "ifontawesome.h"

class QLabel;
class QAction;
class QMenu;

QString TypeToString(const StepType type);
int TypeToFaIcon(const StepType type);
QColor TypeToBgColour(const StepType type);

void UpdateLabelFontSize(QLabel *label);
void DrawFaIconToLabel(QLabel *label, const int faIcon, IFontAwesome *fontAwesome);

std::map<QAction*, StepType> AddTypesToMenu(QMenu *menu, IFontAwesome *fontAwesome);

static const QString FontName("Century Gothic");

#endif // STEPRESOURCES_H
