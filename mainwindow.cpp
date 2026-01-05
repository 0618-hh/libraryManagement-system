#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "database.h"
#include <QVBoxLayout>
#include <QMessageBox>
#include <QApplication>
#include <QTimer>
#include <QStyle>
#include <QSystemTrayIcon>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    
    // 初始化数据库
    if (!Database::instance().initializeDatabase()) {
        QMessageBox::critical(this, "错误", "数据库初始化失败！");
        QApplication::quit();
        return;
    }
    
    setupUI();
    setupOverdueCheck();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::setupUI()
{
    setWindowTitle("图书借阅管理系统");
    setMinimumSize(1200, 800);
    
    // 创建标签页
    tabWidget = new QTabWidget(this);
    setCentralWidget(tabWidget);
    
    // 创建各个功能模块
    bookWidget = new BookWidget(this);
    readerWidget = new ReaderWidget(this);
    borrowWidget = new BorrowWidget(this);
    statisticsWidget = new StatisticsWidget(this);
    
    // 添加标签页
    tabWidget->addTab(bookWidget, "图书管理");
    tabWidget->addTab(readerWidget, "读者管理");
    tabWidget->addTab(borrowWidget, "借还书管理");
    tabWidget->addTab(statisticsWidget, "数据统计");
}

void MainWindow::setupOverdueCheck()
{
    // 创建系统托盘图标
    trayIcon = new QSystemTrayIcon(this);
    trayIcon->setIcon(style()->standardIcon(QStyle::SP_MessageBoxWarning));
    trayIcon->setToolTip("图书借阅管理系统");
    trayIcon->show();
    
    // 创建定时器，每小时检查一次逾期
    overdueCheckTimer = new QTimer(this);
    connect(overdueCheckTimer, &QTimer::timeout, this, &MainWindow::checkOverdueBooks);
    overdueCheckTimer->start(3600000); // 1小时 = 3600000毫秒
    
    // 启动时立即检查一次
    QTimer::singleShot(1000, this, &MainWindow::checkOverdueBooks);
}

void MainWindow::checkOverdueBooks()
{
    QList<QPair<int, QDate>> overdueList = Database::instance().getOverdueBorrows();
    
    if (!overdueList.isEmpty()) {
        showOverdueNotification();
    }
}

void MainWindow::showOverdueNotification()
{
    QList<QPair<int, QDate>> overdueList = Database::instance().getOverdueBorrows();
    int count = overdueList.size();
    
    QString message = QString("有 %1 本图书已逾期，请及时处理！").arg(count);
    
    trayIcon->showMessage("逾期提醒", message, QSystemTrayIcon::Warning, 5000);
    
    // 如果窗口可见，也显示消息框
    if (isVisible()) {
        QMessageBox::warning(this, "逾期提醒", message);
    }
}
