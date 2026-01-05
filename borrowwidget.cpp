#include "borrowwidget.h"
#include "ui_borrowwidget.h"
#include <QDate>
#include <QHeaderView>
#include <QSqlQuery>

BorrowWidget::BorrowWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::BorrowWidget)
{
    ui->setupUi(this);
    setupUI();
    setupModel();
}

BorrowWidget::~BorrowWidget()
{
    delete ui;
}

void BorrowWidget::setupUI()
{
    // 设置日期编辑器
    ui->borrowDateEdit->setDate(QDate::currentDate());
    ui->returnDateEdit->setDate(QDate::currentDate().addDays(30));
    
    // 设置状态下拉框
    ui->statusCombo->setItemData(0, "");
    ui->statusCombo->setItemData(1, "borrowed");
    ui->statusCombo->setItemData(2, "returned");
    
    // 设置按钮提示信息
    ui->returnButton->setToolTip("请先在表格中选择一条已借出的记录");
    
    // 连接信号
    connect(ui->searchButton, &QPushButton::clicked, this, &BorrowWidget::onSearch);
    connect(ui->clearButton, &QPushButton::clicked, this, &BorrowWidget::onClearSearch);
    connect(ui->borrowButton, &QPushButton::clicked, this, &BorrowWidget::onBorrowBook);
    connect(ui->returnButton, &QPushButton::clicked, this, &BorrowWidget::onReturnBook);
    connect(ui->borrowDaysSpin, QOverload<int>::of(&QSpinBox::valueChanged), this, &BorrowWidget::updateBorrowDays);
    connect(ui->borrowDateEdit, &QDateEdit::dateChanged, this, &BorrowWidget::updateBorrowDays);
    connect(ui->tableView->selectionModel(), &QItemSelectionModel::selectionChanged,
            this, &BorrowWidget::onSelectionChanged);
    
    // 确保表格可以接收点击事件
    connect(ui->tableView, &QTableView::clicked, this, [this](const QModelIndex &index) {
        Q_UNUSED(index);
        onSelectionChanged();
    });
}

void BorrowWidget::setupModel()
{
    model = new BorrowModel(this, Database::instance().getDatabase());
    ui->tableView->setModel(model);
    
    // 设置列宽
    ui->tableView->horizontalHeader()->setStretchLastSection(true);
    ui->tableView->setColumnWidth(0, 50);
    ui->tableView->setColumnWidth(1, 200);
    ui->tableView->setColumnWidth(2, 120);
    ui->tableView->setColumnWidth(3, 100);
    ui->tableView->setColumnWidth(4, 100);
    ui->tableView->setColumnWidth(5, 100);
    ui->tableView->setColumnWidth(6, 80);
    
    // 隐藏不需要的列
    ui->tableView->hideColumn(7); // created_at
}

void BorrowWidget::onBorrowBook()
{
    int bookId = ui->bookIdSpin->value();
    int readerId = ui->readerIdSpin->value();
    QDate borrowDate = ui->borrowDateEdit->date();
    QDate returnDate = ui->returnDateEdit->date();
    
    // 验证图书和读者是否存在
    QSqlQuery query(Database::instance().getDatabase());
    query.prepare("SELECT id FROM books WHERE id = ?");
    query.addBindValue(bookId);
    if (!query.exec() || !query.next()) {
        QMessageBox::warning(this, "警告", "图书ID不存在！");
        return;
    }
    
    query.prepare("SELECT id FROM readers WHERE id = ?");
    query.addBindValue(readerId);
    if (!query.exec() || !query.next()) {
        QMessageBox::warning(this, "警告", "读者ID不存在！");
        return;
    }
    
    if (Database::instance().borrowBook(bookId, readerId, borrowDate, returnDate)) {
        QMessageBox::information(this, "成功", "借书成功！");
        model->select();
    } else {
        QMessageBox::critical(this, "错误", "借书失败！可能是图书已全部借出。");
    }
}

