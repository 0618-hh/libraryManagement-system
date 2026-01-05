#include "bookwidget.h"
#include "ui_bookwidget.h"
#include <QDate>
#include <QHeaderView>

BookWidget::BookWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::BookWidget)
{
    ui->setupUi(this);
    setupUI();
    setupModel();
}

BookWidget::~BookWidget()
{
    delete ui;
}

void BookWidget::setupUI()
{
    // 设置日期编辑器
    ui->editPublishDate->setDate(QDate::currentDate());
    
    // 设置按钮提示信息
    ui->editButton->setToolTip("请先在表格中选择一条记录");
    ui->deleteButton->setToolTip("请先在表格中选择一条记录");
    
    // 连接信号
    connect(ui->searchButton, &QPushButton::clicked, this, &BookWidget::onSearch);
    connect(ui->clearButton, &QPushButton::clicked, this, &BookWidget::onClearSearch);
    connect(ui->addButton, &QPushButton::clicked, this, &BookWidget::onAddBook);
    connect(ui->editButton, &QPushButton::clicked, this, &BookWidget::onEditBook);
    connect(ui->deleteButton, &QPushButton::clicked, this, &BookWidget::onDeleteBook);
    connect(ui->tableView->selectionModel(), &QItemSelectionModel::selectionChanged,
            this, &BookWidget::onSelectionChanged);
    
    // 确保表格可以接收点击事件
    connect(ui->tableView, &QTableView::clicked, this, [this](const QModelIndex &index) {
        Q_UNUSED(index);
        // 点击表格时确保选择正确
        onSelectionChanged();
    });
}

void BookWidget::setupModel()
{
    model = new BookModel(this, Database::instance().getDatabase());
    ui->tableView->setModel(model);
    
    // 设置列宽
    ui->tableView->horizontalHeader()->setStretchLastSection(true);
    ui->tableView->setColumnWidth(0, 50);
    ui->tableView->setColumnWidth(1, 150);
    ui->tableView->setColumnWidth(2, 200);
    ui->tableView->setColumnWidth(3, 150);
    ui->tableView->setColumnWidth(4, 150);
    ui->tableView->setColumnWidth(5, 100);
    ui->tableView->setColumnWidth(6, 80);
    ui->tableView->setColumnWidth(7, 80);
    
    // 隐藏不需要的列
    ui->tableView->hideColumn(8); // created_at
}

void BookWidget::onAddBook()
{
    if (ui->editIsbn->text().isEmpty() || ui->editTitle->text().isEmpty() || ui->editAuthor->text().isEmpty()) {
        QMessageBox::warning(this, "警告", "请填写ISBN、书名和作者！");
        return;
    }
    
    if (Database::instance().addBook(ui->editIsbn->text(), ui->editTitle->text(), ui->editAuthor->text(),
                                     ui->editPublisher->text(), ui->editPublishDate->date(),
                                     ui->editTotalCopies->value())) {
        QMessageBox::information(this, "成功", "图书添加成功！");
        clearEditForm();
        model->select();
    } else {
        QMessageBox::critical(this, "错误", "图书添加失败！可能是ISBN已存在。");
    }
}

void BookWidget::onEditBook()
{
    int row = getSelectedRow();
    if (row < 0) {
        QMessageBox::warning(this, "警告", "请选择要修改的图书！");
        return;
    }
    
    int bookId = model->data(model->index(row, 0)).toInt();
    
    if (ui->editIsbn->text().isEmpty() || ui->editTitle->text().isEmpty() || ui->editAuthor->text().isEmpty()) {
        QMessageBox::warning(this, "警告", "请填写ISBN、书名和作者！");
        return;
    }
    
    // 检查总册数是否合理
    if (ui->editTotalCopies->value() < 1) {
        QMessageBox::warning(this, "警告", "总册数必须大于0！");
        return;
    }
    
    if (Database::instance().updateBook(bookId, ui->editIsbn->text(), ui->editTitle->text(),
                                        ui->editAuthor->text(), ui->editPublisher->text(),
                                        ui->editPublishDate->date(), ui->editTotalCopies->value())) {
        QMessageBox::information(this, "成功", "图书修改成功！");
        clearEditForm();
        model->select();
        
        // 尝试重新选中修改的记录
        for (int i = 0; i < model->rowCount(); ++i) {
            if (model->data(model->index(i, 0)).toInt() == bookId) {
                ui->tableView->selectRow(i);
                break;
            }
        }
        
        ui->editButton->setEnabled(false);
        ui->deleteButton->setEnabled(false);
    } else {
        QString errorMsg = "图书修改失败！";
        if (Database::instance().isBookIsbnExists(ui->editIsbn->text(), bookId)) {
            errorMsg += "\n原因：ISBN已存在，请使用其他ISBN。";
        }
        QMessageBox::critical(this, "错误", errorMsg);
    }
}

