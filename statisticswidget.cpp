#include "statisticswidget.h"
#include "ui_statisticswidget.h"
#include <QHeaderView>
#include <QDate>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QPushButton>
#include <QBrush>
#include <QColor>

StatisticsWidget::StatisticsWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::StatisticsWidget)
{
    ui->setupUi(this);
    setupUI();
    refreshStatistics();
}

StatisticsWidget::~StatisticsWidget()
{
    delete ui;
}

void StatisticsWidget::setupUI()
{
    // 设置表格属性
    ui->topBooksTable->horizontalHeader()->setStretchLastSection(true);
    ui->topReadersTable->horizontalHeader()->setStretchLastSection(true);
    ui->overdueTable->horizontalHeader()->setStretchLastSection(true);
    
    // 连接信号
    connect(ui->refreshButton, &QPushButton::clicked, this, &StatisticsWidget::refreshStatistics);
}

void StatisticsWidget::refreshStatistics()
{
    QSqlQuery query(Database::instance().getDatabase());
    
    // 统计图书总数
    query.exec("SELECT COUNT(*) FROM books");
    if (query.next()) {
        ui->totalBooksLabel->setText(QString("图书总数: %1").arg(query.value(0).toInt()));
    }
    
    // 统计读者总数
    query.exec("SELECT COUNT(*) FROM readers");
    if (query.next()) {
        ui->totalReadersLabel->setText(QString("读者总数: %1").arg(query.value(0).toInt()));
    }
    
    // 统计借阅总数
    query.exec("SELECT COUNT(*) FROM borrows");
    if (query.next()) {
        ui->totalBorrowsLabel->setText(QString("借阅总数: %1").arg(query.value(0).toInt()));
    }
    
    // 统计逾期数量
    query.prepare("SELECT COUNT(*) FROM borrows WHERE status = 'borrowed' AND return_date < DATE('now')");
    if (query.exec() && query.next()) {
        int overdueCount = query.value(0).toInt();
        ui->overdueBorrowsLabel->setText(QString("逾期数量: %1").arg(overdueCount));
    }
    
    loadTopBooks();
    loadTopReaders();
    loadOverdueBorrows();
}

void StatisticsWidget::loadTopBooks()
{
    QSqlQuery query(Database::instance().getDatabase());
    query.prepare(R"(
        SELECT b.title, COUNT(*) as borrow_count
        FROM borrows br
        JOIN books b ON br.book_id = b.id
        GROUP BY b.id, b.title
        ORDER BY borrow_count DESC
        LIMIT 10
    )");
    
    ui->topBooksTable->setRowCount(0);
    int row = 0;
    
    if (query.exec()) {
        while (query.next()) {
            ui->topBooksTable->insertRow(row);
            ui->topBooksTable->setItem(row, 0, new QTableWidgetItem(QString::number(row + 1)));
            ui->topBooksTable->setItem(row, 1, new QTableWidgetItem(query.value(0).toString()));
            ui->topBooksTable->setItem(row, 2, new QTableWidgetItem(query.value(1).toString()));
            row++;
        }
    }
}

void StatisticsWidget::loadTopReaders()
{
    QSqlQuery query(Database::instance().getDatabase());
    query.prepare(R"(
        SELECT r.name, COUNT(*) as borrow_count
        FROM borrows br
        JOIN readers r ON br.reader_id = r.id
        GROUP BY r.id, r.name
        ORDER BY borrow_count DESC
        LIMIT 10
    )");
    
    ui->topReadersTable->setRowCount(0);
    int row = 0;
    
    if (query.exec()) {
        while (query.next()) {
            ui->topReadersTable->insertRow(row);
            ui->topReadersTable->setItem(row, 0, new QTableWidgetItem(QString::number(row + 1)));
            ui->topReadersTable->setItem(row, 1, new QTableWidgetItem(query.value(0).toString()));
            ui->topReadersTable->setItem(row, 2, new QTableWidgetItem(query.value(1).toString()));
            row++;
        }
    }
}

void StatisticsWidget::loadOverdueBorrows()
{
    QSqlQuery query(Database::instance().getDatabase());
    query.prepare(R"(
        SELECT br.id, b.title, r.name, br.return_date,
               julianday('now') - julianday(br.return_date) as overdue_days
        FROM borrows br
        JOIN books b ON br.book_id = b.id
        JOIN readers r ON br.reader_id = r.id
        WHERE br.status = 'borrowed' AND br.return_date < DATE('now')
        ORDER BY br.return_date ASC
    )");
    
    ui->overdueTable->setRowCount(0);
    int row = 0;
    
    if (query.exec()) {
        while (query.next()) {
            ui->overdueTable->insertRow(row);
            ui->overdueTable->setItem(row, 0, new QTableWidgetItem(query.value(0).toString()));
            ui->overdueTable->setItem(row, 1, new QTableWidgetItem(query.value(1).toString()));
            ui->overdueTable->setItem(row, 2, new QTableWidgetItem(query.value(2).toString()));
            ui->overdueTable->setItem(row, 3, new QTableWidgetItem(query.value(3).toString()));
            
            int overdueDays = static_cast<int>(query.value(4).toDouble());
            QTableWidgetItem *daysItem = new QTableWidgetItem(QString::number(overdueDays));
            daysItem->setForeground(QBrush(QColor(255, 0, 0)));
            ui->overdueTable->setItem(row, 4, daysItem);
            
            row++;
        }
    }
}
