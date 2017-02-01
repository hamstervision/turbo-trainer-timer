/*************************************
 * Copyright (C) 2017 Michael Pearce *
 *************************************/

#ifndef SHOWTIMESTEPWIDGET_H
#define SHOWTIMESTEPWIDGET_H

#include <QObject>
#include <QResizeEvent>
#include <QPixmap>
#include <QLabel>
#include <QHBoxLayout>
#include "step.h"
#include "ifontawesome.h"

/*! Base class for the interval widgets in the ShowTimeWidget */
class ShowTimeStepWidget : public QWidget
{
    Q_OBJECT
public:
    explicit ShowTimeStepWidget(IFontAwesome *fontAwesome, QWidget *parent = nullptr);

protected: // Event handlers
    void resizeEvent(QResizeEvent *event) override;
    void paintEvent(QPaintEvent *event) override;

protected:
    virtual void adjustLayout() = 0;
    virtual int headerSize() = 0;

protected:
    IFontAwesome    *m_fontAwesome;
    Interval        *m_interval;
    QWidget         *m_topRow;
    QHBoxLayout     *m_topRowLayout;
    QLabel          *m_icon;
    QLabel          *m_type;
    QLabel          *m_time;
    QLabel          *m_text;
    QColor          m_bgrColour;
};

#endif // SHOWTIMESTEPWIDGET_H
