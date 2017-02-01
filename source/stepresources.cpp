/*************************************
 * Copyright (C) 2017 Michael Pearce *
 *************************************/

#include "stepresources.h"
#include "QtAwesome/QtAwesome.h"
#include <QLabel>
#include <QMenu>

QString TypeToString(const StepType type)
{
    switch (type)
    {
    case StepType::WarmUp:
        return "Warm-up";
    case StepType::Drill:
        return "Drill";
    case StepType::Effort:
        return "Effort";
    case StepType::Recovery:
        return "Recovery";
    case StepType::CoolDown:
        return "Cool-down";
    case StepType::Loop:
        return "Loop";
    }

    Q_ASSERT(false);
    return "";
}

int TypeToFaIcon(const StepType type)
{
    switch (type)
    {
    case StepType::WarmUp:
        return fa::fire;
    case StepType::Drill:
        return fa::bicycle;
    case StepType::Effort:
        return fa::arrowcircleup;
    case StepType::Recovery:
        return fa::arrowcircledown;
    case StepType::CoolDown:
        return fa::snowflakeo;
    case StepType::Loop:
        return fa::repeat;
    }

    Q_ASSERT(false);
    return fa::question;
}

QColor TypeToBgColour(const StepType type)
{
    switch (type)
    {
    case StepType::WarmUp:
        return QColor(0xff, 0x96, 0x81);
    case StepType::Drill:
        return QColor(0xdf, 0xdc, 0xcd);
    case StepType::Effort:
        return QColor(0xde, 0x68, 0x68);
    case StepType::Recovery:
        return QColor(0xbf, 0xe2, 0xca);
    case StepType::CoolDown:
        return QColor(0xac, 0xc7, 0xdc);
    case StepType::Loop:
        return QColor(0xbf, 0xb1, 0xd5);
    }

    Q_ASSERT(false);
    return QColor();
}

void UpdateLabelFontSize(QLabel *label, bool maxHeight /*= true*/)
{
    if (!label)
        return;

    QFontMetrics metrics(label->font());

    float factor = maxHeight
            ? (float)label->height() / (float)metrics.height()
            : (float)label->width() / (float)metrics.width(label->text());

    if ((factor < 1) || (factor > 1.25))
    {
        QFont scaledFont(label->font());
        scaledFont.setPointSizeF(scaledFont.pointSizeF() * factor);
        label->setFont(scaledFont);
    }
}

void DrawFaIconToLabel(QLabel *label, const int faIcon, IFontAwesome *fontAwesome)
{
    if (!label || !fontAwesome)
    {
        Q_ASSERT(false);
        return;
    }

    label->setFont(fontAwesome->faFont(label->height()));
    label->setText(QChar(faIcon));
}

std::map<QAction *, StepType> AddTypesToMenu(QMenu *menu, IFontAwesome *fontAwesome)
{
    std::map<QAction*, StepType> actions;

    if (!menu || !fontAwesome)
    {
        Q_ASSERT(false);
        return actions;
    }

    for (int i = (int)StepType::WarmUp; i <= (int)StepType::Loop; ++i)
    {
        StepType type = (StepType)(i);
        QAction *act = menu->addAction(fontAwesome->faIcon(TypeToFaIcon(type)), TypeToString(type));
        actions[act] = type;
    }

    return actions;
}