void BookWidget::onDeleteBook()
{
    int row = getSelectedRow();
    if (row < 0) {
        QMessageBox::warning(this, "警告", "请选择要删除的图书！");
        return;
    }
    
    int bookId = model->data(model->index(row, 0)).toInt();
    QString title = model->data(model->index(row, 2)).toString();
    
    // 检查是否有借阅记录
    if (Database::instance().hasBookBorrows(bookId)) {
        QMessageBox::warning(this, "无法删除", 
                            QString("无法删除图书《%1》！\n\n原因：该图书存在借阅记录，请先处理所有借阅记录后再删除。").arg(title));
        return;
    }
    
    int ret = QMessageBox::question(this, "确认删除", 
                                    QString("确定要删除图书《%1》吗？\n\n此操作不可恢复！").arg(title),
                                    QMessageBox::Yes | QMessageBox::No, QMessageBox::No);
    if (ret == QMessageBox::Yes) {
        if (Database::instance().deleteBook(bookId)) {
            QMessageBox::information(this, "成功", "图书删除成功！");
            clearEditForm();
            model->select();
            ui->editButton->setEnabled(false);
            ui->deleteButton->setEnabled(false);
        } else {
            QMessageBox::critical(this, "错误", "图书删除失败！");
        }
    }
}

void BookWidget::onSearch()
{
    model->setFilter(ui->isbnEdit->text(), ui->titleEdit->text(), ui->authorEdit->text());
}

void BookWidget::onClearSearch()
{
    ui->isbnEdit->clear();
    ui->titleEdit->clear();
    ui->authorEdit->clear();
    model->clearFilter();
}

void BookWidget::onSelectionChanged()
{
    int row = getSelectedRow();
    if (row >= 0) {
        loadBookToForm(row);
        ui->editButton->setEnabled(true);
        ui->deleteButton->setEnabled(true);
        ui->editButton->setToolTip("修改当前选中的图书信息");
        ui->deleteButton->setToolTip("删除当前选中的图书");
    } else {
        clearEditForm();
        ui->editButton->setEnabled(false);
        ui->deleteButton->setEnabled(false);
        ui->editButton->setToolTip("请先在表格中选择一条记录");
        ui->deleteButton->setToolTip("请先在表格中选择一条记录");
    }
}

void BookWidget::clearEditForm()
{
    ui->editIsbn->clear();
    ui->editTitle->clear();
    ui->editAuthor->clear();
    ui->editPublisher->clear();
    ui->editPublishDate->setDate(QDate::currentDate());
    ui->editTotalCopies->setValue(1);
}

void BookWidget::loadBookToForm(int row)
{
    ui->editIsbn->setText(model->data(model->index(row, 1)).toString());
    ui->editTitle->setText(model->data(model->index(row, 2)).toString());
    ui->editAuthor->setText(model->data(model->index(row, 3)).toString());
    ui->editPublisher->setText(model->data(model->index(row, 4)).toString());
    
    QString dateStr = model->data(model->index(row, 5)).toString();
    if (!dateStr.isEmpty()) {
        ui->editPublishDate->setDate(QDate::fromString(dateStr, "yyyy-MM-dd"));
    }
    
    ui->editTotalCopies->setValue(model->data(model->index(row, 6)).toInt());
}

int BookWidget::getSelectedRow()
{
    QModelIndexList indexes = ui->tableView->selectionModel()->selectedRows();
    if (indexes.isEmpty()) {
        return -1;
    }
    return indexes.first().row();
}
