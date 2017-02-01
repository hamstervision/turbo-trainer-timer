/*************************************
 * Copyright (C) 2017 Michael Pearce *
 *************************************/

#ifndef STEPCONTROL_H
#define STEPCONTROL_H

#include <QWidget>
#include <QHBoxLayout>
#include <QLabel>
#include <QPixmap>
#include <QTextEdit>
#include <QTimeEdit>
#include <QSpinBox>
#include <QPushButton>
#include <QContextMenuEvent>

#include "types.h"
#include "ifontawesome.h"

class Step;

/*! Base Widget representing a Step object when setting up the turbo set */
class StepWidget : public QWidget
{
    Q_OBJECT
public:
    explicit StepWidget(Step *step, IFontAwesome *fontAwesome, QWidget *parent = nullptr);

    virtual int idealHeight() const = 0;

    Step* step()
    {
        return m_step;
    }

signals:
    void deleted(Step *step);
    void movedUp(Step *step);
    void movedDown(Step *step);
    void typeChanged(Step *step, const StepType newType);

protected slots:
    void onDelete(bool checked = false);
    void onMoveUp(bool checked = false);
    void onMoveDown(bool checked = false);

protected:
    void initAlignment();
    void initButtons();

    int idealTypeLabelWidth();

protected: // Event handlers
    void contextMenuEvent(QContextMenuEvent *event) override;

protected:
    Step            *m_step;
    IFontAwesome    *m_fontAwesome;

    QFont           m_labelFont;
    QFont           m_editFont;

    QWidget         *m_titleArea;
    QLabel          *m_title;
    QLabel          *m_icon;

    QWidget         *m_buttonsArea;
    QPushButton     *m_delete;
    QPushButton     *m_moveUp;
    QPushButton     *m_moveDown;

};

/*! Represents an Interval object when setting up the turbo set */
class IntervalWidget : public StepWidget
{
    Q_OBJECT
public:
    explicit IntervalWidget(Step *step, IFontAwesome *fontAwesome, QWidget *parent = nullptr);

    int idealHeight() const override;

protected slots:
    void onTimeChanged(const QTime &time);
    void onTextChanged();

protected: // Event handlers
    void paintEvent(QPaintEvent *event) override;

protected:
    QHBoxLayout *m_layout;
    QWidget     *m_timeArea;
    QLabel      *m_timeLabel;
    QTimeEdit   *m_timeEdit;

    QWidget     *m_descriptionArea;
    QLabel      *m_descriptionLabel;
    QTextEdit   *m_textEdit;
};

/*! Represents a Loop object when setting up the turbo set, and is responsible for the layout of that Loop's child objects. */
class LoopStepWidget : public StepWidget
{
    Q_OBJECT
public:
    explicit LoopStepWidget(Step *step, IFontAwesome *fontAwesome, QWidget *parent = nullptr);

    int idealHeight() const override;

    void addChild(StepWidget *child);

public slots:
    void onStepDeleted(Step *step);
    void onStepMovedUp(Step *step);
    void onStepMovedDown(Step *step);
    void onTypeChanged(Step *step, const StepType newType);

protected slots:
    void onAddChildPressed();
    void onIterationsChanged(int iterations);

protected: // Event handlers
    void paintEvent(QPaintEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;

protected:
    void adjustLayout();

protected:
    QWidget     *m_topRow;
    QHBoxLayout *m_topRowLayout;

    QWidget     *m_loopArea;
    QLabel      *m_loopLabel;
    QSpinBox    *m_spinBox;

    QWidget     *m_addButtonArea;
    QLabel      *m_buttonLabel;
    QPushButton *m_addChildButton;

    QList<StepWidget*> m_children;
};

#endif // STEPCONTROL_H
