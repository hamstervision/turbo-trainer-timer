#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QScrollArea>
#include "stagingarea.h"
#include "showtimewidget.h"
#include "turbosetmodel.h"
#include "QtAwesome/QtAwesome.h"
#include "ifontawesome.h"

namespace Ui {
class MainWindow;
}

class QCloseEvent;

class MainWindow : public QMainWindow, public IFontAwesome
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

public: // IFontAwesome
    QIcon faIcon(const int character) override;
    QFont faFont(const int size) override;

private slots:
    void onStagingAreaResized();
    void onSliderMoved(int pos);
    void onShowAddMenu(const QPoint menuPos, Step *parent);

    void resizeEvent(QResizeEvent *event) override;
    void closeEvent(QCloseEvent *event) override;

    void onSetStarted();
    void onSetPaused();
    void onSetResumed();
    void onSetComplete();
    void onSetStopped();

    void on_actionAddStep_triggered();
    void on_actionNew_triggered();
    void on_actionOpen_triggered();
    void on_actionSave_triggered();
    void on_actionExit_triggered();
    void on_actionSaveAs_triggered();
    void on_actionPlay_triggered();
    void on_actionPause_triggered();
    void on_actionStop_triggered();

protected:
    bool save(bool forcePrompt);

private:
    Ui::MainWindow *ui;
    QScrollArea *m_scrollArea;
    StagingArea *m_stagingArea;
    ShowTimeWindow *m_showtimeWindow;
    ShowTimeWidget *m_showtimeWidget;
    TurboSetModel m_setModel;
    QString m_filePath;
    QtAwesome *m_fontAwesome;
    int m_scrollPos;
};

#endif // MAINWINDOW_H
