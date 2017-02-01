/*************************************
 * Copyright (C) 2017 Michael Pearce *
 *************************************/

#ifndef STAGINGAREA_H
#define STAGINGAREA_H

#include <QWidget>
#include <QVBoxLayout>
#include <QList>

#include "turbosetmodel.h"
#include "stepwidget.h"
#include "ifontawesome.h"

/*! Widget used to set up the turbo set */
class StagingArea : public QWidget
{
    Q_OBJECT
public:
    explicit StagingArea(TurboSetModel *model, IFontAwesome *fontAwesome, QWidget *parent = 0);

    int idealHeight() const;

signals:
    void sizeChanged();

public slots:
    void onSetChanged();

protected: // Event handlers
    bool event(QEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;
    void paintEvent(QPaintEvent *event) override;

protected:
    void clearView();
    void adjustLayout();
    void addStep(Step *step, QWidget *parent);

protected:
    TurboSetModel *m_model;
    QList<StepWidget*> m_steps;
    IFontAwesome *m_fontAwesome;
    int m_idealWidth;
    int m_idealHeight;
};

#endif // STAGINGAREA_H
