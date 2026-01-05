#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTabWidget>
#include <QTimer>
#include <QSystemTrayIcon>
#include "bookwidget.h"
#include "readerwidget.h"
#include "borrowwidget.h"
#include "statisticswidget.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void checkOverdueBooks();
    void showOverdueNotification();

private:
    Ui::MainWindow *ui;
    QTabWidget *tabWidget;
    BookWidget *bookWidget;
    ReaderWidget *readerWidget;
    BorrowWidget *borrowWidget;
    StatisticsWidget *statisticsWidget;
    
    QTimer *overdueCheckTimer;
    QSystemTrayIcon *trayIcon;
    
    void setupUI();
    void setupOverdueCheck();
};

#endif // MAINWINDOW_H