void BorrowWidget::onReturnBook()
{
    int row = getSelectedRow();
    if (row < 0) {
        QMessageBox::warning(this, "警告", "请选择要归还的借阅记录！");
        return;
    }
    
    int borrowId = model->data(model->index(row, 0)).toInt();
    QString status = model->data(model->index(row, 6)).toString();
    
    if (status != "borrowed") {
        QMessageBox::warning(this, "警告", "该记录不是借出状态，无法归还！");
        return;
    }
    
    // 获取借阅详细信息
    Database::BorrowInfo info = Database::instance().getBorrowInfo(borrowId);
    if (info.borrowId == 0) {
        QMessageBox::critical(this, "错误", "无法获取借阅记录信息！");
        return;
    }
    
    // 构建确认消息
    QString confirmMsg = QString("确认归还以下图书？\n\n"
                                "图书名称：%1\n"
                                "读者姓名：%2\n"
                                "借阅日期：%3\n"
                                "应还日期：%4")
                        .arg(info.bookTitle)
                        .arg(info.readerName)
                        .arg(info.borrowDate.toString("yyyy-MM-dd"))
                        .arg(info.returnDate.toString("yyyy-MM-dd"));
    
    // 如果逾期，显示逾期信息
    if (info.isOverdue) {
        confirmMsg += QString("\n\n⚠️ 该图书已逾期 %1 天！").arg(info.overdueDays);
    }
    
    int ret = QMessageBox::question(this, "确认还书", confirmMsg,
                                    QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);
    if (ret != QMessageBox::Yes) {
        return;
    }
    
    // 执行还书操作
    if (Database::instance().returnBook(borrowId)) {
        QString successMsg = QString("还书成功！\n\n"
                                    "图书：%1\n"
                                    "读者：%2\n"
                                    "归还日期：%3")
                            .arg(info.bookTitle)
                            .arg(info.readerName)
                            .arg(QDate::currentDate().toString("yyyy-MM-dd"));
        
        if (info.isOverdue) {
            successMsg += QString("\n\n注意：该图书逾期 %1 天归还。").arg(info.overdueDays);
        }
        
        QMessageBox::information(this, "还书成功", successMsg);
        model->select();
        
        // 清除选择
        ui->tableView->clearSelection();
        ui->returnButton->setEnabled(false);
    } else {
        QMessageBox::critical(this, "错误", "还书失败！请检查数据库连接。");
    }
}

void BorrowWidget::onSearch()
{
    QString status = ui->statusCombo->currentData().toString();
    model->setFilter(ui->bookTitleEdit->text(), ui->readerNameEdit->text(), status);
}

void BorrowWidget::onClearSearch()
{
    ui->bookTitleEdit->clear();
    ui->readerNameEdit->clear();
    ui->statusCombo->setCurrentIndex(0);
    model->clearFilter();
}

void BorrowWidget::onSelectionChanged()
{
    int row = getSelectedRow();
    if (row >= 0) {
        QString status = model->data(model->index(row, 6)).toString();
        bool canReturn = (status == "borrowed");
        ui->returnButton->setEnabled(canReturn);
        
        if (canReturn) {
            // 检查是否逾期
            int borrowId = model->data(model->index(row, 0)).toInt();
            Database::BorrowInfo info = Database::instance().getBorrowInfo(borrowId);
            if (info.isOverdue) {
                ui->returnButton->setToolTip(QString("归还当前选中的图书（已逾期 %1 天）").arg(info.overdueDays));
            } else {
                ui->returnButton->setToolTip("归还当前选中的图书");
            }
        } else {
            ui->returnButton->setToolTip("该记录不是借出状态，无法归还");
        }
    } else {
        ui->returnButton->setEnabled(false);
        ui->returnButton->setToolTip("请先在表格中选择一条已借出的记录");
    }
}

void BorrowWidget::updateBorrowDays()
{
    updateReturnDate();
}

void BorrowWidget::updateReturnDate()
{
    QDate borrowDate = ui->borrowDateEdit->date();
    int days = ui->borrowDaysSpin->value();
    ui->returnDateEdit->setDate(borrowDate.addDays(days));
}

int BorrowWidget::getSelectedRow()
{
    QModelIndexList indexes = ui->tableView->selectionModel()->selectedRows();
    if (indexes.isEmpty()) {
        return -1;
    }
    return indexes.first().row();
}
